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
 *     The pin that should be read (0 - 7)
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
    } else {
    	clr_bit(ADMUX, ADLAR); // Right align if 10-bit mode
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
 * Reads an analog signal and scales the returned value to a value from 0 to multiplier.
 * 
 * @param unsigned char channel
 *     The pin that should be read (0 - 7)
 * 
 * @param unsigned char mode
 *     The mode can be MODE_8_BIT or MODE_10_BIT
 *     if mode == MODE_8_BIT, the resolution will be 256
 *     if mode == MODE_10_BIT, the resolution will be 1024
 *
 * @param uint16_t multiplier
 *     The maximum value you want to scale the read value to
 *     Eg. multiplier of 100 will return a value from 0 to 100
 * 
 * @return uint16_t
 *     Scaled representation of analog signal
*/
uint16_t RDAnalogReadToPerc(unsigned char channel, unsigned char mode, uint32_t multiplier) {
	uint32_t scaledRead;

	scaledRead = (RDAnalogRead(channel, mode) * multiplier);

	if (mode == MODE_8_BIT) {
		return scaledRead / 255;
	} else {
		return scaledRead / 1023;
	}
}

/*
 * Reads the voltage of the battery.
 * 
 * @return uint16_t
 *     Voltage of the battery in mV.
*/
uint16_t RDAnalogReadBattV() {
	uint16_t scaledVoltage;

    scaledVoltage = RDAnalogReadToPerc(0, MODE_10_BIT, 5000); // Scaled to Vref (5000mV)
    return scaledVoltage * 11; // ADC0 pin is battery voltage / 11
}

/*
 * Enables/Disables auto trigger source in free running mode. In this mode
 * the ADC will constantly sample and update the ADC register.
 * 
 * @param unsigned char channel
 *     The pin that should be read (0 - 7)
 * 
 * @param unsigned char enable
 *     Can be either ON or OFF
 *     if enable == ON, the ADC will be enabled in free running mode
 *     if enable == OFF, auto trigger will be disabled and the ADC will stop
 *
 * @return void
*/
void RDAnalogReadCont(unsigned char channel, unsigned char enable) {
    ADMUX |= channel;
    set_bit(ADCSRA, ADATE); // Auto trigger enable, ADTS bits 0 for free running mode
    if (enable) {
        set_bit(ADCSRA, ADSC); // Start ADC conversions
    } else {
        clr_bit(ADCSRA, ADATE); // Disable auto trigger (stops conversions)
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
 * @return uint16_t
 *     Average value of digital approximations of analog signal rounded down to
 *     the nearest integer
 */
uint16_t RDAnalogReadAvg(unsigned char channel, unsigned char mode, uint16_t samples) {
	uint32_t sum = 0;
	for (int i = 0; i < samples; i++) {
		sum += RDAnalogRead(channel, mode);
    }
	return (uint16_t)(sum/samples);
}

#endif //RDANALOG_H_
