#include <libopencm3/stm32/i2c.h>

#define I2C_SLAVE_ADDRESS 0x25
#define I2C_BUS I2C1
#define I2C_PIN_PORT GPIOB
#define I2C_PINS GPIO6 | GPIO7
#define I2C_PIN_AF GPIO_AF1

void i2c_setup(void)
{
	rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_GPIOB);

	//clock from 48 MHz
	RCC_CFGR3 |= RCC_CFGR3_I2C1SW;

	//i2c_reset(I2C_BUS);
	RCC_APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	RCC_APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

	gpio_mode_setup(I2C_PIN_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, I2C_PINS);
	gpio_set_af(I2C_PIN_PORT, I2C_PIN_AF, I2C_PINS);

	//i2c_peripheral_disable(I2C_BUS);
	I2C_CR1(I2C_BUS) &= ~I2C_CR1_PE;

	//configure ANFOFF DNF[3:0] in CR1
	//i2c_enable_analog_filter(I2C_BUS);
	I2C_CR1(I2C_BUS) &= ~I2C_CR1_ANFOFF;

	//i2c_set_digital_filter(I2C_BUS, I2C_CR1_DNF_DISABLED); //default is good

	//Configure PRESC[3:0] SDADEL[3:0] SCLDEL[3:0] SCLH[7:0] SCLL[7:0]
	// in TIMINGR
	//i2c_100khz_i2cclk8mhz(I2C_BUS); //let's assume this is good :)

	//I don't think the timing matters when we're slave
	I2C_TIMINGR(I2C_BUS) = 0;

	//configure No-Stretch CR1 (only relevant in slave mode)
	//i2c_enable_stretching(I2C_BUS);

	//addressing mode
	//i2c_set_7bit_addr_mode(I2C_BUS); //default is good

	//slave address
	I2C_OAR1(I2C_BUS) = I2C_OAR1_OA1EN + (I2C_SLAVE_ADDRESS << 1);

	//enable all interrupts
	I2C_CR1(I2C_BUS) |= 0xfe;

	//i2c_peripheral_enable(I2C_BUS);
	I2C_CR1(I2C_BUS) |= I2C_CR1_PE;

	nvic_enable_irq(NVIC_I2C1_IRQ);
}

void handle_i2c_bufs(void);

uint8_t i2c_rx_buf[128];
uint8_t i2c_rx_bytes;
uint8_t i2c_tx_buf[128];
uint8_t i2c_tx_bytes;

void i2c1_isr(void) {
	//WARNING: prints in here are a bad idea
	static int i=0;

	uint32_t isr = I2C_ISR(I2C_BUS);


	if (isr & I2C_ISR_RXNE) {
		//do this first so we don't lose a char if other events are happening
		volatile uint8_t byte = I2C_RXDR(I2C_BUS);
		i2c_rx_buf[i2c_rx_bytes++] = byte;
// 		printf("rx 0x%02x\n", byte);
	}

	if (isr & I2C_ISR_ADDR) {
// 		printf("\n");
// 		printf("\naddress match 0x%04lx dir%d\n", I2C_ISR_ADDCODE_VALG(isr), !!(isr & I2C_ISR_DIR));
		I2C_ICR(I2C_BUS) = I2C_ICR_ADDRCF;
		i2c_rx_bytes = 0;
		i2c_tx_bytes = 0;
		if (isr & I2C_ISR_DIR) {
			I2C_ISR(I2C_BUS) |= I2C_ISR_TXE;
		}
	}

	if ((isr & (I2C_ISR_TXIS | I2C_ISR_DIR)) == (I2C_ISR_TXIS | I2C_ISR_DIR)) {
		if(!(isr & I2C_ISR_STOPF)) {
			volatile uint8_t byte = i2c_tx_buf[i2c_tx_bytes++];
			I2C_TXDR(I2C_BUS) = byte;
// 			printf("t");
		}
// 		printf("i0x%08lx\n", isr);
	}

	if (isr & I2C_ISR_STOPF) {
// 		printf("stop dir%d\n", !!(isr & I2C_ISR_DIR));
		if(!(isr & I2C_ISR_DIR)) {
			handle_i2c_bufs();
		}

		I2C_ICR(I2C_BUS) = I2C_ICR_STOPCF;
		I2C_TXDR(I2C_BUS) = 0; //hack to avoid another txis interrupt at the end
	}

	if (isr & I2C_ISR_NACKF) {
// 		printf("nack\n");
		I2C_ICR(I2C_BUS) = I2C_ICR_NACKCF;
	}

	if (isr & 0x3f00) {
		i++;
		if(i>10) {
			while(1) {
// 				printf("i2c error 0x%08lx\n", isr);
			}
		}
	}

}
