/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2012 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#define LED_RED    (GPIOC, GPIO6)
#define LED_BLUE   (GPIOC, GPIO7)
#define LED_ORANGE (GPIOC, GPIO8)
#define LED_GREEN  (GPIOC, GPIO9)

#include <stdlib.h>
#include "timer.c"
#include "i2c_slave.c"
#include "ws2812.c"
#include "touch.c"

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	//Enable leds
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO7 | GPIO8 | GPIO9);
	gpio_clear(GPIOC, GPIO6 | GPIO7 | GPIO8 | GPIO9);

	//ws2812
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
}

int _write(int file, char *ptr, int len)
{
	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		int i;
		for (i = 0; i < len; i++) {
			usart_send_blocking(USART1, *ptr++);
		}
		return i;
	}
	errno = EIO;
	return -1;
}

void usart_setup(void)
{
	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Setup GPIO pins for USART2 transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	/* Setup TX pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF1, GPIO9);

	/* Setup parameters. */
	usart_set_baudrate(USART1, 1000000);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_CR2_STOP_1_0BIT);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);
}

unsigned char registers[0x100];
/*
 * Map:
 * 0x00 : CHIP_ID "0x25"
 * 0x40-0x4c : 16 bit touch data
 * 0x80-0x91 : RGB LEDs values
 * 0x92 : Commit the RGB LEDs (write anything)
 *
 */
uint8_t ws2812_colors[] = {
	255,0,0,
	255,0,0,
	255,0,0,
	255,0,0,
	200,200,0,
	200,200,0,
	200,200,0,
	200,200,0,
	0,255,0,
	0,255,0,
	0,255,0,
	0,255,0,
	0,50,255,
	0,50,255,
	0,50,255,
	0,50,255,
	255,0,0,
	255,0,0,
	255,0,0,
	255,0,0,
	200,200,0,
	200,200,0,
	200,200,0,
	200,200,0,
	0,255,0,
	0,255,0,
	0,255,0,
	0,255,0,
	0,50,255,
	0,50,255,
	0,50,255,
	0,50,255,
};

#define CHIP_ID 0x25

void registers_init(void) {
	registers[0x00] = CHIP_ID;
}

void registers_read_callback(uint16_t address) {
	//printf("r 0x%04x==0x%02x\n", address, registers[address]);
}
void registers_write_callback(uint16_t address) {
// 	printf("w 0x%04x:0x%02x\n", address, registers[address]);
	if (address == 0x92) {
// 		printf("set leds");
		ws2812_sendarray(ws2812_colors,16*3);
	}
}

int main(void)
{
	clock_setup();
	systick_setup();
	usart_setup();
	gpio_setup();
	registers_init();
	touch_init();

	gpio_set LED_GREEN;
	printf("\n\nGuitar Fret Discovery Board\n");
	printf("Compiled %s\n", COMPILE_TIME);
	printf("Version %s\n", GIT_HASH);
	msleep(100);

	i2c_setup();

	for(unsigned int i=0;i<sizeof(ws2812_colors);i++) {
		ws2812_colors[i]/=8;
	}

	while(1) {
		for(unsigned int i=0;i<16;i++) {
			ws2812_sendarray(ws2812_colors+i*3,16*3);
			msleep(100);
		}
	}

	while(1){
		printf(".");

		touch_read();
		printf("%5u %5u %5u\n", value[0], value[1], value[2]);
		*((uint16_t *) (registers + 0x40)) = value[0];
		*((uint16_t *) (registers + 0x42)) = value[1];
		*((uint16_t *) (registers + 0x44)) = value[2];

		if ((value[0]*140/100)>140)
			gpio_set LED_RED;
		else
			gpio_clear LED_RED;

		if (value[1]>140)
			gpio_set LED_ORANGE;
		else
			gpio_clear LED_ORANGE;

		if (value[2]>140)
			gpio_set LED_GREEN;
		else
			gpio_clear LED_GREEN;

		msleep(10);
	}

	return 0;
}
