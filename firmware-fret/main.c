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

unsigned char registers[0x100];
/*
 * Map:
 * 0x00 : CHIP_ID "0x25"
 * 0x40-0x4c : 16 bit touch data
 * 0x80-0x91 : RGB LEDs values
 * 0x92 : Commit the RGB LEDs (write anything)
 *
 */
uint8_t *ws2812_colors = registers + 0X80;

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
		ws2812_sendarray(ws2812_colors,6*3);
	}
}

int main(void)
{
	clock_setup();
	systick_setup();
	gpio_setup();
	registers_init();

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

		for(unsigned int led = 0; led < 6; led++) {
			for(unsigned int c = 0; c < 3; c++) {
				ws2812_colors[led*3 + c] = rainbow[led][c] * touch_values[led] / 256;
			}
		}
		ws2812_sendarray(ws2812_colors,6*3);

		msleep(10);
	}

	return 0;
}
