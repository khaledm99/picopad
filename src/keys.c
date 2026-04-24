#include "../config/keymap_config.h"
#include "keys.h"

typedef enum { KEY_PRESSED = 0, KEY_RELEASED = 1 } key_state_type_t;

// Software switch debounce
#define DEBOUNCE_MS 5

// Flag is set every 1ms and checked on entry to keys_scan()
static bool timer_fired_flag = false;
static repeating_timer_t timer;


// Current accepted key states
static unsigned STATE[KEY_COUNT];

// Debounce timer, one-to-one correspondence to each key,
// incremented on each scan if a key's state is different from accepted
// state, reset to 0 when stabilized
static unsigned DEBOUNCE[KEY_COUNT];

// Timer callback to set flag for scan
bool callback(repeating_timer_t *rt) {
    timer_fired_flag = true;
    return true;
}

// Initialize gpio pins for each key and setup 1ms repeating timer
void keys_init() {
    for(int i=0; i<KEY_COUNT; i++) {
        gpio_init(KEY_PINS[i]);
        gpio_set_dir(KEY_PINS[i], GPIO_IN);
        gpio_pull_up(KEY_PINS[i]);
    }
    
    add_repeating_timer_ms(-1, callback, NULL, &timer);
    return;
}


// Arbitrary value to store events in buffer, max keycount expected
// to be small enough to be effective
#define EVENT_BUF_SIZE 16

static key_event_t event_buf[EVENT_BUF_SIZE];


// Ring buffer for storing events
static unsigned head = 0;
static unsigned tail = 0;
static void queue_push(key_event_t event) {
    if((tail + 1) % EVENT_BUF_SIZE == head) return;
    event_buf[tail] = event;
    tail = (tail+1)%EVENT_BUF_SIZE;
}


// Public
// Pop most recent key event to send to HID layer
// Returns true on successful event return,
// false if no events
bool keys_get_event(key_event_t *event) {
    if(head==tail) return false;
    key_event_t ret = event_buf[head];
    head = (head + 1) % EVENT_BUF_SIZE;
    *event = ret;
    return true;
}

// Scans gpio pins for key presses/releases and stores in event buffer
void keys_scan() {
    // Immediately return if 1ms repeating timer hasn't fired
    if(!timer_fired_flag) return;
    timer_fired_flag = false;

    bool input;
    for(int i=0; i<KEY_COUNT; i++) {
        input = gpio_get(KEY_PINS[i]);

        // Debounce timer
        if(input != STATE[i]) {
            DEBOUNCE[i]++;
        } else {
            DEBOUNCE[i] = 0;
        }

        // Accept key state change if input stable,
        // push event to buffer
        if(DEBOUNCE[i] == DEBOUNCE_MS) {
            DEBOUNCE[i] = 0;
            if(STATE[i] == KEY_PRESSED) {
                STATE[i] = KEY_RELEASED;
                queue_push((key_event_t){i, KEY_EVENT_RELEASE});
            } else {
                STATE[i] = KEY_PRESSED;
                queue_push((key_event_t){i, KEY_EVENT_PRESS});
            }
        }
    }
}
