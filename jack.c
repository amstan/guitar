#include "jack.h"

int jack_process(jack_nframes_t nframes, void *arg) {
	jack_nframes_t time, offset;
	char size;
	unsigned char* buffer;
	
	void* port_buf = jack_port_get_buffer(output_port, nframes);	
	jack_midi_clear_buffer(port_buf);
	
	jack_nframes_t now = jack_frame_time (jack_client);
	
	while (jack_ringbuffer_peek (ringbuffer, (char *)&time, sizeof(time))) {
		if (time >= now) { //from the future
			break;
		}
		jack_ringbuffer_read_advance (ringbuffer, sizeof (time));
		
		//time it right
		offset = now - time;
		if(offset>nframes) {
			/* from the past, somehow. cram it in at the front */
			offset = 0;
		} else {
			offset = nframes - offset;
		}
		
		//get the size of the event
		jack_ringbuffer_read(ringbuffer, (char *)&size, sizeof(size));
		
		//fprintf(stdout,"event at %d/%d offset %d size %d\n",now,time,offset,size);
		
		//proceed to giving it to jack
		buffer = jack_midi_event_reserve (port_buf, offset, size);
		if(buffer) {
			jack_ringbuffer_read (ringbuffer, (char *)buffer, size);
		} else {
			/*throw it away :( TODO: find more*/
			jack_ringbuffer_read_advance (ringbuffer, size);
			fprintf(stderr,"threw away MIDI event - no space reserved at time %d offset %d\n",time,offset);
		}
	}
	
	return 0;
}

int jack_init() {
	//connect to jack
	if((jack_client = jack_client_new ("guitarseq")) == 0) { //TODO: change this
		fprintf (stderr, "Jack server not running?\n");
		return 1;
	}
	
	//jack port settings
	jack_set_process_callback (jack_client, jack_process, 0);
	output_port = jack_port_register (jack_client, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0); //TODO: set proper terminal port output
	//nframes = jack_get_buffer_size(jack_client); //TODO: take this out if not needed
	
	//activate
	if (jack_activate(jack_client)) {
		fprintf (stderr, "Cannot activate jack client.\n");
		return 1;
	}
	fprintf(stdout,"Jack connection started!\n");
	
	
	//initialize ringbuffer
	ringbuffer = jack_ringbuffer_create(MAX_EVENT_SIZE*RING_BUFFER_SIZE);
	if (!ringbuffer) {
		fprintf(stderr,"Could not initialize ringbuffer.\n");
		return 1;
	}
	jack_ringbuffer_reset(ringbuffer);
	fprintf(stdout,"Ringbuffer initialized!\n");
	
	return 0;
}

int jack_stop() {
	jack_ringbuffer_free(ringbuffer);
	jack_client_close(jack_client);
	
	return 0;
}