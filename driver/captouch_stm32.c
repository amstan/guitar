#include "battery.h"
#include "common.h"
#include "console.h"
#include "ec_commands.h"
#include "gpio.h"
#include "hooks.h"
#include "host_command.h"
#include "registers.h"
#include "system.h"
#include "task.h"
#include "timer.h"
#include "util.h"
#include "captouch_stm32.h"

/* Console output macros */
#define CPUTS(outstr) cputs(CC_CAPTOUCH, outstr)
#define CPRINTS(format, args...) cprints(CC_CAPTOUCH, format, ## args)

#if defined BOARD_DISCOVERY_STM32F072
#define TOUCH_NUM 3
#elif defined BOARD_FRET
#define TOUCH_NUM 6
#else
#error Board not supported
#endif

#define MAXHISTORY 300
uint16_t history[MAXHISTORY][TOUCH_NUM];
uint16_t max[TOUCH_NUM];
uint16_t touch_values[TOUCH_NUM];

void touch_init(void) {
	/* Remux GPIOs */
	gpio_config_module(MODULE_CAPTOUCH, 1);

	STM32_RCC_AHBENR |= STM32_RCC_AHBENR_TSCEN;

	TSC_CR |= 0b1111 << TSC_CR_CTPH_SHIFT; //charge transfer pulse high
	TSC_CR |= 0b1111 << TSC_CR_CTPL_SHIFT; //charge transfer pulse low
	TSC_CR |= 0b1111111 << TSC_CR_SSD_SHIFT; //spread spectrum deviation
	TSC_CR |= 0b110 << TSC_CR_MCV_SHIFT; //max count value
	TSC_CR |= 0b000 << TSC_CR_PGPSC_SHIFT; //clock prescaler

#if defined BOARD_DISCOVERY_STM32F072
	TSC_IOSCR |= TSC_IOSCR_G1(4) | TSC_IOSCR_G2(4) | TSC_IOSCR_G3(3); //sampling capacitors selection per group
	TSC_IOGCSR |= TSC_IOGCSR_GxE(1) | TSC_IOGCSR_GxE(2) | TSC_IOGCSR_GxE(3); //enable aquision on the channels
#elif defined BOARD_FRET
	TSC_IOSCR |= TSC_IOSCR_G1(4) | TSC_IOSCR_G4(4); //sampling capacitors selection per group
	TSC_IOGCSR |= TSC_IOGCSR_GxE(1) | TSC_IOGCSR_GxE(4); //enable aquision on the channels
#else
#error Board not supported
#endif

	TSC_CR |= TSC_CR_TSCE; //enable touch sensing controller

	//get some history
// 	for(i=0; i<100; i++) {
// 		touch_read();
// 	}
}
DECLARE_HOOK(HOOK_INIT, touch_init, HOOK_PRIO_INIT_CAPTOUCH);

void touch_group_read(uint32_t channels) {
	TSC_CR &= ~TSC_CR_TSCE;
	TSC_IOCCR = channels; //channel selection per group
	TSC_CR |= TSC_CR_TSCE;
	msleep(1); //TODO: Figure out a way to do switch channels without a delay

// 	cm_disable_interrupts(); //TODO: Necessary?
	TSC_CR |= TSC_CR_START;
	while(TSC_CR & TSC_CR_START);
// 	cm_enable_interrupts();

// 	while((TSC_IOGCSR & (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3))) != (TSC_IOGCSR_GxS(1) | TSC_IOGCSR_GxS(2) | TSC_IOGCSR_GxS(3)));
}

uint16_t* touch_read(void) {
	static uint16_t values[TOUCH_NUM];

#if defined BOARD_DISCOVERY_STM32F072
	touch_group_read(TSC_IOCCR_G1(3) | TSC_IOCCR_G2(3) | TSC_IOCCR_G3(2));
	values[0] = TSC_IOGxCR(1);
	values[1] = TSC_IOGxCR(2);
	values[2] = TSC_IOGxCR(3);
#elif defined BOARD_FRET
	touch_group_read(TSC_IOCCR_G1(1) | TSC_IOCCR_G4(1));
	values[0] = TSC_IOGxCR(1);
	values[3] = TSC_IOGxCR(4);

	touch_group_read(TSC_IOCCR_G1(2) | TSC_IOCCR_G4(2));
	values[1] = TSC_IOGxCR(1);
	values[4] = TSC_IOGxCR(4);

	touch_group_read(TSC_IOCCR_G1(3) | TSC_IOCCR_G4(3));
	values[4] = TSC_IOGxCR(1);
	values[5] = TSC_IOGxCR(4);
#else
#error Board not supported
#endif

// 	for(unsigned int i = 0; i < TOUCH_NUM; i++) {
// 		history[(MAXHISTORY-1)][i] = values[i];
//
// 		max[i] = 0;
// 		for (uint16_t h = 0; h < (MAXHISTORY - 1); h++) {
// 				if (max[i] < history[h][i])
// 						max[i] = history[h][i];
// 				history[h][i] = history[h+1][i];
// 		}
// 		values[i] = max[i] - values[i];
// 		if(values[i] > 32000)
// 				values[i] = 0;
// 		values[i] /= 6;
// 		if(values[i] > 255)
// 			values[i] = 255;
// 		touch_values[i] = values[i];
// 	}

	return values;
}

static int captouch_read(int argc, char **argv)
{
	uint8_t i;
	uint16_t *values;
	ccprintf("Reading %d pads: ", TOUCH_NUM);

	values = touch_read();

	for (i = 0; i < TOUCH_NUM; i++) {
		ccprintf("0x%04x ", values[i]);
	}
	ccprintf("\n");

	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(touchread, captouch_read,
			"",
			"Read capacitive touch values");
