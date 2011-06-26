#ifndef COMM_H
#define COMM_H

/*! @file comm.h
 * Communication library for the mcu code.
 * Provides various functions to make the code look nicer.
 * @note All functions will automatically include \\n after them and fflush the stream
 */

///Sends user readable data, ignored by guitarseq
#define debug(...)                          printf(dbg,__VA_ARGS__); printf("\n"); fflush(stdout);

///Prepends a message with "Warning: "
#define warning(...)   printf("Warning: "); printf(dbg,__VA_ARGS__); printf("\n"); fflush(stdout);

///Prepends a message with "Error: "
#define warning(...)   printf("Error: ");   printf(dbg,__VA_ARGS__); printf("\n"); fflush(stdout);

///This text will be considered by guitarseq, so respect the protocol
#define guitarseq(...) printf("@");         printf(dbg,__VA_ARGS__); printf("\n"); fflush(stdout);

#endif