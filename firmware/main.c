#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitop.h"

#include "uart.h"
#include "adc.h"

#include "main.h"


FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW); // This line is used to set up file access

int prepare_ports() {
	//String Touchpad
	DDRA  &= 0b11000000; //input direction
 	PORTA &= 0b11000000; //don't pullup these, they already are up
	
	if(STRINGS)
		warning("Strings Touchpad on: %b", STRINGS);
	
	
	//Option LEDs
	DDRB  |= 0b00001111; //output direction
	PORTB |= 0b00001111; //turn them on
	
	DDRC  |= 0b11000000; //output direction
	PORTC |= 0b11000000; //turn them on
	
	
	//Frets Outgoing
	PORTC |= 0b00110000; //turn the DEMUXES off(by setting ENABLE on)
	DDRC  |= 0b00111111; //output direction
	PORTC &=~0b00001111; //set the address to 0
	
	
	//Fret Incoming
	DDRD  &= ~FRETS_MASK; //input direction
	PORTD |= FRETS_MASK;  //pullup
	
	if(FRETS & FRETS_MASK)
		warning("Frets Incoming on: %b", FRETS & FRETS_MASK);
	
	return 1;
}

int send_capabilities() {
	guitarseq("c NO_STRINGS %d",NO_STRINGS);
	guitarseq("c NO_FRETS %d",NO_FRETS);
	guitarseq("c STRING_TYPE %s",STRING_TYPE);
	guitarseq("c FRET_TYPE %s",FRET_TYPE);
	guitarseq("c OLEDS %d",OLEDS);
	guitarseq("c WHAMMY %d",WHAMMY);
}

int main(void) {
	//Connect to uart
	uart_init();
	uart_str= FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
	stdout = stdin = &uart_str;
	debug("Connected to UART!");
	debug("Booting...");
	
	//Prepare ports
	prepare_ports();
	debug("Ports configured.");
	
	//Connect to guitarseq
	debug("Initiating contact with guitarseq (waiting for computer to send \\n)."); guitarseq("hello");
	scanf("\n");
	debug("Connected!\nSending capabilities...");
	send_capabilities();
	
	//Variables
	int i, j;
	
	//The states that are needed to keep track of
	char strings[NO_STRINGS];
	char frets[NO_STRINGS][NO_FRETS];
	char on,pressed;
	
	//Clean the memory
	for(i=0;i<NO_STRINGS;i++) {
		strings[i]=0;
		for(j=0;j<NO_FRETS;j++)
			frets[i][j]=0;
	}
	
	//Main loop
	debug("Starting main loop");
	while(1) {
		//Check the strings
		for(i=0;i<NO_STRINGS;i++) {
			on=test_bit(STRINGS,i)!=0; //either a 1 or 0
			if(on!=strings[i]) { //if strings are different than what's in the memory
				pressed=on?"p":"r";
				guitarseq("%cs%d",pressed,i);
				strings[i]=on;
			}
		}
		
		//Check the frets
		for(i=0;i<NO_FRETS;i++) {
			
		}
	}
}
