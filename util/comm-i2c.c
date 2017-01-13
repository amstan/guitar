/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define _GNU_SOURCE /* for asprintf */

#include <errno.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "comm-host.h"

#define EC_I2C_ADDR 0x1e

#define I2C_ADAPTER_NODE "/sys/class/i2c-adapter/i2c-%d/%d-%04x/name"
#define I2C_ADAPTER_NAME "cros-ec-i2c"
#define I2C_MAX_ADAPTER  32
#define I2C_NODE "/dev/i2c-%d"

#define DEBUG 1
#ifdef DEBUG
#define debug(format, arg...) printf(format, ##arg)
#else
#define debug(...)
#endif

static int i2c_fd = -1;

/*
 * Sends a command to the EC (protocol v2).  Returns the command status code, or
 * -1 if other error.
 *
 * Returns >= 0 for success, or negative if error.
 *
 */
static int ec_command_i2c(int command, int version,
				    const void *outdata, int outsize,
				    void *indata, int insize)
{
	int ret, i;
	int resp_len;
	struct ec_host_request rq;
	struct ec_host_response rs;
	static uint8_t req_buf[EC_LPC_HOST_PACKET_SIZE];
	static uint8_t resp_buf[EC_LPC_HOST_PACKET_SIZE];
	uint8_t sum = 0;
	const uint8_t *c;
	uint8_t *d;
	struct i2c_msg i2c_msg[2];
	struct i2c_rdwr_ioctl_data data;

	/* Fail if output size is too big */
	if (outsize + sizeof(rq) > EC_LPC_HOST_PACKET_SIZE)
		return -EC_RES_REQUEST_TRUNCATED;

	/* Fill in request packet */
	rq.struct_version = EC_HOST_REQUEST_VERSION;
	rq.checksum = 0;
	rq.command = command;
	rq.command_version = version;
	rq.reserved = 0;
	rq.data_len = outsize;

	/* Copy data and start checksum */
	for (i = 0, c = (const uint8_t *)outdata; i < outsize; i++, c++) {
		req_buf[sizeof(rq) + 1 + i] = *c;
		sum += *c;
	}

	/* Finish checksum */
	for (i = 0, c = (const uint8_t *)&rq; i < sizeof(rq); i++, c++)
		sum += *c;

	/* Write checksum field so the entire packet sums to 0 */
	rq.checksum = (uint8_t)(-sum);

	/* Copy header */
	for (i = 0, c = (const uint8_t *)&rq; i < sizeof(rq); i++, c++)
		req_buf[1 + i] = *c;

	/* Set command to use protocol v3 */
	req_buf[0] = EC_COMMAND_PROTOCOL_3;

	/*
	 * Transmit all data and receive 2 bytes for return value and response
	 * length.
	 */
// 	ret = i2c_xfer(I2C_PORT_PD_MCU, CONFIG_USB_PD_I2C_SLAVE_ADDR,
// 			&req_buf[0], outsize + sizeof(rq) + 1, &resp_buf[0],
// 			2, I2C_XFER_START);
	data.msgs = i2c_msg;
	data.nmsgs = 2;
	i2c_msg[0].addr = EC_I2C_ADDR;
	i2c_msg[0].flags = 0;
	i2c_msg[0].buf = (char *)req_buf;
	i2c_msg[0].len = outsize + sizeof(rq) + 1;
	i2c_msg[1].addr = EC_I2C_ADDR;
	i2c_msg[1].flags = I2C_M_RD;
	i2c_msg[1].buf = (char *)resp_buf;
	i2c_msg[1].len = 2;
	ret = ioctl(i2c_fd, I2C_RDWR, &data);
	if (ret < 0) {
		fprintf(stderr, "i2c transfer 1 failed: %d (err: %d)\n",
			ret, errno);
		return -EC_RES_ERROR;
	}

	resp_len = resp_buf[1];

// 	if (resp_len > (insize + sizeof(rs))) {
// 		/* Do a dummy read to generate stop condition */
// 		i2c_xfer(I2C_PORT_PD_MCU, CONFIG_USB_PD_I2C_SLAVE_ADDR,
// 			0, 0, &resp_buf[2], 1, I2C_XFER_STOP);
// 		debug("[response size is too large %d > %d]\n",
// 				resp_len, insize + sizeof(rs));
// 		return -EC_RES_RESPONSE_TOO_BIG;
// 	}

	/* Receive remaining data */
// 	ret = i2c_xfer(I2C_PORT_PD_MCU, CONFIG_USB_PD_I2C_SLAVE_ADDR, 0, 0,
// 			&resp_buf[2], resp_len, I2C_XFER_STOP);
	data.msgs = i2c_msg;
	data.nmsgs = 1;
	i2c_msg[0].addr = EC_I2C_ADDR;
	i2c_msg[0].flags = I2C_M_RD;
	i2c_msg[0].buf = (char *)resp_buf + 2;
	i2c_msg[0].len = resp_len;
	ret = ioctl(i2c_fd, I2C_RDWR, &data);
	if (ret < 0) {
		fprintf(stderr, "i2c transfer 2 failed: %d (err: %d)\n",
			ret, errno);
		return -EC_RES_ERROR;
	}

	/* Check for host command error code */
	ret = resp_buf[0];
	if (ret) {
		debug("[command 0x%02x returned error %d]\n", command,
			ret);
		return -ret;
	}

	/* Read back response header and start checksum */
	sum = 0;
	for (i = 0, d = (uint8_t *)&rs; i < sizeof(rs); i++, d++) {
		*d = resp_buf[i + 2];
		sum += *d;
	}

	if (rs.struct_version != EC_HOST_RESPONSE_VERSION) {
		debug("[PD response version mismatch]\n");
		return -EC_RES_INVALID_RESPONSE;
	}

	if (rs.reserved) {
		debug("[PD response reserved != 0]\n");
		return -EC_RES_INVALID_RESPONSE;
	}

	if (rs.data_len > insize) {
		debug("[PD returned too much data]\n");
		return -EC_RES_RESPONSE_TOO_BIG;
	}

	/* Read back data and update checksum */
	resp_len -= sizeof(rs);
	for (i = 0, d = (uint8_t *)indata; i < resp_len; i++, d++) {
		*d = resp_buf[sizeof(rs) + i + 2];
		sum += *d;
	}


	if ((uint8_t)sum) {
		debug("[command 0x%02x bad checksum returned: "
			"%d]\n", command, sum);
		return -EC_RES_INVALID_CHECKSUM;
	}

	/* Return output buffer size */
	return resp_len;
}

