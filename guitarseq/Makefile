JACK_CFLAGS = $(shell pkg-config --cflags jack)
JACK_LIBS = $(shell pkg-config --libs jack)

.PHONY: all
all: libguitarseq.so

CC = gcc
CFLAGS = $(JACK_CFLAGS) -O3 -Wall -std=gnu99
LDFLAGS = $(JACK_LIBS)

LIBGUITARSEQ_FILES = process.c

libguitarseq.so: $(LIBGUITARSEQ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fPIC -g $^ -o $@

.PHONY: clean
clean:
	find|grep pyc$$|xargs rm -rf
	find|grep __pycache__|xargs rm -rf
	rm -rf *.so

.PHONY: run
run: all
	python -i guitarseq.py
