/* libRobotDev
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

void RDDigitalSetOutput(unsigned char pin){
	set_bit(DDRF, pin);
}

void RDDigitalSetInput(unsigned char pin){
	clr_bit(DDRF, pin);	
}

unsigned char RDDigitalGetState(unsigned char pin){
	return get_bit(PORTF, pin);
}

void RDDigitalSetState(unsigned char pin, unsigned char state){
	if(state == 0x00)
		clr_bit(PORTF, pin);
	if(state == 0x01)
		set_bit(PORTF, pin);
}

unsigned char RDDigitalIsHigh(unsigned char pin){
	return get_bit(PORTF, pin);
}

unsigned char RDDigitalIsLow(unsigned char pin){
	return !get_bit(PORTF, pin);
}

unsigned char RDDigitalGetPort(){
	return PORTF;
}

#endif // RDDIGITAL_H_
