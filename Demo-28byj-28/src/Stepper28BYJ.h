/*
 * Stepper28BYJ.h
 *
 * Stepper Motor driven by ULN2003 driver using FreeRTOS
 *
 *  Created on: 24 Dec 2022
 *      Author: jondurrant
 */

#ifndef FREERTOS_28BYJ_48_SRC_STEPPER28BYJ_H_
#define FREERTOS_28BYJ_48_SRC_STEPPER28BYJ_H_

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "Agent.h"
#include "GPIOObserver.h"
#include "GPIOInputMgr.h"
#include "StepperObserver.h"

#define DELAY 2
#define STEPPER_QUEUE_LEN 10
#define STEPS28BYJ 2048

class Stepper28BYJ : public Agent, public GPIOObserver {
public:
	/***
	 * Constructor
	 * @param gp1 - GPIO Pad
	 * @param gp2 - GPIO Pad
	 * @param gp3 - GPIO Pad
	 * @param gp4 - GPIO Pad
	 * @param slotGP - GPIO Pad for slot detector
	 */
	Stepper28BYJ(uint8_t gp1, uint8_t gp2, uint8_t gp3, uint8_t gp4, uint8_t slotGP );
	virtual ~Stepper28BYJ();

	/***
	 * Step motor through x full steps
	 * Possitive is clockwise, negative is counter clockwise
	 * @param step - Step count >0 CW, < 0 CCW
	 * @param rpm - Revolutions per mininute (under 14 to work)
	 * RPM 0 is maximum speed
	 */
	uint32_t step(int16_t step, int16_t rpm);


	/***
	 * Calibrate the disk and leave at possition zero
	 */
	uint32_t calibrate();

	/***
	 * Step to a possition
	 * @param pos = Possition between 0 and 4047
	 * @param rpm - speed, 0 to 14. 0 is max speed
	 * @param cw - clockwise
	 */
	uint32_t stepTo(int16_t pos, int16_t rpm, bool cw);


	/***
	 * handle GPIO  events
	 * @param gpio - GPIO number
	 * @param events - Event
	 */
	virtual void handleGPIO(uint gpio, uint32_t events);


	/***
	 * Set Observer
	 * @param obs
	 */
	void setObserver(StepperObserver *obs);

protected:

	/***
	 * Run loop for the agent.
	 */
	virtual void run();


	/***
	 * Get the static depth required in words
	 * @return - words
	 */
	virtual configSTACK_DEPTH_TYPE getMaxStackSize();

private:
	/***
	 * Initialise the GPIO
	 */
	void init();

	/***
	 * Process request from the queue
	 */
	uint32_t processQueue();

	/***
	 * Calculate the pos of stepper based on modulus maths
	 * @param pos
	 * @return
	 */
	int16_t modPos(int16_t pos);

	/***
	 * Split RPM speed as a set of four delays over the four phases
	 * @param rpm - 0 will set to maximum speed
	 */
	void setDelay(int16_t rpm);

	uint8_t pGPPAD[4];


	//Queue of commands
	QueueHandle_t xCmdQ;

	int16_t xTargetPos = 0;
	int16_t xPos = 0;
	uint16_t xDelay[4] = {2,2,2,2};
	bool xClockwise = true;

	uint8_t xSlotGP;

	StepperObserver *pObserver = NULL;
	uint32_t xNextId = 1;
};

#endif /* FREERTOS_28BYJ_48_SRC_STEPPER28BYJ_H_ */
