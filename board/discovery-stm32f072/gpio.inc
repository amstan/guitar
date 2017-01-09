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

ALTERNATE(PIN_MASK(B, 0xE000), 0, MODULE_SPI_FLASH, 0)

/* Unimplemented signals which we need to emulate for now */
UNIMPLEMENTED(ENTERING_RW)
UNIMPLEMENTED(WP_L)

ALTERNATE(PIN_MASK(A, 0x0600), 1, MODULE_UART, 0) /* USART1: PA09/PA10 */
