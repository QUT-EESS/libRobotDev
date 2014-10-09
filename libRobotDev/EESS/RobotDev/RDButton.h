/* 
 * libRobotDev
 * RDButton.h
 * Purpose: Abstracts all micro button interface functions
 * Created: 25/07/2014 11:52:35 AM
 * Author(s): Jerry Luck, Jeremy Pearson, Darryl Lee
 * Status: UNTESTED
 */ 

#include <avr/io.h>

#include "RDPinDefs.h"
#include "RDUtil.h"

#ifndef RDBUTTON_H_
#define RDBUTTON_H_

/* 
 * Sets up designated pin of Port F as an input.
 */
void RDSetupButton(unsigned char pin){
	clr_bit(DDRF, pin);
}

/*
 * Returns 1 if designated pin of Port F is high, 0 if low.
 */
unsigned char RDButtonIsPressed(unsigned char pin){
	if(get_bit(PORTF, pin))
		return 1;
	else
		return 0;
}

/*
 * Stays in while loop until the designated pin of Port F is low
 */
void RDButtonWaitForPress(unsigned char pin){
	while(!get_bit(PORTF, pin));
}

/* 
 * Stays in while loop until the designated pin of Port F is high
 */
void RDButtonWaitForRelease(unsigned char pin){
	while(get_bit(PORTF, pin));
}

#endif // RDBUTTON_H_