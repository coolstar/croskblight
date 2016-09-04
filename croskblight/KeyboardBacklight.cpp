#include "stdint.h"
#include "unixio.h"

extern "C" {
#include "comm-host.h"
}

int CROSKBLIGHTGetBacklight() {
	struct ec_response_pwm_get_keyboard_backlight r;

	int rv;
	rv = ec_command(EC_CMD_PWM_GET_KEYBOARD_BACKLIGHT, 0,
		NULL, 0, &r, sizeof(r));
	if (rv < 0)
		return rv;
	return r.percent;
}

int CROSKBLIGHTSetBacklight(int percent) {
	struct ec_params_pwm_set_keyboard_backlight p;
	p.percent = percent;
	char *e;
	int rv;

	rv = ec_command(EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT, 0,
		&p, sizeof(p), NULL, 0);
	if (rv < 0)
		return rv;

	return 0;
}