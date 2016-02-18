#pragma once

///@title I2C software library
///Assumes SDA, SCL, I2C_PORT are defined
///Code modified from the wikipedia example because i was lazy.
///This version of the code does not support arbitration or clock stretching

// Hardware-specific support functions that MUST be customized:
#define I2C_SPEED 7
#define I2C_delay() for (unsigned int delay = 0; delay < I2C_SPEED; delay++) {__asm__("nop");}

#define I2C_READ_HOLDOFF_DELAY 2
#define I2C_READ_HOLDOFF() for (unsigned int delay = 0; delay < I2C_READ_HOLDOFF_DELAY; delay++) {__asm__("nop");}

#define I2C_SLAVE_WRITE 0
#define I2C_SLAVE_READ 1

bool read_SCL(void) {
	// Set SCL as input and return current level of line, 0 or 1
	gpio_set(I2C_PORT,SCL);
	I2C_READ_HOLDOFF();
	return gpio_get(I2C_PORT,SCL);
}

bool read_SDA(void) {
	// Set SDA as input and return current level of line, 0 or 1
	gpio_set(I2C_PORT,SDA);
	I2C_READ_HOLDOFF();
	return gpio_get(I2C_PORT,SDA);
}

void clear_SCL(void) {
	// Actively drive SCL signal low
	gpio_clear(I2C_PORT,SCL);
}

void clear_SDA(void) {
	// Actively drive SDA signal low
	gpio_clear(I2C_PORT,SDA);
}

void arbitration_lost(void) {
	printf("\nError: I2C - Arbitration Lost!\n");
}

bool started = false; // global data
void i2c_start_cond(void) {
	if (started) { // if started, do a restart cond
		// set SDA to 1
		read_SDA();
		I2C_delay();
		read_SCL();
// 		while (read_SCL() == 0) {  // Clock stretching
// 		// You should add timeout to this loop
// 		}
		// Repeated start setup time, minimum 4.7us
		I2C_delay();
	}
	if (read_SDA() == 0) {
		arbitration_lost();
	}
	// SCL is high, set SDA from 1 to 0.
	clear_SDA();
	I2C_delay();
	clear_SCL();
	started = true;
}

void i2c_stop_cond(void) {
	// set SDA to 0
	clear_SDA();
	I2C_delay();
	// Clock stretching
	read_SCL();
// 	while (read_SCL() == 0) {
// 		// add timeout to this loop.
// 	}
	// Stop bit setup time, minimum 4us
	I2C_delay();
	// SCL is high, set SDA from 0 to 1
	if (read_SDA() == 0) {
		arbitration_lost();
	}
	I2C_delay();
	started = false;
	
	I2C_delay();
	I2C_delay();
}

// Write a bit to I2C bus
void i2c_write_bit(bool bit) {
	if (bit) {
		read_SDA();
	} else {
		clear_SDA();
	}
	I2C_delay();
	read_SCL();
// 	while (read_SCL() == 0) { // Clock stretching
// 		// You should add timeout to this loop
// 	}
	// SCL is high, now data is valid
	// If SDA is high, check that nobody else is driving SDA
	if (bit && read_SDA() == 0) {
		arbitration_lost();
	}
	I2C_delay();
	clear_SCL();
	I2C_delay();
}

// Read a bit from I2C bus
bool i2c_read_bit(void) {
	bool bit;
	// Let the slave drive data
	read_SDA();
	I2C_delay();
	while (read_SCL() == 0) { // Clock stretching
// 		printf(".");
// 		fflush(0);
// 		msleep(1);
		// You should add timeout to this loop
	}
	// SCL is high, now data is valid
	bit = read_SDA();
	I2C_delay();
	clear_SCL();
	I2C_delay();
	return bit;
}

// Write a byte to I2C bus. Return 0 if ack by the slave.
bool i2c_write_byte(unsigned char byte) {
	unsigned bit;
	bool nack;
	for (bit = 0; bit < 8; bit++) {
		i2c_write_bit((byte & 0x80) != 0);
		byte <<= 1;
	}
	nack = i2c_read_bit();
	return nack;
}

// Read a byte from I2C bus
unsigned char i2c_read_byte(bool nack) {
	unsigned char byte = 0;
	unsigned bit;
	for (bit = 0; bit < 8; bit++) {
		byte = (byte << 1) | i2c_read_bit();
	}
	i2c_write_bit(nack);
	return byte;
}

//does a write, and if it fails it returns the function where it's called
#define i2c_write_byte_ex(byte) {if(i2c_write_byte(byte)) {i2c_stop_cond(); return 1;}}

//Tries sending data to a device, returns 0 on success
bool i2c_probe(unsigned char address) {
	i2c_start_cond();
	i2c_write_byte_ex(address + I2C_SLAVE_WRITE);
	i2c_stop_cond();
	return 0;
}

//Writes a register, returns 0 if successful
bool i2c_write_register(unsigned char address, unsigned int reg, unsigned char data) {
	i2c_start_cond();
	i2c_write_byte_ex(address + I2C_SLAVE_WRITE);
	i2c_write_byte_ex(reg);
	i2c_write_byte_ex(reg >> 8);
	i2c_write_byte_ex(data);
	i2c_stop_cond();
	return 0;
}

//Reads a register
unsigned char i2c_read_register(unsigned char address, unsigned int reg) {
	i2c_start_cond();
	i2c_write_byte(address + I2C_SLAVE_WRITE);
	i2c_write_byte(reg);
	i2c_write_byte(reg >> 8);
	i2c_start_cond();
	i2c_write_byte(address + I2C_SLAVE_READ);
	unsigned char data = i2c_read_byte(1);
	i2c_stop_cond();
	return data;
}

bool i2c_read_data(unsigned char address, unsigned int reg, unsigned char* dest, unsigned char len) {
// 	printf("i2c read from %x reg0x%04x len%d\n", address, reg, len);
	
	i2c_start_cond();
	i2c_write_byte_ex(address + I2C_SLAVE_WRITE);
	i2c_write_byte_ex(reg);
	i2c_write_byte_ex(reg >> 8);
	i2c_stop_cond();
	
	i2c_start_cond();
	i2c_write_byte_ex(address + I2C_SLAVE_READ);
	
	for(unsigned char i = 0; i < len; i++) {
		*(dest++) = i2c_read_byte(i == len-1);
	}
	i2c_stop_cond();
	
	return 0;
}
