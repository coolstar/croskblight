// croskblightclient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "croskblightclient.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 3) {
        return 0;
    }

    pcroskblight_client client = croskblight_alloc();
    if (!croskblight_connect(client)) {
        printf("Failed to connect to croskbclient\n");
        return -1;
    }

    CrosKBLightSettingsReport writeReport;
    writeReport.ReportID = REPORTID_KBLIGHT;
    writeReport.SetBrightness = 0;
    writeReport.Brightness = 0;

    if (!croskblight_write_message(client, &writeReport)) {
        printf("Failed to send query msg to croskblight\n");
        return -1;
    }
    CrosKBLightGetLightReport readReport;
    if (!croskblight_read_message(client, &readReport)) {
        printf("Failed to read msg from croskblight\n");
        return -1;
    }

    long argInt = strtol(argv[2], NULL, 10);

    int brightness = readReport.Brightness;

    if (strcmp(argv[1], "=") == 0) {
        brightness = argInt;
    } else if (strcmp(argv[1], "+") == 0) {
        brightness += argInt;
    } else if (strcmp(argv[1], "-") == 0) {
        brightness -= argInt;
    }
    else {
        printf("Arg2 must be =, + or -\n");
        return -1;
    }

    if (brightness < 0) {
        brightness = 0;
    }
    else if (brightness > 100) {
        brightness = 100;
    }

    writeReport.SetBrightness = 1;
    writeReport.Brightness = brightness;

    if (!croskblight_write_message(client, &writeReport)) {
        printf("Failed to send set msg to croskblight\n");
        return -1;
    }
    return 0;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nShowCmd)
{
    return main(__argc, __argv);
}