static int ec_command_i2c_old(int command, int version,
			  const void *outdata, int outsize,
			  void *indata, int insize)
{
	struct i2c_rdwr_ioctl_data data;
	int ret = -1;
	int i;
	struct ec_host_request rq;
	static uint8_t req_buf[EC_LPC_HOST_PACKET_SIZE];
// 	static uint8_t resp_buf[EC_LPC_HOST_PACKET_SIZE];
// 	int resp_len = sizeof(resp_buf);
	int resp_len;
	uint8_t *resp_buf = NULL;

	const uint8_t *c;
	uint8_t *d;
	uint8_t sum = 0;
	struct i2c_msg i2c_msg[2];

	if (version > 1) {
		fprintf(stderr, "Command versions >1 unsupported.\n");
		return -EC_RES_ERROR;
	}

	if (i2c_fd < 0) {
		fprintf(stderr, "i2c_fd is negative: %d\n", i2c_fd);
		return -EC_RES_ERROR;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, EC_I2C_ADDR) < 0) {
		fprintf(stderr, "Cannot set I2C slave address\n");
		return -EC_RES_ERROR;
	}

	i2c_msg[0].addr = EC_I2C_ADDR;
	i2c_msg[0].flags = 0;
	i2c_msg[1].addr = EC_I2C_ADDR;
	i2c_msg[1].flags = I2C_M_RD;
	data.msgs = i2c_msg;
	data.nmsgs = 2;

	/*
	 * allocate larger packet
	 * (version, command, size, ..., checksum)
	 */
	rq.struct_version = EC_HOST_REQUEST_VERSION;
	rq.checksum = 0;
	rq.command = command;
	rq.command_version = version;
	rq.reserved = 0;
	rq.data_len = outsize;

		/* Copy data and start checksum */
	for (i = 0, c = (const uint8_t *)outdata; i < outsize; i++, c++) {
		req_buf[sizeof(rq) + 1 + i] = *c;
		sum += *c;
	}

	/* Finish checksum */
	for (i = 0, c = (const uint8_t *)&rq; i < sizeof(rq); i++, c++)
		sum += *c;

	/* Write checksum field so the entire packet sums to 0 */
	rq.checksum = (uint8_t)(-sum);

	/* Copy header */
	for (i = 0, c = (const uint8_t *)&rq; i < sizeof(rq); i++, c++)
		req_buf[1 + i] = *c;

	/* Set command to use protocol v3 */
	req_buf[0] = EC_COMMAND_PROTOCOL_3;

	i2c_msg[0].len = outsize + sizeof(rq) + 1;
	i2c_msg[0].buf = (char *)req_buf;

	debug("i2c req %02x:", command);

	/*
	 * allocate larger packet
	 * (result, size, ..., checksum)
	 */
	resp_len = insize + EC_PROTO2_RESPONSE_OVERHEAD;
	resp_buf = calloc(1, resp_len);
	if (!resp_buf)
		goto done;
	i2c_msg[1].len = resp_len;
	i2c_msg[1].buf = (char *)resp_buf;

	/* send command to EC and read answer */
	ret = ioctl(i2c_fd, I2C_RDWR, &data);
	if (ret < 0) {
		fprintf(stderr, "i2c transfer failed: %d (err: %d)\n",
			ret, errno);
		ret = -EC_RES_ERROR;
		goto done;
	}

	/* check response error code */
	ret = resp_buf[0];

	/* TODO(crosbug.com/p/23824): handle EC_RES_IN_PROGRESS case. */

	resp_len = resp_buf[1];
	if (resp_len > insize) {
		fprintf(stderr, "response size is too large %d > %d\n",
				resp_len, insize);
		ret = -EC_RES_ERROR;
		goto done;
	}

	if (ret) {
		debug("command 0x%02x returned an error %d\n",
			 command, i2c_msg[1].buf[0]);
		/* Translate ERROR to -ERROR and offset */
		ret = -EECRESULT - ret;
	} else if (insize) {
		debug("i2c resp  :");
		/* copy response packet payload and compute checksum */
		sum = resp_buf[0] + resp_buf[1];
		for (i = 0, d = indata; i < resp_len; i++, d++) {
			*d = resp_buf[i + 2];
			sum += *d;
			debug(" %02x", *d);
		}
		debug(", sum=%02x\n", sum);

		if (sum != resp_buf[resp_len + 2]) {
			fprintf(stderr, "bad packet checksum\n");
			ret = -EC_RES_ERROR;
			goto done;
		}

		/* Return output buffer size */
		ret = resp_len;
	}
