/*
 * DemoAgent.cpp
 *
 *  Created on: 29 Dec 2022
 *      Author: jondurrant
 */

#include "DemoAgent.h"
#include <cstdio>
#include <stdlib.h>

#include <stdexcept>
#include <PicoLed.hpp>
#define LEDS_LENGTH 12

DemoAgent::DemoAgent(uint8_t gp, Stepper28BYJ * stepper) {
	pStepper = stepper;

	pStepper->setObserver(this);

	xGP = gp;
}

DemoAgent::~DemoAgent() {
	// TODO Auto-generated destructor stub
}


void DemoAgent::actionComplete(uint32_t actionNum){
	printf("Action complete %ld\n", actionNum);
	xCompleteId = actionNum;
}

/***
 * Run loop for the agent.
 */
void DemoAgent::run(){

	uint8_t start = 0;
	uint8_t end = 0;
	bool cw = true;
	uint8_t dir;
	int8_t d = 1;

	// Initialize LED strip
	auto ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 0, xGP, LEDS_LENGTH, PicoLed::FORMAT_GRB);

	// Keep brightness low as using 3.3v
	ledStrip.setBrightness(10);
	ledStrip.fill( PicoLed::RGB(10, 10, 0xff) );
	ledStrip.setPixelColor((end + 1) % 12, PicoLed::RGB(0, 0xFF, 0));
	ledStrip.show();

	uint32_t actionId = pStepper->calibrate();

	for (;;){
		if (xCompleteId == actionId){
			start = end;
			vTaskDelay(1000);
			end = rand() % 12;
			if (end == start){
				end = (end + 1) % 12;
			}

			dir = rand() % 2;
			if (dir == 0){
				cw = true;
				d = 1;
			} else {
				cw = false;
				d = -1;
			}


			ledStrip.clear();
			ledStrip.setPixelColor((start + 1) %12, PicoLed::RGB(0xFF, 0, 0));
			ledStrip.setPixelColor((end + 1) % 12, PicoLed::RGB(0, 0xFF, 0));

			//Set path as Yellow
			int8_t i = (start + d) % 12;
			if (i < 0) i = 11;
			while (i != end){
				ledStrip.setPixelColor((i + 1) % 12, PicoLed::RGB(0xC0, 0xC0, 0));
				i = (i + d) % 12;
				if (i < 0) i = 11;
			}

			ledStrip.show();
			printf("Move from %d to %d\n", start, end);
			actionId = pStepper->stepTo((2048/12)* end, 7, cw);


		}
		vTaskDelay(1000);
	}

}


/***
 * Get the static depth required in words
 * @return - words
 */
configSTACK_DEPTH_TYPE DemoAgent::getMaxStackSize(){
	return 400;
}
