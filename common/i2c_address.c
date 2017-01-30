/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "clock.h"
#include "console.h"
#include "host_command.h"
#include "i2c.h"
#include "system.h"
#include "task.h"
#include "util.h"
#include "registers.h"

#define CPUTS(outstr) cputs(CC_I2C, outstr)
#define CPRINTS(format, args...) cprints(CC_I2C, format, ## args)

static int command_uuid(int argc, char **argv)
{
	ccprintf("0x%08x, 0x%08x, 0x%08x\n",
	        STM32_UNIQUE_ID[0], STM32_UNIQUE_ID[1], STM32_UNIQUE_ID[2]);

	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(uuid, command_uuid,
			"",
			"Print the stm32 unique id");

static int command_i2caddress(int argc, char **argv)
{
	if (argc == 1) {
		ccprintf("I2C_OAR1 address: 0x%02x\n",
		         STM32_I2C_OAR1(I2C_PORT_EC) & 0xff);
	} else if (argc == 2) {
		char *e;
		uint8_t new_address;

		new_address = strtoi(argv[1], &e, 0);
		if (*e)
			return EC_ERROR_PARAM1;

		STM32_I2C_OAR1(I2C_PORT_EC) = 0;
		STM32_I2C_OAR1(I2C_PORT_EC) = 0x8000 | new_address;
	} else {
		return EC_ERROR_PARAM_COUNT;
	}

	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(i2caddress, command_i2caddress,
			"i2caddress [address]",
			"Set i2c address");

int hc_get_uuid(struct host_cmd_handler_args *args)
{
	struct ec_response_uuid *r = args->response;

	r->uuid[0] = STM32_UNIQUE_ID[0];
	r->uuid[1] = STM32_UNIQUE_ID[1];
	r->uuid[2] = STM32_UNIQUE_ID[2];

	args->response_size = sizeof(*r);

	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_UUID,
                     hc_get_uuid,
                     EC_VER_MASK(0));

int hc_i2c_address(struct host_cmd_handler_args *args)
{
	const struct ec_params_i2c_address *params = args->params;

	if (!((args->params_size == 1) || /* address only */
		  (args->params_size == sizeof(*params)))) {
		return EC_RES_INVALID_PARAM;
	}

	if (args->params_size > 1) {
		CPRINTS("Host looking for: 0x%08x 0x%08x 0x%08x",
		         params->uuid[0], params->uuid[1], params->uuid[2]);
		if (params->uuid[0] != STM32_UNIQUE_ID[0])
			return EC_RES_UNAVAILABLE;
		if (params->uuid[1] != STM32_UNIQUE_ID[1])
			return EC_RES_UNAVAILABLE;
		if (params->uuid[2] != STM32_UNIQUE_ID[2])
			return EC_RES_UNAVAILABLE;
		CPRINTS("That's us!");
	}

	CPRINTS("Host told us to change address to 0x%02x", params->new_address);
	STM32_I2C_OAR1(I2C_PORT_EC) = 0;
	STM32_I2C_OAR1(I2C_PORT_EC) = 0x8000 | params->new_address;

	args->response_size = 0;
	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_I2C_ADDRESS,
		     hc_i2c_address,
		     EC_VER_MASK(0));

// #define EC_CMD_UUID 0x0031
//
// struct __ec_align1 ec_response_uuid {
// 	uint32_t uuid[3];
// };
//
// #define EC_CMD_I2C_ADDRESS 0x0032
//
// struct __ec_align1 ec_params_i2c_address {
// 	uint8_t new_address;
// 	uint32_t uuid[3]; // if this is set, only change if uuid matches with us
// };
