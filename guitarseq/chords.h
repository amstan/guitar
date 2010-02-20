#ifndef CHORDS_H
#define CHORDS_H
#include "notes.h"
#include "main.h"


//! the maximum length of a chord name
#define CHORD_NAME_LEN 20

//! the maximum number of chords
#define MAX_CHORDS 1000


//! chord names array
char chord_name[MAX_CHORDS][CHORD_NAME_LEN];

//! chord array which contains the individual notes for the chord
char chord[MAX_CHORDS][NO_STRINGS];

//! number of chords
int chord_n;

//! chord mappings for each fret
int chord_mappings[NO_STRINGS][NO_FRETS];


/*! Looks for a chord by name
* @param name chord name to search
* @return the id of the chord in the chord_name array, such that chord_look(chord_name[x])=x
*         returns -1 if chord was not found
*/
int chords_look(char *name); //look_chord


/*! Loads the chords from the file to the chord_name and chord array
* @param file file to load the chords from
* @return zero on success, non-zero on error
*/
int chords_load(char *file); //load_chords


/*! Loads the chord mappings from the file to the chord_mappings array
* @param file file to load the chord mappings from
* @return zero on success, non-zero on error
*/
int chords_load_mappings(char *file); //load_chord_mappings

#endif