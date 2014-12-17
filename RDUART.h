/* 
 * libRobotDev
 * RDUART.h
 * Purpose: Abstracts all UART functions
 * Created: September 2014
 * Author(s): Jerrold Luck, Seamus Stephens, Thomas Tutin, Jeremy Pearson
 * Status: TESTED
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#ifndef RDUART_H_
/**
 * Robot Development UART Header.
 */
#define RDUART_H_

/*****************************************************************
 * Defines and Global Variables *
 *****************************************************************/

/**
 * CPU Frequency
 */
#ifndef F_CPU 
#define F_CPU 16000000
#endif

/**
 * Output Buffer Size.
 */
#define OUTPUT_BUFFER_SIZE	64

/**
 * Input Buffer Size.
 */
#define INPUT_BUFFER_SIZE	40

/* Uncomment this define if you wish to erase old data in the input buffer
 * in the event of an buffer overflow
 */
// #define _WIPE_OLD_DATA

/**
 * Ring Buffer data structure.
 */
typedef struct {
    uint8_t tail;
    uint8_t head;
    volatile uint8_t* data;
} ring_buffer;

/**
 * Pre-allocated ring buffers.
 */
static volatile ring_buffer outputBuffer;
static volatile ring_buffer inputBuffer;
static volatile uint8_t outputData[OUTPUT_BUFFER_SIZE];
static volatile uint8_t inputData[INPUT_BUFFER_SIZE];

/*****************************************************************
 * Function and Implementation *
 *****************************************************************/

/**
 * Basic initialization with baudrate option.
 * 
 * @param baud
 *     baud rate for the UART in bits/s
 *
 */
void RDUARTInit(unsigned long baud)
{
    // Turn off interrupts
    cli();
    
	// Resets the tail and headers for both input and output buffers
    outputBuffer.tail = outputBuffer.head  = 0;
    inputBuffer.tail = inputBuffer.head = 0;
    
	// Link data containers
	outputBuffer.data = outputData;
	inputBuffer.data = inputData;
	
	// Set the baud rate for the device. 
    UBRR1 = ((F_CPU / (4 * baud)) - 1)/2;
    
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

/**
 * Put a byte in the output buffer and turn on the transmit interrupt.
 *
 * @param data
 *     byte of data to be transmitted via UART.
 */
void RDUARTSendChar(uint8_t data)
{
    outputBuffer.head++;
    
    if (outputBuffer.head >= OUTPUT_BUFFER_SIZE){
        outputBuffer.head = 0;
    }
    
    // Will wait until the buffer has space before continuing
    while (outputBuffer.tail == outputBuffer.head) {;}
    
    // This should usually be here however, in the event you are sending crazy amounts of data
    // this will break the code
    //cli();
    
    outputBuffer.data[outputBuffer.head] = data;
    UCSR1B |= (1<<UDRIE1);
    
    //sei();
}

/**
 * Get a character from the input buffer.
 * 
 * @return
 *     The oldest byte in the input buffer.
 */
uint8_t RDUARTGetChar(void)
{
    // If no characters are available the function will pause until there is
    while (inputBuffer.head == inputBuffer.tail) {;}
    inputBuffer.tail++;
    if (inputBuffer.tail >= INPUT_BUFFER_SIZE) {
        inputBuffer.tail = 0;
    }
    return inputBuffer.data[inputBuffer.tail];
}

/**
 * Put a string into the output buffer.
 * If the buffer overflows, the function will wait for the buffer to clear
 * before continuing.
 *
 * @param data
 *     A null-terminated string to be transmitted via UART.
 *  
 */
void RDUARTSendString(char *data)
{
    do {
        RDUARTSendChar(*data);
    } while(*data++ != '\0');
}

/**
 * Send a buffer of data with a specified length.
 *
 * @param data
 *     Pointer to buffer
 * @param len
 *		The length of the buffer
 */
void RDUARTSendBuffer(char *data, uint8_t len)
{
    do {
        RDUARTSendChar(*data);
    } while(len-- != 0);
}

/**
 * Checks how many data bytes are currently stored in the receive buffer.
 * 
 * @return
 *     Number of data bytes are currently stored in the receive buffer.
 */
uint8_t RDUARTAvailable(void)
{
    if (inputBuffer.head >= inputBuffer.tail) {
        return inputBuffer.head - inputBuffer.tail;
    }
    return INPUT_BUFFER_SIZE + inputBuffer.head - inputBuffer.tail;
}

/**
 * This interrupt service routine will transmit all characters currently in the
 * transmit buffer.
 * Once all characters have been transmitted the interrupt will be disabled.
 */
ISR(USART1_UDRE_vect)
{
    if (outputBuffer.head == outputBuffer.tail) 
    {
        // Disable the transmit interrupt once all transmissions have completed
        UCSR1B &= ~(1<<UDRIE1);
    }
    else
    {
        outputBuffer.tail++;
        if (outputBuffer.tail >= OUTPUT_BUFFER_SIZE) {
            outputBuffer.tail = 0;
        }
        UDR1 = outputBuffer.data[outputBuffer.tail];
    }
}

/**
 * This interrupt service routine will load every byte of data received via UART into the data buffer.
 * Once the buffer is filled data received will no longer be loaded into the buffer.
 */
ISR(USART1_RX_vect)
{
    uint8_t i;
    i = inputBuffer.head + 1;
    if (i >= INPUT_BUFFER_SIZE){
        i = 0;
    }

    #ifndef _WIPE_OLD_DATA

    // Checks to make sure the buffer head hasn't wrapped around and the circular buffer.
    // In this case, data received will be lost. 
    if (i != inputBuffer.tail)
    {
        inputBuffer.data[i] = UDR1;
        inputBuffer.head = i;
    }

    #endif

    #ifdef _WIPE_OLD_DATA

    // If the inputBuffer.head has wrapped around the circular buffer inputBuffer.tail will be 
    // incremented and the oldest data will be lost
    if (i == inputBuffer.tail)
    {
        if (inputBuffer.tail >= INPUT_BUFFER_SIZE){
            inputBuffer.tail = 0;
        }
    }
    inputBuffer.data[i] = UDR1;
    inputBuffer.head = i;

    #endif
}

#endif // RDUART_H_

