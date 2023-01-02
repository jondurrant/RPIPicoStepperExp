/*
 * Stepper28BYJ.cpp
 *
 *  Stepper Motor driven by ULN2003 driver using FreeRTOS
 *
 *
 *  Created on: 24 Dec 2022
 *      Author: jondurrant
 */

#include "Stepper28BYJ.h"
#include <cstdio>

//Structure to use for queue
enum StepperAction {StepperStep, StepperTo, StepperReset};
struct StepperReq {
	StepperAction action;
	int16_t steps;
	int16_t rpm;
	bool cw;
	uint32_t id;
};
typedef struct StepperReq StepperReqT;


/***
 * Constructor
 * @param gp1 - GPIO Pad
 * @param gp2 - GPIO Pad
 * @param gp3 - GPIO Pad
 * @param gp4 - GPIO Pad
 * @param slotGP - GPIO Pad for slot detector
 */
Stepper28BYJ::Stepper28BYJ(
		uint8_t gp1,
		uint8_t gp2,
		uint8_t gp3,
		uint8_t gp4,
		uint8_t slotGP ){
	pGPPAD[0] = gp1;
	pGPPAD[1] = gp2;
	pGPPAD[2] = gp3;
	pGPPAD[3] = gp4;

	xSlotGP = slotGP;

	xCmdQ = xQueueCreate( STEPPER_QUEUE_LEN, sizeof(StepperReqT));
	if (xCmdQ == NULL){
		printf("ERROR: Unable to create Queue\n");
	}
}


/***
 * Destructor
 */
Stepper28BYJ::~Stepper28BYJ() {
	if (xCmdQ != NULL){
		vQueueDelete(xCmdQ);
	}
}

/***
 * Get the static depth required in words
 * @return - words
 */
configSTACK_DEPTH_TYPE Stepper28BYJ::getMaxStackSize(){
	return 200;
}

/***
 * Initialise the GPIO
 */
void Stepper28BYJ::init(){
	for (uint8_t i=0; i < 4; i++){
		gpio_init(pGPPAD[i]);
		gpio_set_dir(pGPPAD[i], GPIO_OUT);
		gpio_put(pGPPAD[i], 0);
	}

	gpio_init(xSlotGP);
	gpio_set_dir(xSlotGP, GPIO_OUT);
	GPIOInputMgr::getMgr()->addObserver(xSlotGP, this);
}

/***
 * Run loop for the agent.
 */
void Stepper28BYJ::run(){

	init();

	uint8_t sequence[4] = {3, 6, 12, 9};
	uint8_t s;
	uint32_t id = 0;

	for(;;) {
		uint32_t r = ulTaskNotifyTake(pdTRUE, 0);
		if (r > 0) {
			printf("Reset Pos at %d\n", xPos);
			xPos = 0;
		}


		if (xPos == xTargetPos){
			if (id != 0){
				if (pObserver != NULL){
					pObserver->actionComplete(id);
				}
			}
			id = processQueue();
		}

		if ((id != 0) && (xPos != xTargetPos)){

			if (xClockwise){
				xPos = modPos(xPos + 1);
			} else {
				xPos = modPos(xPos - 1);
			}
			s = sequence[xPos % STEPPER_SEQ_LEN];
			for (uint8_t b = 0; b < STEPPER_SEQ_LEN; b++){
				uint8_t m = 1 << b;
				if ((m & s) > 0){
					gpio_put(pGPPAD[b], 1);
				} else {
					gpio_put(pGPPAD[b], 0);
				}
			}
			//printf("%d %d %d\n", xPos, xPos % 4, s);
		}
		vTaskDelay(xDelay[xPos % 4]);

	}

}

/***
 * Step motor through x full steps
 * Possitive is clockwise, negative is counter clockwise
 * @param step - Step count >0 CW, < 0 CCW
 * @param rpm - Revolutions per mininute (under 14 to work)
 * RPM 0 is maximum speed
 */
uint32_t Stepper28BYJ::step(int16_t step, int16_t rpm){
	StepperReqT req;

	req.action = StepperStep;
	req.steps = step;
	req.rpm = rpm;
	req.id = xNextId++;

	if (xCmdQ != NULL){
		BaseType_t res = xQueueSendToBack(xCmdQ, (void *)&req, 0);
		if (res != pdTRUE){
			printf("WARNING: Queue is full\n");
		}
	}
	return req.id;
}


