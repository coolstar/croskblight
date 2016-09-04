/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

/* The I/O asm funcs exist only on x86. */
//This driver only runs on x86 lol

#include <stdint.h>
#include <stdio.h>
#include "unixio.h"
#define	MIN(a,b) (((a)<(b))?(a):(b))
#include "comm-host.h"

#define INITIAL_UDELAY 5     /* 5 us */
#define MAXIMUM_UDELAY 10000 /* 10 ms */

static int ec_command_lpc(int command, int version,
	const void *outdata, int outsize,
	void *indata, int insize)
{
	struct ec_lpc_host_args args;
	const uint8_t *d;
	uint8_t *dout;
	int csum;
	int i;

	/* Fill in args */
	args.flags = EC_HOST_ARGS_FLAG_FROM_HOST;
	args.command_version = version;
	args.data_size = outsize;

	/* Initialize checksum */
	csum = command + args.flags + args.command_version + args.data_size;

	/* Write data and update checksum */
	for (i = 0, d = (uint8_t *)outdata; i < outsize; i++, d++) {
		outb(*d, EC_LPC_ADDR_HOST_PARAM + i);
		csum += *d;
	}

	/* Finalize checksum and write args */
	args.checksum = (uint8_t)csum;
	for (i = 0, d = (const uint8_t *)&args; i < sizeof(args); i++, d++)
		outb(*d, EC_LPC_ADDR_HOST_ARGS + i);

	outb(command, EC_LPC_ADDR_HOST_CMD);

	/* Check result */
	i = inb(EC_LPC_ADDR_HOST_DATA);
	if (i) {
		fprintf(stderr, "EC returned error result code %d\n", i);
		return -EECRESULT - i;
	}

	/* Read back args */
	for (i = 0, dout = (uint8_t *)&args; i < sizeof(args); i++, dout++)
		*dout = inb(EC_LPC_ADDR_HOST_ARGS + i);

	/*
	* If EC didn't modify args flags, then somehow we sent a new-style
	* command to an old EC, which means it would have read its params
	* from the wrong place.
	*/
	if (!(args.flags & EC_HOST_ARGS_FLAG_TO_HOST)) {
		fprintf(stderr, "EC protocol mismatch\n");
		return -EC_RES_INVALID_RESPONSE;
	}

	if (args.data_size > insize) {
		fprintf(stderr, "EC returned too much data\n");
		return -EC_RES_INVALID_RESPONSE;
	}

	/* Start calculating response checksum */
	csum = command + args.flags + args.command_version + args.data_size;

	/* Read response and update checksum */
	for (i = 0, dout = (uint8_t *)indata; i < args.data_size;
		i++, dout++) {
		*dout = inb(EC_LPC_ADDR_HOST_PARAM + i);
		csum += *dout;
	}

	/* Verify checksum */
	if (args.checksum != (uint8_t)csum) {
		fprintf(stderr, "EC response has invalid checksum\n");
		return -EC_RES_INVALID_CHECKSUM;
	}

	/* Return actual amount of data received */
	return args.data_size;
}

static int ec_command_lpc_3(int command, int version,
	const void *outdata, int outsize,
	void *indata, int insize)
{
	struct ec_host_request rq;
	struct ec_host_response rs;
	const uint8_t *d;
	uint8_t *dout;
	int csum = 0;
	int i;

	/* Fail if output size is too big */
	if (outsize + sizeof(rq) > EC_LPC_HOST_PACKET_SIZE)
		return -EC_RES_REQUEST_TRUNCATED;

	/* Fill in request packet */
	/* TODO(crosbug.com/p/23825): This should be common to all protocols */
	rq.struct_version = EC_HOST_REQUEST_VERSION;
	rq.checksum = 0;
	rq.command = command;
	rq.command_version = version;
	rq.reserved = 0;
	rq.data_len = outsize;

	/* Copy data and start checksum */
	for (i = 0, d = (const uint8_t *)outdata; i < outsize; i++, d++) {
		outb(*d, EC_LPC_ADDR_HOST_PACKET + sizeof(rq) + i);
		csum += *d;
	}

	/* Finish checksum */
	for (i = 0, d = (const uint8_t *)&rq; i < sizeof(rq); i++, d++)
		csum += *d;

	/* Write checksum field so the entire packet sums to 0 */
	rq.checksum = (uint8_t)(-csum);

	/* Copy header */
	for (i = 0, d = (const uint8_t *)&rq; i < sizeof(rq); i++, d++)
		outb(*d, EC_LPC_ADDR_HOST_PACKET + i);

	/* Start the command */
	outb(EC_COMMAND_PROTOCOL_3, EC_LPC_ADDR_HOST_CMD);

	/* Check result */
	i = inb(EC_LPC_ADDR_HOST_DATA);
	if (i) {
		fprintf(stderr, "EC returned error result code %d\n", i);
		return -EECRESULT - i;
	}

	/* Read back response header and start checksum */
	csum = 0;
	for (i = 0, dout = (uint8_t *)&rs; i < sizeof(rs); i++, dout++) {
		*dout = inb(EC_LPC_ADDR_HOST_PACKET + i);
		csum += *dout;
	}

	if (rs.struct_version != EC_HOST_RESPONSE_VERSION) {
		fprintf(stderr, "EC response version mismatch\n");
		return -EC_RES_INVALID_RESPONSE;
	}

	if (rs.reserved) {
		fprintf(stderr, "EC response reserved != 0\n");
		return -EC_RES_INVALID_RESPONSE;
	}

	if (rs.data_len > insize) {
		fprintf(stderr, "EC returned too much data\n");
		return -EC_RES_RESPONSE_TOO_BIG;
	}

	/* Read back data and update checksum */
	for (i = 0, dout = (uint8_t *)indata; i < rs.data_len; i++, dout++) {
		*dout = inb(EC_LPC_ADDR_HOST_PACKET + sizeof(rs) + i);
		csum += *dout;
	}

	/* Verify checksum */
	if ((uint8_t)csum) {
		fprintf(stderr, "EC response has invalid checksum\n");
		return -EC_RES_INVALID_CHECKSUM;
	}

	/* Return actual amount of data received */
	return rs.data_len;
}

