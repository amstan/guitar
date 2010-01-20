#ifndef NOTES_H
#define NOTES_H

#define NO_MIDI_NOTES 128
#define NOTE_NAME_LEN 4
#define NOTE_FILE "etc/notes.cfg"

/*! The notes string array.
 * Use this to convert from number to note name
 */
char notes[NO_MIDI_NOTES][NOTE_NAME_LEN];

/*! Looks for a note by name
 * @param target_note the string to search
 * @return the id of the note $name
           with the condition that notes_look(notes[id])=id
 */
int notes_look(char *target_note);

/*! Loads the notes array from the file
 * @param file the file that contains the notes to load
 * @return zero on success, non-zero on error
 */
int notes_load(char *file);

/*! Translate notes to standard format
 * @param note the bad form note to translate
 * @note this function edits the argument
 * @return a new string for the note
 */
char *notes_translate(char *note);

#endif