/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

/* Host communication command constants for Chrome EC */

#ifndef __CROS_EC_COMMANDS_H
#define __CROS_EC_COMMANDS_H

/*
* Current version of this protocol
*
* TODO(crosbug.com/p/11223): This is effectively useless; protocol is
* determined in other ways.  Remove this once the kernel code no longer
* depends on it.
*/
#define EC_PROTO_VERSION          0x00000002

/* Command version mask */
#define EC_VER_MASK(version) (1UL << (version))

/* I/O addresses for ACPI commands */
#define EC_LPC_ADDR_ACPI_DATA  0x62
#define EC_LPC_ADDR_ACPI_CMD   0x66

/* I/O addresses for host command */
#define EC_LPC_ADDR_HOST_DATA  0x200
#define EC_LPC_ADDR_HOST_CMD   0x204

/* I/O addresses for host command args and params */
/* Protocol version 2 */
#define EC_LPC_ADDR_HOST_ARGS    0x800  /* And 0x801, 0x802, 0x803 */
#define EC_LPC_ADDR_HOST_PARAM   0x804  /* For version 2 params; size is
* EC_PROTO2_MAX_PARAM_SIZE */
/* Protocol version 3 */
#define EC_LPC_ADDR_HOST_PACKET  0x800  /* Offset of version 3 packet */
#define EC_LPC_HOST_PACKET_SIZE  0x100  /* Max size of version 3 packet */

/* The actual block is 0x800-0x8ff, but some BIOSes think it's 0x880-0x8ff
* and they tell the kernel that so we have to think of it as two parts. */
#define EC_HOST_CMD_REGION0    0x800
#define EC_HOST_CMD_REGION1    0x880
#define EC_HOST_CMD_REGION_SIZE 0x80

/* EC command register bit functions */
#define EC_LPC_CMDR_DATA	(1 << 0)  /* Data ready for host to read */
#define EC_LPC_CMDR_PENDING	(1 << 1)  /* Write pending to EC */
#define EC_LPC_CMDR_BUSY	(1 << 2)  /* EC is busy processing a command */
#define EC_LPC_CMDR_CMD		(1 << 3)  /* Last host write was a command */
#define EC_LPC_CMDR_ACPI_BRST	(1 << 4)  /* Burst mode (not used) */
#define EC_LPC_CMDR_SCI		(1 << 5)  /* SCI event is pending */
#define EC_LPC_CMDR_SMI		(1 << 6)  /* SMI event is pending */

#define EC_LPC_ADDR_MEMMAP       0x900
#define EC_MEMMAP_SIZE         255 /* ACPI IO buffer max is 255 bytes */
#define EC_MEMMAP_TEXT_MAX     8   /* Size of a string in the memory map */

/* The offset address of each type of data in mapped memory. */
#define EC_MEMMAP_TEMP_SENSOR      0x00 /* Temp sensors 0x00 - 0x0f */
#define EC_MEMMAP_FAN              0x10 /* Fan speeds 0x10 - 0x17 */
#define EC_MEMMAP_TEMP_SENSOR_B    0x18 /* More temp sensors 0x18 - 0x1f */
#define EC_MEMMAP_ID               0x20 /* 0x20 == 'E', 0x21 == 'C' */
#define EC_MEMMAP_ID_VERSION       0x22 /* Version of data in 0x20 - 0x2f */
#define EC_MEMMAP_BATTERY_VERSION  0x24 /* Version of data in 0x40 - 0x7f */
#define EC_MEMMAP_HOST_CMD_FLAGS   0x27 /* Host cmd interface flags (8 bits) */
/* Unused 0x28 - 0x2f */
#define EC_MEMMAP_SWITCHES         0x30	/* 8 bits */
/* Reserve 0x38 - 0x3f for additional host event-related stuff */
/* Battery values are all 32 bits */
#define EC_MEMMAP_BATT_VOLT        0x40 /* Battery Present Voltage */
#define EC_MEMMAP_BATT_RATE        0x44 /* Battery Present Rate */
#define EC_MEMMAP_BATT_CAP         0x48 /* Battery Remaining Capacity */
#define EC_MEMMAP_BATT_FLAG        0x4c /* Battery State, defined below */
#define EC_MEMMAP_BATT_DCAP        0x50 /* Battery Design Capacity */
#define EC_MEMMAP_BATT_DVLT        0x54 /* Battery Design Voltage */
#define EC_MEMMAP_BATT_LFCC        0x58 /* Battery Last Full Charge Capacity */
#define EC_MEMMAP_BATT_CCNT        0x5c /* Battery Cycle Count */
/* Strings are all 8 bytes (EC_MEMMAP_TEXT_MAX) */
#define EC_MEMMAP_BATT_MFGR        0x60 /* Battery Manufacturer String */
#define EC_MEMMAP_BATT_MODEL       0x68 /* Battery Model Number String */
#define EC_MEMMAP_BATT_SERIAL      0x70 /* Battery Serial Number String */
#define EC_MEMMAP_BATT_TYPE        0x78 /* Battery Type String */
/* Unused 0xa6 - 0xdf */