done:
	if (resp_buf)
		free(resp_buf);
	return ret;
}

int comm_init_i2c(void)
{
	char *file_path;
	char buffer[64];
	FILE *f;
	int i;

	(void)ec_command_i2c_old;

	/* find the device number based on the adapter name */
	for (i = 0; i < I2C_MAX_ADAPTER; i++) {
		if (asprintf(&file_path, I2C_ADAPTER_NODE,
			     i, i, EC_I2C_ADDR) < 0)
			return -1;
		f = fopen(file_path, "r");
		if (f) {
			if (fgets(buffer, sizeof(buffer), f) &&
			    !strncmp(buffer, I2C_ADAPTER_NAME, 6)) {
				free(file_path);
				break;
			}
			fclose(f);
		}
		free(file_path);
	}
	if (i == I2C_MAX_ADAPTER) {
		fprintf(stderr, "Cannot find I2C adapter\n");
		return -1;
	}

	if (asprintf(&file_path, I2C_NODE, i) < 0)
		return -1;
	debug("using I2C adapter %s\n", file_path);
	i2c_fd = open(file_path, O_RDWR);
	if (i2c_fd < 0)
		fprintf(stderr, "Cannot open %s : %d\n", file_path, errno);

	free(file_path);

	ec_command_proto = ec_command_i2c;
	ec_max_outsize = ec_max_insize = EC_PROTO2_MAX_PARAM_SIZE;

	return 0;
}
