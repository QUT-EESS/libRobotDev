/*
 * libRobotDev
 * RDBluetooth.h
 * Purpose: Abstracts all Bluetooth functions
 * Created: 29/07/2014
 * Author(s): Lachlan Cesca, Samuel Cunningham-Nelson, Arda Yilmaz
 * Status: UNTESTED
 */ 

#include <util/delay.h>
#include "RDUART.h"

#ifndef RDBLUETOOTH_H_
#define RDBLUETOOTH_H_

#define	KEYPIN	(1 << PB4)
#define	BTPWR	(1 << PB5)

static volatile char bluetoothBaud = 0;

/*
 * Sets module name
 *
 * @param char *name
 *      Name of the module.
 *
 * @return void
 */
static inline void RDBluetoothSetName(char *name) {

    char *preamble = "AT+NAME";		// Preamble
    RDUARTPutsNoNull(preamble);   // Transmit
    RDUARTPutsNoNull(name);	
    _delay_ms(750);
}

/*
 * Sets module pin
 *
 * @param char *pin
 *      Module pairing pin.
 *
 * @return void
 */
static inline void RDBluetoothSetPin(char *pin) {

    char* preamble = "AT+PIN";		// Preamble
    RDUARTPutsNoNull(preamble);	// Transmit
    RDUARTPutsNoNull(pin);
    _delay_ms(750);
}

/*
 * Sets module UART baud rate. For supported baud rates, see RDBluetoothConfig
 *
 * @param char baud
 *      See RDBluetoothConfig
 *
 * @return void
 */
static inline void RDBluetoothSetBR(char baud) {

    char* preamble = "AT+BAUD";		// Preamble
    RDUARTPutsNoNull(preamble);	// Transmit
    RDUARTPutc(baud);
    _delay_ms(750);
}

/*
 * Pings module
 *
 * @param void
 *
 * @return void
 */
static inline void RDBluetoothSendAT(void) {
    
    char *preamble = "AT";          // Preamble
    RDUARTPutsNoNull(preamble);	// Transmit
    _delay_ms(750);
}

/*
 * Sends one byte of data
 *
 * @param char byte
 *      Data byte to send.
 *
 * @return void
 */
void RDBluetoothSendByte(char byte){
    
    RDUARTPutc( (uint8_t)byte );
}

/* UNTESTED
 * Sends packet of given length
 *
 * @param char *buffer
 *      Data buffer to send.
 *
 * @param uint16_t *length
 *      Length of buffer. Including null terminator for strings.
 *
 * @return void
 */
void RDBluetoothSendBuffer(char* buffer, uint16_t length) {
    
    uint16_t i = 0;
    
    for (i = 0; i < length; ++i) RDBluetoothSendByte(buffer[0]);
}

/*
 * Receives one byte of data
 *
 * @param void
 *
 * @return char
 *      Data byte.
 */
char RDBluetoothReceiveByte(void){
    
    return (char)RDUARTGetc();
}

/*
 * Check for "OK" response from module
 *
 * @param void
 *
 * @return uint8_t
 *      0: Invalid response
 *      1: Valid response
 */
static inline uint8_t RDBluetoothCheckOk(void) {
    
    uint8_t i = 5;
    uint8_t okCheckStr = 0;
    
    while (!RDUARTAvailable() && i) {
        
        _delay_ms(100);
        --i;
    }
    
    if (i)	okCheckStr = RDBluetoothReceiveByte();
    else return 0;
    
    return (okCheckStr == 'O') ? 1 : 0;
}

/* UNTESTED
 * Co-ordinates appropriate control pins to place module in configuration mode
 *
 * @param void
 *
 * @return void
 */
static inline void RDBluetoothEnterConfigMode(void) {
    
    // Enable control pins
    DDRB |= BTPWR | KEYPIN;
    
	/* NOTE:
	 *		When using Bolt revision 1.2a (blue mask) comment
	 *		out the BTPWR on/off lines.
	 */
	
    // Place Bluetooth in config mode
    PORTB |= BTPWR;			// Turn off module
    PORTB |= KEYPIN;		// Pull KEY high
    
    _delay_ms(100);
    PORTB &= ~BTPWR;		// Turn on module
}

