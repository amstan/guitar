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
#include <string.h>

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

#define CHIP_ID 0x25

void handle_i2c_bufs(void) {
	uint8_t command = i2c_rx_buf[0];
	uint8_t *args = i2c_rx_buf + 1;

	switch (command) {
		case 0x00:
			i2c_tx_buf[0] = CHIP_ID;
			break;

		case 0x01:
			memcpy(i2c_tx_buf, GIT_HASH, strlen(GIT_HASH));
			break;

		case 0x02:
			memcpy(i2c_tx_buf, COMPILE_TIME, strlen(COMPILE_TIME));
			break;

		case 0x10:
			touch_read();
			printf("%5u %5u %5u\n", value[0], value[1], value[2]);
			((uint16_t*)i2c_tx_buf)[0] = value[0];
			((uint16_t*)i2c_tx_buf)[1] = value[1];
			((uint16_t*)i2c_tx_buf)[2] = value[2];
			break;

		case 0x20:
			ws2812_sendarray(i2c_rx_buf,6*3);
			break;
	}
}

int main(void)
{
	clock_setup();
	systick_setup();
	usart_setup();
	gpio_setup();
	touch_init();

	gpio_set LED_GREEN;
	printf("\n\nGuitar Fret Discovery Board\n");
	printf("Version %s\n", GIT_HASH);
	printf("Compiled %s\n", COMPILE_TIME);
	msleep(100);

	i2c_setup();

	ws2812_sendarray(ws2812_colors,6*3);

	i2c_tx_buf[0] = 0xaa;
	i2c_tx_buf[1] = 0x55;

	while(1){
		printf(".");

// 		touch_read();
// 		printf("%5u %5u %5u\n", value[0], value[1], value[2]);
//
// 		if ((value[0]*140/100)>140)
// 			gpio_set LED_RED;
// 		else
// 			gpio_clear LED_RED;
//
// 		if (value[1]>140)
// 			gpio_set LED_ORANGE;
// 		else
// 			gpio_clear LED_ORANGE;
//
// 		if (value[2]>140)
// 			gpio_set LED_GREEN;
// 		else
// 			gpio_clear LED_GREEN;

		msleep(100);
	}

	return 0;
}
