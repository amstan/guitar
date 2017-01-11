/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* STM32F072-discovery board configuration */

#ifndef __CROS_EC_BOARD_H
#define __CROS_EC_BOARD_H
/* Prepare UART */
#undef CONFIG_UART_TX_BUF_SIZE
#define CONFIG_UART_TX_BUF_SIZE 1024

#undef CONFIG_UART_RX_DMA
#undef CONFIG_UART_TX_DMA

/* 48 MHz SYSCLK clock frequency */
#define CPU_CLOCK 48000000

/* Optional features */
#define CONFIG_STM_HWTIMER32
#define CONFIG_HW_CRC
#define CONFIG_I2C
#define CONFIG_I2C_SLAVE
#undef CONFIG_WATCHDOG_HELP
#undef CONFIG_LID_SWITCH

#define I2C_PORT_SLAVE  0
#define I2C_PORT_EC I2C_PORT_SLAVE

/* slave address for host commands */
#ifdef HAS_TASK_HOSTCMD
#define CONFIG_HOSTCMD_I2C_SLAVE_ADDR 0x3c
#endif

/*
 * Allow dangerous commands all the time, since we don't have a write protect
 * switch.
 */
#define CONFIG_SYSTEM_UNLOCKED

#ifndef __ASSEMBLER__

/* Timer selection */
#define TIM_CLOCK32 2

#include "gpio_signal.h"

#endif /* !__ASSEMBLER__ */
#endif /* __CROS_EC_BOARD_H */
