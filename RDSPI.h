/*
 * libRobotDev
 * RDSPI.h
 * Purpose: Abstracts all SPI functions
 * Created: 04/12/2014
 * Author(s): Arda Yilmaz, Jeremy Pearson
 * Status: TESTED <Blake>
 */ 

/*
 * USAGE
 * 
 * MASTER MODE
 *      
 *      #define SPI_MASTER 1  // Optional, defaults to 1
 *      #include "RDSPI.h"
 *
 * SLAVE MODE
 *
 *      #define SPI_MASTER 0  // Required
 *      #include "RDSPI.h"
 *
 */

#ifndef RDSPI_H_

/**
 * Robot Development In-System Programming Header.
 */
#define RDSPI_H_

#include <avr/io.h>

#if SPI_MASTER == 0
#include <avr/interrupt.h>

/**
 * Stores the current byte received over SPI.
 */
volatile uint8_t RDSPI_RxData;

#endif // SPI_MASTER

/**
 * In-System Programming Port.
 */
#define SPIPORT      DDRB

#ifndef SPI_MASTER

/**
 * Setting for whether the mode will be master (1) or slave (0).
 */
#define SPI_MASTER    1

#endif // SPI_MASTER

/**
 * Initialises SPI.
 *
 * @param endian
 *     0: Little-endian   (Most-Significant-Bit First),
 *     1: Big-endian      (Most-Significant-Bit Last).
 *
 * @param mode
 *     0: (Sample Rising,  Setup Falling),
 *     1: (Setup Rising,   Sample Falling),
 *     2: (Sample Falling, Setup Rising),
 *     3: (Setup Falling,  Sample Rising).
 */
void RDSPIInit(uint8_t endian, uint8_t mode) {

    // Set SPI port directions
    SPIPORT = (SPIPORT & ~(1 << MOSI)) | (SPI_MASTER << MOSI);

    SPIPORT = (SPIPORT & ~(1 << MISO)) | (!SPI_MASTER << MISO);
    
    SPIPORT = (SPIPORT & ~(1 << SCLK)) | (SPI_MASTER << SCLK);
    
    // Set endianness, master/slave mode, clock polarity and phase.
    SPCR = (SPCR  & ~(1 << DORD) ) | (endian << DORD) | (SPI_MASTER << MSTR) | (mode << CPHA);
    
    SPCR |= (1 << SPE);
    
#if SPI_MASTER == 0
    SPCR |= (1 << SPE) | (1 << SPIE);
    sei();
#endif // SPI_MASTER
}

#if SPI_MASTER == 1

/**
 * Sets the SPI Clock's frequency.
 * 
 * @param frq
 *     The frequency to set the clock to.
 *     0: (F_CPU / 4),
 *     1: (F_CPU / 16),
 *     2: (F_CPU / 64),
 *     3: (F_CPU / 128),
 *     4: (F_CPU / 2),
 *     5: (F_CPU / 8),
 *     6: (F_CPU / 32),
 *     7: (F_CPU / 64).
 */
static inline void RDSPISetClock(uint8_t frq) {
    SPCR = (SPCR & ~(3 << SPR0)) | (3 & frq);
    SPSR = (SPSR & ~(1 << SPI2X)) | (frq >> 2);
}

/**
 * Reads and writes using SPI.
 * 
 * @param byte
 *     The byte to transmit.
 * 
 * @param frq
 *     The frequency to set the clock to.
 *     0: (F_CPU / 4),
 *     1: (F_CPU / 16),
 *     2: (F_CPU / 64),
 *     3: (F_CPU / 128),
 *     4: (F_CPU / 2),
 *     5: (F_CPU / 8),
 *     6: (F_CPU / 32),
 *     7: (F_CPU / 64).
 * 
 * @param port
 *     The port that the chip select pin is on.
 * 
 * @param chipSelectPin
 *     The chip select pin.
 * 
 * @return
 *     A byte from the slave, if present.
 */
uint8_t RDSPIRWByte(uint8_t byte, uint8_t frq, volatile uint8_t *port,
                    uint8_t chipSelectPin) {
    // Disable SPI
    //SPCR &= ~(1 << SPE);
    
    // Set clock speed
    RDSPISetClock(frq);
    
    // Re-enable SPI
    //SPCR |= (1 << SPE);
    
    // Pull chip select (CS) low
    *port &= ~(1 << chipSelectPin);
    
    // Send byte
    SPDR = byte;
    while(!(SPSR & (1<<SPIF)));

    // Pull chip select (CS) high
    *port |= (1 << chipSelectPin);
    
    // return anything shifted from slave
    return SPDR;
}

#else

/**
 * Transmits a byte from the slave using SPI.
 * 
 * @param byte
 *     The byte to transmit.
 */
void RDSPISlaveTxByte(uint8_t byte) {
    while(!(SPSR & (1<<SPIF)));
    SPDR = byte;
}

/**
 * Whenever a Serial Transfer Complete interrupt flag is triggered, this
 * Interrupt Service Routine will store the data received, in RDSPI_RxData.
 * Then it will reset SPDR to 0.
 */
ISR(SPI_STC_vect) {
    RDSPI_RxData = SPDR;
    SPDR = 0;
}

#endif // SPI_MASTER == 1

#endif // RDSPI_H_
