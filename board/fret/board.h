/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* STM32F072-discovery board configuration */

#ifndef __CROS_EC_BOARD_H
#define __CROS_EC_BOARD_H
/* Prepare UART */
#undef  CONFIG_UART_CONSOLE
#define CONFIG_UART_CONSOLE 1
#undef  CONFIG_UART_TX_BUF_SIZE
#undef  CONFIG_UART_TX_DMA
#undef  CONFIG_UART_RX_DMA
#define CONFIG_UART_TX_BUF_SIZE 128
#undef  CONFIG_CONSOLE_CMDHELP
#define CONFIG_DEBUG_PRINTF
#define UARTN CONFIG_UART_CONSOLE
#define UARTN_BASE STM32_USART_BASE(CONFIG_UART_CONSOLE)
#define CONFIG_FORCE_CONSOLE_RESUME

/* 48 MHz SYSCLK clock frequency */
#define CPU_CLOCK 48000000

/* Optional features */
#define CONFIG_STM_HWTIMER32
#define CONFIG_HW_CRC
#define CONFIG_I2C
#define CONFIG_I2C_SLAVE
#undef CONFIG_WATCHDOG_HELP
#undef CONFIG_LID_SWITCH

#define CONFIG_TOUCHPAD_STM32

#define I2C_PORT_SLAVE  0
#define I2C_PORT_EC I2C_PORT_SLAVE

/* slave address for host commands */
#ifdef HAS_TASK_HOSTCMD
#define CONFIG_HOSTCMD_I2C_SLAVE_ADDR (0x25 << 1)
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
