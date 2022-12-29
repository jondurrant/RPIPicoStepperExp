/*
 * DemoAgent.h
 *
 *  Created on: 29 Dec 2022
 *      Author: jondurrant
 */

#ifndef DEMO_28BYJ_28_SRC_DEMOAGENT_H_
#define DEMO_28BYJ_28_SRC_DEMOAGENT_H_

#include "Agent.h"
#include "StepperObserver.h"
#include "Stepper28BYJ.h"
#include "pico/stdlib.h"

class DemoAgent : public Agent, public StepperObserver{
public:
	DemoAgent(uint8_t gp, Stepper28BYJ * stepper);
	virtual ~DemoAgent();

	virtual void actionComplete(uint32_t actionNum);


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

	Stepper28BYJ * pStepper = NULL;

	uint32_t xCompleteId = 0;

	uint8_t xGP;

};

#endif /* DEMO_28BYJ_28_SRC_DEMOAGENT_H_ */
