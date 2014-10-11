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
#include <stdlib.h>

#ifndef RDUART_H_
#define RDUART_H_


/*****************************************************************
 * Defines and Global Variables *
 *****************************************************************/

// Ring buffer in struct
#define OUTPUT_BUFFER_SIZE 64
#define INPUT_BUFFER_SIZE 40

/* Uncomment this define if you wish to erase old data in the input buffer
 * in the event of an buffer overflow
 */

// #define _WIPE_OLD_DATA

// The following globals will form the basis for two circular buffers

typedef struct {
    uint8_t bufferTail;
    uint8_t bufferHead;
    uint8_t * dataBuffer;
} uart_buffer;

static volatile uart_buffer outputBuffer;
static volatile uart_buffer inputBuffer;


/*****************************************************************
 * Function and Implementation *
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

void RDUARTInit(unsigned long baud)
{
    // Turn off interrupts (necessary when setting UART).
    cli();
    
    // Resets the tail and headers for both input and output buffers
    outputBuffer.bufferTail = outputBuffer.bufferHead  = 0;
    inputBuffer.bufferTail = inputBuffer.bufferHead = 0;
    
    // Dynamically allocates space for both input and output data buffers
    outputBuffer.dataBuffer = (uint8_t*) malloc(OUTPUT_BUFFER_SIZE * sizeof(uint8_t));
    inputBuffer.dataBuffer = (uint8_t*) malloc(INPUT_BUFFER_SIZE * sizeof(uint8_t));
    
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
void RDUARTInitAdvanced(unsigned long baud, unsigned char parityBit, unsigned char stopBit);


/* Put a byte in the output buffer and turn on the transmit interrupt
 *
 * @param uint8_t data
 *		The byte of data to be transmitted via UART
 *
 */
void RDUARTPutc(uint8_t c)
{
    outputBuffer.bufferHead++;
    
    if (outputBuffer.bufferHead >= OUTPUT_BUFFER_SIZE){
        outputBuffer.bufferHead = 0;
    }
    
    // Will wait until the buffer has space before continuing
    while (outputBuffer.bufferTail == outputBuffer.bufferHead) {;}
    
    // This should usually be here however, in the event you are sending crazy amounts of data
    // this will break the code
    //cli();
    
    outputBuffer.dataBuffer[outputBuffer.bufferHead] = c;
    UCSR1B |= (1<<UDRIE1);
    
    //sei();
}

/* Get a character from the input buffer
 *
 * @return uint8_t
 *		The oldest byte in the input buffer
 *		
 */

 uint8_t RDUARTGetc(void)
{
    // If no characters are available the function will pause until there is
    while (inputBuffer.bufferHead == inputBuffer.bufferTail) {;}
    
    inputBuffer.bufferTail++;
    
    if (inputBuffer.bufferTail >= INPUT_BUFFER_SIZE){
        inputBuffer.bufferTail = 0;
    }
        
    return inputBuffer.dataBuffer[inputBuffer.bufferTail];
}

/* Put a string into the output buffer
 * if the buffer overflows the function will wait for the buffer to clear before continuing 
 *
 * @param char * data
 *		A null terminated string to be transmitted via UART
 *
 */
void RDUARTPuts(char * data)
{
    do {
        RDUARTPutc(*data);
    }
    while(*data++ != '\0');
}

/* Put a string into the output buffer without null terminator
 * if the buffer overflows the function will wait for the buffer to clear before continuing 
 *
 * @param char * data
 *		A null terminated string to be transmitted via UART
 *
 */
void RDUARTPutsNoNull(char * data)
{
    do {
        RDUARTPutc(*data);
    }
    while(*++data != '\0');
}


/* Put a byte in the output buffer and turn on the transmit interrupt
 *
 * @return uint8_t data
 *		The byte of data to be transmitted via UART
 *
 */
uint8_t RDUARTAvailable(void)
{
    
    if (inputBuffer.bufferHead >= inputBuffer.bufferTail) {
        return inputBuffer.bufferHead - inputBuffer.bufferTail;
    }
        
    return INPUT_BUFFER_SIZE + inputBuffer.bufferHead - inputBuffer.bufferTail;
}

/* This interrupt will transmit all characters currently in the transmit buffer
 * Once all characters have been transmitted the interrupt will be disabled
 */
ISR(USART1_UDRE_vect)
{

    if (outputBuffer.bufferHead == outputBuffer.bufferTail) 
    {
        // Disable the transmit interrupt once all transmissions have completed
        UCSR1B &= ~(1<<UDRIE1);
    }
    else
    {
        outputBuffer.bufferTail++;
        
        if (outputBuffer.bufferTail >= OUTPUT_BUFFER_SIZE) {
            outputBuffer.bufferTail = 0;
        }
            
        UDR1 = outputBuffer.dataBuffer[outputBuffer.bufferTail];
    }
}

/* This interrupt will load every byte of data received via UART into the data buffer.
 * Once the buffer is filled data received will no longer be loaded into the buffer.
 */
ISR(USART1_RX_vect)
{
    uint8_t i;

    i = inputBuffer.bufferHead + 1;
    
    if (i >= INPUT_BUFFER_SIZE){
        i = 0;
    }
    
    #ifndef _WIPE_OLD_DATA
    
    // Checks to make sure the buffer head hasn't wrapped around and the circular buffer.
    // In this case, data received will be lost. 
    if (i != inputBuffer.bufferTail)
    {
        inputBuffer.dataBuffer[i] = UDR1;
        inputBuffer.bufferHead = i;
    }
    
    #endif
    
    #ifdef _WIPE_OLD_DATA
    
    // If the inputBuffer.bufferHead has wrapped around the circular buffer inputBuffer.bufferTail will be 
    // incremented and the oldest data will be lost
    if (i == inputBuffer.bufferTail)
    {
        if (inputBuffer.bufferTail >= INPUT_BUFFER_SIZE){
            inputBuffer.bufferTail = 0;
        }
    }

    inputBuffer.dataBuffer[i] = UDR1;
    inputBuffer.bufferHead = i;

    
    #endif
}

#endif // RDUART_H_
