#ifndef TUNING_H
#define TUNING_H
#include "notes.h"
#include "main.h"

//! tuning notes for each string, note in number format
char tuning[NO_STRINGS];

/*! loads the tuning array from the file
 * @param file the file that contains the tuning
 * @return zero on success, non-zero on error
 */
int load_tuning(char *file);

#endif