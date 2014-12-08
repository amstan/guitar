JACK_CFLAGS = $(shell pkg-config --cflags jack)
JACK_LIBS = $(shell pkg-config --libs jack)

JACK_INCLUDE_PATH = /usr/include/jack
JACK_INCLUDES = $(wildcard $(JACK_INCLUDE_PATH)/*.h)
JACK_BINDINGS = $(patsubst $(JACK_INCLUDE_PATH)/%.h,libjack/%.py,$(JACK_INCLUDES))

all: libjack libguitarseq.so

.PHONY: libjack
libjack: $(JACK_BINDINGS) libjack/__init__.py

libjack/%.py: $(JACK_INCLUDE_PATH)/%.h
	ctypesgen.py $(JACK_LIBS) $(JACK_CFLAGS) $< -o $@

libjack/__init__.py: $(JACK_BINDINGS) libjack/__init__.footer.py
	ls libjack/*.py|grep -v __|sed -e "s/libjack\/\(.*\).py/import \1/" | tee $@
	cat libjack/__init__.footer.py >> $@

CC = gcc
CFLAGS = $(JACK_CFLAGS) -O3 -Wall
LDFLAGS = $(JACK_LIBS)

libguitarseq.so: process.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fPIC $^ -o $@

.PHONY: clean
clean:
	find|grep *.pyc|xargs rm -rf
	find|grep pycache|xargs rm -rf
	rm -rf $(JACK_BINDINGS) libjack/__init__.py libjack/*.pyc
	rm -rf *.so

.PHONY: run
run: all
	python2 -i main.py