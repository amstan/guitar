#!/usr/bin/env python

from __future__ import print_function

import jack
import cffi
import os

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
		self.out_port = self.jack_client.outports.register("midi_in", is_terminal = True, is_physical = True, is_midi = True)
		self._struct.out_port = self.out_port._ptr
		#self.out_buffer = libjack.ringbuffer.jack_ringbuffer_create(ringbuffer_size)
		#libjack.ringbuffer.jack_ringbuffer_reset(self.out_buffer)

		#Input port
		self.in_port = self.jack_client.inports.register("midi_out", is_terminal = True, is_physical = True, is_midi = True)
		self._struct.in_port = self.in_port._ptr
		#self.in_buffer = libjack.ringbuffer.jack_ringbuffer_create(ringbuffer_size)
		#libjack.ringbuffer.jack_ringbuffer_reset(self.in_buffer)

		self.jack_client.set_process_callback(_libguitarseq.process, userdata=self._struct_ptr)

		self.jack_client.activate()

	def error(self, msg):
		print("Error:", msg, end="")
	def info(self, msg):
		print(msg, end="")

	#def out_event(self,*data):
		#time = libjack.jack_frame_time(self.jack_client)

		#space_left = libjack.ringbuffer.jack_ringbuffer_write_space(self.out_buffer)
		#space_needed = ctypes.sizeof(libjack.types.jack_nframes_t) + 1 + len(data)
		#if space_left < space_needed:
			#pass
			##TODO: find out why i can't make this a wait loop
			##raise Exception("No space left in ringbuffer")

		#libjack.ringbuffer.jack_ringbuffer_write(self.out_buffer, ctypes.byref(libjack.types.jack_nframes_t(time)), ctypes.sizeof(libjack.types.jack_nframes_t))
		#libjack.ringbuffer.jack_ringbuffer_write(self.out_buffer, ctypes.byref(ctypes.c_size_t(len(data))), ctypes.sizeof(ctypes.c_size_t))
		#for element in data:
			#libjack.ringbuffer.jack_ringbuffer_write(self.out_buffer, ctypes.byref(ctypes.c_ubyte(element)), 1)

	#def jack_get_ports(self, port_name_pattern = "", type_name_pattern = "", flags = 0):
		#ports = libjack.jack_get_ports(self.jack_client, port_name_pattern, type_name_pattern, flags)
		#try:
			#for port in ports:
				#if not port:
					#break
				#yield ctypes.string_at(port)
			#libjack.jack_free(ports)
		#except ValueError:
			#raise StopIteration()


	#def note(self, on=True, note=64, velocity=64):
		#self.out_event(0x80 + 0x10*bool(on), note, velocity)

	#def get_event(self):
		#ctypes.sizeof(libjack.types.jack_nframes_t)

		#time = libjack.types.jack_nframes_t()
		#size = ctypes.c_size_t()

		#if libjack.ringbuffer.jack_ringbuffer_read_space(self.in_buffer) < (ctypes.sizeof(time) + ctypes.sizeof(size)):
			#return None

		#libjack.ringbuffer.jack_ringbuffer_read(self.in_buffer, ctypes.byref(time), ctypes.sizeof(time))
		#libjack.ringbuffer.jack_ringbuffer_read(self.in_buffer, ctypes.byref(size), ctypes.sizeof(size))
		#time = time.value
		#size = size.value

		##TODO: Do a safer read, in case only part of the message is here
		#while libjack.ringbuffer.jack_ringbuffer_read_space(self.in_buffer) < size:
			#print("Empty")

		#buf = (ctypes.c_ubyte * size)()
		#libjack.ringbuffer.jack_ringbuffer_read(self.in_buffer, ctypes.byref(buf), size)

		#return time, size, list(buf)

if __name__=="__main__":
	self=GuitarSeq()

	#import notes
	#while 1:
		#ret = self.get_event()
		#if ret is not None:
			#print(ret, notes.Note(ret[2][1]))

	#self.get_byte()
	#self.out_event(*([1]*10))
	#ret = self.jack_get_ports()

	#import time
	#import random

	#intervals = [3,4,3,2,3,4,3,2,3,4,3,2]
	#start = 64
	#scale = [add+start for add in map(sum,[intervals[0:i+1] for i in range(len(intervals))])]

	#while 1:
		#for note in scale:
			#note = random.choice(scale)
			#self.note(True,note)
			#time.sleep(random.gauss(0.5,0.01))
			#self.note(False,note)
			#time.sleep(random.gauss(0.1,0.01))
