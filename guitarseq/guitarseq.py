#!/usr/bin/env python2

import ctypes
import libjack
import os

libguitarseq = ctypes.CDLL(os.path.join(os.path.dirname(os.path.realpath(__file__)), "libguitarseq.so"))
libguitarseq.process = libjack.types.JackProcessCallback(libguitarseq.process)

libjack.types.logging_function = ctypes.CFUNCTYPE(libjack.UNCHECKED(None), libjack.String)
class struct_guitarseq(ctypes.Structure):
	_fields_ = [
		('jack_client', ctypes.POINTER(libjack.jack_client_t)),

		('_c_logging_buffer', ctypes.c_char_p),
		('_c_logging_buffer_size', ctypes.c_size_t),
		('_c_error_callback', libjack.types.logging_function),
		('_c_info_callback', libjack.types.logging_function),

		('out_port', ctypes.POINTER(libjack.jack_port_t)),
		('out_buffer', ctypes.POINTER(libjack.ringbuffer.jack_ringbuffer_t)),

		('in_port', ctypes.POINTER(libjack.jack_port_t)),
		('in_buffer', ctypes.POINTER(libjack.ringbuffer.jack_ringbuffer_t)),
	]

class JackInterface(struct_guitarseq):
	_callbacks = {}
	_callback_wrappers = {}

	def __init__(self, name = "guitarseq", ringbuffer_size = 100):
		self.jack_client = libjack.jack_client_open("guitarseq", libjack.types.JackNoStartServer, 0)

		#setup c logging functions
		self._c_error_callback = self._set_python_callback(libjack.jack_set_error_function, libjack.types.logging_function, self._error_callback)
		self._c_info_callback = self._set_python_callback(libjack.jack_set_info_function, libjack.types.logging_function, self._info_callback)
		self._c_logging_buffer_size = 256
		self._c_logging_buffer = ctypes.cast(ctypes.create_string_buffer(self._c_logging_buffer_size),ctypes.c_char_p)

		#setup ports
		#out
		self.out_port = libjack.jack_port_register(
			self.jack_client, 
			"midi_out", 
			libjack.types.JACK_DEFAULT_MIDI_TYPE, 
			libjack.types.JackPortIsPhysical | libjack.types.JackPortIsTerminal | libjack.types.JackPortIsOutput, 
			0
		)
		self.out_buffer = libjack.ringbuffer.jack_ringbuffer_create(ringbuffer_size)
		libjack.ringbuffer.jack_ringbuffer_reset(self.out_buffer)

		#in
		self.in_port = libjack.jack_port_register(
			self.jack_client,
			"midi_in",
			libjack.types.JACK_DEFAULT_MIDI_TYPE,
			libjack.types.JackPortIsPhysical | libjack.types.JackPortIsTerminal | libjack.types.JackPortIsInput,
			0
		)
		self.in_buffer = libjack.ringbuffer.jack_ringbuffer_create(ringbuffer_size)
		libjack.ringbuffer.jack_ringbuffer_reset(self.in_buffer)

		#callbacks
		libjack.jack_set_process_callback(self.jack_client, libguitarseq.process, ctypes.addressof(self))
		self._set_python_callback(libjack.jack_set_port_connect_callback, libjack.types.JackPortConnectCallback, self._port_connect_callback)
		self._set_python_callback(libjack.jack_set_buffer_size_callback, libjack.types.JackBufferSizeCallback, self._buffer_size_callback)
		self._set_python_callback(libjack.jack_set_xrun_callback, libjack.types.JackXRunCallback, self._xrun_callback)

		#activate
		libjack.jack_activate(self.jack_client)

	def close(self):
		print ("Cleaning up")
		libjack.jack_deactivate(self.jack_client)
		libjack.jack_client_close(self.jack_client)
	__del__=close

	def _set_python_callback(self, set_callback, callback_type, python_method):
		logging_callback = len(set_callback.argtypes) == 1

		 #Workaround for callback as a method
		def callback_wrapper(*args):
			if not logging_callback:
				args = args[:-1] #crop the last useless argument
				print "%s%r" % (python_method.__name__, args)
			ret = python_method(*args)
			if ret is None:
				return 0
			return ret

		print "%s(%r)" % (set_callback.name, python_method)
		self._callback_wrappers[set_callback.name] = callback_wrapper #Remember a reference to prevent SystemError: Objects/cellobject.c:24: bad argument to internal function
		self._callbacks[set_callback.name] = callback_type(callback_wrapper) #Remember a reference to this too to prevent random segfaults(in libcffi), ugh!

		if not logging_callback:
			set_callback(self.jack_client, self._callbacks[set_callback.name], 0)
		else:
			set_callback(self._callbacks[set_callback.name])

		return self._callbacks[set_callback.name]

	def _port_connect_callback(self, a, b, connect_flag):
		pass

	def _buffer_size_callback(self, nframes):
		self.nframes = nframes

	def _xrun_callback(self):
		pass

	def _error_callback(self, msg):
		print "Error:", msg
	def _info_callback(self, msg):
		print msg

	def out_event(self,*data):
		time = libjack.jack_frame_time(self.jack_client)

		space_left = libjack.ringbuffer.jack_ringbuffer_write_space(self.out_buffer)
		space_needed = ctypes.sizeof(libjack.types.jack_nframes_t) + 1 + len(data)
		if space_left < space_needed:
			pass
			#TODO: find out why i can't make this a wait loop
			#raise Exception("No space left in ringbuffer")

		libjack.ringbuffer.jack_ringbuffer_write(self.out_buffer, ctypes.byref(libjack.types.jack_nframes_t(time)), ctypes.sizeof(libjack.types.jack_nframes_t))
		libjack.ringbuffer.jack_ringbuffer_write(self.out_buffer, ctypes.byref(ctypes.c_size_t(len(data))), ctypes.sizeof(ctypes.c_size_t))
		for element in data:
			libjack.ringbuffer.jack_ringbuffer_write(self.out_buffer, ctypes.byref(ctypes.c_ubyte(element)), 1)

	def jack_get_ports(self, port_name_pattern = "", type_name_pattern = "", flags = 0):
		ret = libjack.jack_get_ports(self.jack_client, port_name_pattern, type_name_pattern, flags)
		return ret
		#TODO still broken

	def note(self, on=True, note=64, velocity=64):
		self.out_event(0x80 + 0x10*bool(on), note, velocity)

	def get_event(self):
		ctypes.sizeof(libjack.types.jack_nframes_t)

		time = libjack.types.jack_nframes_t()
		size = ctypes.c_size_t()

		if libjack.ringbuffer.jack_ringbuffer_read_space(self.in_buffer) < (ctypes.sizeof(time) + ctypes.sizeof(size)):
			return None

		libjack.ringbuffer.jack_ringbuffer_read(self.in_buffer, ctypes.byref(time), ctypes.sizeof(time))
		libjack.ringbuffer.jack_ringbuffer_read(self.in_buffer, ctypes.byref(size), ctypes.sizeof(size))
		time = time.value
		size = size.value

		#TODO: Do a safer read, in case only part of the message is here
		while libjack.ringbuffer.jack_ringbuffer_read_space(self.in_buffer) < size:
			print "Empty"

		buf = (ctypes.c_ubyte * size)()
		libjack.ringbuffer.jack_ringbuffer_read(self.in_buffer, ctypes.byref(buf), size)

		return time, size, list(buf)

if __name__=="__main__":
	self=JackInterface()

	import notes
	while 1:
		ret = self.get_event()
		if ret is not None:
			print ret, notes.Note(ret[2][1])

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
