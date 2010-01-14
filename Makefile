CC=gcc
CFLAGS=-c -std=c99 `pkg-config --cflags jack`
LDFLAGS=`pkg-config --libs jack`
SOURCES=main.c jack.c args.c notes.c tuning.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=guitarseq

all: doc $(SOURCES) $(EXECUTABLE)

run: all
	./$(EXECUTABLE)

clean:
	rm -f ${EXECUTABLE} ${OBJECTS}
	rm -R doc/

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

doc: 
	doxygen


#cool stuff:
runx: all
	./emulated/emulated|./$(EXECUTABLE) -o