/* libRobotDev
 * RDUSART.h
 * Purpose: Abstracts all USART functions
 * Created: 27/07/2014 12:21:53 PM
 * Author(s): Jerry Luck, Jeremy Pearson
 * Status: UNTESTED
 */ 

#include <avr/io.h>

#ifndef RDUSART_H_
#define RDUSART_H_

void RDUSARTInit(unsigned int baud){
	//Baud rate 9600
	UBRRHn = (unsigned char)(baud>>8);
	UBRRLn = (unsigned char)(baud);
	//Enable sender & receiver
	UCSRnB = (1<<RXENn) | (1<<TXENn);
	//Frame format
	UCSRnC = (1<<USBSn) | (3<<UCSZn0);
}

void RDUSARTSend(unsigned char* buffer, uint16_t length){
	for(int i=0; i<length; i++)
		RDUSARTTransmit(buffer[i]);
}

static void RDUSARTTransmit(unsigned char data){
	//Wait for empty buffer
	while( !(UCSRnA & (1<<UDREn)) );
	//Put data into buffer
	UDRn = data;
}

unsigned char RDUSARTReceive( void ){
	//Wait for data in buffer
	while( !(UCSRnA & (1<<RXCn)) );
	//Return data
	return UDRn;
}

static void RDUSARTFlush( void ){
	unsigned char dummy;
	while( UCSRnA & (1<<RXCn) ) dummy = UDRn;
}

#endif // RDUSART_H_
