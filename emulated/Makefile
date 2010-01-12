CC=gcc
CFLAGS=-c -Wall -I/usr/X11R6/include
LDFLAGS=-L/usr/X11R6/lib -lX11
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=emulated

all: $(SOURCES) $(EXECUTABLE)

run:
	./$(EXECUTABLE)

clean:
	rm -f ${EXECUTABLE} ${OBJECTS}

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@