#!/usr/bin/env python2

import ctypes
import libjack
import os

libguitarseq = ctypes.CDLL(os.path.join(os.path.dirname(os.path.realpath(__file__)), "libguitarseq.so"))
libguitarseq.process = libjack.types.JackProcessCallback(libguitarseq.process)

class JackError(IOError):
	pass

class JackInterface(object):
	refs=[ctypes, libjack, JackError]
	
	def __init__(self):
		self.jack_client = libjack.jack.jack_client_open("guitarseq", libjack.types.JackNoStartServer, 0)
		self.output_port = libjack.jack.jack_port_register(
			self.jack_client, 
			"midi_out", 
			libjack.types.JACK_DEFAULT_MIDI_TYPE, 
			libjack.types.JackPortIsPhysical | libjack.types.JackPortIsTerminal | libjack.types.JackPortIsOutput, 
			0
		)
		libjack.jack.jack_set_process_callback(self.jack_client, libguitarseq.process, 0)
		libjack.jack.jack_activate(self.jack_client)
		
	def __del__(self):
		print ("Cleaning up")
		libjack.jack.jack_deactivate(self.jack_client)
		libjack.jack.jack_client_close(self.jack_client)

self=JackInterface()