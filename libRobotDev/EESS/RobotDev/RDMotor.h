/* 
 * libRobotDev
 * RDMotor.h
 * Purpose: Abstracts all motor control functions.
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson, Shaun Karran
 * Status: UNTESTED
 */
 
#include <stdint.h>
#include <avr/interrupt.h>
#include "RDPinDefs.h"
#include "RDUtil.h"
 
#ifndef RDMOTOR_H_
#define RDMOTOR_H_

#define M1_OCRA OCR1A
#define M1_OCRB OCR1B 
#define M2_OCRA OCR3A
#define M2_OCRB OCR3B

ISR(TIMER1_COMPA_vect) {
	
}

/*
 * Initialises Timer1 and Timer3.
 * 
 * @return void
 */
void RDTimerInit(void) {
	set_bit(TCCR1A, COM1A1); // Compare Match A: Clear on match, Set at TOP.
	set_bit(TCCR1A, COM1B1); // Compare Match B: Clear on match, Set at TOP.
	set_bit(TCCR1B, WGM12); set_bit(TCCR1A, WGM00); // Set 8bit fast PWM.
	set_bit(TCCR1B, CS11); // /8 prescaler.
	TCNT1 = 0x00; // Set counter to 0.

	set_bit(TCCR3A, COM3A1); // Compare Match A: Clear on match, Set at TOP.
	set_bit(TCCR3A, COM3B1); // Compare Match B: Clear on match, Set at TOP.
	set_bit(TCCR3B, WGM32); set_bit(TCCR3A, WGM30); // Set 8bit fast PWM.
	set_bit(TCCR3B, CS31); // /8 prescaler.
	TCNT3 = 0x00; // Set counter to 0.
}

/*
 * Converts a value from 0-100 to a value from 0-255.
 * 
 * @param uint8_t percent
 *     A value from 0-100.
 * 
 * @return uint8_t
 *     An equivalent value on a scale of 0-255.
 */
uint8_t RDDutyCycle(uint8_t percent) {
	// Use casting to get rid of * 10 code.
	return (percent * 10 / 100) * 25.5;
}

/*
 * Initialise motors.
 *
 * @return void
 */
void RDMotorInit(void) {
	RDTimerInit();
	
	// Motor pins to output.
	set_bit(DDRB, MCTRL_IN1);
	set_bit(DDRB, MCTRL_IN2);
	set_bit(DDRC, MCTRL_IN3);
	set_bit(DDRC, MCTRL_IN4);

	// All pins to HIGH to brake motors.
	M1_OCRA = RDDutyCycle(100);
	M1_OCRB = RDDutyCycle(100);
	M2_OCRA = RDDutyCycle(100);
	M2_OCRB = RDDutyCycle(100);
}

/*
 * Sets Motor1 speed.
 * 
 * @param uint8_t speed
 *     The speed of the motor as percentage. Negative values for reverse.
 *
 * @return void
 */
void RDSetM1Speed(uint8_t speed) {
	M1_OCRA = (speed < 0) ? 0:RDDutyCycle(speed);
	M1_OCRB = (speed < 0) ? -RDDutyCycle(speed):0;
}

/*
 * Sets Motor2 speed.
 * 
 * @param uint8_t speed
 *     The speed of the motor as percentage. Negative values for reverse.
 *
 * @return void
 */
void RDSetM2Speed(uint8_t speed) {
	M2_OCRA = (speed < 0) ? 0:RDDutyCycle(speed);
	M2_OCRB = (speed < 0) ? -RDDutyCycle(speed):0;
}

/*
 * Sets Motor1 to brake.
 *
 * @return void
 */
void RDSetM1Brake(void) {
	M1_OCRA = RDDutyCycle(100);
	M1_OCRB = RDDutyCycle(100);
}

/*
 * Sets Motor2 to brake.
 *
 * @return void
 */
void RDSetM2Brake(void) { 
	M1_OCRA = RDDutyCycle(100);
	M1_OCRB = RDDutyCycle(100);
}

void RDM1Forward(uint8_t speed, uint8_t time) {
	RDSetM2Speed(speed);
	
}

#endif //RDMOTOR_H_
