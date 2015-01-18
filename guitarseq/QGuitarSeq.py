#!/usr/bin/env python3

import sys
import time

from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

from guitarseq import *
import notes

def fix_ctrlc(app):
	app.event = lambda e: QApplication.event(app, e)
	app.startTimer(100)

	import signal
	signal.signal(signal.SIGINT,lambda *args:app.quit())

class KeyboardEmulator(QWidget):
	strum_delay_ms = 0.015

	guitar_event = pyqtSignal(str)

	def __init__(self, parent = None):
		super().__init__(parent)
		self.setWindowTitle("Keyboard Based Emulator")

	def keyPressEvent(self, event):
		self.keyEvent(event, True)
	def keyReleaseEvent(self, event):
		self.keyEvent(event, False)

	def keyEvent(self, event, pressed):
		event.accept()

		if event.isAutoRepeat():
			return

		code = event.nativeScanCode()
		pressed="rp"[pressed]

		#frets
		if code in range(67, 76 + 1): #"F1-F10"
			fret = code - 67
			self.guitar_event.emit("%sf5%d" % (pressed, fret))
		elif code in range(10, 21 + 1): #1234567890-=
			fret = code - 10
			self.guitar_event.emit("%sf4%d" % (pressed, fret))
		elif code in range(24, 35 + 1): #qwertyuiop[]
			fret = code - 24
			self.guitar_event.emit("%sf3%d" % (pressed, fret))
		elif code in range(38, 48 + 1): #asdfghjkl;'
			fret = code - 38
			self.guitar_event.emit("%sf2%d" % (pressed, fret)); time.sleep(self.strum_delay_ms)
			self.guitar_event.emit("%sf1%d" % (pressed, fret)); time.sleep(self.strum_delay_ms)
			self.guitar_event.emit("%sf0%d" % (pressed, fret))

		#strings
		elif code == 96: #F12
			self.guitar_event.emit("%ss5" % (pressed,))
		elif code == 22: #Backspace
			self.guitar_event.emit("%ss4" % (pressed,))
		elif code == 51: #\
			self.guitar_event.emit("%ss3" % (pressed,))
		elif code == 36: #Enter
			self.guitar_event.emit("%ss2" % (pressed,)); time.sleep(self.strum_delay_ms)
			self.guitar_event.emit("%ss1" % (pressed,)); time.slee/p(self.strum_delay_ms)
			self.guitar_event.emit("%ss0" % (pressed,))
		elif code == 105: #Ctrl
			for string in range(6):
				self.guitar_event.emit("%ss%d" % (pressed,string))
				time.sleep(self.strum_delay_ms)
		else:
			sys.stderr.write("unknown %s %r %s \n" % (event.nativeScanCode(), event.text(), pressed))

class Fret(QPushButton):
	def __init__(self, string_id, fret_id, parent = None):
		self.string_id = string_id
		self.fret_id = fret_id
		self.note = parent.guitarseq.tuning[string_id] + fret_id + 1

		super().__init__("%s" % (self.note), parent)

		self.setSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding)
		self.setCheckable(True)
		self.setChecked(False)

		self.toggled.connect(self.on_toggled)

		self._off_color = self.palette()
		self._off_color.setColor(QPalette.Button, QColor(40,40,40))

		self._on_color = QPalette()
		self._on_color.setColor(QPalette.Button, QColor(QColor.fromHslF((self.note.id % 12)/12,1,0.5)))

		self.setAutoFillBackground(True)
		self.on_toggled(self.isChecked())
		self.update()

	def on_toggled(self, checked):
		self.setPalette((self._off_color, self._on_color)[checked])
		self.update()

class GuitarPanel(QWidget):
	def __init__(self, strings, frets, guitarseq, parent = None):
		self.guitarseq = guitarseq

		super().__init__(parent)

		layout = QGridLayout(self)

		self.frets = []
		for string_id in range(strings):
			for fret_id in range(frets):
				fret = Fret(string_id, fret_id, self)

				layout.addWidget(fret, strings - string_id - 1, fret_id)
				self.frets.append(fret)

class QGuitarSeq(QMainWindow):
	def __init__(self, parent=None):
		super().__init__(parent)

		self.guitarseq = GuitarSeq()

		vsplitter=QSplitter(Qt.Vertical,self)
		self.setCentralWidget(vsplitter)

		self.guitar_panel = GuitarPanel(self.guitarseq.string_count, self.guitarseq.fret_count, self.guitarseq)
		vsplitter.addWidget(self.guitar_panel)

		self.setWindowTitle("QGuitarSeq")

		self.timer = QTimer(self)
		self.timer.timeout.connect(self.scan_for_notes)
		self.timer.start(10)

		self.emulator = KeyboardEmulator()
		self.emulator.guitar_event.connect(self.guitarseq.on_guitar_event)
		self.emulator.show()

	def scan_for_notes(self):
		while 1:
			ret = self.guitarseq.get_midi_in_event()
			if ret is not None:
				time, data = ret
				try:
					(command, note_id, velocity) = data
					print(time, (hex(command), note_id, velocity), notes.Note(note_id))
					if (command & 0xe0) == 0x80:
						self.handle_note(notes.Note(note_id), command & 0x10)
				except Exception:
					print(time, data)
				continue
			break

	def handle_note(self, note, pressed):
		for fret in self.guitar_panel.frets:
			if fret.note.id == note.id:
				fret.setChecked(pressed)

if __name__ == '__main__':
	import sys

	app = QApplication(sys.argv)

	window = QGuitarSeq()
	window.show()

	fix_ctrlc(app)
	sys.exit(app.exec_())
