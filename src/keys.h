#pragma once
#include "pico/stdlib.h"

typedef enum { KEY_EVENT_PRESS, KEY_EVENT_RELEASE } key_event_type_t;

typedef struct {
    uint8_t key_index;
    key_event_type_t type;
} key_event_t;

// Initialize all GPIO inputs for the keys
void keys_init();
void keys_scan();
bool keys_get_event(key_event_t *event);


