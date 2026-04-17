#pragma once

#include "keys.h"

// hid report accepts 6 keys max
#define MAX_HID_KEYS 6

void consume_event(key_event_t event);

void capture_active_set(uint8_t hid_buf[MAX_HID_KEYS]);

