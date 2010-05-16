CC=gcc
CFLAGS=-c -g -std=c99 `pkg-config --cflags jack`
LDFLAGS=`pkg-config --libs jack`
SOURCES=main.c jack.c args.c notes.c tuning.c chords.c config.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=guitarseq

all: $(SOURCES) $(EXECUTABLE)

run: all
	./$(EXECUTABLE)

clean:
	rm -f ${EXECUTABLE} ${OBJECTS}

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

doc:
	doxygen
clean-doc:
	rm -R doc/


#cool stuff:
runx: all
	./emulated/emulated|./$(EXECUTABLE) -o -s

test: all
	./$(EXECUTABLE)</dev/null