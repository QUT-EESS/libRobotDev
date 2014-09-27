/* 
 * libRobotDev
 * RDAnalog.h
 * Purpose: Abstracts all micro analog functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson, Shaun Karran
 * Status: UNTESTED
 */ 

#include <stdint.h>
#include <avr/io.h>

#include "RDPinDefs.h"
#include "RDConstants.h"
#include "RDUtil.h"

#ifndef RDANALOG_H_
#define RDANALOG_H_

#define OFF 0
#define ON 1

#define ADC_8MHZ 1
#define ADC_4MHZ 2
#define ADC_2MHZ 3
#define ADC_1MHZ 4
#define ADC_500KHZ 5
#define ADC_250KHZ 6
#define ADC_125KHZ 7

#define MODE_8_BIT 0
#define MODE_10_BIT 1

/*
 * Initialises the Analog to Digital Converter
 * 
 * @param unsigned char prescaler
 *     Can be set using macros defined in RDAnalog.h for CPUclk 16Mhz
 *     More info, see Section 26.9.2 | Page 324 | "Bits 2:0 - ADPS2:0
 *     ADC Prescaler Select Bits" of AT90USB1286 Data Sheet
 * 
 * @return void
*/
void RDAnalogInit(unsigned char prescaler) {
	set_bit(ADMUX, REFS0); // External Vref
	ADCSRA |= prescaler;
	set_bit(ADCSRA, ADEN); // ADC enable

	/*************************************
     * Note: Do not start conversion yet *
     *************************************/
}

/*
 * Reads an analog signal
 * 
 * @param unsigned char channel
 *     The pin that should be read (0 - 7).
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
	ADMUX |= channel; // Channel to be read.
	if (mode == MODE_8_BIT) {
		set_bit(ADMUX, ADLAR); // Left align if 8-bit mode
    }
	set_bit(ADCSRA, ADSC); // Start conversion
	while (get_bit(ADCSRA, ADSC)) { ; } // Wait for conversion to finish
    // Return value mode-specific
	if (mode == MODE_8_BIT) {
		return ADCH;
    } else {
		return ADC;
    }
}

/*
 * Enables/Disables auto trigger source in free running mode. I this mode
 * the ADC will constantly sample and update the ADC register.
 * 
 * @param unsigned char channel
 *     The pin that should be read (0 - 7).
 * 
 * @param unsigned char enable
 *     Can be either ON or OFF.
 *     if enable == ON, the ADC will be enabled in free running mode.
 *     if enable == OFF, auto trigger will be disabled and the ADC will stop
 * 
 * @return uint16_t
 *     Digital representation of analog signal
*/
void RDAnalogContRead(unsigned char channel, unsigned char enable) {
    ADMUX |= channel;
    SET_BIT(ADCSRA, ADATE); // Auto trigger enable, ADTS bits 0 for free running mode
    if (enable) {
        SET_BIT(ADCSRA, ADSC); // Start ADC conversions
    } else {
        CLEAR_BIT(ADCSRA, ADATE); // Disable auto trigger (stops conversions)
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
