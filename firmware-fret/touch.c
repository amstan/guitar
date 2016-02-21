#include <libopencm3/stm32/tsc.h>

#define S1_PORT GPIOA
#define S1_P GPIO2 //TS_G1_IO3
#define S1_C GPIO3 //TS_G1_IO4

#define S2_PORT GPIOA
#define S2_P GPIO6 //TS_G2_IO3
#define S2_C GPIO7 //TS_G2_IO4

#define S3_PORT GPIOB
#define S3_P GPIO0 //TS_G3_IO2
#define S3_C GPIO1 //TS_G3_IO3

uint16_t history[1000][3];
uint16_t max[3];
uint16_t value[3];

int touch_read(void) {
// 	gpio_clear(S2_PORT, S2_C);
// 	gpio_mode_setup(S2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, S2_C);
// 	msleep(50);
// 	gpio_mode_setup(S2_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, S2_C);

	gpio_mode_setup(S1_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, S1_C | S1_P);
	gpio_set_af(S1_PORT, GPIO_AF3, S1_C | S1_P);

	gpio_mode_setup(S2_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, S2_C | S2_P);
	gpio_set_af(S2_PORT, GPIO_AF3, S2_C | S2_P);

	gpio_mode_setup(S3_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, S3_C | S3_P);
	gpio_set_af(S3_PORT, GPIO_AF3, S3_C | S3_P);

	TSC_CR |= TSC_CR_START;
	while(TSC_CR & TSC_CR_START);

// 	while((TSC_IOGCSR & (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3))) != (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3)));

	for (unsigned int i = 0; i < 3; i++) {
		value[i] = TSC_IOGxCR(i+1);
		history[(1000-1)][i] = value[i];

		max[i] = 0;
		for (uint16_t h = 0; h < (1000 - 1); h++) {
			if (max[i] < history[h][i])
				max[i] = history[h][i];

			history[h][i] = history[h+1][i];
		}

// 		max[0] = 3043;
// 		max[1] = 3452;
// 		max[2] = 3651;

		value[i] = max[i] - value[i];
		if(value[i] > 32000)
			value[i] = 0;
// 		if(value[i] >= 256)
// 			value[i] = 256;
	}
}

void touch_init(void) {
	rcc_periph_clock_enable(RCC_TSC);

	TSC_CR |= 0b1111 << TSC_CR_CTPH_SHIFT; //charge transfer pulse high
	TSC_CR |= 0b1111 << TSC_CR_CTPL_SHIFT; //charge transfer pulse low
	TSC_CR |= 0b1111111 << TSC_CR_SSD_SHIFT; //spread spectrum deviation
	TSC_CR |= 0b110 << TSC_CR_MCV_SHIFT; //max count value
	TSC_CR |= 0b000 << TSC_CR_PGPSC_SHIFT; //clock prescaler

	TSC_IOSCR |= TSC_IOSCR_G1(4) | TSC_IOSCR_G2(4) | TSC_IOSCR_G3(3); //sampling capacitors selection per group
	TSC_IOCCR |= TSC_IOCCR_G1(3) | TSC_IOCCR_G2(3) | TSC_IOCCR_G3(2); //channel selection per grou

	TSC_IOGCSR |= TSC_IOGCSR_GxE(1) | TSC_IOGCSR_GxE(2) | TSC_IOGCSR_GxE(3); //enable aquision on the channels
	//statuses: TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3);

	TSC_CR |= TSC_CR_TSCE; //enable touch sensing controller

	//get some history
	for(unsigned int i=0; i<100; i++) {
		touch_read();
	}
}
