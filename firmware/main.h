#ifndef MAIN_H
#define MAIN_H

/*! @file main.h
 * Main header file
 */

//Guitar capabilities
#define NO_STRINGS  6
#define NO_FRETS    23
#define STRING_TYPE "digital"
#define FRET_TYPE   "digital"
#define OLEDS       2
#define WHAMMY      0

#define STRINGS    PINA

#define FRETS      PIND
#define FRETS_MASK 0b11111100

#define OLED1(R,G,B) change_bit()

#endif