/*
* ACPI is unable to access memory mapped data at or above this offset due to
* limitations of the ACPI protocol. Do not place data in the range 0xe0 - 0xfe
* which might be needed by ACPI.
*/
#define EC_MEMMAP_NO_ACPI 0xe0

/* Battery bit flags at EC_MEMMAP_BATT_FLAG. */
#define EC_BATT_FLAG_AC_PRESENT   0x01
#define EC_BATT_FLAG_BATT_PRESENT 0x02
#define EC_BATT_FLAG_DISCHARGING  0x04
#define EC_BATT_FLAG_CHARGING     0x08
#define EC_BATT_FLAG_LEVEL_CRITICAL 0x10

/* Switch flags at EC_MEMMAP_SWITCHES */
#define EC_SWITCH_LID_OPEN               0x01
#define EC_SWITCH_POWER_BUTTON_PRESSED   0x02
#define EC_SWITCH_WRITE_PROTECT_DISABLED 0x04
/* Was recovery requested via keyboard; now unused. */
#define EC_SWITCH_IGNORE1		 0x08
/* Recovery requested via dedicated signal (from servo board) */
#define EC_SWITCH_DEDICATED_RECOVERY     0x10
/* Was fake developer mode switch; now unused.  Remove in next refactor. */
#define EC_SWITCH_IGNORE0                0x20

/* Host command interface flags */
/* Host command interface supports LPC args (LPC interface only) */
#define EC_HOST_CMD_FLAG_LPC_ARGS_SUPPORTED  0x01
/* Host command interface supports version 3 protocol */
#define EC_HOST_CMD_FLAG_VERSION_3   0x02

/*****************************************************************************/
/*
* ACPI commands
*
* These are valid ONLY on the ACPI command/data port.
*/

/*
* ACPI Read Embedded Controller
*
* This reads from ACPI memory space on the EC (EC_ACPI_MEM_*).
*
* Use the following sequence:
*
*    - Write EC_CMD_ACPI_READ to EC_LPC_ADDR_ACPI_CMD
*    - Wait for EC_LPC_CMDR_PENDING bit to clear
*    - Write address to EC_LPC_ADDR_ACPI_DATA
*    - Wait for EC_LPC_CMDR_DATA bit to set
*    - Read value from EC_LPC_ADDR_ACPI_DATA
*/
#define EC_CMD_ACPI_READ 0x80

/*
* ACPI Write Embedded Controller
*
* This reads from ACPI memory space on the EC (EC_ACPI_MEM_*).
*
* Use the following sequence:
*
*    - Write EC_CMD_ACPI_WRITE to EC_LPC_ADDR_ACPI_CMD
*    - Wait for EC_LPC_CMDR_PENDING bit to clear
*    - Write address to EC_LPC_ADDR_ACPI_DATA
*    - Wait for EC_LPC_CMDR_PENDING bit to clear
*    - Write value to EC_LPC_ADDR_ACPI_DATA
*/
#define EC_CMD_ACPI_WRITE 0x81

/*
* ACPI Burst Enable Embedded Controller
*
* This enables burst mode on the EC to allow the host to issue several
* commands back-to-back. While in this mode, writes to mapped multi-byte
* data are locked out to ensure data consistency.
*/
#define EC_CMD_ACPI_BURST_ENABLE 0x82

