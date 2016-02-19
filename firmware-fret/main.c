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

int main(void)
{
	clock_setup();
	systick_setup();
	usart_setup();
	gpio_setup();

	gpio_set LED_GREEN;
	printf("\n\nGuitar Fret Discovery Board\n");
	msleep(100);

	while(1){
		printf(".\n");
		msleep(500);
	}

	return 0;
}