static int ec_readmem_lpc(int offset, int bytes, void *dest)
{
	int i = offset;
	char *s = dest;
	int cnt = 0;

	if (offset >= EC_MEMMAP_SIZE - bytes)
		return -1;

	if (bytes) {				/* fixed length */
		for (; cnt < bytes; i++, s++, cnt++)
			*s = inb(EC_LPC_ADDR_MEMMAP + i);
	}
	else {				/* string */
		for (; i < EC_MEMMAP_SIZE; i++, s++) {
			*s = inb(EC_LPC_ADDR_MEMMAP + i);
			cnt++;
			if (!*s)
				break;
		}
	}

	return cnt;
}

int comm_init_lpc(void)
{
	int i;
	int byte = 0xff;

	/* Request I/O privilege */ //We're a fucking driver, we don't need this.
								/*if (iopl(3) < 0) {
								perror("Error getting I/O privilege");
								return -3;
								}*/

								/*
								* Test if the I/O port has been configured for Chromium EC LPC
								* interface.  Chromium EC guarantees that at least one status bit will
								* be 0, so if the command and data bytes are both 0xff, very likely
								* that Chromium EC is not present.  See crosbug.com/p/10963.
								*/
	byte &= inb(EC_LPC_ADDR_HOST_CMD);
	byte &= inb(EC_LPC_ADDR_HOST_DATA);
	if (byte == 0xff) {
		fprintf(stderr, "Port 0x%x,0x%x are both 0xFF.\n",
			EC_LPC_ADDR_HOST_CMD, EC_LPC_ADDR_HOST_DATA);
		fprintf(stderr,
			"Very likely this board doesn't have a Chromium EC.\n");
		return -4;
	}

	/*
	* Test if LPC command args are supported.
	*
	* The cheapest way to do this is by looking for the memory-mapped
	* flag.  This is faster than sending a new-style 'hello' command and
	* seeing whether the EC sets the EC_HOST_ARGS_FLAG_FROM_HOST flag
	* in args when it responds.
	*/
	if (inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_ID) != 'E' ||
		inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_ID + 1) != 'C') {
		fprintf(stderr, "Missing Chromium EC memory map.\n");
		return -5;
	}

	/* Check which command version we'll use */
	i = inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_HOST_CMD_FLAGS);

	if (i & EC_HOST_CMD_FLAG_VERSION_3) {
		/* Protocol version 3 */
		ec_command_proto = ec_command_lpc_3;
		ec_max_outsize = EC_LPC_HOST_PACKET_SIZE -
			sizeof(struct ec_host_request);
		ec_max_insize = EC_LPC_HOST_PACKET_SIZE -
			sizeof(struct ec_host_response);

	}
	else if (i & EC_HOST_CMD_FLAG_LPC_ARGS_SUPPORTED) {
		/* Protocol version 2 */
		ec_command_proto = ec_command_lpc;
		ec_max_outsize = ec_max_insize = EC_PROTO2_MAX_PARAM_SIZE;

	}
	else {
		fprintf(stderr, "EC doesn't support protocols we need.\n");
		return -5;
	}

	/* Either one supports reading mapped memory directly. */
	ec_readmem = ec_readmem_lpc;
	return 0;
}