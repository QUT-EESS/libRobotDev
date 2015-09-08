/*
 * libRobotDev
 * RDPiezo.h
 * Purpose: To play doof doofs
 * Created: 31/07/2015
 * Author(s): Lachlan Cesca
 * Status: Wroking
 */ 
 
#ifndef RDPIEZO_H_
#define RDPIEZO_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pitches.h"

// Uses same pin as Servo Driver
#define PIEZOPORT   PORTB
#define P1 PB7

#define CLKFREQ 16000000
#define PRESCALER 1024

#define pace 1450

#define restDuration 500

#define FREQ_SCALING (CLKFREQ/(PRESCALER*2))



//Prescaler 1024
//Max Frequency 8kHz
//Min Frequency 30Hz

//Prescaler 256
//Max Frequency 15,625Hz
//Min Frequency 122Hz



// Initialise timers and Counters

int regTimer;
uint16_t timerDuration;
uint16_t songCounter;
uint16_t restCounter;
uint16_t durationCounter;

int on = 0;
int initialised = 0;

int noNotes = 54;
int note[] = {
  /*NOTE_G4,NOTE_G4,NO_SOUND,NOTE_G4,NOTE_G4,NO_SOUND,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,
   NOTE_B3,NOTE_G3,NOTE_C4,NOTE_G3,NOTE_CS4,NOTE_G3,NOTE_C4,NOTE_G3,NOTE_B3,NOTE_G3,NOTE_C4,NOTE_G3,NOTE_CS4,NOTE_G3,NOTE_C4,NOTE_G3,
   NOTE_E4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_E4,NOTE_E4,NOTE_E4,
   NOTE_E4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_E4,NOTE_E4,NOTE_E4,*/
   //Introduction
  NOTE_E4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_E4,NOTE_E4,NOTE_E4,
  NOTE_E4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_E4,NOTE_E4,NOTE_E4,
  NOTE_E4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_F4,NOTE_E4,NOTE_E4,NOTE_E4,
  NOTE_E4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_G4,NOTE_E4,NOTE_E4,NOTE_E4,
  NOTE_DS5,NOTE_D5,NOTE_B4,NOTE_A4,NOTE_B4,
  NOTE_E4,NOTE_G4,NOTE_DS5,NOTE_D5,NOTE_G4,NOTE_B4,
  NOTE_B4,NOTE_FS5,NOTE_F5,NOTE_B4,NOTE_D5,NOTE_AS5,
  NOTE_A5,NOTE_F5,NOTE_A5,NOTE_DS6,NOTE_D6,NO_SOUND
};
int duration[] = {
  /*8,8,2,8,8,2,16,8,16,8,8,
   2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,
   8,16,16,8,4,8,8,8,
   8,16,16,8,4,8,8,8,*/
  8,16,16,8,4,8,8,8,
  8,16,16,8,4,8,8,8,
  8,16,16,8,4,8,8,8,
  8,16,16,8,4,8,8,8,
  8,2,8,8,1,
  8,4,8,4,8,8,
  8,8,4,8,4,8,
  4,8,4,8,3
};


void turn_on_pwm(void){
	//Activate PWM
	DDRB |= (1<<P1); //PWM (B7) output
	PIEZOPORT |= (1<<P1);
}

void turn_off_pwm(void){
	//Activate PWM
	DDRB &= ~(1<<P1); //PWM (B7) output
	PIEZOPORT &= ~(1<<P1);
}

void turn_off_piezo(void){
	turn_off_pwm();
	on = 0;
	TIMSK0 &= ~(1 << TOIE0);
}
void  turn_on_piezo(void){
	TIMSK0 |= (1 << TOIE0);
	on = 1;
}

void set_frequency(int hertz, int duration){
	timerDuration = duration;
	if(hertz > 0){
		double num = FREQ_SCALING;
		double den = hertz;
		regTimer = (int)num/den;
		OCR0A = regTimer;
		turn_on_pwm();
	}else{
		turn_off_pwm();
	}
}

void RDPiezo_Init(void){
	durationCounter = 0;
	songCounter = 0;
	restCounter = 0;
	initialised = 1;

	TCCR0A = 0x00; //Could be removed
	TCCR0B = 0x00; //Could be removed
	set_bit(TCCR0A, COM0A0); //Toggle on match
	set_bit(TCCR0B, WGM02); set_bit(TCCR0A, WGM01); set_bit(TCCR0A, WGM00); // Set 8bit fast PWM.
	set_bit(TCCR0B, CS02); set_bit(TCCR0B, CS00); // /256 prescaler.
	OCR0A = 0xFF; // Set TOP value for 8bit resolution.
	TCNT0 = 0x00; // Set counter to 0.
	TIMSK0 |= (1 << TOIE0);
	sei();
	
}


int calc_freq(){
	double num = FREQ_SCALING;
	double den = regTimer;
	return (int)(num/den);
}


int PWM_length(double milliseconds){
	double duration = milliseconds/1000.0;
	return (int)(duration*calc_freq());
}



ISR(TIMER0_OVF_vect) {
	//If not on rest note
	if(restCounter<=0){
		//Wait till notes finished playing
		++durationCounter;
		if (durationCounter >= PWM_length(timerDuration)) {
			durationCounter = 0;
			restCounter = 1;
			set_frequency(0,timerDuration*1.2);
		}
	}else{
		restCounter++;
		if(restCounter >= PWM_length(timerDuration)){
			restCounter = 0;
			if(songCounter < noNotes){
				set_frequency(note[songCounter],(pace/duration[songCounter]));
				songCounter++;
				
			}else{
				turn_off_piezo();
			}
		}
	}
}




void play_tune(){
	songCounter = 0;
	restCounter = 0;
	if(initialised == 0){
	   RDPiezo_Init();
	}
	if(on == 0){
		turn_on_piezo();
	}
}





#endif // RDPIEZO_H_