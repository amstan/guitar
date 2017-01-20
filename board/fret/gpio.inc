/* -*- mode:c -*-
 *
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Declare symbolic names for all the GPIOs that we care about.
 * Note: Those with interrupt handlers must be declared first. */

/* Outputs */
GPIO(LED_WS2812, PIN(A, 7), GPIO_OUT_LOW)

/*
 * I2C pins should be configured as inputs until I2C module is
 * initialized. This will avoid driving the lines unintentionally.
 */
GPIO(SLAVE_I2C_SCL,       PIN(B, 6), GPIO_INPUT)
GPIO(SLAVE_I2C_SDA,       PIN(B, 7), GPIO_INPUT)

ALTERNATE(PIN_MASK(B, 0xE000), 0, MODULE_SPI_FLASH, 0)

/* Unimplemented signals which we need to emulate for now */
UNIMPLEMENTED(ENTERING_RW)
UNIMPLEMENTED(WP_L)

ALTERNATE(PIN_MASK(B, 0x00c0), 1, MODULE_I2C, 0)  /* I2C Slave:PA14/15 */
