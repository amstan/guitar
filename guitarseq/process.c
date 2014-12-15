#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#include <stdio.h>

jack_client_t *jack_client = NULL;
jack_port_t *output_port = NULL;
jack_ringbuffer_t *outgoing_ringbuffer = NULL;

#define MIDI_EVENT_SIZE 3

int process(jack_nframes_t nframes, void *arg) {
	if (!jack_client)
		return 0;
	if (!output_port)
		return 0;
	if (!outgoing_ringbuffer)
		return 0;
	
	jack_nframes_t time, offset;
	char size;
	unsigned char* buffer;
	
	void* port_buf = jack_port_get_buffer(output_port, nframes);
	jack_midi_clear_buffer(port_buf);
	
	jack_nframes_t now = jack_frame_time (jack_client);
	
// 	while (jack_ringbuffer_read (outgoing_ringbuffer, &size, 1)) {
// 		printf("%02x\n",size);
// 	}
	
	while (jack_ringbuffer_read (outgoing_ringbuffer, (char*)&time, sizeof(time))) {
// 		if (time >= now) { // from the future
// 			break;
// 		}
		
		// time it right
		offset = time - now + nframes - 1;
		
		// get the size of the event
		jack_ringbuffer_read(outgoing_ringbuffer, (char *)&size, sizeof(size));
		
// 		printf("event at %d%+d size %d\n", now, offset, size);
		
		if (offset > nframes)
			// from the past, somehow. cram it in at the front
			offset = 0;
		
		//proceed to giving it to jack
		buffer = jack_midi_event_reserve (port_buf, offset, size);
		if(buffer) {
			jack_ringbuffer_read (outgoing_ringbuffer, (char *)buffer, size);
		} else {
			// throw it away :( TODO: find more
			jack_ringbuffer_read_advance (outgoing_ringbuffer, size);
			printf("threw away MIDI event - no space reserved at time %d offset %d\n",time,offset);
		}
	}
	
	return 0;
}
