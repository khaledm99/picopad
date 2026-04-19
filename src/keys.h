#pragma once
#include "pico/stdlib.h"

typedef enum { KEY_EVENT_PRESS, KEY_EVENT_RELEASE } key_event_type_t;

typedef struct {
    uint8_t key_index;
    key_event_type_t type;
} key_event_t;

// Initialize all GPIO inputs for the keys,
// start repeating timer which fires every 1ms for scan
void keys_init();

// Scan keys and capture key press/release event in private
// buffer. Every 1ms based on 1ms repeating timer
void keys_scan();

// Pop most recent key event from private buffer and returns
// it through *event argument
bool keys_get_event(key_event_t *event);


