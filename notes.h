#ifndef NOTES_H
#define NOTES_H

#define NO_MIDI_NOTES 128
#define NOTE_NAME_LEN 4
#define NOTE_FILE "etc/notes.cfg"

char notes[NO_MIDI_NOTES][NOTE_NAME_LEN];

//functions
int notes_look(char *target_note); //look_note
int notes_load(char *file); //load_notes

#endif