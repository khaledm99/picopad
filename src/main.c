#include "pico/stdlib.h"
#include "keys.h"

int main(void) {
	const uint LED_PIN = 25;
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

    keys_init();

    key_event_t event;
	while (true) {
        keys_scan();
        while(keys_get_event(&event)) {
            if(event.type == KEY_EVENT_PRESS) gpio_put(LED_PIN, 1);
            if(event.type == KEY_EVENT_RELEASE) gpio_put(LED_PIN, 0);
        }
	}
}

