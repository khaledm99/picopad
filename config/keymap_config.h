#pragma once

#include "class/hid/hid.h"

// Number of keys on macropad
#define KEY_COUNT  4

// RPi Pico GPIO pins for each key
static const unsigned KEY_PINS[KEY_COUNT] = {15,14,13,12};

// Assigned HID Key Codes for each key
static const unsigned KEY_MAP[KEY_COUNT] = 
    {HID_KEY_0, HID_KEY_1, HID_KEY_2, HID_KEY_3};
