/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include <errno.h>
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unixio.h"

#include "comm-host.h"
#include "ec_commands.h"

int(*ec_command_proto)(int command, int version,
	const void *outdata, int outsize,
	void *indata, int insize);

int(*ec_readmem)(int offset, int bytes, void *dest);

int ec_max_outsize, ec_max_insize;
static int command_offset;

int comm_init_lpc(void);

void set_command_offset(int offset)
{
	command_offset = offset;
}

int ec_command(int command, int version,
	const void *outdata, int outsize,
	void *indata, int insize)
{
	/* Offset command code to support sub-devices */
	return ec_command_proto(command_offset + command, version,
		outdata, outsize,
		indata, insize);
}