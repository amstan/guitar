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

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/tsc.h>
#include <libopencm3/stm32/usart.h>
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

static void i2c_setup(void)
{
	rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_set_i2c_clock_hsi(I2C1);

	i2c_reset(I2C1);
	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6| GPIO7);
	i2c_peripheral_disable(I2C1);
	//configure ANFOFF DNF[3:0] in CR1
	i2c_enable_analog_filter(I2C1);
	i2c_set_digital_filter(I2C1, I2C_CR1_DNF_DISABLED);
	//Configure PRESC[3:0] SDADEL[3:0] SCLDEL[3:0] SCLH[7:0] SCLL[7:0]
	// in TIMINGR
	i2c_100khz_i2cclk8mhz(I2C1);
	//configure No-Stretch CR1 (only relevant in slave mode)
	i2c_enable_stretching(I2C1);
	//addressing mode
	i2c_set_7bit_addr_mode(I2C1);
	i2c_peripheral_enable(I2C1);
}

 void read_i2c(uint32_t i2c, uint8_t i2c_addr, uint8_t reg, uint8_t size,
              uint8_t *data)
{
	int wait;
	int i;
	while (i2c_busy(i2c) == 1);
	while (i2c_is_start(i2c) == 1);
	/*Setting transfer properties*/
	i2c_set_bytes_to_transfer(i2c, 2);
	i2c_set_7bit_address(i2c, i2c_addr);
	i2c_set_write_transfer_dir(i2c);
	i2c_disable_autoend(i2c);
	/*start transfer*/
	i2c_send_start(i2c);

	wait = true;
	while (wait) {
		if (i2c_transmit_int_status(i2c)) {
			wait = false;
		}
		while (i2c_nack(i2c)); /* Some error */
	}
	i2c_send_data(i2c, 0);
	i2c_send_data(i2c, reg);

	while (i2c_is_start(i2c) == 1);
	/*Setting transfer properties*/
	i2c_set_bytes_to_transfer(i2c, size);
	i2c_set_7bit_address(i2c, i2c_addr);
	i2c_set_read_transfer_dir(i2c);
	i2c_enable_autoend(i2c);
	/*start transfer*/
	i2c_send_start(i2c);

	for (i = 0; i < size; i++) {
		while (i2c_received_data(i2c) == 0);
		data[i] = i2c_get_data(i2c);
	}
}


int main(void)
{
	clock_setup();
	systick_setup();
	usart_setup();
	gpio_setup();
	i2c_setup();

	gpio_set LED_GREEN;
	printf("\n\nGuitar Fret Discovery Board\n");
	msleep(100);

	while(1){
		printf(".\n");
		
		uint8_t data[100];
		
		read_i2c(I2C1, 0x4b, 0, 7, data);
		for(unsigned int i = 0; i < 7; i++) {
			printf("0x%02x ");
		}
		printf("\n");
		
		msleep(500);
	}

	return 0;
}
