#!/usr/bin/env python2

import ctypes
import libjack
import os

libguitarseq = ctypes.CDLL(os.path.join(os.path.dirname(os.path.realpath(__file__)), "libguitarseq.so"))
libguitarseq.jack_client = ctypes.POINTER(libjack.jack_client_t).in_dll(libguitarseq, "jack_client")
libguitarseq.output_port = ctypes.POINTER(libjack.jack_port_t).in_dll(libguitarseq, "output_port")
libguitarseq.outgoing_ringbuffer = ctypes.POINTER(libjack.ringbuffer.jack_ringbuffer_t).in_dll(libguitarseq, "outgoing_ringbuffer")
libguitarseq.process = libjack.types.JackProcessCallback(libguitarseq.process)

class JackInterface(object):
	def __init__(self):
		
		self.jack_client = libjack.jack_client_open("guitarseq", libjack.types.JackNoStartServer, 0)
		self.output_port = libjack.jack_port_register(
			self.jack_client, 
			"midi_out", 
			libjack.types.JACK_DEFAULT_MIDI_TYPE, 
			libjack.types.JackPortIsPhysical | libjack.types.JackPortIsTerminal | libjack.types.JackPortIsOutput, 
			0
		)
		
		self.outgoing_ringbuffer=libjack.ringbuffer.jack_ringbuffer_create(3*10)
		libjack.ringbuffer.jack_ringbuffer_reset(self.outgoing_ringbuffer)
		
		libguitarseq.jack_client.contents = self.jack_client.contents
		libguitarseq.output_port.contents = self.output_port.contents
		libguitarseq.outgoing_ringbuffer.contents = self.outgoing_ringbuffer.contents
		
		libjack.jack_set_process_callback(self.jack_client, libguitarseq.process, 0)
		libjack.jack_activate(self.jack_client)
		
	def __del__(self):
		print ("Cleaning up")
		libjack.jack_deactivate(self.jack_client)
		libjack.jack_client_close(self.jack_client)
	
	def add_event(self,*data):
		time = libjack.jack_frame_time(self.jack_client)

		space_left = libjack.ringbuffer.jack_ringbuffer_write_space(self.outgoing_ringbuffer)
		space_needed = ctypes.sizeof(libjack.types.jack_nframes_t) + 1 + len(data)
		if space_left < space_needed:
			pass
			#raise Exception("No space left in ringbuffer")

		libjack.ringbuffer.jack_ringbuffer_write(self.outgoing_ringbuffer, ctypes.byref(libjack.types.jack_nframes_t(time)), ctypes.sizeof(libjack.types.jack_nframes_t))
		libjack.ringbuffer.jack_ringbuffer_write(self.outgoing_ringbuffer, ctypes.byref(ctypes.c_size_t(len(data))), 1)
		for element in data:
			libjack.ringbuffer.jack_ringbuffer_write(self.outgoing_ringbuffer, ctypes.byref(ctypes.c_ubyte(element)), 1)
	
	def note(self, on=True, note=64, velocity=64):
		self.add_event(0x80 + 0x10*bool(on), note, velocity)

if __name__=="__main__":
	self=JackInterface()
	#self.add_event(*([1]*10))

	import time
	import random

	intervals = [3,4,3,2,3,4,3,2,3,4,3,2]
	start = 64
	scale = [add+start for add in map(sum,[intervals[0:i+1] for i in range(len(intervals))])]

	while 1:
		for note in scale:
			note = random.choice(scale)
			self.note(True,note)
			time.sleep(random.gauss(0.5,0.01))
			self.note(False,note)
			time.sleep(random.gauss(0.1,0.01))