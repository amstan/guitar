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

#include <string.h>

#include "timer.c"
#include "i2c_slave.c"
#include "ws2812.c"
#include "touch.c"

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	//Enable leds
// 	rcc_periph_clock_enable(RCC_GPIOC);
// 	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO7 | GPIO8 | GPIO9);
// 	gpio_clear(GPIOC, GPIO6 | GPIO7 | GPIO8 | GPIO9);

	//ws2812
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
}

volatile uint8_t demo_mode = 1;

#define CHIP_ID 0x25

void handle_i2c_bufs(void) {
	uint8_t command = i2c_rx_buf[0];
	uint8_t *args = i2c_rx_buf + 1;

	uint32_t *location;
	uint8_t length;
	uint32_t *data;

	switch (command) {
		/* Meta */
		/* CHIP_ID */
		case 0x00:
			i2c_tx_buf[0] = CHIP_ID;
			break;

		/* Version */
		case 0x01:
			memcpy(i2c_tx_buf, GIT_HASH, strlen(GIT_HASH));
			break;

		/* COMPILE_TIME */
		case 0x02:
			memcpy(i2c_tx_buf, COMPILE_TIME, strlen(COMPILE_TIME));
			break;

		/* Touch */
		case 0x10:
			memcpy(i2c_tx_buf, touch_values, sizeof(touch_values));
			break;

		/* LEDs */
		case 0x20:
			ws2812_sendarray(args,6*3);
			memcpy(i2c_tx_buf, args, 6*3);
			break;

		/* Misc */
		case 0xe0:
			demo_mode = args[0];
			break;

		/* Memory and programming */
		/* Read */
		case 0xf0:
			location = ((uint32_t *)args)[0];
			length = ((uint32_t *)args)[1];
			memcpy(i2c_tx_buf, location, length);
			break;

		/* Write */
		case 0xf1:
			location = ((uint32_t *)args)[0];
			length = ((uint32_t *)args)[1];
			data = &((uint32_t *)args)[2];
			memcpy(location, *data, length);
			break;
	}
}

int main(void)
{
	clock_setup();
	systick_setup();
	gpio_setup();
	touch_init();

	uint8_t ws2812_colors[6*3] = {0};
	msleep(100);
	ws2812_sendarray(ws2812_colors,6*3);
	ws2812_colors[0] = 255;
	ws2812_colors[1] = 128;
	ws2812_colors[2] = 0;
	ws2812_sendarray(ws2812_colors,6*3);
	msleep(100);
	ws2812_colors[0] = 255;
	ws2812_colors[1] = 0;
	ws2812_colors[2] = 0;
	ws2812_sendarray(ws2812_colors,6*3);

	touch_init();

	i2c_setup();
	msleep(100);
	ws2812_colors[0] = 0;
	ws2812_colors[1] = 0;
	ws2812_colors[2] = 0;
	ws2812_sendarray(ws2812_colors,6*3);

	uint8_t rainbow[6][3] = {
		{255,0,0},
		{192,192,0},
		{0,255,0},
		{0,192,192},
		{0,0,255},
		{192,0,192},
	};

	while(1){
		touch_read();

		if (demo_mode) {
			for(unsigned int led = 0; led < 6; led++) {
				for(unsigned int c = 0; c < 3; c++) {
					uint8_t new_value = rainbow[led][c] * touch_values[led] / 256;
					uint8_t step = 7;
					if (ws2812_colors[led*3 + c] > 2) {
						ws2812_colors[led*3 + c] = ((uint16_t)ws2812_colors[led*3 + c]) * 85 / 100;
					} else
						ws2812_colors[led*3 + c] = 0;
					if (ws2812_colors[led*3 + c] < new_value)
						ws2812_colors[led*3 + c] = new_value;
				}
			}
			ws2812_sendarray(ws2812_colors,6*3);
		}

		msleep(10);
	}

	return 0;
}