/*
* ACPI Burst Disable Embedded Controller
*
* This disables burst mode on the EC and stops preventing EC writes to mapped
* multi-byte data.
*/
#define EC_CMD_ACPI_BURST_DISABLE 0x83

/*
* ACPI Query Embedded Controller
*
* This clears the lowest-order bit in the currently pending host events, and
* sets the result code to the 1-based index of the bit (event 0x00000001 = 1,
* event 0x80000000 = 32), or 0 if no event was pending.
*/
#define EC_CMD_ACPI_QUERY_EVENT 0x84

/* Valid addresses in ACPI memory space, for read/write commands */

/* Memory space version; set to EC_ACPI_MEM_VERSION_CURRENT */
#define EC_ACPI_MEM_VERSION            0x00
/*
* Test location; writing value here updates test compliment byte to (0xff -
* value).
*/
#define EC_ACPI_MEM_TEST               0x01
/* Test compliment; writes here are ignored. */
#define EC_ACPI_MEM_TEST_COMPLIMENT    0x02

/*
* ACPI addresses 0x20 - 0xff map to EC_MEMMAP offset 0x00 - 0xdf.  This data
* is read-only from the AP.  Added in EC_ACPI_MEM_VERSION 2.
*/
#define EC_ACPI_MEM_MAPPED_BEGIN   0x20
#define EC_ACPI_MEM_MAPPED_SIZE    0xe0

/* Current version of ACPI memory address space */
#define EC_ACPI_MEM_VERSION_CURRENT 2


/*
* This header file is used in coreboot both in C and ACPI code.  The ACPI code
* is pre-processed to handle constants but the ASL compiler is unable to
* handle actual C code so keep it separate.
*/
#ifndef __ACPI__

/*
* Define __packed if someone hasn't beat us to it.  Linux kernel style
* checking prefers __packed over __attribute__((packed)).
*/
#ifndef __packed
#define __packed( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif

/* LPC command status byte masks */
/* EC has written a byte in the data register and host hasn't read it yet */
#define EC_LPC_STATUS_TO_HOST     0x01
/* Host has written a command/data byte and the EC hasn't read it yet */
#define EC_LPC_STATUS_FROM_HOST   0x02
/* EC is processing a command */
#define EC_LPC_STATUS_PROCESSING  0x04
/* Last write to EC was a command, not data */
#define EC_LPC_STATUS_LAST_CMD    0x08
/* EC is in burst mode */
#define EC_LPC_STATUS_BURST_MODE  0x10
/* SCI event is pending (requesting SCI query) */
#define EC_LPC_STATUS_SCI_PENDING 0x20
/* SMI event is pending (requesting SMI query) */
#define EC_LPC_STATUS_SMI_PENDING 0x40
/* (reserved) */
#define EC_LPC_STATUS_RESERVED    0x80

/*
* EC is busy.  This covers both the EC processing a command, and the host has
* written a new command but the EC hasn't picked it up yet.
*/
#define EC_LPC_STATUS_BUSY_MASK \
	(EC_LPC_STATUS_FROM_HOST | EC_LPC_STATUS_PROCESSING)

/* Host command response codes */
enum ec_status {
	EC_RES_SUCCESS = 0,
	EC_RES_INVALID_COMMAND = 1,
	EC_RES_ERROR = 2,
	EC_RES_INVALID_PARAM = 3,
	EC_RES_ACCESS_DENIED = 4,
	EC_RES_INVALID_RESPONSE = 5,
	EC_RES_INVALID_VERSION = 6,
	EC_RES_INVALID_CHECKSUM = 7,
	EC_RES_IN_PROGRESS = 8,		/* Accepted, command in progress */
	EC_RES_UNAVAILABLE = 9,		/* No response available */
	EC_RES_TIMEOUT = 10,		/* We got a timeout */
	EC_RES_OVERFLOW = 11,		/* Table / data overflow */
	EC_RES_INVALID_HEADER = 12,     /* Header contains invalid data */
	EC_RES_REQUEST_TRUNCATED = 13,  /* Didn't get the entire request */
	EC_RES_RESPONSE_TOO_BIG = 14,   /* Response was too big to handle */
	EC_RES_BUS_ERROR = 15,          /* Communications bus error */
	EC_RES_BUSY = 16                /* Up but too busy.  Should retry */
};

