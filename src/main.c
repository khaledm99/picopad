#include "pico/stdlib.h"

int main(void) {
	const uint LED_PIN = 25;
    const uint BUTTON_ZERO = 15;
    uint input;
	gpio_init(LED_PIN);
    gpio_init(BUTTON_ZERO);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_set_dir(BUTTON_ZERO, GPIO_IN);

	while (true) {
        input = gpio_get(BUTTON_ZERO);
        if(input == 0) {
            gpio_put(LED_PIN, 1);
        } else {
            gpio_put(LED_PIN, 0);
        }
		//sleep_ms(500);
	}
}

