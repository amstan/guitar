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
			self.guitar_event.emit("%ss1" % (pressed,)); time.sleep(self.strum_delay_ms)
			self.guitar_event.emit("%ss0" % (pressed,))
		elif code == 105: #Ctrl
			for string in range(6):
				self.guitar_event.emit("%ss%d" % (pressed,string))
				time.sleep(self.strum_delay_ms)
		else:
			sys.stderr.write("Unknown KeyEvent %s %r %s \n" % (event.nativeScanCode(), event.text(), pressed))

import device
class GuitarDevice(device.Device, QThread):
	guitar_event = pyqtSignal(str)

	def __init__(self, read_packet_iter):
		device.Device.__init__(self, read_packet_iter)
		QThread.__init__(self)

	def run(self):
		for event in self.process_strings():
			self.guitar_event.emit("ps%d" % event["string_id"])
			self.guitar_event.emit("rs%dv%03d" % (event["string_id"], event["velocity"]))

class RaspberryPiRpc(QThread):
	guitar_event = pyqtSignal(str)

	def __init__(self, socket=("192.168.0.109", 25000)):
		import rpyc

		self.rpyc_connection = rpyc.connect(*socket)
		self.collection = self.rpyc_connection.root.collection
		self.set_led = self.rpyc_connection.root.set_led
		print(self.collection)

		self.frets = []
		for fret_id in range(9):
			self.frets.append([])
			for string_id in range(6):
				self.frets[fret_id].append(
					self.collection[0x30 + fret_id].leds[string_id])

		self.touch = []
		for fret_id in range(10):
			self.touch.append(
				self.collection[0x30 + fret_id].touch)

		QThread.__init__(self)

	def run(self):
		while True:
			for fret_id in range(10):
				old_reading = self.touch[fret_id]
				new_reading = self.collection[0x30 + fret_id].touch
				self.touch[fret_id] = new_reading

				if fret_id < 9:
					for string_id in range(6):
						if (old_reading[string_id] < 100) and (new_reading[string_id] >= 100):
							self.guitar_event.emit("pf%1d%d" % (string_id, fret_id))
						if (old_reading[string_id] > 100) and (new_reading[string_id] <= 100):
							self.guitar_event.emit("rf%1d%d" % (string_id, fret_id))
				else:
					# strum for now
					for string_id in range(3):
						if (old_reading[string_id] < 100) and (new_reading[string_id] >= 100):
							self.guitar_event.emit("ps%1d" % (string_id))
						if (old_reading[string_id] > 100) and (new_reading[string_id] <= 100):
							self.guitar_event.emit("rs%1d" % (string_id))

				#print(new_reading)
			time.sleep(0.02)

class LocalI2C(QThread):
	guitar_event = pyqtSignal(str)

	device_list = {
		(0x00420051, 0x46335716, 0x20333539): (0x30, "fret 0"),
		(0x002c0055, 0x46335717, 0x20333539): (0x31, "fret 1"),
		(0x001d003a, 0x46335716, 0x20333539): (0x32, "fret 2"),
		(0x0028004b, 0x46335717, 0x20333539): (0x33, "fret 3"),
		(0x0030003b, 0x46335717, 0x20333539): (0x34, "fret 4"),
		(0x00290033, 0x46335716, 0x20333539): (0x35, "fret 5"),
		(0x001e0038, 0x46335716, 0x20333539): (0x36, "fret 6"),
		(0x002f0033, 0x46335716, 0x20333539): (0x37, "fret 7"),
		(0x002c0038, 0x46335716, 0x20333539): (0x38, "fret 8"),
		(0x00400035, 0x46335717, 0x20333539): (0x39, "fret 9"),

		(0x00380058, 0x46335717, 0x20333539): (0x50, "option 0"),
		(0x003b0037, 0x46335717, 0x20333539): (0x51, "option 1"),
	}

	def __init__(self):
		import fret_tester
		import threading

		self.i2c = fret_tester.periphery.I2C("/dev/i2c-5")
		self.collection = fret_tester.FretCollection(self.i2c)
		self.collection.enumerate(self.device_list)
		self.touchpad = fret_tester.Touchpad(self.i2c)

		for f in self.collection.values():
			if f.version["Firmware copy"]!="RW":
				f.jump("RW")

		print("start local i2c tasks")
		for f in self.collection.values():
			t = threading.Thread(target=f.task, args=())
			t.daemon = True
			t.start()

		self.touch = []
		for fret_id in range(10):
			self.touch.append(
				self.collection[0x30 + fret_id].touch)

		QThread.__init__(self)

	def set_led(self, string_id, fret_id, r, g, b):
		self.collection[0x30 + fret_id].leds[string_id] = [r, g, b]

	def set_led2(self, string_id, fret_id, r, g, b):
		self.collection[0x30 + fret_id].leds2[string_id] = [r, g, b]

	def run(self):
		while True:
			for fret_id in range(10):
				old_reading = self.touch[fret_id]
				new_reading = self.collection[0x30 + fret_id].touch
				self.touch[fret_id] = new_reading

				for string_id in range(6):
					if fret_id < 9:
						spot = "f%1d%d" % (string_id, fret_id)
					else:
						spot = "s%1d" % (string_id)

					rising_threshold = 130
					falling_threshold = 130

					if ((old_reading[string_id] < rising_threshold) and
					    (new_reading[string_id] >= rising_threshold)):
						self.guitar_event.emit("p%s" % (spot))
						self.set_led2(string_id, fret_id, 30, 30, 30)

					if ((old_reading[string_id] > falling_threshold) and
					    (new_reading[string_id] <= falling_threshold)):
						self.guitar_event.emit("r%s" % (spot))
						self.set_led2(string_id, fret_id, 0, 0, 0)

				#print(new_reading)
			time.sleep(0.02)

class QFret(QPushButton):
	on_color = QColor(0,0,0)
	off_color = QColor(0,0,0)

	def __init__(self, string_id, fret_id, parent = None):
		self.string_id = string_id
		self.fret_id = fret_id
		super().__init__("%d, %d" % (self.string_id, self.fret_id), parent)

		self.setSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding)
		self.setCheckable(True)
		self.setChecked(False)

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

		#self.emulator = KeyboardEmulator()
		#self.emulator.guitar_event.connect(self.guitarseq.on_guitar_event)
		#self.emulator.show()

		#self.rpi = RaspberryPiRpc()
		#self.rpi.guitar_event.connect(self.guitarseq.on_guitar_event)
		#self.rpi.start()

		self.locali2c = LocalI2C()
		self.locali2c.guitar_event.connect(self.guitarseq.on_guitar_event)
		self.locali2c.start()

		self.guitar_device = GuitarDevice(self.locali2c.touchpad.read_packet_iter)
		self.guitar_device.guitar_event.connect(self.guitarseq.on_guitar_event)
		self.guitar_device.start()

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
				except Exception as e:
					print(time, data, e)
				continue
			break

	def handle_note(self, note, pressed, velocity):
		pressed = pressed!=0
		for fret in self.main_frets:
			if fret.note.id == note.id:
				fret.setChecked(pressed)
				if pressed:
					color = QColor.fromHslF((fret.note.id % 12)/12,1,velocity/128.*0.7)
					fret.update_color(color)
				if pressed:
					self.locali2c.set_led(fret.string_id, fret.fret_id, *color.getRgb()[:3])
				else:
					self.locali2c.set_led(fret.string_id, fret.fret_id, 0, 0, 0)

if __name__ == '__main__':
	import sys

	app = QApplication(sys.argv)

	window = QGuitarSeq()
	window.show()

	fix_ctrlc(app)
	sys.exit(app.exec_())