/*
* Host event codes.  Note these are 1-based, not 0-based, because ACPI query
* EC command uses code 0 to mean "no event pending".  We explicitly specify
* each value in the enum listing so they won't change if we delete/insert an
* item or rearrange the list (it needs to be stable across platforms, not
* just within a single compiled instance).
*/
enum host_event_code {
	EC_HOST_EVENT_LID_CLOSED = 1,
	EC_HOST_EVENT_LID_OPEN = 2,
	EC_HOST_EVENT_POWER_BUTTON = 3,
	EC_HOST_EVENT_AC_CONNECTED = 4,
	EC_HOST_EVENT_AC_DISCONNECTED = 5,
	EC_HOST_EVENT_BATTERY_LOW = 6,
	EC_HOST_EVENT_BATTERY_CRITICAL = 7,
	EC_HOST_EVENT_BATTERY = 8,
	EC_HOST_EVENT_THERMAL_THRESHOLD = 9,
	EC_HOST_EVENT_THERMAL_OVERLOAD = 10,
	EC_HOST_EVENT_THERMAL = 11,
	EC_HOST_EVENT_USB_CHARGER = 12,
	EC_HOST_EVENT_KEY_PRESSED = 13,
	/*
	* EC has finished initializing the host interface.  The host can check
	* for this event following sending a EC_CMD_REBOOT_EC command to
	* determine when the EC is ready to accept subsequent commands.
	*/
	EC_HOST_EVENT_INTERFACE_READY = 14,
	/* Keyboard recovery combo has been pressed */
	EC_HOST_EVENT_KEYBOARD_RECOVERY = 15,

	/* Shutdown due to thermal overload */
	EC_HOST_EVENT_THERMAL_SHUTDOWN = 16,
	/* Shutdown due to battery level too low */
	EC_HOST_EVENT_BATTERY_SHUTDOWN = 17,

	/* Suggest that the AP throttle itself */
	EC_HOST_EVENT_THROTTLE_START = 18,
	/* Suggest that the AP resume normal speed */
	EC_HOST_EVENT_THROTTLE_STOP = 19,

	/* Hang detect logic detected a hang and host event timeout expired */
	EC_HOST_EVENT_HANG_DETECT = 20,
	/* Hang detect logic detected a hang and warm rebooted the AP */
	EC_HOST_EVENT_HANG_REBOOT = 21,

	/* PD MCU triggering host event */
	EC_HOST_EVENT_PD_MCU = 22,

	/* Battery Status flags have changed */
	EC_HOST_EVENT_BATTERY_STATUS = 23,

	/* EC encountered a panic, triggering a reset */
	EC_HOST_EVENT_PANIC = 24,

	/*
	* The high bit of the event mask is not used as a host event code.  If
	* it reads back as set, then the entire event mask should be
	* considered invalid by the host.  This can happen when reading the
	* raw event status via EC_MEMMAP_HOST_EVENTS but the LPC interface is
	* not initialized on the EC, or improperly configured on the host.
	*/
	EC_HOST_EVENT_INVALID = 32
};
/* Host event mask */
#define EC_HOST_EVENT_MASK(event_code) (1UL << ((event_code) - 1))

/* Arguments at EC_LPC_ADDR_HOST_ARGS */
__packed(struct ec_lpc_host_args {
	uint8_t flags;
	uint8_t command_version;
	uint8_t data_size;
	/*
	* Checksum; sum of command + flags + command_version + data_size +
	* all params/response data bytes.
	*/
	uint8_t checksum;
});

/* Flags for ec_lpc_host_args.flags */
/*
* Args are from host.  Data area at EC_LPC_ADDR_HOST_PARAM contains command
* params.
*
* If EC gets a command and this flag is not set, this is an old-style command.
* Command version is 0 and params from host are at EC_LPC_ADDR_OLD_PARAM with
* unknown length.  EC must respond with an old-style response (that is,
* withouth setting EC_HOST_ARGS_FLAG_TO_HOST).
*/
#define EC_HOST_ARGS_FLAG_FROM_HOST 0x01
/*
* Args are from EC.  Data area at EC_LPC_ADDR_HOST_PARAM contains response.
*
* If EC responds to a command and this flag is not set, this is an old-style
* response.  Command version is 0 and response data from EC is at
* EC_LPC_ADDR_OLD_PARAM with unknown length.
*/
#define EC_HOST_ARGS_FLAG_TO_HOST   0x02