/* UNTESTED
 * Co-ordinates appropriate control pins to restart module
 *
 * @param void
 *
 * @return void
 */
static inline void RDBluetoothRestart(void) {
    
	/* NOTE:
	 *		When using Bolt revision 1.2a (blue mask) comment
	 *		out the BTPWR on/off lines.
	 */
	
    PORTB &= ~KEYPIN;		// Pull KEY low
    PORTB |= BTPWR;			// Turn off module
    _delay_ms(100);
    PORTB &= ~BTPWR;		// Turn on module
}

/*
 * Returns UL baud rate corresponding to designator
 *
 * @param char
 *      Baud rate designator (see RDBluetoothSetBR header).
 *
 * @return unsigned long
 *      Corresponding UL baud rate. e.g. char "4" -> 9600UL
 */
static unsigned long RDBluetoothReturnBaudUL(char baud) {
    
    unsigned long bauds[] = {1200,2400,4800,9600,19200,38400,57600,
                             115200,230400,460800,912600,1382400,};
    
    return (baud < 'A') ? bauds[baud - '1'] : bauds[(baud - 'A') + 9];
}

/*
 * Queries module for set baud rate and saves it to bluetoothBaud
 *
 * @param void
 *
 * @return void
 */
static inline void RDBluetoothGetBaud(void) {

    char* baudSweep = "123456789ABC";
    uint8_t i = 0;
    
    RDBluetoothEnterConfigMode();
            
    for (i = 0; baudSweep[i] != '\0'; ++i) {
    
        RDUARTInit( RDBluetoothReturnBaudUL( baudSweep[i] ) );		// Initialise UART
        
        RDBluetoothSendAT();				// Send ping
        
        bluetoothBaud = baudSweep[i];       // Store current test baud rate
        if (RDBluetoothCheckOk()) break;    // Check response
    }
    
    RDBluetoothRestart();
}

/*
 * Initialises UART to modules current baud rate.
 *
 * @param void
 *
 * @return unsigned long
 *      Baud rate detected from module.
 */
unsigned long RDBluetoothInit(void) {

    // Get current baud rate
    RDBluetoothGetBaud();
    // Initialize UART with last baud rate
    unsigned long baud = RDBluetoothReturnBaudUL(bluetoothBaud);
    RDUARTInit(baud);
    
    return baud;
}

/*
 * Renames device, sets pin, and changes current baud rate. UART is reconfigured
 * automatically to the new baud rate. Module is automatically placed and 
 * brought out of configuration mode.
 * 
 * @param char *name
 *      Name of the module.
 *
 * @param char *pin
 *      Module pairing pin.
 *
 * @param char baud
 *      Corresponding baud rate designator:
 *      char baud	Baud Rate
 *      "1"			1200
 *      "2"			2400
 *      "3"			4800
 *      "4"			9600
 *      "5"			19200
 *      "6"		    38400
 *      "7"			57600
 *      "8"			115200
 *      "9"			230400
 *      "A"			460800
 *      "B"			912600
 *      "C"			1382400
 *
 * @return void
 */
void RDBluetoothConfig(char *name, char* pin, char baud) {
    
    RDBluetoothEnterConfigMode();
        
    // Configure
    RDBluetoothSetName(name);	// Set name
    RDBluetoothSetPin(pin);		// Set pin
    RDBluetoothSetBR(baud);		// Set baud rate
    
    RDBluetoothRestart();
    
    bluetoothBaud = baud;		// Update baud rate
    RDUARTInit( RDBluetoothReturnBaudUL(bluetoothBaud) );	// Reinitialise UART with new baud
}

//void* RDBluetoothPacket(char header, char* buffer, uint16_t length) {
    /* Dynamic Packet struct
     * char 	header
     * uint16_t 	data_len
 * char* 	data
     */
    //Allocate memory
//	void* packet = (void*)malloc(sizeof(header)+length);
//	//Copy data into malloc
//	memcpy(packet, header, sizeof(header));			// header
//	memcpy(packet+sizeof(header), length, sizeof(length));	// data_len
//	memcpy(packet+sizeof(header)+sizeof(length), buffer, length);	// data
    
//	return packet;
//}

#endif // RDBLUETOOTH_H_