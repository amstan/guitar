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
const uint8_t ws2812_colors[] = {
	255, 0, 0, 254, 0, 1, 253, 0, 2, 252, 0, 3, 251, 0, 4, 250, 0, 5, 249, 0, 6, 248, 0, 7, 247, 0, 8, 246, 0, 9, 245, 0, 10, 244, 0, 11, 243, 0, 12, 242, 0, 13, 241, 0, 14, 240, 0, 15, 239, 0, 16, 238, 0, 17, 237, 0, 18, 236, 0, 19, 235, 0, 20, 234, 0, 21, 233, 0, 22, 232, 0, 23, 231, 0, 24, 230, 0, 25, 229, 0, 26, 228, 0, 27, 227, 0, 28, 226, 0, 29, 225, 0, 30, 224, 0, 31, 223, 0, 32, 222, 0, 33, 221, 0, 34, 220, 0, 35, 219, 0, 36, 218, 0, 37, 217, 0, 38, 216, 0, 39, 215, 0, 40, 214, 0, 41, 213, 0, 42, 212, 0, 43, 211, 0, 44, 210, 0, 45, 209, 0, 46, 208, 0, 47, 207, 0, 48, 206, 0, 49, 205, 0, 50, 204, 0, 51, 203, 0, 52, 202, 0, 53, 201, 0, 54, 200, 0, 55, 199, 0, 56, 198, 0, 57, 197, 0, 58, 196, 0, 59, 195, 0, 60, 194, 0, 61, 193, 0, 62, 192, 0, 63, 191, 0, 64, 190, 0, 65, 189, 0, 66, 188, 0, 67, 187, 0, 68, 186, 0, 69, 185, 0, 70, 184, 0, 71, 183, 0, 72, 182, 0, 73, 181, 0, 74, 180, 0, 75, 179, 0, 76, 178, 0, 77, 177, 0, 78, 176, 0, 79, 175, 0, 80, 174, 0, 81, 173, 0, 82, 172, 0, 83, 171, 0, 84, 170, 0, 85, 169, 0, 86, 168, 0, 87, 167, 0, 88, 166, 0, 89, 165, 0, 90, 164, 0, 91, 163, 0, 92, 162, 0, 93, 161, 0, 94, 160, 0, 95, 159, 0, 96, 158, 0, 97, 157, 0, 98, 156, 0, 99, 155, 0, 100, 154, 0, 101, 153, 0, 102, 152, 0, 103, 151, 0, 104, 150, 0, 105, 149, 0, 106, 148, 0, 107, 147, 0, 108, 146, 0, 109, 145, 0, 110, 144, 0, 111, 143, 0, 112, 142, 0, 113, 141, 0, 114, 140, 0, 115, 139, 0, 116, 138, 0, 117, 137, 0, 118, 136, 0, 119, 135, 0, 120, 134, 0, 121, 133, 0, 122, 132, 0, 123, 131, 0, 124, 130, 0, 125, 129, 0, 126, 128, 0, 127, 127, 0, 128, 126, 0, 129, 125, 0, 130, 124, 0, 131, 123, 0, 132, 122, 0, 133, 121, 0, 134, 120, 0, 135, 119, 0, 136, 118, 0, 137, 117, 0, 138, 116, 0, 139, 115, 0, 140, 114, 0, 141, 113, 0, 142, 112, 0, 143, 111, 0, 144, 110, 0, 145, 109, 0, 146, 108, 0, 147, 107, 0, 148, 106, 0, 149, 105, 0, 150, 104, 0, 151, 103, 0, 152, 102, 0, 153, 101, 0, 154, 100, 0, 155, 99, 0, 156, 98, 0, 157, 97, 0, 158, 96, 0, 159, 95, 0, 160, 94, 0, 161, 93, 0, 162, 92, 0, 163, 91, 0, 164, 90, 0, 165, 89, 0, 166, 88, 0, 167, 87, 0, 168, 86, 0, 169, 85, 0, 170, 84, 0, 171, 83, 0, 172, 82, 0, 173, 81, 0, 174, 80, 0, 175, 79, 0, 176, 78, 0, 177, 77, 0, 178, 76, 0, 179, 75, 0, 180, 74, 0, 181, 73, 0, 182, 72, 0, 183, 71, 0, 184, 70, 0, 185, 69, 0, 186, 68, 0, 187, 67, 0, 188, 66, 0, 189, 65, 0, 190, 64, 0, 191, 63, 0, 192, 62, 0, 193, 61, 0, 194, 60, 0, 195, 59, 0, 196, 58, 0, 197, 57, 0, 198, 56, 0, 199, 55, 0, 200, 54, 0, 201, 53, 0, 202, 52, 0, 203, 51, 0, 204, 50, 0, 205, 49, 0, 206, 48, 0, 207, 47, 0, 208, 46, 0, 209, 45, 0, 210, 44, 0, 211, 43, 0, 212, 42, 0, 213, 41, 0, 214, 40, 0, 215, 39, 0, 216, 38, 0, 217, 37, 0, 218, 36, 0, 219, 35, 0, 220, 34, 0, 221, 33, 0, 222, 32, 0, 223, 31, 0, 224, 30, 0, 225, 29, 0, 226, 28, 0, 227, 27, 0, 228, 26, 0, 229, 25, 0, 230, 24, 0, 231, 23, 0, 232, 22, 0, 233, 21, 0, 234, 20, 0, 235, 19, 0, 236, 18, 0, 237, 17, 0, 238, 16, 0, 239, 15, 0, 240, 14, 0, 241, 13, 0, 242, 12, 0, 243, 11, 0, 244, 10, 0, 245, 9, 0, 246, 8, 0, 247, 7, 0, 248, 6, 0, 249, 5, 0, 250, 4, 0, 251, 3, 0, 252, 2, 0, 253, 1, 0, 254, 0, 0, 255, 0, 1, 254, 0, 2, 253, 0, 3, 252, 0, 4, 251, 0, 5, 250, 0, 6, 249, 0, 7, 248, 0, 8, 247, 0, 9, 246, 0, 10, 245, 0, 11, 244, 0, 12, 243, 0, 13, 242, 0, 14, 241, 0, 15, 240, 0, 16, 239, 0, 17, 238, 0, 18, 237, 0, 19, 236, 0, 20, 235, 0, 21, 234, 0, 22, 233, 0, 23, 232, 0, 24, 231, 0, 25, 230, 0, 26, 229, 0, 27, 228, 0, 28, 227, 0, 29, 226, 0, 30, 225, 0, 31, 224, 0, 32, 223, 0, 33, 222, 0, 34, 221, 0, 35, 220, 0, 36, 219, 0, 37, 218, 0, 38, 217, 0, 39, 216, 0, 40, 215, 0, 41, 214, 0, 42, 213, 0, 43, 212, 0, 44, 211, 0, 45, 210, 0, 46, 209, 0, 47, 208, 0, 48, 207, 0, 49, 206, 0, 50, 205, 0, 51, 204, 0, 52, 203, 0, 53, 202, 0, 54, 201, 0, 55, 200, 0, 56, 199, 0, 57, 198, 0, 58, 197, 0, 59, 196, 0, 60, 195, 0, 61, 194, 0, 62, 193, 0, 63, 192, 0, 64, 191, 0, 65, 190, 0, 66, 189, 0, 67, 188, 0, 68, 187, 0, 69, 186, 0, 70, 185, 0, 71, 184, 0, 72, 183, 0, 73, 182, 0, 74, 181, 0, 75, 180, 0, 76, 179, 0, 77, 178, 0, 78, 177, 0, 79, 176, 0, 80, 175, 0, 81, 174, 0, 82, 173, 0, 83, 172, 0, 84, 171, 0, 85, 170, 0, 86, 169, 0, 87, 168, 0, 88, 167, 0, 89, 166, 0, 90, 165, 0, 91, 164, 0, 92, 163, 0, 93, 162, 0, 94, 161, 0, 95, 160, 0, 96, 159, 0, 97, 158, 0, 98, 157, 0, 99, 156, 0, 100, 155, 0, 101, 154, 0, 102, 153, 0, 103, 152, 0, 104, 151, 0, 105, 150, 0, 106, 149, 0, 107, 148, 0, 108, 147, 0, 109, 146, 0, 110, 145, 0, 111, 144, 0, 112, 143, 0, 113, 142, 0, 114, 141, 0, 115, 140, 0, 116, 139, 0, 117, 138, 0, 118, 137, 0, 119, 136, 0, 120, 135, 0, 121, 134, 0, 122, 133, 0, 123, 132, 0, 124, 131, 0, 125, 130, 0, 126, 129, 0, 127, 128, 0, 128, 127, 0, 129, 126, 0, 130, 125, 0, 131, 124, 0, 132, 123, 0, 133, 122, 0, 134, 121, 0, 135, 120, 0, 136, 119, 0, 137, 118, 0, 138, 117, 0, 139, 116, 0, 140, 115, 0, 141, 114, 0, 142, 113, 0, 143, 112, 0, 144, 111, 0, 145, 110, 0, 146, 109, 0, 147, 108, 0, 148, 107, 0, 149, 106, 0, 150, 105, 0, 151, 104, 0, 152, 103, 0, 153, 102, 0, 154, 101, 0, 155, 100, 0, 156, 99, 0, 157, 98, 0, 158, 97, 0, 159, 96, 0, 160, 95, 0, 161, 94, 0, 162, 93, 0, 163, 92, 0, 164, 91, 0, 165, 90, 0, 166, 89, 0, 167, 88, 0, 168, 87, 0, 169, 86, 0, 170, 85, 0, 171, 84, 0, 172, 83, 0, 173, 82, 0, 174, 81, 0, 175, 80, 0, 176, 79, 0, 177, 78, 0, 178, 77, 0, 179, 76, 0, 180, 75, 0, 181, 74, 0, 182, 73, 0, 183, 72, 0, 184, 71, 0, 185, 70, 0, 186, 69, 0, 187, 68, 0, 188, 67, 0, 189, 66, 0, 190, 65, 0, 191, 64, 0, 192, 63, 0, 193, 62, 0, 194, 61, 0, 195, 60, 0, 196, 59, 0, 197, 58, 0, 198, 57, 0, 199, 56, 0, 200, 55, 0, 201, 54, 0, 202, 53, 0, 203, 52, 0, 204, 51, 0, 205, 50, 0, 206, 49, 0, 207, 48, 0, 208, 47, 0, 209, 46, 0, 210, 45, 0, 211, 44, 0, 212, 43, 0, 213, 42, 0, 214, 41, 0, 215, 40, 0, 216, 39, 0, 217, 38, 0, 218, 37, 0, 219, 36, 0, 220, 35, 0, 221, 34, 0, 222, 33, 0, 223, 32, 0, 224, 31, 0, 225, 30, 0, 226, 29, 0, 227, 28, 0, 228, 27, 0, 229, 26, 0, 230, 25, 0, 231, 24, 0, 232, 23, 0, 233, 22, 0, 234, 21, 0, 235, 20, 0, 236, 19, 0, 237, 18, 0, 238, 17, 0, 239, 16, 0, 240, 15, 0, 241, 14, 0, 242, 13, 0, 243, 12, 0, 244, 11, 0, 245, 10, 0, 246, 9, 0, 247, 8, 0, 248, 7, 0, 249, 6, 0, 250, 5, 0, 251, 4, 0, 252, 3, 0, 253, 2, 0, 254, 1, 0, 255, 0, 1, 254, 0, 2, 253, 0, 3, 252, 0, 4, 251, 0, 5, 250, 0, 6, 249, 0, 7, 248, 0, 8, 247, 0, 9, 246, 0, 10, 245, 0, 11, 244, 0, 12, 243, 0, 13, 242, 0, 14, 241, 0, 15, 240, 0, 16, 239, 0, 17, 238, 0, 18, 237, 0, 19, 236, 0, 20, 235, 0, 21, 234, 0, 22, 233, 0, 23, 232, 0, 24, 231, 0, 25, 230, 0, 26, 229, 0, 27, 228, 0, 28, 227, 0, 29, 226, 0, 30, 225, 0, 31, 224, 0, 32, 223, 0, 33, 222, 0, 34, 221, 0, 35, 220, 0, 36, 219, 0, 37, 218, 0, 38, 217, 0, 39, 216, 0, 40, 215, 0, 41, 214, 0, 42, 213, 0, 43, 212, 0, 44, 211, 0, 45, 210, 0, 46, 209, 0, 47, 208, 0, 48, 207, 0, 49, 206, 0, 50, 205, 0, 51, 204, 0, 52, 203, 0, 53, 202, 0, 54, 201, 0, 55, 200, 0, 56, 199, 0, 57, 198, 0, 58, 197, 0, 59, 196, 0, 60, 195, 0, 61, 194, 0, 62, 193, 0, 63, 192, 0, 64, 191, 0, 65, 190, 0, 66, 189, 0, 67, 188, 0, 68, 187, 0, 69, 186, 0, 70, 185, 0, 71, 184, 0, 72, 183, 0, 73, 182, 0, 74, 181, 0, 75, 180, 0, 76, 179, 0, 77, 178, 0, 78, 177, 0, 79, 176, 0, 80, 175, 0, 81, 174, 0, 82, 173, 0, 83, 172, 0, 84, 171, 0, 85, 170, 0, 86, 169, 0, 87, 168, 0, 88, 167, 0, 89, 166, 0, 90, 165, 0, 91, 164, 0, 92, 163, 0, 93, 162, 0, 94, 161, 0, 95, 160, 0, 96, 159, 0, 97, 158, 0, 98, 157, 0, 99, 156, 0, 100, 155, 0, 101, 154, 0, 102, 153, 0, 103, 152, 0, 104, 151, 0, 105, 150, 0, 106, 149, 0, 107, 148, 0, 108, 147, 0, 109, 146, 0, 110, 145, 0, 111, 144, 0, 112, 143, 0, 113, 142, 0, 114, 141, 0, 115, 140, 0, 116, 139, 0, 117, 138, 0, 118, 137, 0, 119, 136, 0, 120, 135, 0, 121, 134, 0, 122, 133, 0, 123, 132, 0, 124, 131, 0, 125, 130, 0, 126, 129, 0, 127, 128, 0, 128, 127, 0, 129, 126, 0, 130, 125, 0, 131, 124, 0, 132, 123, 0, 133, 122, 0, 134, 121, 0, 135, 120, 0, 136, 119, 0, 137, 118, 0, 138, 117, 0, 139, 116, 0, 140, 115, 0, 141, 114, 0, 142, 113, 0, 143, 112, 0, 144, 111, 0, 145, 110, 0, 146, 109, 0, 147, 108, 0, 148, 107, 0, 149, 106, 0, 150, 105, 0, 151, 104, 0, 152, 103, 0, 153, 102, 0, 154, 101, 0, 155, 100, 0, 156, 99, 0, 157, 98, 0, 158, 97, 0, 159, 96, 0, 160, 95, 0, 161, 94, 0, 162, 93, 0, 163, 92, 0, 164, 91, 0, 165, 90, 0, 166, 89, 0, 167, 88, 0, 168, 87, 0, 169, 86, 0, 170, 85, 0, 171, 84, 0, 172, 83, 0, 173, 82, 0, 174, 81, 0, 175, 80, 0, 176, 79, 0, 177, 78, 0, 178, 77, 0, 179, 76, 0, 180, 75, 0, 181, 74, 0, 182, 73, 0, 183, 72, 0, 184, 71, 0, 185, 70, 0, 186, 69, 0, 187, 68, 0, 188, 67, 0, 189, 66, 0, 190, 65, 0, 191, 64, 0, 192, 63, 0, 193, 62, 0, 194, 61, 0, 195, 60, 0, 196, 59, 0, 197, 58, 0, 198, 57, 0, 199, 56, 0, 200, 55, 0, 201, 54, 0, 202, 53, 0, 203, 52, 0, 204, 51, 0, 205, 50, 0, 206, 49, 0, 207, 48, 0, 208, 47, 0, 209, 46, 0, 210, 45, 0, 211, 44, 0, 212, 43, 0, 213, 42, 0, 214, 41, 0, 215, 40, 0, 216, 39, 0, 217, 38, 0, 218, 37, 0, 219, 36, 0, 220, 35, 0, 221, 34, 0, 222, 33, 0, 223, 32, 0, 224, 31, 0, 225, 30, 0, 226, 29, 0, 227, 28, 0, 228, 27, 0, 229, 26, 0, 230, 25, 0, 231, 24, 0, 232, 23, 0, 233, 22, 0, 234, 21, 0, 235, 20, 0, 236, 19, 0, 237, 18, 0, 238, 17, 0, 239, 16, 0, 240, 15, 0, 241, 14, 0, 242, 13, 0, 243, 12, 0, 244, 11, 0, 245, 10, 0, 246, 9, 0, 247, 8, 0, 248, 7, 0, 249, 6, 0, 250, 5, 0, 251, 4, 0, 252, 3, 0, 253, 2, 0, 254, 1, 0,
	255, 0, 0, 254, 0, 1, 253, 0, 2, 252, 0, 3, 251, 0, 4, 250, 0, 5, 249, 0, 6, 248, 0, 7, 247, 0, 8, 246, 0, 9, 245, 0, 10, 244, 0, 11, 243, 0, 12, 242, 0, 13, 241, 0, 14, 240, 0, 15, 239, 0, 16, 238, 0, 17, 237, 0, 18, 236, 0, 19, 235, 0, 20, 234, 0, 21, 233, 0, 22, 232, 0, 23, 231, 0, 24, 230, 0, 25, 229, 0, 26, 228, 0, 27, 227, 0, 28, 226, 0, 29, 225, 0, 30, 224, 0, 31, 223, 0, 32, 222, 0, 33, 221, 0, 34, 220, 0, 35, 219, 0, 36, 218, 0, 37, 217, 0, 38, 216, 0, 39, 215, 0, 40, 214, 0, 41, 213, 0, 42, 212, 0, 43, 211, 0, 44, 210, 0, 45, 209, 0, 46, 208, 0, 47, 207, 0, 48, 206, 0, 49, 205, 0, 50, 204, 0, 51, 203, 0, 52, 202, 0, 53, 201, 0, 54, 200, 0, 55, 199, 0, 56, 198, 0, 57, 197, 0, 58, 196, 0, 59, 195, 0, 60, 194, 0, 61, 193, 0, 62, 192, 0, 63, 191, 0, 64, 190, 0, 65, 189, 0, 66, 188, 0, 67, 187, 0, 68, 186, 0, 69, 185, 0, 70, 184, 0, 71, 183, 0, 72, 182, 0, 73, 181, 0, 74, 180, 0, 75, 179, 0, 76, 178, 0, 77, 177, 0, 78, 176, 0, 79, 175, 0, 80, 174, 0, 81, 173, 0, 82, 172, 0, 83, 171, 0, 84, 170, 0, 85, 169, 0, 86, 168, 0, 87, 167, 0, 88, 166, 0, 89, 165, 0, 90, 164, 0, 91, 163, 0, 92, 162, 0, 93, 161, 0, 94, 160, 0, 95, 159, 0, 96, 158, 0, 97, 157, 0, 98, 156, 0, 99, 155, 0, 100, 154, 0, 101, 153, 0, 102, 152, 0, 103, 151, 0, 104, 150, 0, 105, 149, 0, 106, 148, 0, 107, 147, 0, 108, 146, 0, 109, 145, 0, 110, 144, 0, 111, 143, 0, 112, 142, 0, 113, 141, 0, 114, 140, 0, 115, 139, 0, 116, 138, 0, 117, 137, 0, 118, 136, 0, 119, 135, 0, 120, 134, 0, 121, 133, 0, 122, 132, 0, 123, 131, 0, 124, 130, 0, 125, 129, 0, 126, 128, 0, 127, 127, 0, 128, 126, 0, 129, 125, 0, 130, 124, 0, 131, 123, 0, 132, 122, 0, 133, 121, 0, 134, 120, 0, 135, 119, 0, 136, 118, 0, 137, 117, 0, 138, 116, 0, 139, 115, 0, 140, 114, 0, 141, 113, 0, 142, 112, 0, 143, 111, 0, 144, 110, 0, 145, 109, 0, 146, 108, 0, 147, 107, 0, 148, 106, 0, 149, 105, 0, 150, 104, 0, 151, 103, 0, 152, 102, 0, 153, 101, 0, 154, 100, 0, 155, 99, 0, 156, 98, 0, 157, 97, 0, 158, 96, 0, 159, 95, 0, 160, 94, 0, 161, 93, 0, 162, 92, 0, 163, 91, 0, 164, 90, 0, 165, 89, 0, 166, 88, 0, 167, 87, 0, 168, 86, 0, 169, 85, 0, 170, 84, 0, 171, 83, 0, 172, 82, 0, 173, 81, 0, 174, 80, 0, 175, 79, 0, 176, 78, 0, 177, 77, 0, 178, 76, 0, 179, 75, 0, 180, 74, 0, 181, 73, 0, 182, 72, 0, 183, 71, 0, 184, 70, 0, 185, 69, 0, 186, 68, 0, 187, 67, 0, 188, 66, 0, 189, 65, 0, 190, 64, 0, 191, 63, 0, 192, 62, 0, 193, 61, 0, 194, 60, 0, 195, 59, 0, 196, 58, 0, 197, 57, 0, 198, 56, 0, 199, 55, 0, 200, 54, 0, 201, 53, 0, 202, 52, 0, 203, 51, 0, 204, 50, 0, 205, 49, 0, 206, 48, 0, 207, 47, 0, 208, 46, 0, 209, 45, 0, 210, 44, 0, 211, 43, 0, 212, 42, 0, 213, 41, 0, 214, 40, 0, 215, 39, 0, 216, 38, 0, 217, 37, 0, 218, 36, 0, 219, 35, 0, 220, 34, 0, 221, 33, 0, 222, 32, 0, 223, 31, 0, 224, 30, 0, 225, 29, 0, 226, 28, 0, 227, 27, 0, 228, 26, 0, 229, 25, 0, 230, 24, 0, 231, 23, 0, 232, 22, 0, 233, 21, 0, 234, 20, 0, 235, 19, 0, 236, 18, 0, 237, 17, 0, 238, 16, 0, 239, 15, 0, 240, 14, 0, 241, 13, 0, 242, 12, 0, 243, 11, 0, 244, 10, 0, 245, 9, 0, 246, 8, 0, 247, 7, 0, 248, 6, 0, 249, 5, 0, 250, 4, 0, 251, 3, 0, 252, 2, 0, 253, 1, 0, 254, 0, 0, 255, 0, 1, 254, 0, 2, 253, 0, 3, 252, 0, 4, 251, 0, 5, 250, 0, 6, 249, 0, 7, 248, 0, 8, 247, 0, 9, 246, 0, 10, 245, 0, 11, 244, 0, 12, 243, 0, 13, 242, 0, 14, 241, 0, 15, 240, 0, 16, 239, 0, 17, 238, 0, 18, 237, 0, 19, 236, 0, 20, 235, 0, 21, 234, 0, 22, 233, 0, 23, 232, 0, 24, 231, 0, 25, 230, 0, 26, 229, 0, 27, 228, 0, 28, 227, 0, 29, 226, 0, 30, 225, 0, 31, 224, 0, 32, 223, 0, 33, 222, 0, 34, 221, 0, 35, 220, 0, 36, 219, 0, 37, 218, 0, 38, 217, 0, 39, 216, 0, 40, 215, 0, 41, 214, 0, 42, 213, 0, 43, 212, 0, 44, 211, 0, 45, 210, 0, 46, 209, 0, 47, 208, 0, 48, 207, 0, 49, 206, 0, 50, 205, 0, 51, 204, 0, 52, 203, 0, 53, 202, 0, 54, 201, 0, 55, 200, 0, 56, 199, 0, 57, 198, 0, 58, 197, 0, 59, 196, 0, 60, 195, 0, 61, 194, 0, 62, 193, 0, 63, 192, 0, 64, 191, 0, 65, 190, 0, 66, 189, 0, 67, 188, 0, 68, 187, 0, 69, 186, 0, 70, 185, 0, 71, 184, 0, 72, 183, 0, 73, 182, 0, 74, 181, 0, 75, 180, 0, 76, 179, 0, 77, 178, 0, 78, 177, 0, 79, 176, 0, 80, 175, 0, 81, 174, 0, 82, 173, 0, 83, 172, 0, 84, 171, 0, 85, 170, 0, 86, 169, 0, 87, 168, 0, 88, 167, 0, 89, 166, 0, 90, 165, 0, 91, 164, 0, 92, 163, 0, 93, 162, 0, 94, 161, 0, 95, 160, 0, 96, 159, 0, 97, 158, 0, 98, 157, 0, 99, 156, 0, 100, 155, 0, 101, 154, 0, 102, 153, 0, 103, 152, 0, 104, 151, 0, 105, 150, 0, 106, 149, 0, 107, 148, 0, 108, 147, 0, 109, 146, 0, 110, 145, 0, 111, 144, 0, 112, 143, 0, 113, 142, 0, 114, 141, 0, 115, 140, 0, 116, 139, 0, 117, 138, 0, 118, 137, 0, 119, 136, 0, 120, 135, 0, 121, 134, 0, 122, 133, 0, 123, 132, 0, 124, 131, 0, 125, 130, 0, 126, 129, 0, 127, 128, 0, 128, 127, 0, 129, 126, 0, 130, 125, 0, 131, 124, 0, 132, 123, 0, 133, 122, 0, 134, 121, 0, 135, 120, 0, 136, 119, 0, 137, 118, 0, 138, 117, 0, 139, 116, 0, 140, 115, 0, 141, 114, 0, 142, 113, 0, 143, 112, 0, 144, 111, 0, 145, 110, 0, 146, 109, 0, 147, 108, 0, 148, 107, 0, 149, 106, 0, 150, 105, 0, 151, 104, 0, 152, 103, 0, 153, 102, 0, 154, 101, 0, 155, 100, 0, 156, 99, 0, 157, 98, 0, 158, 97, 0, 159, 96, 0, 160, 95, 0, 161, 94, 0, 162, 93, 0, 163, 92, 0, 164, 91, 0, 165, 90, 0, 166, 89, 0, 167, 88, 0, 168, 87, 0, 169, 86, 0, 170, 85, 0, 171, 84, 0, 172, 83, 0, 173, 82, 0, 174, 81, 0, 175, 80, 0, 176, 79, 0, 177, 78, 0, 178, 77, 0, 179, 76, 0, 180, 75, 0, 181, 74, 0, 182, 73, 0, 183, 72, 0, 184, 71, 0, 185, 70, 0, 186, 69, 0, 187, 68, 0, 188, 67, 0, 189, 66, 0, 190, 65, 0, 191, 64, 0, 192, 63, 0, 193, 62, 0, 194, 61, 0, 195, 60, 0, 196, 59, 0, 197, 58, 0, 198, 57, 0, 199, 56, 0, 200, 55, 0, 201, 54, 0, 202, 53, 0, 203, 52, 0, 204, 51, 0, 205, 50, 0, 206, 49, 0, 207, 48, 0, 208, 47, 0, 209, 46, 0, 210, 45, 0, 211, 44, 0, 212, 43, 0, 213, 42, 0, 214, 41, 0, 215, 40, 0, 216, 39, 0, 217, 38, 0, 218, 37, 0, 219, 36, 0, 220, 35, 0, 221, 34, 0, 222, 33, 0, 223, 32, 0, 224, 31, 0, 225, 30, 0, 226, 29, 0, 227, 28, 0, 228, 27, 0, 229, 26, 0, 230, 25, 0, 231, 24, 0, 232, 23, 0, 233, 22, 0, 234, 21, 0, 235, 20, 0, 236, 19, 0, 237, 18, 0, 238, 17, 0, 239, 16, 0, 240, 15, 0, 241, 14, 0, 242, 13, 0, 243, 12, 0, 244, 11, 0, 245, 10, 0, 246, 9, 0, 247, 8, 0, 248, 7, 0, 249, 6, 0, 250, 5, 0, 251, 4, 0, 252, 3, 0, 253, 2, 0, 254, 1, 0, 255, 0, 1, 254, 0, 2, 253, 0, 3, 252, 0, 4, 251, 0, 5, 250, 0, 6, 249, 0, 7, 248, 0, 8, 247, 0, 9, 246, 0, 10, 245, 0, 11, 244, 0, 12, 243, 0, 13, 242, 0, 14, 241, 0, 15, 240, 0, 16, 239, 0, 17, 238, 0, 18, 237, 0, 19, 236, 0, 20, 235, 0, 21, 234, 0, 22, 233, 0, 23, 232, 0, 24, 231, 0, 25, 230, 0, 26, 229, 0, 27, 228, 0, 28, 227, 0, 29, 226, 0, 30, 225, 0, 31, 224, 0, 32, 223, 0, 33, 222, 0, 34, 221, 0, 35, 220, 0, 36, 219, 0, 37, 218, 0, 38, 217, 0, 39, 216, 0, 40, 215, 0, 41, 214, 0, 42, 213, 0, 43, 212, 0, 44, 211, 0, 45, 210, 0, 46, 209, 0, 47, 208, 0, 48, 207, 0, 49, 206, 0, 50, 205, 0, 51, 204, 0, 52, 203, 0, 53, 202, 0, 54, 201, 0, 55, 200, 0, 56, 199, 0, 57, 198, 0, 58, 197, 0, 59, 196, 0, 60, 195, 0, 61, 194, 0, 62, 193, 0, 63, 192, 0, 64, 191, 0, 65, 190, 0, 66, 189, 0, 67, 188, 0, 68, 187, 0, 69, 186, 0, 70, 185, 0, 71, 184, 0, 72, 183, 0, 73, 182, 0, 74, 181, 0, 75, 180, 0, 76, 179, 0, 77, 178, 0, 78, 177, 0, 79, 176, 0, 80, 175, 0, 81, 174, 0, 82, 173, 0, 83, 172, 0, 84, 171, 0, 85, 170, 0, 86, 169, 0, 87, 168, 0, 88, 167, 0, 89, 166, 0, 90, 165, 0, 91, 164, 0, 92, 163, 0, 93, 162, 0, 94, 161, 0, 95, 160, 0, 96, 159, 0, 97, 158, 0, 98, 157, 0, 99, 156, 0, 100, 155, 0, 101, 154, 0, 102, 153, 0, 103, 152, 0, 104, 151, 0, 105, 150, 0, 106, 149, 0, 107, 148, 0, 108, 147, 0, 109, 146, 0, 110, 145, 0, 111, 144, 0, 112, 143, 0, 113, 142, 0, 114, 141, 0, 115, 140, 0, 116, 139, 0, 117, 138, 0, 118, 137, 0, 119, 136, 0, 120, 135, 0, 121, 134, 0, 122, 133, 0, 123, 132, 0, 124, 131, 0, 125, 130, 0, 126, 129, 0, 127, 128, 0, 128, 127, 0, 129, 126, 0, 130, 125, 0, 131, 124, 0, 132, 123, 0, 133, 122, 0, 134, 121, 0, 135, 120, 0, 136, 119, 0, 137, 118, 0, 138, 117, 0, 139, 116, 0, 140, 115, 0, 141, 114, 0, 142, 113, 0, 143, 112, 0, 144, 111, 0, 145, 110, 0, 146, 109, 0, 147, 108, 0, 148, 107, 0, 149, 106, 0, 150, 105, 0, 151, 104, 0, 152, 103, 0, 153, 102, 0, 154, 101, 0, 155, 100, 0, 156, 99, 0, 157, 98, 0, 158, 97, 0, 159, 96, 0, 160, 95, 0, 161, 94, 0, 162, 93, 0, 163, 92, 0, 164, 91, 0, 165, 90, 0, 166, 89, 0, 167, 88, 0, 168, 87, 0, 169, 86, 0, 170, 85, 0, 171, 84, 0, 172, 83, 0, 173, 82, 0, 174, 81, 0, 175, 80, 0, 176, 79, 0, 177, 78, 0, 178, 77, 0, 179, 76, 0, 180, 75, 0, 181, 74, 0, 182, 73, 0, 183, 72, 0, 184, 71, 0, 185, 70, 0, 186, 69, 0, 187, 68, 0, 188, 67, 0, 189, 66, 0, 190, 65, 0, 191, 64, 0, 192, 63, 0, 193, 62, 0, 194, 61, 0, 195, 60, 0, 196, 59, 0, 197, 58, 0, 198, 57, 0, 199, 56, 0, 200, 55, 0, 201, 54, 0, 202, 53, 0, 203, 52, 0, 204, 51, 0, 205, 50, 0, 206, 49, 0, 207, 48, 0, 208, 47, 0, 209, 46, 0, 210, 45, 0, 211, 44, 0, 212, 43, 0, 213, 42, 0, 214, 41, 0, 215, 40, 0, 216, 39, 0, 217, 38, 0, 218, 37, 0, 219, 36, 0, 220, 35, 0, 221, 34, 0, 222, 33, 0, 223, 32, 0, 224, 31, 0, 225, 30, 0, 226, 29, 0, 227, 28, 0, 228, 27, 0, 229, 26, 0, 230, 25, 0, 231, 24, 0, 232, 23, 0, 233, 22, 0, 234, 21, 0, 235, 20, 0, 236, 19, 0, 237, 18, 0, 238, 17, 0, 239, 16, 0, 240, 15, 0, 241, 14, 0, 242, 13, 0, 243, 12, 0, 244, 11, 0, 245, 10, 0, 246, 9, 0, 247, 8, 0, 248, 7, 0, 249, 6, 0, 250, 5, 0, 251, 4, 0, 252, 3, 0, 253, 2, 0, 254, 1, 0,
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

// 	for(unsigned int i=0;i<sizeof(ws2812_colors);i++) {
// 		ws2812_colors[i]/=8;
// 	}

#define LEDS 16*3
	uint8_t temp[LEDS];
	uint16_t count = sizeof(ws2812_colors)/3/2;

	while(1) {
		for(unsigned int f=0;f<count;f++) {
			for(unsigned int l=0; l<16; l++)
				for(unsigned int c=0;c<3;c++)
					temp[l*3+c] = ws2812_colors[l*3+c + (f+l*(count/16))*3];
			ws2812_sendarray(temp,LEDS);
			msleep(16);
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
