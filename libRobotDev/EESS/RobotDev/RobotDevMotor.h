/* libRobotDev
 * RobotDevMotor.h
 * Purpose: Abstracts all motor control functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck
 * Status: UNTESTED
 */ 

#include "RobotDevPinDefs.h"

#ifndef ROBOTDEVMOTOR_H_
#define ROBOTDEVMOTOR_H_

void RDInitMotors( void ){
  //Set data directions
  set_bit(DDRB, PB5); //OCR1A
  set_bit(DDRB, PB6); //OCR1B
  set_bit(DDRC, PC6); //OCR3A
  set_bit(DDRC, PC5); //OCR3B
  /* WARNING motor driver will be changed to 8-bit
   * timers for next revision, therefore this code 
   * will be obselete
   */
  //Using both 16-bit timers
  //Set on compare match
  //Fast PWM
  //No prescaling
  TCCR1A = TCCR3A = 0b11110001;
  TCCR1B = TCCR3B = 0b10000001;
  
  //Reset counters
  OCR0A = OCR0B = OCR2A = OCR2B = 0;
}

void RDSetM1Speed(int speed){
  //Depends on motor orientation
  OCR0A = (speed < 0) ? 0:speed;
  OCR0B = (speed < 0) ? -speed:0;
}

void RDSetM2Speed(int speed){
  //Depends on motor orientation
  OCR3A = (speed < 0) ? 0:speed;
  OCR3B = (speed < 0) ? -speed: 0;	
}

void RDSetMotorSpeeds(int speed_m1, int speed_m2){
	RDSetM1Speed(speed_m1);
	RDSetM2Speed(speed_m2);
}


#endif /* ROBOTDEVMOTOR_H_ */