/* libRobotDev
 * RobotDevLCD.h
 * Purpose: Provides LCD functions
 * Created: 27/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson
 * Status: TESTED
 */ 


#ifndef ROBOTDEVLCD_H_
#define ROBOTDEVLCD_H_

#include "ASCIIFont.h"

// What pins on PORTB did we connect D/C and RST to
#define DCPIN		3
#define RSTPIN		1

// What PORTB pins are the SPI lines on
#define MOSIPIN		2
#define SCKPIN		1
#define SCEPIN		0

// LCD Command and Data
#define LCD_C		0
#define LCD_D		1

// LCD Contrast levels, you may have to change these for your display
#define LCD_LOW_CONTRAST		0x2F
#define LCD_DEFAULT_CONTRAST	0x3F
#define LCD_HIGH_CONTRAST		0x4F

// Dimensions of the LCD Screen
#define LCD_X		84
#define LCD_Y		48

// CPU Frequency Defines
#define CPU_PRESCALE(n) (CLKPR = 0x00, CLKPR = (n))
#define CPU_8MHz        0x01

// Helpful Macros
#define SET_INPUT(portdir, pin)			((portdir) &= ~(1 << (pin)))
#define SET_OUTPUT(portdir, pin)		((portdir) |= (1 << (pin)))
#define OUTPUT_WRITE(port, pin, value)	((port) = ((port) & ~(1 << (pin))) | ((value) << (pin)))
#define OUTPUT_LOW(port, pin)			((port) &= ~(1 << (pin)))
#define OUTPUT_HIGH(port, pin)			((port) |= (1 << (pin)))
#define READ_LINE(port, pin)			((port) & (1 << (pin)))

void RDSPIInit(void) {
	// Set MOSI and SCK as outputs for the SPI module
	SET_OUTPUT(DDRB, MOSIPIN);
	SET_OUTPUT(DDRB, SCKPIN);
	SET_OUTPUT(DDRC, SCEPIN);
	
	// Enable SPI in Master mode, leave the clock speed as fclk/4 (Refer to datasheet for SPCR register)
	SPCR = (0<<7/*SPIE*/)|(1<<6/*SPE*/)|(1<<4/*MSTR*/)|(0<<1/*SPR1*/)|(0<<0/*SPR0*/);
}

void RDSPIWriteByte(unsigned char byte) {
	// Write our byte to the SPI data register
	
	SPDR = byte; //SPDR is the output buffer register
	
	// Wait until the SPI module signals that transmission is finished
	while(!(SPSR & (1<<7/*SPIF*/)));
}

void RDLCDWrite(unsigned char dc, unsigned char data_in) {
	
	// Set the DC pin based on the parameter 'dc' (Hint: use the OUTPUT_WRITE macro)
	OUTPUT_WRITE(PORTC, DCPIN, dc);
	
	// Pull the SCE/SS pin low to signal the LCD we have data
	OUTPUT_LOW(PORTC, SCEPIN);
	
	// Write the byte of data using SPI
	RDSPIWriteByte(data_in);
	
	// Pull SCE/SS high to signal the LCD we are done
	OUTPUT_HIGH(PORTC, SCEPIN);
}

// Initialize the LCD with our desired settings
void RDLCDInitialise(unsigned char contrast) {
	RDSPIInit();
	
	SET_OUTPUT(DDRC, DCPIN);
	SET_OUTPUT(DDRC, RSTPIN);
  
	OUTPUT_LOW(PORTC, RSTPIN);
	OUTPUT_HIGH(PORTC, RSTPIN);
  
	RDLCDWrtie(LCD_C, 0x21 ); // Enable LCD extended command set
	RDLCDWrtie(LCD_C, 0x80 | contrast ); // Set LCD Vop (Contrast)
	RDLCDWrtie(LCD_C, 0x13 ); // LCD bias mode 1:48
  
	RDLCDWrtie(LCD_C, 0x20 ); // Enable LCD basic command set
	RDLCDWrtie(LCD_C, 0x0C ); // LCD in normal mode.
  
	RDLCDWrtie(LCD_C, 0x40 ); // Reset row to 0
	RDLCDWrtie(LCD_C, 0x80 ); // Reset column to 0
}

// Simply blank out the memory of the LCD to clear the screen
void RDLCDClear(void) {
	int i;
	for (i = 0; i < LCD_X * LCD_Y / 8; i++) {
		RDLCDWrite(LCD_D, 0x00);
	}
}

void RDLCDCharacter(unsigned char character) {
	/*
		Blank pixel column before the character, to increase readability
		Also, our font is only 5 pixels wide, since our LCD is 84 pixels wide
		we can fit twelve 7 pixel wide characters across.
	*/
	RDLCDWrite(LCD_D, 0x00);
	int index;
	for (index = 0; index < 5; index++) {
		// Write each of the 5 pixel rows to the LCD, we subtract 0x20 since
		// our table doesn't have the unprintable ASCII characters (...)
		RDLCDWrite(LCD_D, ASCII[character - 0x20][index]);
	}
	
	RDLCDWrite(LCD_D, 0x00);
}

// Iterate over the string, sending each character one at a time
void RDLCDString(unsigned char *characters) {
	// Your code goes here...
	char temp_char, i, exit_case;
	i = 0;
	exit_case = 0;
	
	while (exit_case == 0){
		temp_char = characters[i];
		if (temp_char != 0x00){
			RDLCDCharacter(temp_char);
			i++;
		}
		else
			exit_case = 1;
	}
}

// Set the position in the LCD's memory that we want to begin writing from
void RDLCDPosition(unsigned char x, unsigned char y) {
	// x=x*7;//7 columns = 1 char
	RDLCDWrite(LCD_C, 0x20);
	RDLCDWrite(LCD_C, 0x40|y);
	RDLCDWrite(LCD_C, 0x80|x);
	RDLCDWrite(LCD_C, 0x21);
}


#endif /* ROBOTDEVLCD_H_ */