/* Parameter length was limited by the LPC interface */
#define EC_PROTO2_MAX_PARAM_SIZE 0xfc

/* Maximum request and response packet sizes for protocol version 2 */
#define EC_PROTO2_MAX_REQUEST_SIZE (EC_PROTO2_REQUEST_OVERHEAD +	\
				    EC_PROTO2_MAX_PARAM_SIZE)
#define EC_PROTO2_MAX_RESPONSE_SIZE (EC_PROTO2_RESPONSE_OVERHEAD +	\
				     EC_PROTO2_MAX_PARAM_SIZE)

/*****************************************************************************/

/*
* Value written to legacy command port / prefix byte to indicate protocol
* 3+ structs are being used.  Usage is bus-dependent.
*/
#define EC_COMMAND_PROTOCOL_3 0xda

#define EC_HOST_REQUEST_VERSION 3

/* Version 3 request from host */
__packed(struct ec_host_request {
	/* Struct version (=3)
	*
	* EC will return EC_RES_INVALID_HEADER if it receives a header with a
	* version it doesn't know how to parse.
	*/
	uint8_t struct_version;

	/*
	* Checksum of request and data; sum of all bytes including checksum
	* should total to 0.
	*/
	uint8_t checksum;

	/* Command code */
	uint16_t command;

	/* Command version */
	uint8_t command_version;

	/* Unused byte in current protocol version; set to 0 */
	uint8_t reserved;

	/* Length of data which follows this header */
	uint16_t data_len;
});

#define EC_HOST_RESPONSE_VERSION 3

/* Version 3 response from EC */
__packed(struct ec_host_response {
	/* Struct version (=3) */
	uint8_t struct_version;

	/*
	* Checksum of response and data; sum of all bytes including checksum
	* should total to 0.
	*/
	uint8_t checksum;

	/* Result code (EC_RES_*) */
	uint16_t result;

	/* Length of data which follows this header */
	uint16_t data_len;

	/* Unused bytes in current protocol version; set to 0 */
	uint16_t reserved;
});

/*****************************************************************************/
/*
* Notes on commands:
*
* Each command is an 16-bit command value.  Commands which take params or
* return response data specify structs for that data.  If no struct is
* specified, the command does not input or output data, respectively.
* Parameter/response length is implicit in the structs.  Some underlying
* communication protocols (I2C, SPI) may add length or checksum headers, but
* those are implementation-dependent and not defined here.
*/

/*****************************************************************************/
/* General / test commands */

/*
* Get protocol version, used to deal with non-backward compatible protocol
* changes.
*/
#define EC_CMD_PROTO_VERSION 0x00

__packed(struct ec_response_proto_version {
	uint32_t version;
});

/*
* Hello.  This is a simple command to test the EC is responsive to
* commands.
*/
#define EC_CMD_HELLO 0x01

__packed(struct ec_params_hello {
	uint32_t in_data;  /* Pass anything here */
});

__packed(struct ec_response_hello {
	uint32_t out_data;  /* Output will be in_data + 0x01020304 */
});

/* Get version number */
#define EC_CMD_GET_VERSION 0x02

enum ec_current_image {
	EC_IMAGE_UNKNOWN = 0,
	EC_IMAGE_RO,
	EC_IMAGE_RW
};

__packed(struct ec_response_get_version {
	/* Null-terminated version strings for RO, RW */
	char version_string_ro[32];
	char version_string_rw[32];
	char reserved[32];       /* Was previously RW-B string */
	uint32_t current_image;  /* One of ec_current_image */
});

/*
* Read memory-mapped data.
*
* This is an alternate interface to memory-mapped data for bus protocols
* which don't support direct-mapped memory - I2C, SPI, etc.
*
* Response is params.size bytes of data.
*/
#define EC_CMD_READ_MEMMAP 0x07

