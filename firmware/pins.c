#define I2C_PORT GPIOB
#define SDA GPIO9
#define SCL GPIO6

static void gpio_setup(void)
{
	//Enable leds
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15);
	gpio_clear(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	//I2C
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(I2C_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA | SCL);
	gpio_set_output_options(I2C_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ, SDA | SCL);
}
