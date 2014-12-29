/*
 * libRobotDev
 * RDI2C.h
 * Purpose: Abstracts all I2C functions
 * Created: 08/12/2014
 * Author(s): Arda Yilmaz
 * Status: UNTESTED
 */ 

#ifndef RDI2C_H_
#define RDI2C_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define I2C_STATUS		(TWSR & 0xf8)
#define I2CStart()		(TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE))
#define I2CStop()		(TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE))
#define I2CACK()		(TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA))
#define I2CNACK()		(TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE))
#define I2CContinue()	(TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE))

#define START_SENT		0x08
#define REP_START_SENT	0x10
#define ARBIT_LOST		0x38

#define MT_SLA_W_ACK	0x18
#define MT_SLA_W_NACK	0x20
#define MT_DATAT_ACK	0x28
#define MT_DATAT_NACK	0x30

#define MR_SLA_R_ACK	0x40
#define MR_SLA_R_NACK	0x48
#define MR_DATAR_ACK	0x50
#define MR_DATAR_NACK	0x58

typedef struct I2CSM {

	uint8_t SLA_RW;
	uint8_t	repeat;
	uint8_t *buffer;
	uint8_t bufferIndex;
	uint8_t bufferLength;	
} I2CSM;

static I2CSM RDI2CSM = {0, 0, NULL, 0, 1, };

void RDI2CInit(uint8_t scalef) {
	
	// Configure SCL
	TWSR = 0x00;
	TWBR = scalef;

	// Enable I2C
	TWCR |= (1 << TWEN) | (1 << TWIE);
}
static uint8_t I2CReady(void) {

	return (RDI2CSM.buffer == NULL);
}

void RDI2CRead(uint8_t addr, uint8_t *buffer, uint8_t bufferLength) {

	// Wait until interface becomes available
	while (!I2CReady());
	
	// Set slave address and write mode
	RDI2CSM.SLA_RW = (addr << 1) | 1;
	
	// Set burst (repeated) write mode
	RDI2CSM.repeat = (bufferLength > 1);
	
	RDI2CSM.bufferLength = bufferLength;
	
	// Set buffer pointer to given buffer address
	RDI2CSM.buffer = buffer;
	
	// Send start condition
	I2CStart();
	
	// Wait for data to be read
	while (!I2CReady());
}

uint8_t RDI2CWrite(uint8_t addr, uint8_t *buffer, uint8_t bufferLength) {
	
	// Wait until interface becomes available
	while (!I2CReady());
	
	// Allocate memory
	RDI2CSM.buffer = malloc(bufferLength * sizeof(uint8_t));
	
	if (RDI2CSM.buffer == NULL) return 0;

	RDI2CSM.bufferLength = bufferLength;

	// Copy data into transmit buffer
	memcpy(RDI2CSM.buffer, buffer, bufferLength);
	
	// Set slave address and write mode
	RDI2CSM.SLA_RW = (addr << 1);
	
	// Set burst (repeated) write mode
	RDI2CSM.repeat = (bufferLength > 1);
	
	// Send start condition
	I2CStart();
	
	return 1;
}

ISR(TWI_vect) {

	switch (I2C_STATUS) {
	
		case START_SENT:	// Start condition sent
			
			// Copy slave address and access type to TWDR
			TWDR = RDI2CSM.SLA_RW;
			I2CContinue();
			break;
		
		case MT_SLA_W_ACK:	// Write request acknowledged
			
			// Copy data to TWDR
			TWDR = *buffer;
			++bufferIndex;
			I2CContinue();
			break;
			
		case MT_SLA_W_NACK:	// Write request declined
		case MR_SLA_R_NACK:	// Read request declined
	
			I2CStart();
			break;
			
		case MR_SLA_R_ACK:	// Read request acknowledged
			
			// Copy TWDR to buffer
			*buffer = TWDR;
			++bufferIndex;
			I2CACK();
			break;
					
		case MT_DATAT_ACK:	// Data transmitted, ACK received
			
			// If there is still data to be transmitted
			if (RDI2CSM.repeat && (bufferIndex < bufferLength)) {
				
				// Send repeated start condition
				I2CStart();
			} else {
			
				// Deallocate memory
				free(RDI2CSM.buffer);
				buffer = NULL;
				bufferIndex = 0;
				
				// Send stop condition
				I2CStop();
			}
			
			I2CContinue();
			break;
			
		case MT_DATAT_NACK:	// Data transmitted, NACK received
			
			// Deallocate memory
			free(RDI2CSM.buffer);
			buffer = NULL;
			bufferIndex = 0;
			
			// Send stop condition
			I2CStop();
			break;
			
		case MR_DATAR_ACK:	// Data received, ACK transmitted
			
			// If there is still data to be received
			if (RDI2CSM.repeat && (bufferIndex < bufferLength)) {
				
				// Send repeated start condition
				I2CStart();
			} else {
				
				// Send NACK
				I2CNACK();
			}
			
			I2CContinue();
			break;
			
		case MR_DATAR_NACK: // Data received, NACK transmitted

			// Clear buffer pointer
			buffer = NULL;
			bufferIndex = 0;

			I2CStop();
			
		case REP_START_SENT:
			
			if (RDI2CSM.SLA_RW & 1) {
				
				// Read operations
				buffer[bufferIndex] = TWDR;
				++bufferIndex;
				I2CACK();
				
			} else {
			
				// Copy data to TWDR
				TWDR = buffer[bufferIndex];
				++bufferIndex;
				I2CContinue();
			}
			break;
		
		default:
			
			I2CContinue();
			break
	}
}

#endif // RDI2C_H_