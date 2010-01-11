#include <stdio.h>

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#define MAX_EVENT_SIZE 10
#define RING_BUFFER_SIZE 10

jack_client_t *jack_client;
jack_port_t *output_port;
jack_ringbuffer_t *ringbuffer;
