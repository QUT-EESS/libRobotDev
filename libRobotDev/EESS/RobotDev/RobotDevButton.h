/* libRobotDev
 * RobotDevButton.h
 * Purpose: Abstracts all micro button interface functions
 * Created: 25/07/2014 11:52:35 AM
 * Author(s): Jerry Luck
 * Status: UNTESTED
 */ 

#include <avr/io.h>

#include "RobotDevPinDefs.h"
#include "RobotDevUtil.h"

#ifndef ROBOTDEVBUTTON_H_
#define ROBOTDEVBUTTON_H_

void RDSetupButton(unsigned char pin){
	clr_bit(DDRF, pin);
}

unsigned char RDButtonIsPressed(unsigned char pin){
	if(get_bit(PORTF, pin))
		return 1;
	else
		return 0;
}

void RDButtonWaitForPress(unsigned char pin){
	while(!get_bit(PORTF, pin));
}

void RDButtonWaitForRelease(unsigned char pin){
	while(get_bit(PORF, pin));
}

#endif /* ROBOTDEVBUTTON_H_ */