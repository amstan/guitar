#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

//!The midi event size
#define MAX_EVENT_SIZE 10

//!the ringbuffer capacity, # of events in it
#define RING_BUFFER_SIZE 10


jack_client_t *jack_client;
jack_port_t *output_port;
jack_ringbuffer_t *ringbuffer;


//functions
/*! The process function that jack executes in another thread for sending the data
 * @param nframes the number of frames to be processed
 * @return zero on success, non-zero on error
 */
int jack_process(jack_nframes_t nframes, void *arg);


/*! Initializes the jack connection, including the ringbuffer
 * @param name the name for the jack_init
 * @param connect_output a regex to find a port to connect the output to. ex: jack_keyboard to connect to jack keyboard
 * @return zero on success, non-zero on error
 */
int jack_init(char *name, char *connect_output);


/*! Sends a midi note to jack
 * @param on note status, 1 if press, 0 if release
 * @param note the 7-bit note
 * @param velocity the velocity to press the note
 * @param channel the midi channel to send the note
 * @return zero on success, non-zero on error
 */
int jack_midinote(int on, char note, char velocity, char channel);


/*! Stop Jack
 * @return zero on success, non-zero on erro
 */
int jack_stop();