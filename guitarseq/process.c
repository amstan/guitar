#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#include <string.h>
#include <stdio.h>

#include "guitarseq.cffi.h"

struct __attribute__((__packed__)) midi_event_header {
	jack_nframes_t time;
	size_t size;
};

#define INFO(...)  {fprintf(stdout, __VA_ARGS__); fflush(stdout);}
#define ERROR(...) {fprintf(stderr, __VA_ARGS__); fflush(stderr);}

int process(jack_nframes_t nframes, void *arg) {
	struct guitarseq *guitarseq = arg;
	if (!guitarseq) {
		fprintf(stderr, "No guitarseq instance!\n");
		return 1;
	}

	void* port_buf;
	jack_nframes_t now = jack_frame_time (guitarseq->jack_client);

	//Output
	port_buf = jack_port_get_buffer(guitarseq->out_port, nframes);
	jack_midi_clear_buffer(port_buf);
	while (1) {
		struct midi_event_header meh;

		if (jack_ringbuffer_peek(guitarseq->out_buffer, (char *)&meh, sizeof(meh)) != sizeof(meh)) {
			/* we don't have the event ready in there yet */
			break;
		}

		if (jack_ringbuffer_read_space(guitarseq->out_buffer) < sizeof(meh) + meh.size) {
			/* we still don't have the event ready yet */
			break;
		}

		// from the future?
		if (meh.time >= now) {
			break;
		}

		// skip the header now
		jack_ringbuffer_read_advance(guitarseq->out_buffer, sizeof(meh));

		// time it right
		jack_nframes_t offset = meh.time - now + nframes - 1;

		INFO("out event at %u%+d size %zu\n", now, offset, meh.size);

		if (offset > nframes)
			// from the past, somehow. cram it in at the front
			offset = 0;

		// proceed to giving it to jack
		jack_midi_data_t *buffer = jack_midi_event_reserve(port_buf, offset, meh.size);
		if (buffer) {
			jack_ringbuffer_read(guitarseq->out_buffer, (char *)buffer, meh.size);
		} else {
			// throw it away :( TODO: find more
			jack_ringbuffer_read_advance (guitarseq->out_buffer, meh.size);
			ERROR("threw away MIDI event - no space reserved at time %u offset %d\n", meh.time, offset);
		}
	}

	// Input
	port_buf = jack_port_get_buffer(guitarseq->in_port, nframes);
	jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
	for (jack_nframes_t i=0; i<event_count; i++) {
		jack_midi_event_t in_event;
		jack_midi_event_get(&in_event, port_buf, i);

		// adds a note to the ringbuffer
		if (jack_ringbuffer_write_space(guitarseq->in_buffer) >= sizeof(in_event.time)+sizeof(in_event.size)+in_event.size) {
			jack_ringbuffer_write(guitarseq->in_buffer, (char *)&in_event.time, sizeof(in_event.time));
			jack_ringbuffer_write(guitarseq->in_buffer, (char *)&in_event.size, sizeof(in_event.size));
			jack_ringbuffer_write(guitarseq->in_buffer, (char *)in_event.buffer, in_event.size);
		} else {
			ERROR("Couldn't write to ringbuffer at %u, %zu midi data bytes lost\n", in_event.time, in_event.size);
		}
	}

	return 0;
}
