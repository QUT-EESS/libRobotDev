/*
 * libRobotDev
 * RDLCD.h
 * Purpose: Abstracts all LCD functions
 * Created: 05/12/2014
 * Author(s): Arda Yilmaz
 * Status: UNTESTED
 */

#ifndef RDLCD_H_
#define RDLCD_H_

#include <avr/io.h>

#define RDSPI_MASTER 1
#include "RDSPI.h"

// Ascii font data
#include "ASCIIFont.h"

// Font width
#define RDLCD_FONT_W 5


// LCD pins
#define RDLCD_CS    PC0
#define RDLCD_RST   PC1
#define RDLCD_DC    PC3
#define RDLCD_PORT  PORTC

// LCD Command/Data
#define RDLCD_C     0
#define RDLCD_D     1

// LCD Commands
#define LCD_BSC_FN  0x20
#define LCD_EXT_FN  0x21

#define LCD_BLANK   0x08
#define LCD_NORM    0x0C
#define LCD_BLACK   0x09
#define LCD_INVRT   0x0D

#define	LCD_SET_CONTRAST	0x80

#define LCD_SET_X   0x80
#define LCD_SET_Y   0x40

// SPI Parameters
#define LCD_CLK_PS	0

// LCD Parameters
#define RDLCD_W         84
#define RDLCD_H         48
#define RDLCD_CNTR_X    RDLCD_W / 2
#define RDLCD_CNTR_Y    RDLCD_H / 2
#define RDLCD_ROW_H     8
#define RDLCD_DEFAULT_CONTRAST	0x3F

void RDLCDInit(void) {
	
	// Init LCD control pins, pull CS and RST high
	DDRC |= (1 << RDLCD_CS) | (1 << RDLCD_RST) | (1 << RDLCD_DC);
	RDLCD_PORT |= (1 << RDLCD_CS) | (1 << RDLCD_RST);
	
	// Reset LCD
	RDLCD_PORT &= ~(1 << RDLCD_RST);
	RDLCD_PORT |= (1 << RDLCD_RST);
	
	// Init SPI - little endian, CPOL = 0, CPHA = 0
	RDSPIInit(0,0);
    
    // Use extended instruction set
    RDLCDWrite(LCD_EXT_FN, RDLCD_C);
    
    // Set default contrast
    RDLCDWrite(LCD_SET_CONTRAST | RDLCD_DEFAULT_CONTRAST, RDLCD_C);
    
    // Use basic instruction set
    RDLCDWrite(LCD_BSC_FN, RDLCD_C);
    
    //Set LCD to normal mode
    RDLCDWrite(LCD_NORM, RDLCD_C);
    
    //Reset the LCD position
    RDLCDWrite(LCD_SET_X | 0x00, RDLCD_C);
    RDLCDWrite(LCD_SET_Y | 0x00, RDLCD_C);
    
}

void RDLCDWrite(uint8_t byte, uint8_t dc) {

	// Set data/command mode
	RDLCD_PORT = (RDLCD_PORT & ~(1 << RDLCD_DC)) | (dc << RDLCD_DC);
	
	// Write byte to SPI
	RDSPIRWByte(byte, 0, &RDLCD_PORT, RDLCD_CS);
}

void RDLCDClear(void) {
    
    for (int i = 0; i < RDLCD_W * RDLCD_ROW_H; i++){
        RDLCDWrite(0x00, RDLCD_D);
    }
}

void RDLCDCharacter(unsigned char character) {

    // Clear padding between each character
    RDLCDWrite(0x00, RDLCD_D);
    
    for (int i = 0; i < RDLCD_FONT_W; i++)
    {
        RDLCDWrite(ASCII[character - 0x20][i]);
    }
    
    RDLCDWrite(0x00, RDLCD_D);
}

void RDLCDString(unsigned char *characters) {
    
    while (*characters != '\0')
    {
        RDlCDChacaracter(*(characters++));
    }
}


void RDLCDPosition(unsigned char x, unsigned char y) {
    
    if((x < RDLCD_W) & (y < (RDLCD_ROW_H)))
    {
        RDLCDWrite(LCD_SET_X | x, RDLCD_D);
        RDLCDWrite(LCD_SET_Y | y, RDLCD_D);
    }
}

#endif // RDLCD_H_
