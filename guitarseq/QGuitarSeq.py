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

class QFret(QPushButton):
	on_color = QColor(0,0,0)
	off_color = QColor(0,0,0)

	def __init__(self, string_id, fret_id, parent = None):
		self.string_id = string_id
		self.fret_id = fret_id
		super().__init__("%d, %d" % (self.string_id, self.fret_id), parent)

		self.setSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding)
		self.setCheckable(True)
		self.setChecked(True)

		#Background color
		self.released_color = self.palette()
		self.released_color.setColor(QPalette.Button, QColor(30,30,30))
		self.setPalette(self.released_color)

		#LED
		self.update_color(self.off_color)

	def resizeEvent(self, e):
		self.led_border = 3.5
		self.led_size = e.size().height() - self.led_border * 2
		self.setStyleSheet("text-align: left; padding-left: %dpx; padding-right: %dpx" % (self.led_border * 2 + self.led_size, self.led_border * 2))
		#self.update()

	def event(self, e):
		if type(e) == QPaintEvent:
			if self.isDown() or self.isChecked():
				pal = self.palette()
				pal.setColor(QPalette.Button, self.on_color.darker(600))
				self.setPalette(pal)
				self._current_color = self.on_color
			else:
				self.setPalette(self.released_color)
				self._current_color = self.off_color
		ret = super().event(e)
		return ret

	def paintEvent(self, e):
		super().paintEvent(e)
		qp = QPainter()
		qp.begin(self)
		qp.setRenderHint(QPainter.Antialiasing,True)
		qp.setRenderHint(QPainter.HighQualityAntialiasing,True)
		self.drawLED(qp)
		qp.end()

	def drawLED(self, qp):
		border = 3.5

		qp.setPen(QColor(64,64,64))
		qp.setBrush(self._current_color)
		qp.drawEllipse(QRectF(
			self.led_border,
			self.led_border,
			self.led_size,
			self.led_size
		))

	def update_color(self, color):
		self.on_color = color
		self.update()

class QFretPanel(QFrame):
	def __init__(self, strings, frets, parent = None):
		self.strings = strings
		self.frets = frets

		super().__init__(parent)

		self.layout = QGridLayout(self)

		for string_id in range(self.strings):
			for fret_id in range(self.frets):
				fret = QFret(self.strings - string_id -1, fret_id, self)
				self.layout.addWidget(fret, string_id + 1, fret_id + 1)

	def __iter__(self):
		for string_id in range(self.strings):
			for fret_id in range(self.frets):
				yield self[string_id,fret_id]

	def __getitem__(self, index):
		return self.layout.itemAtPosition(self.strings - index[0], index[1] + 1).widget()

class QGuitarSeq(QMainWindow):
	def __init__(self, parent=None):
		super().__init__(parent)

		self.guitarseq = GuitarSeq()

		vsplitter=QSplitter(Qt.Vertical,self)
		self.setCentralWidget(vsplitter)

		tophsplitter=QSplitter(Qt.Horizontal,self)
		vsplitter.addWidget(tophsplitter)

		self.main_frets = QFretPanel(self.guitarseq.string_count, self.guitarseq.fret_count)
		for fret in self.main_frets:
			fret.note = self.guitarseq.tuning[fret.string_id] + fret.fret_id + 1
			fret.setText(str(fret.note))
			fret.on_color = QColor.fromHslF((fret.note.id % 12)/12,1,0.5)
		tophsplitter.addWidget(self.main_frets)

		self.string_frets = QFretPanel(self.guitarseq.string_count, 1)
		for string in self.string_frets:
			string.setText("String %d" % (string.string_id + 1))
		tophsplitter.addWidget(self.string_frets)

		test=QGraphicsView()
		print(test.frameShape())
		test.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
		tophsplitter.addWidget(test)

		self.option_frets = QFretPanel(self.guitarseq.string_count, 2)
		for fret in self.option_frets:
			fret.setText("Option " + fret.text())
		tophsplitter.addWidget(self.option_frets)

		vsplitter.addWidget(QGraphicsView())
		vsplitter.addWidget(QLabel("hi"))

		self.setWindowTitle("QGuitarSeq")

		self.timer = QTimer(self)
		self.timer.timeout.connect(self.scan_for_notes)
		self.timer.start(10)

		self.emulator = KeyboardEmulator(self)
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
						self.handle_note(notes.Note(note_id), command & 0x10, velocity)
				except Exception:
					print(time, data)
				continue
			break

	def handle_note(self, note, pressed, velocity):
		for fret in self.main_frets:
			if fret.note.id == note.id:
				fret.setChecked(pressed)
				if pressed:
					fret.update_color(QColor.fromHslF((fret.note.id % 12)/12,1,velocity/128.*0.7))

if __name__ == '__main__':
	import sys

	app = QApplication(sys.argv)

	window = QGuitarSeq()
	window.show()

	fix_ctrlc(app)
	sys.exit(app.exec_())
