/* 
 * libRobotDev
 * RDDigital.h
 * Purpose: Abstracts all micro digital functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson
 * Status: UNTESTED
 */ 

#include <avr/io.h>
#include "RDUtil.h"

#ifndef RDDIGITAL_H_
#define RDDIGITAL_H_

/*
 * Sets up specified pin of Port F as an output.
 * @param unsigned char pin
 *     The specific pin of Port F to be set as an output.
 * 
 * @return void
 */
void RDDigitalSetOutput(unsigned char pin){
	set_bit(DDRF, pin);
}

/*
 * Sets up specified pin of Port F as an input.
 * @param unsigned char pin
 *     The specific pin of Port F to be set as an input.
 * 
 * @return void
 */
void RDDigitalSetInput(unsigned char pin){
	clr_bit(DDRF, pin);	
}

/*
 * Reads state of specified pin of Port F.
 * @param unsigned char pin
 *     The specific pin of Port F to be checked.
 * 
 * @return unsigned char
 *     1 if specified pin of Port F is high,
 *     0 if specified pin of Port F is low.
 */
unsigned char RDDigitalGetState(unsigned char pin){
	return get_bit(PORTF, pin);
}

/*
 * Sets specified pin of Port F to specified state.
 * @param unsigned char pin
 *     The specific pin of Port F to be to set to the specified state.
 * 
 * @param unsigned char state
 *     The state that the specified pin of Port F should be set to.
 * 
 * @return void
 */
void RDDigitalSetState(unsigned char pin, unsigned char state){
	if(state == 0x00)
		clr_bit(PORTF, pin);
	if(state == 0x01)
		set_bit(PORTF, pin);
}

/*
 * Checks if specified pin of Port F is high.
 * @param unsigned char pin
 *     The specific pin of Port F to be checked.
 * 
 * @return unsigned char
 *     1 if specified pin of Port F is high,
 *     0 if specified pin of Port F is low.
 */
unsigned char RDDigitalIsHigh(unsigned char pin){
	return get_bit(PORTF, pin);
}

/*
 * Checks if specified pin of Port F is low.
 * @param unsigned char pin
 *     The specific pin of Port F to be checked.
 * 
 * @return unsigned char
 *     1 if specified pin of Port F is low,
 *     0 if specified pin of Port F is high.
 */
unsigned char RDDigitalIsLow(unsigned char pin){
	return !get_bit(PORTF, pin);
}

/*
 * Reads state of Port F.
 * 
 * @return unsigned char
 *     Port F's state
 */
unsigned char RDDigitalGetPort(){
	return PORTF;
}

#endif // RDDIGITAL_H_

