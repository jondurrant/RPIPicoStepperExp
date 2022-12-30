/**
 * Jon Durrant.
 *
 * Stepper Exp
 */

#include "pico/stdlib.h"

#define DELAY 2

#define LED 0

#include <stdio.h>



int main() {

	bool on = false;

	uint8_t stepPads[4] = {2,3,4,5};

	uint8_t sequence[4] = {3, 6, 12, 9};


	uint8_t halfSeq[8] = {3, 7, 6, 15, 12, 13, 9, 11  };

	uint8_t s;

	stdio_init_all();

	sleep_ms(2000);
	printf("GO\n");

	gpio_init(LED);
	gpio_set_dir(LED, GPIO_OUT);
	gpio_put(LED, on);

	for (uint8_t i=0; i < 4; i++){
		gpio_init(stepPads[i]);
		gpio_set_dir(stepPads[i], GPIO_OUT);
		gpio_put(stepPads[i], 0);
	}


	for(;;) {
		for (uint8_t i=0; i < 8; i++){

			s = halfSeq[i]; //sequence[i];
			for (uint8_t b = 0; b < 4; b++){
				uint8_t m = 1 << b;
				if ((m & s) > 0){
					gpio_put(stepPads[b], 1);
				} else {
					gpio_put(stepPads[b], 0);
				}

			}
			sleep_ms(DELAY);

		}

		on = ! on;
		gpio_put(LED, on);
	}

}
