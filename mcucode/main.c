#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitop.h"

#include "uart.h"
#include "adc.h"

#include "main.h"


FILE uart_str;

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
}

int main(void)
{
	uart_init();
	uart_str= FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
	stdout = stdin = &uart_str;
	debug("Connected to UART!");
	
	prepare_ports();
	debug("Ports configured.");
	
	debug("Initiating contact with guitarseq.");
	printf("mcucode")
}
