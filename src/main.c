#include "pico/stdlib.h"
#include "keys.h"
#include "keymap.h"
#include "class/hid/hid.h"

int main(void) {
	const uint LED_PIN = 25;
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

    keys_init();

    key_event_t event;
    uint8_t hid_buf[MAX_HID_KEYS];
    bool buf_empty_flag = true;
	while (true) {
        keys_scan();
        while(keys_get_event(&event)) {
            consume_event(event);
        }
        capture_active_set(hid_buf);
        for(int i=0; i<MAX_HID_KEYS; i++) {
            if(hid_buf[i] != HID_KEY_NONE) {
                buf_empty_flag = false;
                break;
            }
            buf_empty_flag = true;
        }
        if(buf_empty_flag) gpio_put(LED_PIN, 0);
        else gpio_put(LED_PIN, 1);
	}
}

