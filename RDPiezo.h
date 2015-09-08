/*
 * libRobotDev
 * RDPiezo.h
 * Purpose: To play doof doofs
 * Created: 31/07/2015
 * Author(s): Lachlan Cesca
 * Status: Working
 */ 
 
#ifndef RDPIEZO_H_
#define RDPIEZO_H_

#include <avr/io.h>
#include <avr/interrupt.h>

// Uses same pin as servo
#define PIEZOPORT   PORTB
#define P1 PB7

#define CLKFREQ 16000000
#define PRESCALER 1024

#define FREQ_SCALING (CLKFREQ/(PRESCALER*2))

const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;
const int R = 0;



//Prescaler 1024
//Max Frequency 8kHz
//Min Frequency 30Hz

//Prescaler 256
//Max Frequency 15,625Hz
//Min Frequency 122Hz

//Prescaler 

// CLEAN VARIABLES

int regTimer;

uint16_t timerDuration;
uint16_t songCounter;

uint16_t restDuration = 500;

static uint16_t durationCounter;
int restCounter = -1;



int noNotes = 17;
int note[] = {440, 0, 440,0, 440,0, 349,0, 523,0, 440,0, 349,0, 523,0, 440};
int duration[] = {500,500,500,500,500,500,350,500,150,500,500,500,350,500,150,500,650};


void turn_on_piezo(void){
	//Activate PWM
	DDRB |= (1<<P1); //PWM (B7) output
	PIEZOPORT |= (1<<P1);
}

void turn_off_piezo(void){
	//Activate PWM
	DDRB &= ~(1<<P1); //PWM (B7) output
	PIEZOPORT &= ~(1<<P1);
}

void set_frequency(int hertz, int duration){
	timerDuration = duration;
	if(hertz > 0){
		double num = FREQ_SCALING;
		double den = hertz;
		regTimer = (int)num/den;
		OCR0A = regTimer;
		turn_on_piezo();
	}else{
		turn_off_piezo();
	}
}

void RDPiezo_Init(void){
	durationCounter = 0;
	songCounter = 0;
	

	TCCR0A = 0x00; //Could be removed
	TCCR0B = 0x00; //Could be removed
	set_bit(TCCR0A, COM0A0); //Toggle on match
	set_bit(TCCR0B, WGM02); set_bit(TCCR0A, WGM01); set_bit(TCCR0A, WGM00); // Set 8bit fast PWM.
	set_bit(TCCR0B, CS02); set_bit(TCCR0B, CS00); // /256 prescaler.
	OCR0A = 0xFF; // Set TOP value for 8bit resolution.
	TCNT0 = 0x00; // Set counter to 0.
	TIMSK0 |= (1 << TOIE0);
	sei();
	
	set_frequency(note[songCounter],duration[songCounter]);
	songCounter++;
	
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
	if(restCounter<0){
		++durationCounter;
		if (durationCounter >= PWM_length(timerDuration)) {
			durationCounter = 0;
			restCounter = 0;
			if(songCounter < noNotes){
				set_frequency(note[songCounter],duration[songCounter]);
				songCounter++;
				
			}else{
				turn_off_piezo();
				songCounter = 0;
			}
		}
	}else{
		restCounter++;
		if(restCounter >= PWM_length(restDuration)){
			restCounter = -1;
		}
	}
}


void play_tune(int songNo){
	turn_on_piezo();
}





#endif // RDPIEZO_H_