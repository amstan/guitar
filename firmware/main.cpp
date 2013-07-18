#include <msp430.h>
#include <stdint.h>
#include "bitop.h"

void chip_init(void) {
	WDTCTL = WDTPW + WDTHOLD; //Stop WDT
	
	BCSCTL1 = CALBC1_16MHZ; //set DCO clock for MCLK and SMCLK
	DCOCTL = CALDCO_16MHZ;
	
	__bis_SR_register(GIE); //interrupts enabled
	
	#define LED_RGB 0
	set_bit(P1DIR,LED_RGB);
	
	#define LED_2 6
	set_bit(P1DIR,LED_2);
}

void write_ws2811_hs(uint8_t *data, uint16_t length, uint8_t pinmask);

int main(void) {
	chip_init();
	
	uint8_t data[]={255,255,255};
	uint16_t length=3;
	write_ws2811_hs(data,length,1<<LED_RGB);
	
	set_bit(P1OUT,LED_2);
	
	while(1);
}