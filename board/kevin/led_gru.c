/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * gru battery LED control - derived from standard policy with PWM
 * color control rather than GPIO.
 */

#include "battery.h"
#include "chipset.h"
#include "charge_state.h"
#include "chipset.h"
#include "hooks.h"
#include "led_common.h"
#include "lid_switch.h"
#include "pwm.h"
#include "util.h"

#define GRU_BAT_LED_PWM PWM_CH_LED_RED

const enum ec_led_id supported_led_ids[] = { EC_LED_ID_BATTERY_LED };

const int supported_led_ids_count = ARRAY_SIZE(supported_led_ids);

enum led_color {
	LED_OFF = 0,
	LED_RED,
	LED_AMBER,
	LED_GREEN,
	LED_COLOR_COUNT  /* Number of colors, not a color itself */
};

/* One LED active at a time. PWM low period determines which LED is active. */
static const int led_color_to_pwm_duty[LED_COLOR_COUNT] = {
	[LED_OFF] =   100,
	[LED_RED] =   0,
	[LED_AMBER] = 80,
	[LED_GREEN] = 10,
};

static int bat_led_set_color(enum led_color color)
{
	pwm_set_duty(GRU_BAT_LED_PWM, led_color_to_pwm_duty[color]);
	return EC_SUCCESS;
}

void led_get_brightness_range(enum ec_led_id led_id, uint8_t *brightness_range)
{
	switch (led_id) {
	case EC_LED_ID_BATTERY_LED:
		brightness_range[EC_LED_COLOR_RED] = 1;
		brightness_range[EC_LED_COLOR_AMBER] = 1;
		brightness_range[EC_LED_COLOR_GREEN] = 1;
		break;
	default:
		/* ignore */
		break;
	}
}

int led_set_brightness(enum ec_led_id led_id, const uint8_t *brightness)
{
	switch (led_id) {
	case EC_LED_ID_BATTERY_LED:
		if (brightness[EC_LED_COLOR_RED] != 0)
			bat_led_set_color(LED_RED);
		else if (brightness[EC_LED_COLOR_AMBER] != 0)
			bat_led_set_color(LED_AMBER);
		else if (brightness[EC_LED_COLOR_GREEN] != 0)
			bat_led_set_color(LED_GREEN);
		else
			bat_led_set_color(LED_OFF);
		break;
	default:
		return EC_ERROR_UNKNOWN;
	}
	return EC_SUCCESS;
}

static void gru_led_set_led(void)
{
	static int second;

	uint8_t green = 0;
	uint8_t red = 0;

	second++;

	if (chipset_in_state(CHIPSET_STATE_ANY_OFF))
		green = 0;
	else if (chipset_in_state(CHIPSET_STATE_ON))
		green = 1;
	else if (chipset_in_state(CHIPSET_STATE_SUSPEND))
		green = (second & 3) ? 0 : 1;

	switch (charge_get_state()) {
	case PWR_STATE_CHARGE:
		red = 1;
		break;
	case PWR_STATE_CHARGE_NEAR_FULL:
		red = 1;
		break;
	case PWR_STATE_DISCHARGE:
		if (charge_get_percent() < 3)
			red = (second & 1) ? 0 : 1;
		else if (charge_get_percent() < 10)
			red = (second & 3) ? 0 : 1;
		else
			red = 0;
		break;
	case PWR_STATE_ERROR:
		red = (second & 1) ? 0 : 1;
		break;
	case PWR_STATE_IDLE: /* External power connected in IDLE. */
		red = 0;
		break;
	default:
		/* Other states don't alter LED behavior */
		break;
	}

	if ((green == 0) && (red == 0))
		bat_led_set_color(LED_OFF);
	if ((green == 0) && (red == 1))
		bat_led_set_color(LED_RED);
	if ((green == 1) && (red == 0))
		bat_led_set_color(LED_GREEN);
	if ((green == 1) && (red == 1))
		bat_led_set_color(LED_AMBER);
}

/* Called by hook task every 1 sec */
static void led_second(void)
{
	if (led_auto_control_is_enabled(EC_LED_ID_BATTERY_LED))
		gru_led_set_led();
}
DECLARE_HOOK(HOOK_SECOND, led_second, HOOK_PRIO_DEFAULT);

