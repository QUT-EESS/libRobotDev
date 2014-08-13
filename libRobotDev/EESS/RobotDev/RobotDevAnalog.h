/* libRobotDev
 * RobotDevAnalog.h
 * Purpose: Abstracts all micro analog functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck
 * Status: UNTESTED
 */ 

#include <stdint.h>
#include <avr/io.h>

#include "RobotDevPinDefs.h"
#include "RobotDevConstants.h"
#include "RobotDevUtil.h"

#ifndef ROBOTDEVANALOG_H_
#define ROBOTDEVANALOG_H_

void RDAnalogInit(unsigned char prescaler){
	//Turn on ADC
	set_bit(ADCSRA, ADEN);
	//Do not start conversion yet
	//Turn off 'Free Running Mode'
	//Set Interrupt flag to 0
	//Turn off Interrupt
	//Set a prescaler of 128 (20MHz/128=158kHz)
	ADCSRA |= (1<<prescaler);
}

uint16_t RDAnalogRead(unsigned char channel, unsigned char mode){
	//Set which pin we want read from (0 - 7)
	ADMUX = channel;
	//Left align if 8-bit mode
	if(mode == MODE_8_BIT)
		set_bit(ADMUX, ADLAR);
	//Start Conversion
	set_bit(ADCSRA, ADSC);
	//Wait for conversion
	while(get_bit(ADCSRA, ADSC));
	//Return value mode specific
	if(mode == MODE_8_BIT)
		return ADCH;
	else
		return ADC;
}

uint16_t RDAnalogReadAvg(unsigned char channel, uint16_t samples){
	double avg = 0;
	for(int i=0; i<samples; i++)
		avg += RDAnalogRead(channel);
	//Return sample average
	return (uint16_t)(avg/samples);
}

uint16_t RDReadBatteryMillivolts(){
	return RDAnalogRead(ADC_VBATT);
}

#endif /* ROBOTDEVANALOG_H_ */