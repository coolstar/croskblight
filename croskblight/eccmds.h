#ifndef __CROS_EC_REGS_H__
#define __CROS_EC_REGS_H__

#define BIT(nr) (1UL << (nr))

/* Command version mask */
#define EC_VER_MASK(version) (1UL << (version))

/* Get keyboard backlight */
/* OBSOLETE - Use EC_CMD_PWM_SET_DUTY */
#define EC_CMD_PWM_GET_KEYBOARD_BACKLIGHT 0x0022

#include <pshpack1.h>

struct ec_response_pwm_get_keyboard_backlight {
	UINT8 percent;
	UINT8 enabled;
};

#include <poppack.h>

/* Set keyboard backlight */
/* OBSOLETE - Use EC_CMD_PWM_SET_DUTY */
#define EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT 0x0023

#include <pshpack1.h>

struct ec_params_pwm_set_keyboard_backlight {
	UINT8 percent;
};

#include <poppack.h>

/* Set target fan PWM duty cycle */
#define EC_CMD_PWM_SET_FAN_DUTY 0x0024

/* Version 0 of input params */
#include <pshpack4.h>

struct ec_params_pwm_set_fan_duty_v0 {
	UINT32 percent;
};

#include <poppack.h>

/* Version 1 of input params */
#include <pshpack1.h>

struct ec_params_pwm_set_fan_duty_v1 {
	UINT32 percent;
	UINT8 fan_idx;
};

#include <poppack.h>

#define EC_CMD_PWM_SET_DUTY 0x0025
/* 16 bit duty cycle, 0xffff = 100% */
#define EC_PWM_MAX_DUTY 0xffff

enum ec_pwm_type {
	/* All types, indexed by board-specific enum pwm_channel */
	EC_PWM_TYPE_GENERIC = 0,
	/* Keyboard backlight */
	EC_PWM_TYPE_KB_LIGHT,
	/* Display backlight */
	EC_PWM_TYPE_DISPLAY_LIGHT,
	EC_PWM_TYPE_COUNT,
};

#include <pshpack4.h>

struct ec_params_pwm_set_duty {
	UINT16 duty;     /* Duty cycle, EC_PWM_MAX_DUTY = 100% */
	UINT8 pwm_type;  /* ec_pwm_type */
	UINT8 index;     /* Type-specific index, or 0 if unique */
};

#include <poppack.h>

#define EC_CMD_PWM_GET_DUTY 0x0026

#include <pshpack1.h>

struct ec_params_pwm_get_duty {
	UINT8 pwm_type;  /* ec_pwm_type */
	UINT8 index;     /* Type-specific index, or 0 if unique */
};

#include <poppack.h>

#include <pshpack2.h>

struct ec_response_pwm_get_duty {
	UINT16 duty;     /* Duty cycle, EC_PWM_MAX_DUTY = 100% */
};

#include <poppack.h>

/* Read versions supported for a command */
#define EC_CMD_GET_CMD_VERSIONS 0x0008

/**
 * struct ec_params_get_cmd_versions - Parameters for the get command versions.
 * @cmd: Command to check.
 */
#include <pshpack1.h>
struct ec_params_get_cmd_versions {
	UINT8 cmd;
};
#include <poppack.h>

/**
 * struct ec_params_get_cmd_versions_v1 - Parameters for the get command
 *         versions (v1)
 * @cmd: Command to check.
 */
#include <pshpack2.h>
struct ec_params_get_cmd_versions_v1 {
	UINT16 cmd;
};
#include <poppack.h>

/**
 * struct ec_response_get_cmd_version - Response to the get command versions.
 * @version_mask: Mask of supported versions; use EC_VER_MASK() to compare with
 *                a desired version.
 */
#include <pshpack4.h>
struct ec_response_get_cmd_versions {
	UINT32 version_mask;
};
#include <poppack.h>


/* Get protocol information */
#define EC_CMD_GET_PROTOCOL_INFO	0x0b

/* Flags for ec_response_get_protocol_info.flags */
/* EC_RES_IN_PROGRESS may be returned if a command is slow */
#define EC_PROTOCOL_INFO_IN_PROGRESS_SUPPORTED (1 << 0)

#include <pshpack4.h>

struct ec_response_get_protocol_info {
	/* Fields which exist if at least protocol version 3 supported */

	/* Bitmask of protocol versions supported (1 << n means version n)*/
	UINT32 protocol_versions;

	/* Maximum request packet size, in bytes */
	UINT16 max_request_packet_size;

	/* Maximum response packet size, in bytes */
	UINT16 max_response_packet_size;

	/* Flags; see EC_PROTOCOL_INFO_* */
	UINT32 flags;
};

#include <poppack.h>

#endif /* __CROS_EC_REGS_H__ */