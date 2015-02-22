struct guitarseq {
	jack_client_t *jack_client;

	jack_port_t *out_port;
	jack_ringbuffer_t *out_buffer;

	jack_port_t *in_port;
	jack_ringbuffer_t *in_buffer;
};

int process(jack_nframes_t nframes, void *arg);