/***
 * Process request from the queue
 */
uint32_t Stepper28BYJ::processQueue(){
	StepperReqT req;
	float delay;

	if (xCmdQ == NULL){
		return 0;
	}

	BaseType_t res = xQueueReceive(xCmdQ, (void *)&req, 0);
	if (res == pdTRUE){
		switch(req.action){
		case StepperStep:
			xTargetPos = modPos(xPos + req.steps);
			if (req.steps < 0){
				xClockwise = false;
			} else {
				xClockwise = true;
			}

			setDelay(req.rpm);

			break;
		case StepperTo:
			xTargetPos = req.steps;
			xClockwise = req.cw;
			setDelay(req.rpm);
		default:
			break;
		}
		if (xClockwise){
			printf("Start CW ");
		} else {
			printf("Start CCW ");
		}
		printf("%d at %d. From %d to possition %d\n", req.steps, req.rpm, xPos, xTargetPos);

		return req.id;
	}
	return 0;
}

/***
 * Calculate the pos of stepper based on modulus maths
 * @param pos
 * @return
 */
int16_t Stepper28BYJ::modPos(int16_t pos){
	int16_t p = pos % STEPS28BYJ;
	if (p < 0){
		p = STEPS28BYJ + p;
	}
	return p;
}

/***
 * Split RPM speed as a set of four delays over the four phases
 * @param rpm - 0 will set to maximum speed
 */
void Stepper28BYJ::setDelay(int16_t rpm){
	float delay;

	if (rpm < 1){
		for (uint8_t i=0; i < STEPPER_SEQ_LEN; i++){
			xDelay[i] = DELAY;
		}
		return;
	}

	delay = (60000.0 / (float)(rpm)) / (float)STEPS28BYJ;

	for (uint8_t i=0; i < STEPPER_SEQ_LEN; i++){
		xDelay[i] = delay;
	}

	uint16_t e = (delay * (float)STEPPER_SEQ_LEN) - xDelay[0]*STEPPER_SEQ_LEN;
	switch(e){
	case 1:
		xDelay[1]++;
		break;
	case 2:
		xDelay[1]++;
		xDelay[3]++;
		break;
	case 3:
		xDelay[1]++;
		xDelay[2]++;
		xDelay[3]++;
		break;
	}

	printf("Delay: %d, %d, %d, %d\n",
			xDelay[0], xDelay[1],
			xDelay[2], xDelay[3]);

}

/***
 * handle GPIO  events
 * @param gpio - GPIO number
 * @param events - Event
 */
void Stepper28BYJ::handleGPIO(uint gpio, uint32_t events){

	if (gpio == xSlotGP){
		if ((events & GPIO_IRQ_EDGE_RISE) > 0){
			//printf("Reset Pos at %d\n", xPos);
			xTaskNotifyGive(getTask());
		}
	}
}


/***
 * Calibrate the disk and leave at possition zero
 */
uint32_t Stepper28BYJ::calibrate(){
	step(STEPS28BYJ /2 + 10, 0);
	step(STEPS28BYJ /2 + 10, 0);
	return stepTo(0, 0, false);
}

/***
 * Step to a possition
 * @param pos = Possition between 0 and 4047
 * @param rpm - speed, 0 to 14. 0 is max speed
 * @param cw - clockwise
 */
uint32_t Stepper28BYJ::stepTo(int16_t pos, int16_t rpm, bool cw){
	StepperReqT req;

	req.action = StepperTo;
	req.steps = pos;
	req.rpm = rpm;
	req.cw = cw;
	req.id = xNextId++;

	if (xCmdQ != NULL){
		BaseType_t res = xQueueSendToBack(xCmdQ, (void *)&req, 0);
		if (res != pdTRUE){
			printf("WARNING: Queue is full\n");
		}
	}

	return req.id;
}



/***
 * Set Observer
 * @param obs
 */
void Stepper28BYJ::setObserver(StepperObserver *obs){
	pObserver = obs;
}
