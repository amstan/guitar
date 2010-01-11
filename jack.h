#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

jack_client_t *jack_client;
jack_port_t *output_port;
jack_ringbuffer_t *ringbuffer;
