DBUS := dbus

CPPFLAGS := `pkg-config --cflags dbus-glib-1`
CFLAGS := -Wall -g -O2
LDFLAGS := `pkg-config --libs dbus-glib-1`

all: EchoObject EchoObjectClient

EchoObjectClient: EchoObjectClient.c EchoObjectBindings.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

EchoObject: EchoObject.c EchoObject.h EchoObjectGlue.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

EchoObjectGlue.h: EchoObject.xml
	dbus-binding-tool --mode=glib-server --output=$@ $^

EchoObjectBindings.h: EchoObject.xml
	dbus-binding-tool --mode=glib-client --output=$@ $^


clean:
	rm -f EchoObject EchoObjectClient EchoObjectGlue.h EchoObjectBindings.h

.PHONY: all clean
