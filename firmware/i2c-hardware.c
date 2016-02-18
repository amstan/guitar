#include <libopencm3/stm32/i2c.h>

#define I2C_PORT I2C1

static void i2c_init(void)
{
	/* clocks */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_I2C1);

	/* GPIO for I2C1 */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO6 | GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ,
				GPIO6 | GPIO9);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6 | GPIO9);



	/* Disable the I2C before changing any configuration. */
	i2c_peripheral_disable(I2C_PORT);

	/* APB1 is running at 36MHz. */
	i2c_set_clock_frequency(I2C_PORT, I2C_CR2_FREQ_36MHZ);

	/* 400KHz - I2C Fast Mode */
	i2c_set_fast_mode(I2C_PORT);

	/*
	 * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
	 * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
	 * Datasheet suggests 0x1e.
	 */
	i2c_set_ccr(I2C_PORT, 0x1e*4);

	/*
	 * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
	 * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
	 * Incremented by 1 -> 11.
	 */
	i2c_set_trise(I2C_PORT, 0x0b);

	/*
	 * This is our slave address - needed only if we want to receive from
	 * other masters.
	 */
	i2c_set_own_7bit_slave_address(I2C_PORT, 0x32);

	/* If everything is configured -> enable the peripheral. */
	i2c_peripheral_enable(I2C1);
}

// bool i2c_read_data(unsigned char address, unsigned int reg, unsigned char* dest, unsigned char len) {
// // 	printf("i2c read from %x reg0x%04x len%d\n", address, reg, len);
//
// 	i2c_start_cond();
// 	i2c_write_byte_ex(address+I2C_SLAVE_WRITE);
// 	i2c_write_byte_ex(reg%256);
// 	i2c_write_byte_ex(reg/256);
// 	i2c_stop_cond();
//
// 	i2c_start_cond();
// 	i2c_write_byte_ex(address+I2C_SLAVE_READ);
//
// 	for(unsigned char i=0; i<len; i++) {
// 		*(dest++) = i2c_read_byte(i==len-1);
// 	}
// 	i2c_stop_cond();
//
// 	return 0;
// }

bool i2c_probe(unsigned char address) {
	i2c_send_start(I2C_PORT);
	/* Waiting for START is send and switched to master mode. */
	while (!((I2C_SR1(I2C_PORT) & I2C_SR1_SB)
		& (I2C_SR2(I2C_PORT) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	/* Send destination address. */
	i2c_send_7bit_address(I2C_PORT, address, I2C_WRITE);

	/* Waiting for address is transferred. */
	unsigned int sr1;
	while (!(sr1 = I2C_SR1(I2C_PORT)));

	printf("sr1 = 0x%08x\n", sr1);

// 		/* Sending the data. */
// 		i2c_send_data(I2C_PORT, 0x1); /* stts75 config register */
// 		while (!(I2C_SR1(I2C_PORT) & I2C_SR1_BTF)); /* Await ByteTransferedFlag. */
// 		/* Polarity reverse - LED glows if temp is below Tos/Thyst. */
// 		i2c_send_data(I2C_PORT, 0x4);
// 		while (!(I2C_SR1(I2C_PORT) & (I2C_SR1_BTF | I2C_SR1_TxE)));

	/* Send STOP condition. */
	i2c_send_stop(I2C_PORT);

	return 1;
}
