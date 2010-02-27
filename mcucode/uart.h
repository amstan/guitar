#ifndef UART_H
#define UART_H

/*! @file uart.h
 * UART Communication Library
 *
 * @author Joerg Wunsch
 *
 * @par License:
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you can do whatever you want with this stuff. If we meet some day, and you think this stuff is worth it, you can buy me a beer in return.
 *
 */

/*! Perform UART startup initialization.
 * Initialize the UART to 9600 Bd, tx/rx, 8N1.
 */
void uart_init(void);

/*! Send character c down the UART Tx
 * Wait until tx holding register is empty.
 * @param c character to send
 * @param stream file stream to putchar on
 * @returns 
 */
int uart_putchar(char c, FILE *stream);

/*
 * Size of internal line buffer used by uart_getchar().
 */
#define RX_BUFSIZE 80

/*!
 * Receive a character from the UART Rx.
 *
 * This features a simple line-editor that allows to delete and re-edit the characters entered, until either CR or NL is entered. Printable characters entered will be echoed using uart_putchar().
 *
 * @par Editing characters:
 * - \b (BS) or \177 (DEL) delete the previous character
 * - ^u kills the entire input buffer
 * - ^w deletes the previous word
 * - ^r sends a CR, and then reprints the buffer
 * - \t will be replaced by a single space
 *
 * All other control characters will be ignored.
 *
 * The internal line buffer is RX_BUFSIZE (80) characters long, which includes the terminating \\n (but no terminating \0).  If the buffer is full (i. e., at RX_BUFSIZE-1 characters in order to keep space for the trailing \\n), any further input attempts will send a \\a to uart_putchar() (BEL character), although line editing is still allowed.
 *
 * Input errors while talking to the UART will cause an immediate return of -1 (error indication).  Notably, this will be caused by a framing error (e. g. serial line "break" condition), by an input overrun, and by a parity error (if parity was enabled and automatic parity recognition is supported by hardware).
 *
 * Successive calls to uart_getchar() will be satisfied from the internal buffer until that buffer is emptied again.
 */
int uart_getchar(FILE *stream);

#endif