#include "battery.h"
#include "common.h"
#include "console.h"
#include "ec_commands.h"
#include "gpio.h"
#include "hooks.h"
#include "host_command.h"
#include "math_util.h"
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

uint16_t *current_touch_values;

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
	#define HISTORY_SIZE 20 /* about 10 minutes */
	const size_t switch_counter_max = 6000; /* about 600ms */
	static uint32_t switch_counter;
	static uint16_t history[HISTORY_SIZE][TOUCH_NUM];
	uint16_t temp_values[TOUCH_NUM];
	static uint16_t return_values[TOUCH_NUM];
	unsigned int i, h;
	uint16_t max[TOUCH_NUM];

#if defined BOARD_DISCOVERY_STM32F072
	touch_group_read(TSC_IOCCR_G1(3) | TSC_IOCCR_G2(3) | TSC_IOCCR_G3(2));
	temp_values[0] = TSC_IOGxCR(1);
	temp_values[1] = TSC_IOGxCR(2);
	temp_values[2] = TSC_IOGxCR(3);
#elif defined BOARD_FRET
	touch_group_read(TSC_IOCCR_G1(1) | TSC_IOCCR_G4(1));
	temp_values[0] = TSC_IOGxCR(1);
	temp_values[3] = TSC_IOGxCR(4);

	touch_group_read(TSC_IOCCR_G1(2) | TSC_IOCCR_G4(2));
	temp_values[1] = TSC_IOGxCR(1);
	temp_values[4] = TSC_IOGxCR(4);

	touch_group_read(TSC_IOCCR_G1(3) | TSC_IOCCR_G4(3));
	temp_values[2] = TSC_IOGxCR(1);
	temp_values[5] = TSC_IOGxCR(4);
#else
#error Board not supported
#endif

	switch_counter++;
	if (switch_counter > switch_counter_max)
		switch_counter = 0;

	for(i = 0; i < TOUCH_NUM; i++) {
		/* keep track of max on the last entry */
		uint16_t *last_touch_entry = &history[(HISTORY_SIZE - 1)][i];
		if (*last_touch_entry < temp_values[i])
			*last_touch_entry = temp_values[i];

		/* If there's a switch, advance the history too */
		if (switch_counter == 0) {
			for (h = 0; h < (HISTORY_SIZE - 1); h++)
					history[h][i] = history[h + 1][i];
			history[HISTORY_SIZE - 1][i] = temp_values[i];
		}

		/* find the max in the history */
		max[i] = 0;
		for (h = 0; h < HISTORY_SIZE; h++)
			if ((max[i] < history[h][i]))
				max[i] = history[h][i];

		temp_values[i] = max[i] - temp_values[i];
		return_values[i] = temp_values[i];
	}

	return return_values;
}

static int captouch_read(int argc, char **argv)
{
	uint8_t i;
	ccprintf("Reading %d pads: ", TOUCH_NUM);

	for (i = 0; i < TOUCH_NUM; i++) {
		ccprintf("0x%04x ", current_touch_values[i]);
	}
	ccprintf("\n");

	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(touchread, captouch_read,
			"",
			"Read capacitive touch values");

int hc_captouch_raw(struct host_cmd_handler_args *args)
{
	uint16_t *p = args->response;

	memcpy(p, current_touch_values, TOUCH_NUM * 2);
	args->response_size = TOUCH_NUM * 2;

	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_CAPTOUCH_RAW,
                     hc_captouch_raw,
                     EC_VER_MASK(0));

void captouch_task(void)
{
	CPRINTS("captouch task starting");

	while (1) {
		current_touch_values = touch_read();
		task_wait_event(10000);
	}
}
