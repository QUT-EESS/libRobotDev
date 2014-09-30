/* 
 * libRobotDev
 * RDUART.h
 * Purpose: Abstracts all UART functions
 * Created: 21/09/2014 19:21:53 PM
 * Author(s): Seamus Stephens, Thomas Tutin (https://www.youtube.com/watch?v=cuNhfSM-144)
 * Status: TESTED
 * 
 * 
 * FUTURE CHANGES
 * Creates and transmits a data packet
 * void RDUART_sendPacket();
 * 9 bit transfer options 
 * Parity checking 
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>

#ifndef RDUART_H_
#define RDUART_H_

/*****************************************************************
 * Function Prototypes *
 *****************************************************************/

/* Basic initialisation only the baud rate is necessary
 * Sets interrupt driven UART
 * No Parity bit
 * One Stop bit 
 * 
 * @param unsigned int baud
 *		The baud rate for the UART in bits/s
 *
 */
void RDUART_Init(unsigned int baud);

/* Advanced inilitiasation allows the user to input their choice of parity bits
 * and the number of stop bits (NOT YET IMPLEMENTED)
 *
 * @param unsigned int baud
 *		The baud rate for the UART in bits/s
 *
 * @param unsigned char parityBit
 *		If set sends a parity bit with each UART transmission
 *
 * @param unsigned char stopBit
 *		Sets the number of stop bits, can be either 1 or 2
 *
 */
void RDUART_Init_Advanced(unsigned int baud, unsigned char parityBit, unsigned char stopBit);


/* Get a character from the input buffer
 *
 * @return uint8_t
 *		The oldest byte in the input buffer
 *		
 */
uint8_t RDUART_getC();

/* Checks to see if how many bytes are in the receive buffer
 * in the case there are none 0 will be returned
 *
 * @return uint8_t
 *		The number of bytes available in the input buffer
 *
 */
uint8_t RDUART_Available();

/* Put a byte in the output buffer and turn on the transmit interrupt
 *
 * @param uint8_t data
 *		The byte of data to be transmitted via UART
 *
 */
void RDUART_putC(uint8_t data);

/* Put a string into the output buffer
 * if the buffer overflows the function will wait for the buffer to clear before continuing 
 *
 * @param char * data
 *		A null terminated string to be transmitted via UART
 *
 */
void RDUART_putS(char * data);

/*****************************************************************
 * Defines and Global Variables *
 *****************************************************************/

#define OUTPUT_BUFFER_SIZE 64
#define INPUT_BUFFER_SIZE 40


/* Uncomment this define if you wish to erase old data in the input buffer
 * in the event of an buffer overflow
 */

// #define _WIPE_OLD_DATA

// The following globals will form the basis for two circular buffers
static volatile uint8_t outputDataBuffer[OUTPUT_BUFFER_SIZE];
static volatile uint8_t outputBufferHead;
static volatile uint8_t outputBufferTail;
static volatile uint8_t inputDataBuffer[INPUT_BUFFER_SIZE];
static volatile uint8_t inputBufferHead;
static volatile uint8_t inputBufferTail;


/*****************************************************************
 * Function and Implementation *
 *****************************************************************/

/* Basic initialisation only the baud rate is necessary
 * Sets interrupt driven UART
 * No Parity bit
 * One Stop bit 
 */

void RDUART_Init(unsigned int baud)
{
	// Turn off interrupts (necessary when setting UART).
	cli();
	
	// Set the baud rate for the device. 
	//UBRR1 = (F_CPU / (8 * baud)) - 1; Apparently this doesn't work - Tested with arduino
	UBRR1 = ((F_CPU / (4 * baud)) - 1)/2; // Apparently this does work :(
	
	// Sets the control & status register A to use double transmission speeds.
	UCSR1A = (1 << U2X1);
	
	// Sets the control & status register B to enable transmitting and 
	// receiving. Transmitting and receiving interrupts is also enabled.
	UCSR1B = (1 << RXCIE1);
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1); 
	
	// Sets the control & status register C. Parity bits are disabled and only 1 stop bit used.
	// 8 bits will be used for transferral (UCSZn1 and UCSZn0 are both set to 1)
	UCSR1C = (3 << UCSZ10);
	
	// Turn on interrupts
	sei();
}

// Put a character in the output buffer and turn on the transmit interrupt
void RDUART_putC(uint8_t c)
{
	outputBufferHead++;
	
	// Will wait until the buffer has space before continuing
	while (outputBufferTail == outputBufferHead) {;}
	
	cli();
	
	outputDataBuffer[outputBufferHead] = c;
	UCSR1B |= (1<<UDRIE1);
	
	sei();
}

// Get a character from the input buffer
uint8_t RDUART_getC(void)
{
	// If no characters are available the function will pause until there is
	while (inputBufferHead == inputBufferTail) {;}
	
	inputBufferTail++;
	
	if (inputBufferTail >= INPUT_BUFFER_SIZE){
		inputBufferTail = 0;
	}
		
	return inputDataBuffer[inputBufferTail];
}

/* Put a string into the output buffer
 * if the buffer overflows the function will wait for the buffer to clear before continuing 
 */
void RDUART_putS(char * data)
{
	do {
		RDUART_putC(*data);
	}
	while(*data++ != '\0');
}


/* Checks to see if how many bytes are in the receive buffer
 * in the case there are none 0 will be returned
 */
uint8_t RDUART_Available(void)
{
	
	if (inputBufferHead >= inputBufferTail) {
		return inputBufferHead - inputBufferTail;
	}
		
	return INPUT_BUFFER_SIZE + inputBufferHead - inputBufferTail;
}

/* This interrupt will transmit all characters currently in the transmit buffer
 * Once all characters have been transmitted the interrupt will be disabled
 */
ISR(USART1_UDRE_vect)
{

	if (outputBufferHead == outputBufferTail) 
	{
		// Disable the transmit interrupt once all transmissions have completed
		UCSR1B &= ~(1<<UDRIE1);
	}
	else
	{
		outputBufferTail++;
		
		if (outputBufferTail >= OUTPUT_BUFFER_SIZE) {
			outputBufferTail = 0;
		}
			
		UDR1 = outputDataBuffer[outputBufferTail];
	}
}

/* This interrupt will load every byte of data received via UART into the data buffer.
 * Once the buffer is filled data received will no longer be loaded into the buffer.
 */
ISR(USART1_RX_vect)
{
	uint8_t i;

	i = inputBufferHead + 1;
	
	if (i >= INPUT_BUFFER_SIZE){
		i = 0;
	}
	
	#ifndef _WIPE_OLD_DATA
	
	// Checks to make sure the buffer head hasn't wrapped around and the circular buffer.
	// In this case, data received will be lost. 
	if (i != inputBufferTail)
	{
		inputDataBuffer[i] = UDR1;
		inputBufferHead = i;
	}
	
	#endif
	
	#ifdef _WIPE_OLD_DATA
	
	// If the inputBufferHead has wrapped around the circular buffer inputBufferTail will be 
	// incremented and the oldest data will be lost
	if (i == inputBufferTail)
	{
		if (inputBufferTail >= INPUT_BUFFER_SIZE){
			inputBufferTail = 0;
		}
	}

	inputDataBuffer[i] = UDR1;
	inputBufferHead = i;

	
	#endif
}

#endif // RDUART_H_
