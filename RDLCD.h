/*
 * libRobotDev
 * RDLCD.h
 * Purpose: Abstracts all LCD functions
 * Created: 05/12/2014
 * Author(s): Arda Yilmaz, Jeremy Pearson
 * Status: UNTESTED
 */

#include <avr/io.h>

// Ascii font data
#include "RDASCIIFont.h"

#ifndef RDLCD_H_
/**
 * Robot Development Liquid Crystal Display Header.
 */
#define RDLCD_H_

/**
 * Setting for whether the mode will be master (1) or slave (0).
 */
#define SPI_MASTER 1
#include "RDSPI.h"

/**
 * LCD Font Width.
 */
#define RDLCD_FONT_W 5

/************
 * LCD pins *
 ************/

/**
 * LCD Chip Select pin.
 */
#define RDLCD_CS    PC0

/**
 * LCD Reset pin.
 */
#define RDLCD_RST   PC1

/**
 * LCD Data/Command pin.
 */
#define RDLCD_DC    PC3

/**
 * LCD Port.
 */
#define RDLCD_PORT  PORTC

/**
 * LCD Command Binary Representation.
 */
#define RDLCD_C     0

/**
 * LCD Data Binary Representation.
 */
#define RDLCD_D     1

/****************
 * LCD Commands *
 ****************/

/**
 * LCD Basic Functionality Mode.
 */
#define LCD_BSC_FN  0x20

/**
 * LCD Extended Functionality Mode.
 */
#define LCD_EXT_FN  0x21

/**
 * LCD Set Screen To Blank.
 */
#define LCD_BLANK   0x08

/**
 * LCD Normal Mode.
 */
#define LCD_NORM    0x0C

/**
 * LCD Set Screen To Black.
 */
#define LCD_BLACK   0x09

/**
 * LCD Inverted Screen Mode.
 */
#define LCD_INVRT   0x0D

/**
 * LCD Set Contrast.
 */
#define	LCD_SET_CONTRAST	0x80

/**
 * LCD Set X Position of Cursor.
 */
#define LCD_SET_X   0x80

/**
 * LCD Set Y Position of Cursor.
 */
#define LCD_SET_Y   0x40

/******************
 * SPI Parameters *
 ******************/

/**
 * LCD Clock Prescaler.
 */
#define LCD_CLK_PS	0

/*******************
 * LCD Parameters. *
 *******************/

/**
 * LCD Width.
 */
#define RDLCD_W         84

/**
 * LCD Height.
 */
#define RDLCD_H         48

/**
 * LCD X Coordinate of Center.
 */
#define RDLCD_CNTR_X    RDLCD_W / 2

/**
 * LCD Y Coordinate of Center.
 */
#define RDLCD_CNTR_Y    RDLCD_H / 2

/**
 * LCD Number of Vertical Rows.
 */
#define RDLCD_ROW_H     8

/**
 * LCD Default Contrast.
 */
#define RDLCD_DEFAULT_CONTRAST	0x3F

/**
 * LCD High Contrast.
 */
#define RDLCD_HIGH_CONTRAST 0x4F

/******************
 * LCD Functions. *
 ******************/

/**
 * Writes a byte of pixel-data or a command to LCD screen.
 * 
 * @param byte
 *     The byte of pixel-data or a command.
 * 
 * @param dc
 *     Boolean of whether the byte should be treated as a command (0) or
 *     pixel-data (1).
 */
void RDLCDWrite(uint8_t byte, uint8_t dc) {
	// Set data/command mode
	RDLCD_PORT = (RDLCD_PORT & ~(1 << RDLCD_DC)) | (dc << RDLCD_DC);
	// Write byte to SPI
	RDSPIRWByte(byte, 5, &RDLCD_PORT, RDLCD_CS);
}

/**
 * Initialises the LCD screen.
 */
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

/**
 * Clears the LCD screen.
 */
void RDLCDClear(void) {
    for (int i = 0; i < RDLCD_W * RDLCD_ROW_H; i++){
        RDLCDWrite(0x00, RDLCD_D);
    }
}

/**
 * Writes the given character to the LCD screen at the current cursor position.
 * 
 * @param character
 *     The character to be written to the LCD screen.
 */
void RDLCDCharacter(unsigned char character) {
    // Clear padding between each character
    RDLCDWrite(0x00, RDLCD_D);
    for (int i = 0; i < RDLCD_FONT_W; i++)
    {
        RDLCDWrite(ASCII[character - 0x20][i], RDLCD_D);
    }
    RDLCDWrite(0x00, RDLCD_D);
}

/**
 * Writes the given string to the LCD screen, starting from the current cursor
 * position.
 * 
 * @param characters
 *     The string to be written to the LCD screen.
 */
void RDLCDString(unsigned char *characters) {
    while (*characters != '\0')
    {
        RDLCDCharacter(*(characters++));
    }
}

/**
 * Sets the position of the LCD cursor to the given x- and y-coordinates.
 * 
 * @param x
 *     The x-coordinate of the desired cursor position.
 * 
 * @param y
 *     The y-coordinate of the desired cursor position.
 */
void RDLCDPosition(unsigned char x, unsigned char y) {
    if((x < RDLCD_W) & (y < (RDLCD_ROW_H)))
    {
        RDLCDWrite(LCD_SET_X | x, RDLCD_C);
        RDLCDWrite(LCD_SET_Y | y, RDLCD_C);
    }
}

#endif // RDLCD_H_

