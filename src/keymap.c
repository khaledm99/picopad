#include "keymap.h"
#include "keymap_config.h"
#include "keys.h"
#include "class/hid/hid.h"

// Private functions
static bool add_to_active_set(uint8_t hid_key);
static void remove_from_active_set(uint8_t hid_key);

// Set of keys currently being pressed.
// we can initialize the whole active set with {0},
// as 0x00 maps to HID_KEY_NONE
static uint8_t active_set[MAX_HID_KEYS] = {0};

// Copy latest active set to buffer for HID report
void capture_active_set(uint8_t hid_buf[MAX_HID_KEYS]) {
    for(int i=0; i<MAX_HID_KEYS; i++) {
        hid_buf[i] = active_set[i];
    }
}

// Takes a key event as argument, adds it's HID key code
// to active set
void consume_event(key_event_t event) {
  if(event.type == KEY_EVENT_PRESS) {
      add_to_active_set(KEY_MAP[event.key_index]);
  }
  if(event.type == KEY_EVENT_RELEASE) {
      remove_from_active_set(KEY_MAP[event.key_index]);
  } 
}

// Adds key to active set and returns true if space available,
// drops key event and returns false if active set is full
static bool add_to_active_set(uint8_t hid_key) {
    for(int i=0; i<MAX_HID_KEYS; i++) {
        if(active_set[i] == HID_KEY_NONE) {
            active_set[i] = hid_key;
            return true;
        }
    }
    return false;
}

static void remove_from_active_set(uint8_t hid_key) {
    for(int i=0; i<MAX_HID_KEYS; i++) {
        if(active_set[i] == hid_key) {
            active_set[i] = HID_KEY_NONE;
        }
    }
}


