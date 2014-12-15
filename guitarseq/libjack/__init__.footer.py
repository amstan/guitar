
#footer to libjack/__init__.py

from jack import *

libraries=[value for name,value in globals().items() if not name.startswith("__")]
import ctypes

ringbuffer.jack_ringbuffer_write.argtypes = [ctypes.POINTER(ringbuffer.jack_ringbuffer_t), ctypes.POINTER(ctypes.c_char), ctypes.c_size_t]

class JackError(IOError):
	pass

def non_zero_error(result, func, arguments):
	if result != 0:
		raise JackError("%s(%s) returned %d" % (func.name, arguments, result))
	return result

def null_error(result, func, arguments):
	if result is None:
		raise JackError("%s(%s) returned NULL." % (func.name, arguments))
	return result

#__dict__.update(jack.__dict__)
d={}
for library in libraries:
	#print "####", library
	for name in dir(library):
		func = getattr(library, name)
		if repr(type(func)) not in ("<class 'ctypes._FuncPtr'>"): #type(jack.jack_client_open), 
			continue
		
		func.name=name
		
		if func.restype in (ctypes.c_int,):
			func.errcheck = non_zero_error
		if repr(func.restype).find("struct") != -1:
			func.errcheck = null_error
		
		d[name]=func