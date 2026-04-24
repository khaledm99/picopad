#pragma once

#include "class/hid/hid.h"

// Number of keys on macropad
#define KEY_COUNT  9

// RPi Pico GPIO pins for each key
static const unsigned KEY_PINS[KEY_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

// Assigned HID Key Codes for each key
static const unsigned KEY_MAP[KEY_COUNT] = 
    {HID_KEY_1, HID_KEY_2, HID_KEY_3, 
     HID_KEY_4, HID_KEY_5, HID_KEY_6,
     HID_KEY_7, HID_KEY_8, HID_KEY_9};
