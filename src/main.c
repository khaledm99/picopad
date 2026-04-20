#include "pico/stdlib.h"
#include "tusb.h"
#include "device/usbd.h"
#include "keys.h"
#include "keymap.h"
#include "usb_hid.h"
#include "class/hid/hid.h"

int main(void) {

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    // Initialize gpio pins for keys and the repeating timer
    keys_init();
    tusb_init();

    // Store the most recently popped key event
    key_event_t event;

    // Stores the active set
    uint8_t hid_buf[MAX_HID_KEYS];

    bool active_set_empty = true;
	while (true) {
        // Scan keys every 1ms
        active_set_empty = true;
        keys_scan();

        // Add and remove keys from active set based on current state
        while(keys_get_event(&event)) {
            consume_event(event);
        }
        for(int i=0; i<MAX_HID_KEYS; i++) {
            if(hid_buf[i] != HID_KEY_NONE) {
                active_set_empty = false;        
            }
        }
        if(!active_set_empty) gpio_put(25, 1);
        else gpio_put(25, 0);

        // Snapshot active set for sending
        capture_active_set(hid_buf);

        // Prepare hid keyboard report with active set
        hid_task(hid_buf);

        // TinyUSB handles the rest through this API call
        tud_task();
	}
}

