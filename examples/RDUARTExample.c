/* 
 * libRobotDev
 * File: RDUARTExample.c
 * Purpose: UART example
 * Created: December 2014
 * Author(s): Jerrold Luck
 * Status: TESTED
 */ 

#include <avr/io.h>
#include "RDUART.h"

int main(void)
{
	/* initialize uart */
	RDUARTInit(9600);
	
	while(1)
	{
		/* wait until character is available */
		if(RDUARTAvailable()){
			uint8_t in = RDUARTGetChar();
			
			/* check character */
			if(in != 'H'){
				RDUARTSendChar(in);
			} else{
				RDUARTSendString("Hello!");
			}
		}
	}
}