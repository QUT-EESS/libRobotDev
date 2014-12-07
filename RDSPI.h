/*
 * libRobotDev
 * RDSPI.h
 * Purpose: Abstracts all SPI functions
 * Created: 04/12/2014
 * Author(s): Arda Yilmaz
 * Status: UNTESTED
 */ 

/*
 * USAGE
 * 
 * MASTER MODE
 *      
 *      #define RDSPI_MASTER 1  // Optional, defaults to 1
 *      #include "RDSPI.h"
 *
 * SLAVE MODE
 *
 *      #define RDSPI_MASTER 0  // Required
 *      #include "RDSPI.h"
 *
 */

#ifndef RDSPI_H_
#define RDSPI_H_

#include <avr/io.h>

#if RDSPI_MASTER == 0
#include <avr/interrupt.h>

volatile uint8_t RDSPI_RxData;
#endif // RDSPI_MASTER

#define RDSPI_MOSI      PB2
#define RDSPI_MISO      PB3
#define RDSPI_SCK       PB1
#define RDSPI_PORT      DDRB

#ifndef RDSPI_MASTER
#define RDSPI_MASTER    1
#endif // RDSPI_MASTER

/*
 * endian   0 - Little endian   (MSB first)
 *          1 - Big endian      (MSB last)
 *
 * mode     0 - Sample (rising), Setup (falling)
 *          1 - Setup (rising), Sample (falling)
 *          2 - Sample (falling), Setup (rising)
 *          3 - Setup (falling), Sample (rising)
 *
 */
void RDSPIInit(uint8_t endian, uint8_t mode) {

    // Set SPI port directions
    RDSPI_PORT = (RDSPI_PORT & ~(1 << RDSPI_MOSI)) | (RDSPI_MASTER << RDSPI_MOSI);

    RDSPI_PORT = (RDSPI_PORT & ~(1 << RDSPI_MISO)) | (!RDSPI_MASTER << RDSPI_MISO);
    
    RDSPI_PORT = (RDSPI_PORT & ~(1 << RDSPI_SCK)) | (RDSPI_MASTER << RDSPI_SCK);
    
    // Set endianness, master/slave mode, clock polarity and phase.
    SPCR = (SPCR  & ~(1 << DORD) ) | (endian << DORD) | (RDSPI_MASTER << MSTR) | (mode << CPHA);
    
    SPCR |= (1 << SPE);
    
#if RDSPI_MASTER == 0
    SPCR |= (1 << SPE) | (1 << SPIE);
    sei();
#endif // RDSPI_MASTER
}

#if RDSPI_MASTER == 1

static inline void RDSPISetClock(uint8_t frq) {
    
    SPCR = (SPCR & ~(3 << SPR0)) | (3 & frq);
    SPSR = (SPSR & ~(1 << SPI2X)) | (frq >> 2);
}

/*
 * frq      0 - F_CPU / 4
 *          1 - F_CPU / 16
 *          2 - F_CPU / 64
 *          3 - F_CPU / 128
 *          4 - F_CPU / 2
 *          5 - F_CPU / 8
 *          6 - F_CPU / 32
 *          7 - F_CPU / 64
 */
uint8_t
RDSPIRWByte(uint8_t byte, uint8_t frq, volatile uint8_t *port, uint8_t mask) {

    // Disable SPI
    //SPCR &= ~(1 << SPE);
    
    // Set clock speed
    RDSPISetClock(frq);
    
    // Re-enable SPI
    //SPCR |= (1 << SPE);
    
    // Pull chip select (CS) low
    *port &= ~(1 << mask);
    
    // Send byte
    SPDR = byte;
    while(!(SPSR & (1<<SPIF)));

    // Pull chip select (CS) high
    *port |= (1 << mask);
    
    // return anything shifted from slave
    return SPDR;
}

#else

void RDSPISlaveTxByte(uint8_t byte) {
    
    while(!(SPSR & (1<<SPIF)));
    SPDR = byte;
}

ISR(SPI_STC_vect) {
    
    RDSPI_RxData = SPDR;
    SPDR = 0;
}

#endif // RDSPI_MASTER == 1

#endif // RDSPI_H_
