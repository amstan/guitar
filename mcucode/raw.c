#include <ctype.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>

#include "uart.h"
#include "adc.h"

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW); // This line is used to set up file access


char bol_state(int port)
{
	return (PORTB&(1<<port))>0;
}

char bol_stateE(int port)
{
	return (PORTE&(1<<port))>0;
}

void toggle(int port)
{
	
	if(bol_state(port))
	{
		PORTB&=~(1<<port);
	}
	else
	{
		PORTB|=1<<port;
	}	
}

void clear(void)
{
	int i;
	for(i=0;i<20;i++)
	{
		printf("\n");
	}
}

char *state(int port)
{
	if(bol_state(port)) return "yes";
	else return "no";
}

double voltage(void)
{
	int sensor;
	ADC_init(7);
	sensor=ADC_read();
	
	return 0.019004975*(double)sensor-3.35;
}

void printfloat(double x)
{
	printf("%d.%d",(int)x,(int)(x*100)-((int)x)*100);
}

/*int percentage(double x) //x is voltage
{
	char states;
	states=bol_state(PB3)*4+bol_state(PB2)*2+bol_state(PB1);
	//states bits: AC, Solar, LEDs
	if(states==0b101) return (int)(-0.0065-(0.0501*x)-(0.331*x*x)-(1.4277*x*x*x)+(0.2104*x*x*x*x)-(0.0074*x*x*x*x*x)); 	//Leds and AC
	if(states==0b001) return (int)(0.0095*x*x*x*x*x)-(0.1911*x*x*x*x)+(0.9264*x*x*x)+(0.2416*x*x)+(0.0417*x)+0.0059;	//Leds Only
	if(states==0b100) return (int)(-0.0059-(0.0482*x)-(0.33*x*x)-(1.492*x*x*x)+(0.2173*x*x*x*x)-(0.0076*x*x*x*x*x));	//AC only
	return (int)((0.0039*x*x*x*x*x)-(0.0447*x*x*x*x)-(0.0073*x*x*x)-(0.0009004*x*x)-(0.000098248*x)-(0.000010044));	//Nothing On
}*/

int main(void)
{
	uart_init(); // Initializes the UART for 19200 baud
	
	stdout = stdin = &uart_str;  // Set standard input and output streams
	//b1, b2, b3=led,solar,ac
	//buttons b6, b7, b4 center, e2, e3
	DDRB =0b00001110;
	PORTB=0b11010000;
	PORTE=0b00001100;
	
	int choice;
	
	clear();
	
	printf("===LED Lighting System===\n");
	printf(" made by Alex and Andrew\n");
	printf("=========================\n");
	printf("\n");
	

menu:
	printf("=System State=\n");
	printf("Battery Voltage: "); printfloat(voltage()); printf("V\n");
	printf("LEDs: %s\n",state(PB1));
	printf("AC charger: %s\n",state(PB3));
	printf("Solar panels: %s\n",state(PB2));
		
	printf("\n");
	
	printf("==Main Menu==\n");
	printf("0. Refresh\n");
	printf("1. Toggle LEDs\n");
	printf("2. Toggle AC charger\n");
	printf("3. Toggle Solar panels\n");
	printf("4. Continiously Monitor Battery State\n");
	printf("9. Print Credits\n");
	printf("Pick an option: ");
	scanf("%d", &choice);
	switch(choice)
	{
		case 0:
			break;
			
		case 1:
			toggle(PB1);
			break;
		
		case 2:
			toggle(PB3);
			break;
		
		case 3:
			toggle(PB2);
			break;
		
		case 4:
			printf("\n=Battery Monitor=\n");
			printf("Press enter to refresh, press 0 to exit.\n");
			printf("Voltage\n");
			int i;
			double volts;
			int n=3;
			if(bol_state(PB3)) n=16; //charger on
			do
			{
				volts=0;
				for(i=0;i<n;i++)
				{
					volts+=voltage();
					_delay_ms(250);
				}
				volts/=n;
				printfloat(volts);
				printf("V");
			}
			while(uart_getchar(stdin)!='0');
			clear();
			break;
						
		case 9:
			clear();
			printf("===LED Lighting System===\n");
			printf("Design: Alex Stan & Andrew Rossignol\n");
			printf("Hardware: Alex Stan & Andrew Rossignol\n");
			printf("Theory: Andrew Rossignol\n");
			printf("Programming: Alex Stan\n");
			printf("Thanks to LEDs and AVR Butterfly Demo Boards!\n");
			printf("\n\n");
			
			uart_getchar(stdin);
			clear();
			
			goto menu;
			
			break;
			
		default:
			printf("Invalid choice! Try again.\n\n\n");
			goto menu;
	}
	clear();
	goto menu;
}
