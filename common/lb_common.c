/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Lightbar IC interface
 *
 * Here's the API provided by this file.
 *
 * Looking at it from the outside, the lightbar has four "segments", each of
 * which can be independently adjusted to display a unique color such as blue,
 * purple, yellow, pinkish-white, etc. Segment 0 is on the left (looking
 * straight at it from behind).
 *
 * The lb_set_rgb() and lb_get_rgb() functions let you specify the color of a
 * segment using individual Red, Green, and Blue values in the 0x00 to 0xFF
 * range (see https://en.wikipedia.org/wiki/Web_color for background info).
 *
 * The lb_set_brightness() function provides a simple way to set the intensity,
 * over a range of 0x00 (off) to 0xFF (full brightness). It does this by
 * scaling each RGB value proportionally. For example, an RGB value of #FF8000
 * appears orange. To make the segment half as bright, you could specify a RGB
 * value of #7f4000, or you could leave the RGB value unchanged and just set
 * the brightness to 0x80.
 *
 * That covers most of the lb_* functions found in include/lb_common.h, and
 * those functions are what are used to implement the various colors and
 * sequences for displaying power state changes and other events.
 *
 * The internals are a little more messy.
 *
 * Each segment has three individual color emitters - red, green, and blue. A
 * single emitter may consist of 3 to 7 physical LEDs, but they are all wired
 * in parallel so there is only one wire that provides current for any one
 * color emitter. That makes a total of 12 current control wires for the
 * lightbar: four segments, three color emitters per segment.
 *
 * The ICs that we use each have seven independently adjustable
 * current-limiters. We use six of those current limiters (called "Independent
 * Sink Controls", or "ISC"s ) from each of two ICs to control the 12 color
 * emitters in the lightbar. The ICs are not identical, but they're close
 * enough that we can treat them the same. We call the ICs "controller 0" and
 * "controller 1".
 *
 * For no apparent reason, each Chromebook has wired the ICs and the ISCs
 * differently, so there are a couple of lookup tables that ensure that when we
 * call lb_set_rgb() to make segment 1 yellow, it looks the same on all
 * Chromebooks.
 *
 * Each ISC has a control register to set the amount of current that passes
 * through the color emitter control wire. We need to limit the max current so
 * that the current through each of the emitter's LEDs doesn't exceed the
 * manufacturer's specifications. For example, if a particular LED can't handle
 * more than 5 mA, and the emitter is made up of four LEDs in parallel, the
 * maxiumum limit for that particular ISC would be 20 mA.
 *
 * Although the specified maximum currents are usually similar, the three
 * different colors of LEDs have different brightnesses. For any given current,
 * green LEDs are pretty bright, red LEDS are medium, and blue are fairly dim.
 * So we calibrate the max current per ISC differently, depending on which
 * color it controls.
 *
 * First we set one segment to red, one to green, and one to blue, using the
 * ISC register to allow the max current per LED that the LED manufacturer
 * recommends. Then we adjust the current of the brighter segments downward
 * until all three segments appear equally bright to the eye. The MAX_RED,
 * MAX_BLUE, and MAX_GREEN values are the ISC control register values at this
 * point. This means that if we set all ISCs to their MAX_* values, all
 * segments should appear white.
 *
 * To translate the RGB values passed to lb_set_rgb() into ISC values, we
 * perform two transformations. The color value is first scaled according to
 * the current brightness setting, and then that intensity is scaled according
 * to the MAX_* value for the particular color. The result is the ISC register
 * value to use.
 *
 * To add lightbar support for a new Chromebook, you do the following:
 *
 * 1. Figure out the segment-to-IC and color-to-ISC mappings so that
 *    lb_set_rgb() does the same thing as on the other Chromebooks.
 *
 * 2. Calibrate the MAX_RED, MAX_GREEN, and MAX_BLUE values so that white looks
 *    white, and solid red, green, and blue all appear to be the same
 *    brightness.
 *
 * 3. Use lb_set_rgb() to set the colors to what *should be* the Google colors
 *    (at maximum brightness). Tweak the RGB values until the colors match,
 *    then edit common/lightbar.c to set them as the defaults.
 *
 * 4. Curse because the physical variation between the LEDs prevents you from
 *    getting everything exactly right: white looks bluish, yellow turns
 *    orange at lower brightness, segment 3 has a bright spot when displaying
 *    solid red, etc. Go back to step 2, and repeat until deadline.
 */

#ifdef SECTION_IS_RW

#include "common.h"
#include "console.h"
#include "ec_commands.h"
#include "lb_common.h"
#include "chip/stm32/registers.h"
#include "util.h"

/* Console output macros */
#define CPUTS(outstr) cputs(CC_LIGHTBAR, outstr)
#define CPRINTF(format, args...) cprintf(CC_LIGHTBAR, format, ## args)
#define CPRINTS(format, args...) cprints(CC_LIGHTBAR, format, ## args)

static inline uint8_t scale_abs(int val, int max)
{
	return (val * max)/255;
}

/* This is the overall brightness control. */
static int brightness = 0xc0;

/* So that we can make brightness changes happen instantly, we need to track
 * the current values. The values in the controllers aren't very helpful. */
static uint8_t current[NUM_LEDS][3];

/* Scale 0-255 by brightness */
static inline uint8_t scale(int val, int max)
{
	return scale_abs((val * brightness)/255, max);
}

#include "include/gpio.h"
#include "light_ws2812_cortex.c"

/* Helper function to set one LED color and remember it for later */
static void refresh_leds(void)
{
	volatile int i;
	asm volatile("cpsid i");
	ws2812_sendarray((uint8_t*)current, sizeof(current));
	asm volatile("cpsie i");
	for (i = 0; i < 200; i++);
}

/* LEDs are numbered 0-3, RGB values should be in 0-255.
 * If you specify too large an LED, it sets them all. */
void lb_set_rgb(unsigned int led, int red, int green, int blue)
{
	int i;
	if (led >= NUM_LEDS) {
		for (i = 0; i < NUM_LEDS; i++) {
			current[i][0] = green;
			current[i][1] = red;
			current[i][2] = blue;
		}
		refresh_leds();
	} else {
		current[led][0] = green;
		current[led][1] = red;
		current[led][2] = blue;
		refresh_leds();
	}
}


void lb_set_rgb_all(const uint8_t *ledarray, int length)
{
	int i;
	for (i = 0; i < (length / 3); i++) {
			current[i][0] = ledarray[i * 3 + 1];
			current[i][1] = ledarray[i * 3 + 0];
			current[i][2] = ledarray[i * 3 + 2];
	}
	refresh_leds();
}

/* Get current LED values, if the LED number is in range. */
int lb_get_rgb(unsigned int led, uint8_t *red, uint8_t *green, uint8_t *blue)
{
	if (led < 0 || led >= NUM_LEDS)
		return EC_RES_INVALID_PARAM;

	*green = current[led][0];
	*red = current[led][1];
	*blue = current[led][2];

	return EC_RES_SUCCESS;
}

/* Change current display brightness (0-255) */
void lb_set_brightness(unsigned int newval)
{
// 	int i;
	CPRINTS("LB_bright 0x%02x", newval);
	brightness = newval;
// 	for (i = 0; i < NUM_LEDS; i++)
// 		setrgb(i, current[i][0], current[i][1], current[i][2]);
	refresh_leds();
}

/* Get current display brightness (0-255) */
uint8_t lb_get_brightness(void)
{
	return brightness;
}

/* Initialize the controller ICs after reset */
void lb_init(int use_lock)
{
	CPRINTF("[%T LB_init");
	memset(current, 0, sizeof(current));
	refresh_leds();
	CPRINTF("]\n");
}

/* Just go into standby mode. No register values should change. */
void lb_off(void)
{
	CPRINTS("LB_off");
	memset(current, 0, sizeof(current));
	refresh_leds();
}

/* Come out of standby mode. */
void lb_on(void)
{
	CPRINTS("LB_on");
	refresh_leds();
}

// static const uint8_t dump_reglist[] = {
// 	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
// 	0x08, 0x09, 0x0a,			  0x0f,
// 	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
// 	0x18, 0x19, 0x1a
// };

/* Helper for host command to dump controller registers */
void lb_hc_cmd_dump(struct ec_response_lightbar *out)
{
// 	int i;
// 	uint8_t reg;
//
// 	BUILD_ASSERT(ARRAY_SIZE(dump_reglist) ==
// 		     ARRAY_SIZE(out->dump.vals));
//
// 	for (i = 0; i < ARRAY_SIZE(dump_reglist); i++) {
// 		reg = dump_reglist[i];
// 		out->dump.vals[i].reg = reg;
// 		i2c_lock(I2C_PORT_LIGHTBAR, 1);
// 		out->dump.vals[i].ic0 = controller_read(0, reg);
// 		out->dump.vals[i].ic1 = controller_read(1, reg);
// 		i2c_lock(I2C_PORT_LIGHTBAR, 0);
// 	}
	//TODO
}

/* Helper for host command to write controller registers directly */
void lb_hc_cmd_reg(const struct ec_params_lightbar *in)
{
	//TODO
// 	i2c_lock(I2C_PORT_LIGHTBAR, 1);
// 	controller_write(in->reg.ctrl, in->reg.reg, in->reg.value);
// 	i2c_lock(I2C_PORT_LIGHTBAR, 0);
}

#endif
