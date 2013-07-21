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

void printint(int i) {
	if(i<0) {
		i*=-1;
		usci0.xmit('-');
	}
	for(unsigned int digit=10000;digit!=0;digit/=10) {
		usci0.xmit('0'+((i/digit)%10));
	}
}

void chip_init(void) {
	WDTCTL = WDTPW + WDTHOLD; //Stop WDT
	
	BCSCTL1 = CALBC1_16MHZ; //set DCO clock for MCLK and SMCLK
	DCOCTL = CALDCO_16MHZ;
	
	usci0.init();
	usci0.xmit("\nGuitar touch testing program.\n");
	
	__bis_SR_register(GIE); //interrupts enabled
	
	#define LED_CENTER 0
	set_bit(P1DIR,LED_CENTER);
	
	#define LED_RGB 0
	set_bit(P1DIR,LED_RGB);
	
	#define LED_2 6
	set_bit(P1DIR,LED_2);
}

void printint(unsigned int i) {
	for(unsigned int digit=10000;digit!=0;digit/=10) {
		usci0.xmit('0'+((i/digit)%10));
	}
}

//Adapted from TI_CTS_RO_PINOSC_TA0_WDTp_HAL from http://www.ti.com/tool/capsenselibrary
uint16_t read_touch(unsigned int pin) {
	uint16_t count;
	//** Setup Measurement timer***************************************************
	// Choices are TA0,TA1,TB0,TB1,TD0,TD1 these choices are pushed up into the
	// capacitive touch layer.
	
	// Configure and Start Timer
	TA0CTL = TASSEL_3+MC_2; //INCLK, cont mode
	TA0CCTL1 = CM_3+CCIS_2+CAP; //Pos&Neg,GND,Cap
	IE1 |= WDTIE; //enable WDT interrupt
	//for (uint8_t i = 0; i<(group->numElements); i++)
	//{
		// Configure Ports for relaxation oscillator
// 		*((group->arrayPtr[i])->inputPxselRegister) &= ~((group->arrayPtr[i])->inputBits);
// 		*((group->arrayPtr[i])->inputPxsel2Register) |= ((group->arrayPtr[i])->inputBits);
		clear_bit(P2SEL,pin);
		set_bit(P2SEL2,pin);
		
		//**  Setup Gate Timer ********************************************************
		// Set duration of sensor measurment
		WDTCTL = WDT_MDLY_32;
		TA0CTL |= TACLR;                     // Clear Timer_A TAR
		__bis_SR_register(LPM0_bits+GIE);   // Wait for WDT interrupt
		count = TA0CCR1;                 // Save result
		WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
	//}
	// End Sequence
	//reset port
	clear_bit(P2SEL,pin);
	clear_bit(P2SEL2,pin);
	return count;
}

extern "C" {
	void write_ws2811_hs(uint8_t *data, uint16_t length, uint8_t pinmask);
}

int main(void) {
	chip_init();
	
	#define G 0
	#define R 1
	#define B 2
	uint8_t data[]={0x00,0x00,0x00};
	uint16_t length=3;
	
	unsigned char offset=0;
	while(1) {
		#define PROXIMITY 0
		#define LEFT 1
		#define DOWN 2
		#define RIGHT 3
		#define UP 4
		#define CENTER 5
		for(unsigned int i=0;i<6;i++) {
			int value=read_touch(i);
			switch(i) {
				case LEFT:
					value=-(value-3680);
					value+=offset;
					if(value<0) value=0;
					if(value>255) value=255;
					data[R]=value;
					break;
				
				case UP:
					value=-(value-3450);
					value+=offset;
					if(value<0) value=0;
					if(value>255) value=255;
					data[G]=value;
					break;
				
				case RIGHT:
					value=-(value-3700);
					value+=offset;
					if(value<0) value=0;
					if(value>255) value=255;
					data[B]=value;
					break;
				
				case DOWN:
					value=-(value-3720);
					if(value<0) value=0;
					if(value>255) value=255;
					offset=value;
					break;
			}
			printint(value);
			usci0.xmit(" ");
		}
		usci0.xmit("\n");
		write_ws2811_hs(data,length,1<<LED_RGB);
	}
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void) {
	TA0CCTL1 ^= CCIS0;                   // Create SW capture of CCR1
	__bic_SR_register_on_exit(LPM3_bits);           // Exit LPM3 on reti
}