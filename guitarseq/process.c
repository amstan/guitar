#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#include <string.h>
#include <stdio.h>

#include "guitarseq.cffi.h"

#define INFO(...)  {snprintf(guitarseq->logging_buffer, guitarseq->logging_buffer_size, __VA_ARGS__); guitarseq->info_callback(guitarseq->logging_buffer);}
#define ERROR(...) {snprintf(guitarseq->logging_buffer, guitarseq->logging_buffer_size, __VA_ARGS__); guitarseq->error_callback(guitarseq->logging_buffer);}

int process(jack_nframes_t nframes, void *arg) {
	struct guitarseq *guitarseq = arg;
	if(!guitarseq) {
		printf("No guitarseq instance!\n");
		return 1;
	}

	void* port_buf;
	jack_nframes_t now = jack_frame_time (guitarseq->jack_client);

	//Output
	port_buf = jack_port_get_buffer(guitarseq->out_port, nframes);
	jack_midi_clear_buffer(port_buf);
	while (1) {
		jack_nframes_t time;
		//TODO: Do a safer read, in case only part of the message is here
		if (!jack_ringbuffer_read (guitarseq->out_buffer, (char *)&time, sizeof(time))) {
			break;
		}

		// from the future?
		if (time >= now) {
			break;
		}

		// time it right
		jack_nframes_t offset = time - now + nframes - 1;

		// get the size of the event
		size_t size;
		jack_ringbuffer_read(guitarseq->out_buffer, (char *)&size, sizeof(size));

		INFO("out event at %d%+d size %zu\n", now, offset, size);

		if (offset > nframes)
			// from the past, somehow. cram it in at the front
			offset = 0;

		// proceed to giving it to jack
		jack_midi_data_t *buffer = jack_midi_event_reserve (port_buf, offset, size);
		if(buffer) {
			jack_ringbuffer_read(guitarseq->out_buffer, (char *)buffer, size);
		} else {
			// throw it away :( TODO: find more
			jack_ringbuffer_read_advance (guitarseq->out_buffer, size);
			ERROR("threw away MIDI event - no space reserved at time %d offset %d\n",time,offset);
		}
	}

	// 	Input
	port_buf = jack_port_get_buffer(guitarseq->in_port, nframes);
	jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
	for(jack_nframes_t i=0; i<event_count; i++) {
		jack_midi_event_t in_event;
		jack_midi_event_get(&in_event, port_buf, i);

		//adds a note to the ringbuffer
		if (jack_ringbuffer_write_space(guitarseq->in_buffer) >= sizeof(in_event.time)+sizeof(in_event.size)+in_event.size) {
			jack_ringbuffer_write(guitarseq->in_buffer, (char *)&in_event.time, sizeof(in_event.time));
			jack_ringbuffer_write(guitarseq->in_buffer, (char *)&in_event.size, sizeof(in_event.size));
			jack_ringbuffer_write(guitarseq->in_buffer, (char *)in_event.buffer, in_event.size);
		} else {
			ERROR("Couldn't write to ringbuffer at %d, %zu midi data bytes lost\n", in_event.time, in_event.size);
		}
	}

	return 0;
}
