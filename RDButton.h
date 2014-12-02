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
 * Sets up specified pin of Port F as an input.
 * @param unsigned char pin
 *     The specific pin of Port F to be set as an input.
 * 
 * @return void
 */
void RDSetupButton(unsigned char pin){
	clr_bit(DDRF, pin);
}

/*
 * Checks if button is pressed.
 *     @param unsigned char pin
 *         The specific pin of Port F that the button is attached to.
 * 
 * @return unsigned char
 *     1 if specified pin of Port F is high (button is pressed),
 *     0 if specified pin of Port F is low (button is not pressed).
 */
unsigned char RDButtonIsPressed(unsigned char pin){
	if(get_bit(PORTF, pin))
		return 1;
	else
		return 0;
}

/*
 * Waits until button is pressed, before it returns.
 *     @param unsigned char pin
 *         The specific pin of Port F that the button is attached to.
 * 
 * @return void
 */
void RDButtonWaitForPress(unsigned char pin){
	while(!get_bit(PORTF, pin));
}

/* 
 * Waits until button is not pressed, before it returns.
 *     @param unsigned char pin
 *         The specific pin of Port F that the button is attached to.
 * 
 * @return void
 */
void RDButtonWaitForRelease(unsigned char pin){
	while(get_bit(PORTF, pin));
}

#endif // RDBUTTON_H_
