/* 
 * libRobotDev
 * RDAnalog.h
 * Purpose: Abstracts all micro analog functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson
 * Status: UNTESTED
 */ 

#include <stdint.h>
#include <avr/io.h>

#include "RobotDevPinDefs.h"
#include "RobotDevConstants.h"
#include "RobotDevUtil.h"

#ifndef ROBOTDEVANALOG_H_
#define ROBOTDEVANALOG_H_

/*
 * Initialises the Analog to Digital Converter
 * 
 * @param unsigned char prescaler_factor
 *     See Section 26.9.2 | Page 324 | "Bits 2:0 - ADPS2:0: ADC Prescaler Select
 *     Bits" of AT90USB1286 Data Sheet
 * 
 * @return void
*/
void RDAnalogInit(unsigned char prescaler_factor) {
	// Turn on ADC
	set_bit(ADCSRA, ADEN);

	/*************************************
     * Note: Do not start conversion yet *
     *************************************/

	ADCSRA |= (1<<prescaler_factor);
}

/*
 * Reads an analog signal
 * 
 * @param unsigned char channel
 *     The pin that should be read
 * 
 * @param unsigned char mode
 *     The mode can be MODE_8_BIT or MODE_10_BIT
 *     if mode == MODE_8_BIT, the resolution will be 256
 *     if mode == MODE_10_BIT, the resolution will be 1024
 * 
 * @return uint16_t
 *     Digital representation of analog signal
*/
uint16_t RDAnalogRead(unsigned char channel, unsigned char mode) {
	// Set which pin we want read (0 - 7)
	ADMUX = channel;
	// Left align if 8-bit mode
	if (mode == MODE_8_BIT) {
		set_bit(ADMUX, ADLAR);
    }
	// Start Conversion
	set_bit(ADCSRA, ADSC);
	// Wait for conversion
	while (get_bit(ADCSRA, ADSC)) { ; }
	// Return value mode-specific
	if (mode == MODE_8_BIT) {
		return ADCH;
    } else {
		return ADC;
    }
}

/*
 * Reads an analog signal [samples] number of times and returns the average of
 * the digital approximations, rounded down to the nearest integer
 *
 * @param unsigned char channel
 *     The pin that should be read
 * 
 * @param uint16_t samples
 *     The number of times the pin should be read
 * 
 * @returns uint16_t
 *     Average value of digital approximations of analog signal rounded down to
 *     the nearest integer
 */
uint16_t RDAnalogReadAvg(unsigned char channel, uint16_t samples) {
	double sum = 0;
	for (int i=0; i<samples; i++) {
		sum += RDAnalogRead(channel);
    }
	return (uint16_t)(sum/samples);
}

/**************************************************************
 * Note: This function is deprecated and will soon be removed *
 **************************************************************/
uint16_t RDReadBatteryMillivolts() {
	return RDAnalogRead(ADC_VBATT);
}

#endif // ROBOTDEVANALOG_H_

