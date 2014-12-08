

#footer to libjack/__init__.py
libraries=[value for name,value in globals().items() if not name.startswith("__")]
import ctypes

class JackError(IOError):
	pass

def non_zero_error(result, func, arguments):
	if result != 0:
		raise JackError("%s(%s) returned %d" % (func, arguments, result.value))

def null_error(result, func, arguments):
	if result is None:
		raise JackError("%s(%s) returned NULL." % (func, arguments))

l={}
for library in libraries:
	#print "####", library
	for name in dir(library):
		function = getattr(library, name)
		if repr(type(function)) not in ("<class 'ctypes._FuncPtr'>"): #type(jack.jack_client_open), 
			continue
		
		if function.restype in (ctypes.c_int,):
			function.errcheck = non_zero_error
		if repr(function.restype).find("struct") != -1:
			function.errcheck = null_error
		l[name]=function