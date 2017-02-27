#!/usr/bin/env python3

import jack
import cffi
import os
import sys

import notes

#load libguitarseq
_libguitarseq_so_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), "libguitarseq.so")
_guitarseq_cffi_h = os.path.join(os.path.dirname(os.path.realpath(__file__)), "guitarseq.cffi.h")
_ffi = cffi.FFI()
_ffi.include(jack._ffi)
_ffi.cdef(open(_guitarseq_cffi_h).read())
_libguitarseq = _ffi.dlopen(_libguitarseq_so_file)

class GuitarSeq(object):
	tuning = [notes.Note(name) for name in "D2 A2 D3 G3 B3 E4".split(" ")]
	fret_count = 9
	string_count = 6

	def __init__(self):
		self.setup_jack()

		self.fret_bitmap = []
		for string in range(self.string_count):
			self.fret_bitmap.append([False] * self.fret_count)
		self.recalculate_active_frets()
		self.last_played = [None] * self.string_count

	def setup_jack(self):
		self.jack_client = jack.Client("guitarseq")

		#struct guitarseq
		self._struct_ptr = _ffi.new("struct guitarseq[1]")
		self._struct = self._struct_ptr[0]
		self._struct.jack_client = self.jack_client._ptr

		#Output port
		self.out_port = self.jack_client.midi_outports.register("midi_out", is_terminal = True, is_physical = True)
		self._struct.out_port = self.out_port._ptr
		self.out_buffer = jack.RingBuffer(256)
		self._struct.out_buffer = self.out_buffer._ptr

		#Input port
		self.in_port = self.jack_client.midi_inports.register("midi_in", is_terminal = True, is_physical = True)
		self._struct.in_port = self.in_port._ptr
		self.in_buffer = jack.RingBuffer(4096)
		self._struct.in_buffer = self.in_buffer._ptr

		jack._check(
			jack._lib.jack_set_process_callback(
				self.jack_client._ptr,
				_libguitarseq.process,
				self._struct_ptr
			),
			"Error setting guitarseq C process callback"
		)

		self.jack_client.activate()

	@staticmethod
	@jack.set_error_function
	def jack_error(msg):
		print("Error: %s" % msg)
		sys.stdout.flush()

	@staticmethod
	@jack.set_info_function
	def jack_info(msg):
		print("%s" % msg)
		sys.stdout.flush()

	def out_event(self,*data):
		ffi_data = _ffi.new("jack_midi_data_t[]", len(data))
		ffi_data[0:len(ffi_data)] = data

		time = _ffi.new("jack_nframes_t[1]")
		time[0] = self.jack_client.frame_time
		size = _ffi.new("size_t[1]")
		size[0] = len(data)

		space_left = self.out_buffer.write_space
		space_needed = _ffi.sizeof(time) + _ffi.sizeof(size) + len(data)
		if space_left < space_needed:
			pass
			#TODO: find out why i can't make this a wait loop
			#raise Exception("No space left in ringbuffer")

		self.out_buffer.write(_ffi.buffer(time))
		self.out_buffer.write(_ffi.buffer(size))
		self.out_buffer.write(_ffi.buffer(ffi_data))

	def note(self, on=True, note=64, velocity=64):
		self.out_event(0x80 + 0x10*bool(on), note, velocity)

	def get_midi_in_event(self):
		if self.in_buffer.read_space < (_ffi.sizeof("jack_nframes_t") + _ffi.sizeof("size_t")):
			return None

		time = _ffi.cast("jack_nframes_t[1]", _ffi.from_buffer(self.in_buffer.read(_ffi.sizeof("jack_nframes_t"))))[0]
		size = _ffi.cast("size_t[1]", _ffi.from_buffer(self.in_buffer.read(_ffi.sizeof("size_t"))))[0]
		return time, tuple(bytes(self.in_buffer.read(size)))

	def recalculate_active_frets(self):
		self.active_frets = [
			max(
				((fret_id, fret_value) for fret_id, fret_value in list(enumerate(string)) + [(-1,True)] if fret_value),
				key=lambda *args: args[0]
			)[0] for string in self.fret_bitmap
		]

	def on_guitar_event(self, event):
		print(event)
		pressed = event[0] == 'p'
		event_type = event[1]
		string = int(event[2])

		if event_type == 'f':
			fret = int(event[3:])
			self.fret_bitmap[string][fret] = pressed
			last_played_fret = self.active_frets[string]
			self.recalculate_active_frets()

			if self.last_played[string] != None:
				if fret >= last_played_fret:
					#mute string
					self.note(False, self.last_played[string].id)
					self.last_played[string] = None

		elif event_type == 's':
			if len(event) > 3 and event[3] == 'v':
				velocity = int(event[4:])
			else:
				velocity = 64

			if pressed:
				if self.last_played[string] != None:
					#mute string
					self.note(False, self.last_played[string].id)
			else: #released
				note = self.tuning[string] + self.active_frets[string] + 1
				self.last_played[string] = note
				self.note(True, note.id, velocity)

if __name__=="__main__":
	import notes
	self=GuitarSeq()
	
	try:
		print("Print all input events")
		while 1:
			ret = self.get_midi_in_event()
			if ret is not None:
				time, data = ret
				try:
					(command, note_id, velocity) = data
					print(time, (hex(command), note_id, velocity), notes.Note(note_id))
				except Exception:
					print(time, data)
	except KeyboardInterrupt:
		pass

	import time
	import random

	intervals = [3,4,3,2,3,4,3,2,3,4,3,2]
	start = 64
	scale = [add+start for add in map(sum,[intervals[0:i+1] for i in range(len(intervals))])]

	print("Random notes from a scale: %r" % list(map(notes.Note,scale)))

	while 1:
		for note in scale:
			note = random.choice(scale)
			self.note(True,note)
			time.sleep(random.gauss(0.5,0.01))
			self.note(False,note)
			time.sleep(random.gauss(0.1,0.01))
