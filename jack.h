#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#define MAX_EVENT_SIZE 10
#define RING_BUFFER_SIZE 10

jack_client_t *jack_client;
jack_port_t *output_port;
jack_ringbuffer_t *ringbuffer;

//functions
int jack_process(jack_nframes_t nframes, void *arg);
int jack_init();
int jack_midinote(int on, char note, char velocity, char channel);
int jack_stop();