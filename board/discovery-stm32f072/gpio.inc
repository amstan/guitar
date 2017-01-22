/* -*- mode:c -*-
 *
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Declare symbolic names for all the GPIOs that we care about.
 * Note: Those with interrupt handlers must be declared first. */

GPIO_INT(USER_BUTTON, PIN(A, 0), GPIO_INT_FALLING, button_event)

/* Outputs */
GPIO(LED_U, PIN(C, 6), GPIO_OUT_LOW)
GPIO(LED_D, PIN(C, 7), GPIO_OUT_LOW)
GPIO(LED_L, PIN(C, 8), GPIO_OUT_LOW)
GPIO(LED_R, PIN(C, 9), GPIO_OUT_LOW)
GPIO(LED_WS2812, PIN(C, 10), GPIO_OUT_LOW)

/* Capacitive Touch */
GPIO(TOUCH_PAD0, PIN(A,  2), GPIO_INPUT) /* TS_G1_IO3 */
GPIO(TOUCH_CAP0, PIN(A,  3), GPIO_INPUT) /* TS_G1_IO4 */
GPIO(TOUCH_PAD1, PIN(A,  6), GPIO_INPUT) /* TS_G2_IO3 */
GPIO(TOUCH_CAP1, PIN(A,  7), GPIO_INPUT) /* TS_G2_IO4 */
GPIO(TOUCH_PAD2, PIN(B,  0), GPIO_INPUT) /* TS_G3_IO2 */
GPIO(TOUCH_CAP2, PIN(B,  1), GPIO_INPUT) /* TS_G3_IO3 */
ALTERNATE(PIN_MASK(A, 0x00cc), 3, MODULE_CAPTOUCH, 0)
ALTERNATE(PIN_MASK(B, 0x0003), 3, MODULE_CAPTOUCH, 0)

/*
 * I2C pins should be configured as inputs until I2C module is
 * initialized. This will avoid driving the lines unintentionally.
 */
GPIO(SLAVE_I2C_SCL,       PIN(B, 6), GPIO_INPUT)
GPIO(SLAVE_I2C_SDA,       PIN(B, 7), GPIO_INPUT)
ALTERNATE(PIN_MASK(B, 0x00c0), 1, MODULE_I2C, 0)  /* I2C Slave */

/* Unimplemented signals which we need to emulate for now */
UNIMPLEMENTED(ENTERING_RW)
UNIMPLEMENTED(WP_L)

ALTERNATE(PIN_MASK(A, 0x0600), 1, MODULE_UART, 0) /* USART1: PA09/PA10 */
