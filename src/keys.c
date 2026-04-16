#include "../config/keymap_config.h"
#include "keys.h"

typedef enum { KEY_PRESSED = 0, KEY_RELEASED = 1 } key_state_type_t;
#define DEBOUNCE_MS 5

static bool timer_fired_flag = false;
static repeating_timer_t timer;


static unsigned STATE[KEY_COUNT];
static unsigned DEBOUNCE[KEY_COUNT];

bool callback(repeating_timer_t *rt) {
    timer_fired_flag = true;
    return true;
}
void keys_init() {
    for(int i=0; i<KEY_COUNT; i++) {
        gpio_init(KEY_PINS[i]);
        gpio_set_dir(KEY_PINS[i], GPIO_IN);
    }
    
    add_repeating_timer_ms(-1, callback, NULL, &timer);
    return;
}


#define EVENT_BUF_SIZE 16
static key_event_t event_buf[EVENT_BUF_SIZE];
static unsigned head = 0;
static unsigned tail = 0;
static void queue_push(key_event_t event) {
    if((tail + 1) % EVENT_BUF_SIZE == head) return;
    event_buf[tail] = event;
    tail = (tail+1)%EVENT_BUF_SIZE;
}
bool keys_get_event(key_event_t *event) {
    if(head==tail) return false;
    key_event_t ret = event_buf[head];
    head = (head + 1) % EVENT_BUF_SIZE;
    *event = ret;
    return true;

}
void keys_scan() {
    if(!timer_fired_flag) return;
    
    bool input;
    for(int i=0; i<KEY_COUNT; i++) {
        input = gpio_get(KEY_PINS[i]);
        if(input != STATE[i]) {
            DEBOUNCE[i]++;
        } else {
            DEBOUNCE[i] = 0;
        }

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
