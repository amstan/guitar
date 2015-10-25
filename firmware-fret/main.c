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
#include <libopencm3/stm32/tsc.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include "timer.c"

#define LED_RED    (GPIOC, GPIO6)
#define LED_BLUE   (GPIOC, GPIO7)
#define LED_ORANGE (GPIOC, GPIO8)
#define LED_GREEN  (GPIOC, GPIO9)

#define S1_PORT GPIOA
#define S1_P GPIO2 //TS_G1_IO3
#define S1_C GPIO3 //TS_G1_IO4

#define S2_PORT GPIOA
#define S2_P GPIO6 //TS_G2_IO3
#define S2_C GPIO7 //TS_G2_IO4

#define S3_PORT GPIOB
#define S3_P GPIO0 //TS_G3_IO2
#define S3_C GPIO1 //TS_G3_IO3

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	//Enable leds
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO7 | GPIO8 | GPIO9);
	gpio_clear(GPIOC, GPIO6 | GPIO7 | GPIO8 | GPIO9);
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

int read_touch(void) {

// 	gpio_clear(S2_PORT, S2_C);
// 	gpio_mode_setup(S2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, S2_C);
// 	msleep(50);
// 	gpio_mode_setup(S2_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, S2_C);

	gpio_mode_setup(S1_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, S1_C | S1_P);
	gpio_set_af(S1_PORT, GPIO_AF3, S1_C | S1_P);

	gpio_mode_setup(S2_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, S2_C | S2_P);
	gpio_set_af(S2_PORT, GPIO_AF3, S2_C | S2_P);

	gpio_mode_setup(S3_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, S3_C | S3_P);
	gpio_set_af(S3_PORT, GPIO_AF3, S3_C | S3_P);

	TSC_CR |= TSC_CR_START;
	while(TSC_CR & TSC_CR_START);

// 	while((TSC_IOGCSR & (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3))) != (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3)));

	return 0;
}

uint16_t history[1000][3];

int main(void)
{
	clock_setup();
	systick_setup();
	usart_setup();
	gpio_setup();

	uint16_t max[3];
	uint16_t value[3];

// 	for (uint16_t h = 0; h < 1000; h++) {
// 		for (uint8_t i = 0; i < 3; i++) {
// 			history[h][i] = 0
// 		}
// 	}

	gpio_set LED_GREEN;
// 	printf("\n\nGuitar Fret Discovery Board\n");
	msleep(100);

	rcc_periph_clock_enable(RCC_TSC);

	TSC_CR |= 0b1111 << TSC_CR_CTPH_SHIFT; //charge transfer pulse high
	TSC_CR |= 0b1111 << TSC_CR_CTPL_SHIFT; //charge transfer pulse low
	TSC_CR |= 0b1111111 << TSC_CR_SSD_SHIFT; //spread spectrum deviation
	TSC_CR |= 0b110 << TSC_CR_MCV_SHIFT; //max count value
	TSC_CR |= 0b000 << TSC_CR_PGPSC_SHIFT; //clock prescaler

	TSC_IOSCR |= TSC_IOSCR_G1(4) | TSC_IOSCR_G2(4) | TSC_IOSCR_G3(3); //sampling capacitors selection per group
	TSC_IOCCR |= TSC_IOCCR_G1(3) | TSC_IOCCR_G2(3) | TSC_IOCCR_G3(2); //channel selection per grou

	TSC_IOGCSR |= TSC_IOGCSR_GxE(1) | TSC_IOGCSR_GxE(2) | TSC_IOGCSR_GxE(3); //enable aquision on the channels
	//statuses: TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3);

	TSC_CR |= TSC_CR_TSCE; //enable touch sensing controller

	while (1) {
		uint8_t valid = 1;
		gpio_toggle LED_BLUE;

		read_touch();

		for (unsigned int i = 0; i < 3; i++) {
			value[i] = TSC_IOGxCR(i+1);
			history[(1000-1)][i] = value[i];

			max[i] = 0;
			for (uint16_t h = 0; h < (1000 - 1); h++) {
				if (max[i] < history[h][i])
					max[i] = history[h][i];

				history[h][i] = history[h+1][i];
			}

			max[0] = 3043;
			max[1] = 3452;
			max[2] = 3651;

			value[i] = max[i] - value[i];
			if(value[i] > 1000)
				valid = 0;
		}

		if(valid)
			printf("%5u %5u %5u\n", value[0]*170/100, value[1], value[2]);

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



// 		msleep(500);
	}

	return 0;
}
