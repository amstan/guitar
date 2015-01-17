#!/usr/bin/env python3

import sys
import time

from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

from guitarseq import *
from notes import *

def fix_ctrlc(app):
	app.event = lambda e: QApplication.event(app, e)
	app.startTimer(100)

	import signal
	signal.signal(signal.SIGINT,lambda *args:app.quit())

class KeyCaptureWindow(QWidget):
	strum_delay_ms = 0.015

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
		if code in range(67, 76 + 1):
			fret = code - 67
			enqueue("%sf5%d" % (pressed, fret))
		elif code in range(10, 21 + 1):
			fret = code - 10
			enqueue("%sf4%d" % (pressed, fret))
		elif code in range(24, 35 + 1):
			fret = code - 24
			enqueue("%sf3%d" % (pressed, fret))
		elif code in range(38, 48 + 1):
			fret = code - 38
			enqueue("%sf2%d" % (pressed, fret)); time.sleep(self.strum_delay_ms)
			enqueue("%sf1%d" % (pressed, fret)); time.sleep(self.strum_delay_ms)
			enqueue("%sf0%d" % (pressed, fret))

		#strings
		elif code == 96:
			enqueue("%ss5" % (pressed,))
		elif code == 22:
			enqueue("%ss4" % (pressed,))
		elif code == 51:
			enqueue("%ss3" % (pressed,))
		elif code == 36:
			enqueue("%ss2" % (pressed,)); time.sleep(self.strum_delay_ms)
			enqueue("%ss1" % (pressed,)); time.sleep(self.strum_delay_ms)
			enqueue("%ss0" % (pressed,))
		elif code == 105:
			for string in range(6):
				enqueue("%ss%d" % (pressed,string))
				time.sleep(self.strum_delay_ms)
		else:
			sys.stderr.write("unknown %s %r %s \n" % (event.nativeScanCode(), event.text(), pressed))

		global test
		test=event

tuning = [Note(name) for name in "E2 A2 D3 G3 B3 E4".split(" ")]
FRET_COUNT = 19
STRING_COUNT = 6

fret_bitmap = []
for string in range(STRING_COUNT):
	fret_bitmap.append([False] * FRET_COUNT)
last_frets = [-1] * STRING_COUNT
last_played = [None] * 6

def calculate_last_frets(fret_bitmap):
	return [
		max(
			((fret_id, fret_value) for fret_id, fret_value in list(enumerate(string)) + [(-1,True)] if fret_value),
			key=lambda *args: args[0]
		)[0] for string in fret_bitmap]

def enqueue(command):
	print(command)
	global last_frets

	pressed = command[0] == 'p'
	command_type = command[1]
	string = int(command[2])

	if command_type == 'f':
		fret = int(command[3:])
		fret_bitmap[string][fret] = pressed
		print(last_frets, fret)
		last_played_fret = last_frets[string]
		last_frets = calculate_last_frets(fret_bitmap)

		if last_played[string] != None:
			if fret >= last_played_fret:
				#mute string
				i.note(False, last_played[string].id)
				last_played[string] = None

	elif command_type == 's':
		if pressed:
			if last_played[string] != None:
				#mute string
				i.note(False, last_played[string].id)
		else: #released
			note = tuning[string]+last_frets[string]+1
			last_played[string] = note
			i.note(True, note.id)

if __name__ == '__main__':
	import sys

	app = QApplication(sys.argv)

	window = KeyCaptureWindow()
	window.show()

	i=GuitarSeq()

	fix_ctrlc(app)
	sys.exit(app.exec_())
