#if !defined(_CROSTP_CLIENT_H_)
#define _CROSTP_CLIENT_H_

#include <windows.h>
#include "croskblightcommon.h"

typedef struct _croskblight_client_t* pcroskblight_client;

USHORT getVendorID();

pcroskblight_client croskblight_alloc(void);

void croskblight_free(pcroskblight_client vmulti);

BOOL croskblight_connect(pcroskblight_client vmulti);

void croskblight_disconnect(pcroskblight_client vmulti);

BOOL croskblight_read_message(pcroskblight_client vmulti, CrosKBLightGetLightReport* pReport);

BOOL croskblight_write_message(pcroskblight_client vmulti, CrosKBLightSettingsReport* pReport);

#endif