/* 
 * libRobotDev
 * RDButton.h
 * Purpose: Abstracts all micro button interface functions
 * Created: 25/07/2014 11:52:35 AM
 * Author(s): Jerry Luck, Jeremy Pearson
 * Status: UNTESTED
 */ 

#include <avr/io.h>

#include "RDPinDefs.h"
#include "RDUtil.h"

#ifndef RDBUTTON_H_
#define RDBUTTON_H_

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

#endif // RDBUTTON_H_

