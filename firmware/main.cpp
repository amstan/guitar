#include <msp430.h>
#include <legacymsp430.h>
#include <stdint.h>
#include "config.h"
#include "ringbuffer.h"
#include "usci_serial.h"

#include "bitop.h"

ringbuffer_ui8_16 usci_buffer = { 0, 0, { 0 } };
Serial<ringbuffer_ui8_16> usci0 = { usci_buffer };

interrupt(USCIAB0RX_VECTOR) USCI0RX_ISR(void) {
    usci_buffer.push_back(UCA0RXBUF);
}

void chip_init(void) {
	WDTCTL = WDTPW + WDTHOLD; //Stop WDT
	
	BCSCTL1 = CALBC1_16MHZ; //set DCO clock for MCLK and SMCLK
	DCOCTL = CALDCO_16MHZ;
	
	usci0.init();
	usci0.xmit("\nGuitar touch testing program.\n");
	
	__bis_SR_register(GIE); //interrupts enabled
	
	#define BIG 0
	//P2REN=(1<<BIG);
	
	#define SWITCH 3
	#define switch_pressed() (!test_bit(P1IN,SWITCH))
	P1OUT=(1<<SWITCH);
	P1REN=(1<<SWITCH);
}

int main(void) {
	chip_init();
	
	while(1) {
		for(unsigned int i=0;i<60;i++) {
			while(test_bit(P2IN,BIG));
			while(!test_bit(P2IN,BIG));
		}
		usci0.xmit('*');
	}
}
