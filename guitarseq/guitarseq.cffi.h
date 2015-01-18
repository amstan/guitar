struct guitarseq {
	jack_client_t *jack_client;

	char *logging_buffer;
	size_t logging_buffer_size;
	//These will be slow, they're python
	void (* error_callback)(const char *msg);
	void (* info_callback)(const char *msg);

	jack_port_t *out_port;
	jack_ringbuffer_t *out_buffer;

	jack_port_t *in_port;
	jack_ringbuffer_t *in_buffer;
};

int process(jack_nframes_t nframes, void *arg);
