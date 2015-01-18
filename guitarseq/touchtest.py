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

def self.guitar_event.emit(command):
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
