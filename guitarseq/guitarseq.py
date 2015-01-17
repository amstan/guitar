#!/usr/bin/env python3

import jack
import cffi
import os
import sys

#load libguitarseq
_libguitarseq_so_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), "libguitarseq.so")
_guitarseq_cffi_h = os.path.join(os.path.dirname(os.path.realpath(__file__)), "guitarseq.cffi.h")
_ffi = cffi.FFI()
_ffi.include(jack._ffi)
_ffi.cdef(open(_guitarseq_cffi_h).read().replace("CALLBACK_ARG", "void *"))
##Broken due to gcc being passed the full path to _libguitarseq_so_file
#_lib_guitarseq = _ffi.verify(
	#jack._verify_includes + "\n#include \"%s\"" % (_guitarseq_cffi_h),
	#libraries = [_libguitarseq_so_file]
#)
_libguitarseq = _ffi.dlopen(_libguitarseq_so_file)

class GuitarSeq(object):
	def __init__(self, name = "guitarseq", ringbuffer_size = 100):
		self.jack_client = jack.Client(name)

		#struct guitarseq
		self._struct_ptr = _ffi.new("struct guitarseq[1]")
		self._struct = self._struct_ptr[0]
		self._struct.jack_client = self.jack_client._ptr

		#c logging
		jack.set_info_function(self.info)
		jack.set_error_function(self.error)
		self._struct.error_callback = jack._callback_ptrs[jack._lib.jack_set_error_function]
		self._struct.info_callback = jack._callback_ptrs[jack._lib.jack_set_info_function]
		self._struct.logging_buffer_size = 256
		self._struct.logging_buffer = _ffi.new("char[]", self._struct.logging_buffer_size)

		#Output port
		self.out_port = self.jack_client.midi_outports.register("midi_out", is_terminal = True, is_physical = True)
		self._struct.out_port = self.out_port._ptr
		self.out_buffer = jack.RingBuffer(ringbuffer_size)
		self._struct.out_buffer = self.out_buffer._ptr

		#Input port
		self.in_port = self.jack_client.midi_inports.register("midi_in", is_terminal = True, is_physical = True)
		self._struct.in_port = self.in_port._ptr
		self.in_buffer = jack.RingBuffer(ringbuffer_size)
		self._struct.in_buffer = self.in_buffer._ptr

		self.jack_client.set_process_callback(_libguitarseq.process, userdata=self._struct_ptr)

		self.jack_client.activate()

	def error(self, msg):
		print("Error:", msg, end="")
	def info(self, msg):
		print(msg, end="")

	def out_event(self,*data):
		data = bytes(data)

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

		self.out_buffer.write(time)
		self.out_buffer.write(size)
		self.out_buffer.write(data)

	def note(self, on=True, note=64, velocity=64):
		self.out_event(0x80 + 0x10*bool(on), note, velocity)

	def get_event(self):
		if self.in_buffer.read_space < (_ffi.sizeof("jack_nframes_t") + _ffi.sizeof("size_t")):
			return None

		time = _ffi.cast("jack_nframes_t[1]", self.in_buffer._read(_ffi.sizeof("jack_nframes_t"))[1])[0]
		size = _ffi.cast("size_t[1]", self.in_buffer._read(_ffi.sizeof("size_t"))[1])[0]
		return time,self.in_buffer.read(size)

if __name__=="__main__":
	self=GuitarSeq()

	try:
		print("Print all input events")
		import notes
		while 1:
			ret = self.get_event()
			if ret is not None:
				time, data = ret
				data = list(data)
				try:
					(command, velocity, note_id) = data
					print(time, (command, velocity, note_id), notes.Note(note_id))
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