__packed(struct ec_params_read_memmap {
	uint8_t offset;   /* Offset in memmap (EC_MEMMAP_*) */
	uint8_t size;     /* Size to read in bytes */
});

/* Get protocol information */
#define EC_CMD_GET_PROTOCOL_INFO	0x0b

/* Flags for ec_response_get_protocol_info.flags */
/* EC_RES_IN_PROGRESS may be returned if a command is slow */
#define EC_PROTOCOL_INFO_IN_PROGRESS_SUPPORTED (1 << 0)

__packed(struct ec_response_get_protocol_info {
	/* Fields which exist if at least protocol version 3 supported */

	/* Bitmask of protocol versions supported (1 << n means version n)*/
	uint32_t protocol_versions;

	/* Maximum request packet size, in bytes */
	uint16_t max_request_packet_size;

	/* Maximum response packet size, in bytes */
	uint16_t max_response_packet_size;

	/* Flags; see EC_PROTOCOL_INFO_* */
	uint32_t flags;
});

#endif  /* !__ACPI__ */
/*****************************************************************************/
/*
* Passthru commands
*
* Some platforms have sub-processors chained to each other.  For example.
*
*     AP <--> EC <--> PD MCU
*
* The top 2 bits of the command number are used to indicate which device the
* command is intended for.  Device 0 is always the device receiving the
* command; other device mapping is board-specific.
*
* When a device receives a command to be passed to a sub-processor, it passes
* it on with the device number set back to 0.  This allows the sub-processor
* to remain blissfully unaware of whether the command originated on the next
* device up the chain, or was passed through from the AP.
*
* In the above example, if the AP wants to send command 0x0002 to the PD MCU,
*     AP sends command 0x4002 to the EC
*     EC sends command 0x0002 to the PD MCU
*     EC forwards PD MCU response back to the AP
*/

/* Offset and max command number for sub-device n */
#define EC_CMD_PASSTHRU_OFFSET(n) (0x4000 * (n))
#define EC_CMD_PASSTHRU_MAX(n) (EC_CMD_PASSTHRU_OFFSET(n) + 0x3fff)

/*****************************************************************************/
/*
* Deprecated constants. These constants have been renamed for clarity. The
* meaning and size has not changed. Programs that use the old names should
* switch to the new names soon, as the old names may not be carried forward
* forever.
*/
#define EC_HOST_PARAM_SIZE      EC_PROTO2_MAX_PARAM_SIZE
#define EC_LPC_ADDR_OLD_PARAM   EC_HOST_CMD_REGION1
#define EC_OLD_PARAM_SIZE       EC_HOST_CMD_REGION_SIZE

/*****************************************************************************/
/* PWM commands */

/* Get fan target RPM */
#define EC_CMD_PWM_GET_FAN_TARGET_RPM 0x20

__packed(struct ec_response_pwm_get_fan_rpm {
	uint32_t rpm;
});

/* Set target fan RPM */
#define EC_CMD_PWM_SET_FAN_TARGET_RPM 0x21

/* Version 0 of input params */
__packed(struct ec_params_pwm_set_fan_target_rpm_v0 {
	uint32_t rpm;
});

/* Version 1 of input params */
__packed(struct ec_params_pwm_set_fan_target_rpm_v1 {
	uint32_t rpm;
	uint8_t fan_idx;
});

/* Get keyboard backlight */
#define EC_CMD_PWM_GET_KEYBOARD_BACKLIGHT 0x22
__packed(struct ec_response_pwm_get_keyboard_backlight {
	uint8_t percent;
	uint8_t enabled;
});

/* Set keyboard backlight */
#define EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT 0x23
__packed(struct ec_params_pwm_set_keyboard_backlight {
	uint8_t percent;
});

/* Set target fan PWM duty cycle */
#define EC_CMD_PWM_SET_FAN_DUTY 0x24

/* Version 0 of input params */
__packed(struct ec_params_pwm_set_fan_duty_v0 {
	uint32_t percent;
});

/* Version 1 of input params */
__packed(struct ec_params_pwm_set_fan_duty_v1 {
	uint32_t percent;
	uint8_t fan_idx;
});

#endif  /* __CROS_EC_COMMANDS_H */