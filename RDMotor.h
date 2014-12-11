/*
 * libRobotDev
 * RDMotor.h
 * Purpose: Abstracts all motor control functions.
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson, Shaun Karran
 * Status: UNTESTED
 */
 
#include <avr/io.h>
#include <stdint.h>

#include "RDPinDefs.h"
#include "RDUtil.h"
 
#ifndef RDMOTOR_H_
/**
 * Robot Development Motor Header.
 */
#define RDMOTOR_H_

/**
 * Motor1 Timer/Counter Output Compare Register A.
 */
#define M1_OCRA OCR1A

/**
 * Motor1 Timer/Counter Output Compare Register B.
 */
#define M1_OCRB OCR1B 

/**
 * Motor2 Timer/Counter Output Compare Register A.
 */
#define M2_OCRA OCR3A

/**
 * Motor2 Timer/Counter Output Compare Register B.
 */
#define M2_OCRB OCR3B

/**
 * Initialises Timer1 and Timer3.
 */
void RDTimerInit(void) {
	set_bit(TCCR1A, COM1A1); // Compare Match A: Clear on match, Set at TOP.
	set_bit(TCCR1A, COM1B1); // Compare Match B: Clear on match, Set at TOP.
	set_bit(TCCR1B, WGM13); set_bit(TCCR1B, WGM12); set_bit(TCCR1A, WGM11); // Set 16bit fast PWM. TOP at ICR.
	set_bit(TCCR1B, CS11); set_bit(TCCR1B, CS10); // /64 prescaler.
	ICR1 = 0x0FFF; // Set TOP value for 12bit resolution.
	TCNT1 = 0x00; // Set counter to 0.

	set_bit(TCCR3A, COM3A1); // Compare Match A: Clear on match, Set at TOP.
	set_bit(TCCR3A, COM3B1); // Compare Match B: Clear on match, Set at TOP.
	set_bit(TCCR3B, WGM33); set_bit(TCCR3B, WGM32); set_bit(TCCR3A, WGM31); // Set 16bit fast PWM. TOP at ICR.
	set_bit(TCCR3B, CS31); set_bit(TCCR3B, CS30); // /64 prescaler.
	ICR3 = 0x0FFF; // Set TOP value for 12bit resolution.
	TCNT3 = 0x00; // Set counter to 0.
}

/**
 * Converts a double from range 0-100 to a int from 0-4095.
 * 
 * @param percent
 *     A value from 0-100.
 * 
 * @return
 *     An equivalent value on a scale of 0-4095.
 */
uint16_t RDDutyCycle(double percent) {
	return (uint16_t)((percent * 4095) / 100);
}

/**
 * Initialise motors.
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

/**
 * Sets Motor1 speed.
 * 
 * @param speed
 *     The speed of the motor as percentage. Negative values for reverse.
 */
void RDSetM1Speed(double speed) {
	M1_OCRA = (speed < 0) ? 0:RDDutyCycle(speed);
	M1_OCRB = (speed < 0) ? RDDutyCycle(-speed):0;
}

/**
 * Sets Motor2 speed.
 * 
 * @param speed
 *     The speed of the motor as percentage. Negative values for reverse.
 */
void RDSetM2Speed(double speed) {
	M2_OCRA = (speed < 0) ? 0:RDDutyCycle(speed);
	M2_OCRB = (speed < 0) ? RDDutyCycle(-speed):0;
}

/*
 * Sets Motor1 to brake.
 */
void RDSetM1Brake(void) {
	M1_OCRA = RDDutyCycle(100);
	M1_OCRB = RDDutyCycle(100);
}

/*
 * Sets Motor2 to brake.
 */
void RDSetM2Brake(void) { 
	M2_OCRA = RDDutyCycle(100);
	M2_OCRB = RDDutyCycle(100);
}

#endif //RDMOTOR_H_
