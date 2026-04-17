#pragma once

#include "class/hid/hid.h"

#define KEY_COUNT  4

static const unsigned KEY_PINS[KEY_COUNT] = {15,14,13,12};

static const unsigned KEY_MAP[KEY_COUNT] = 
    {HID_KEY_0, HID_KEY_1, HID_KEY_2, HID_KEY_4};
