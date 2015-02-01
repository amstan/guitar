#include "timer.c"
#include "usb.c"
#include <stdio.h>

static void clock_setup(void)
{
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);
}

static void gpio_setup(void)
{
	//Enable leds
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15);
	gpio_clear(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
}

int main(void) {
	clock_setup();
	gpio_setup();
	systick_setup();
	usbd_device *usbd_dev = usb_init();

	while(1) {
		gpio_toggle(GPIOD, GPIO12);
		printf("ping\n");
		msleep(1000);
	}
}
