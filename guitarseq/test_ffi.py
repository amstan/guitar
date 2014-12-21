#!/usr/bin/env python
import cffi
import re

ffi = cffi.FFI()

contents = open("test.h").read()
#contents = re.sub(" *__nptr *","",contents)
#contents = re.sub(" *__endptr *","",contents)

contents = re.sub(";[^;]*__deprecated__[^;]*;",";",contents)
contents = re.sub(";[^;]*__deprecated__[^;]*;",";",contents)
contents = re.sub(" *__attribute__ *\(\([^\)]*\)\) *","",contents)
#contents = re.sub(" *__attribute__ *\(\(__weak__\)\) *","",contents)

contents = re.sub("^#.*$","",contents,flags=re.MULTILINE)

#contents = re.sub(" *PRE_PACKED_STRUCTURE *","",contents)
#contents = re.sub(" *POST_PACKED_STRUCTURE *","",contents)

contents = re.sub("^.*jack_error_callback.*$","",contents,flags=re.MULTILINE)
contents = re.sub("^.*jack_info_callback.*$","",contents,flags=re.MULTILINE)


open("/tmp/test.h","w").write(contents)


ffi.cdef(contents)


c=ffi.verify("#include <jack/jack.h>",libraries=["jack"])
#c=ffi.dlopen("jack")
c.jack_client_open(b"world",0,ffi.NULL)
