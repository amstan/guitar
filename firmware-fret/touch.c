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

#define MAXHISTORY 300
uint16_t history[MAXHISTORY][6];
uint16_t max[6];
uint16_t touch_values[6];

int touch_group_read(uint32_t channels, uint16_t *g1, uint16_t *g4) {
	TSC_CR &= ~TSC_CR_TSCE;
	TSC_IOCCR = channels; //channel selection per group
	TSC_CR |= TSC_CR_TSCE;
	msleep(1); //TODO: Figure out a way to do switch channels without a delay

// 	cm_disable_interrupts(); //TODO: Necessary?
	TSC_CR |= TSC_CR_START;
	while(TSC_CR & TSC_CR_START);
// 	cm_enable_interrupts();

// 	while((TSC_IOGCSR & (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3))) != (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3)));

	*g1 = TSC_IOGxCR(1);
	*g4 = TSC_IOGxCR(4);
}

void touch_read(void) {
	uint16_t values[6];
	touch_group_read(TSC_IOCCR_G1(1) | TSC_IOCCR_G4(1), &values[0], &values[3]);
	touch_group_read(TSC_IOCCR_G1(2) | TSC_IOCCR_G4(2), &values[1], &values[4]);
	touch_group_read(TSC_IOCCR_G1(3) | TSC_IOCCR_G4(3), &values[2], &values[5]);

	for(unsigned int i = 0; i < 6; i++) {
		history[(MAXHISTORY-1)][i] = values[i];

		max[i] = 0;
		for (uint16_t h = 0; h < (MAXHISTORY - 1); h++) {
				if (max[i] < history[h][i])
						max[i] = history[h][i];
				history[h][i] = history[h+1][i];
		}
		values[i] = max[i] - values[i];
		if(values[i] > 32000)
				values[i] = 0;
		values[i] /= 6;
		if(values[i] > 255)
			values[i] = 255;
		touch_values[i] = values[i];
	}
}

void touch_init(void) {
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO9 | GPIO10 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF3, GPIO0 | GPIO1 | GPIO2 | GPIO3| GPIO9 | GPIO10 | GPIO11 | GPIO12);

// 	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO0 | GPIO1 | GPIO2 | GPIO3);
// 	gpio_set_af(GPIOA, GPIO_AF3, GPIO0 | GPIO1 | GPIO2 | GPIO3);

	rcc_periph_clock_enable(RCC_TSC);

	TSC_CR |= 0b1111 << TSC_CR_CTPH_SHIFT; //charge transfer pulse high
	TSC_CR |= 0b1111 << TSC_CR_CTPL_SHIFT; //charge transfer pulse low
	TSC_CR |= 0b1111111 << TSC_CR_SSD_SHIFT; //spread spectrum deviation
	TSC_CR |= 0b110 << TSC_CR_MCV_SHIFT; //max count value
	TSC_CR |= 0b000 << TSC_CR_PGPSC_SHIFT; //clock prescaler

// 	TSC_IOSCR |= TSC_IOSCR_G1(4) | TSC_IOSCR_G2(4) | TSC_IOSCR_G3(3); //sampling capacitors selection per group
// 	TSC_IOCCR |= TSC_IOCCR_G1(3) | TSC_IOCCR_G2(3) | TSC_IOCCR_G3(2); //channel selection per grou
	TSC_IOSCR |= TSC_IOSCR_G1(4) | TSC_IOSCR_G4(4); //sampling capacitors selection per group

	//TSC_IOGCSR |= TSC_IOGCSR_GxE(1) | TSC_IOGCSR_GxE(2) | TSC_IOGCSR_GxE(3); //enable aquision on the channels
	TSC_IOGCSR |= TSC_IOGCSR_GxE(1) | TSC_IOGCSR_GxE(4); //enable aquision on the channels
	//statuses: TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3);

	TSC_CR |= TSC_CR_TSCE; //enable touch sensing controller

	//get some history
	for(unsigned int i=0; i<100; i++) {
		touch_read();
	}
}
