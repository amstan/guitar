#ifndef CONFIG_H
#define CONFIG_H

/*! @file config.h
 * Configuration file processing library.
 * This library is meant to be short and simple for parsing ini style files.
 * The files should consist of "key = value" pairs, though "key value" and "key : value" pairs, as long as the key has a whitespace after
 * Lines starting with '#' are ignored
 */

//!configuration names
char config_name[100][256];

//!configuration values
char config_value[100][1024];

//!number of configuration entries
int config_n;

/*! Initializes the configuration(reads the file)
 * @param file file to read the conf from
 * @return zero on success, non-zero on error
 */
int config_init(char *file);

/*! Search the configuration for a key
 * @param name name of the configuration key
 * @return the value of the configuration
 */
char *config_look(char *name);

#endif