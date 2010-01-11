CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c jack.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=guitarseq

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@