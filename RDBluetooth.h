/*
 * libRobotDev
 * RDBluetooth.h
 * Purpose: Abstracts all Bluetooth functions
 * Created: 29/07/2014
 * Author(s): Lachlan Cesca, Samuel Cunningham-Nelson, Arda Yilmaz,
 *            Jeremy Pearson
 * Status: TESTED <Blake>
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "RDPinDefs.h"
#include "RDUART.h"

#ifndef RDBLUETOOTH_H_
/**
 * Robot Development Bluetooth Header.
 */
#define RDBLUETOOTH_H_

/**
 * Bluetooth Key pin.
 */
#define	KEYPIN	(1 << PE5)

/**
 * Bluetooth Power Enable pin.
 */
#define	BTPWR	(1 << PE4)

/**
 * Bluetooth Data Direction Register.
 */
#define BTDDR DDRE

/**
 * Bluetooth Port.
 */
#define BTPORT PORTE

static volatile char bluetoothBaud = 0;

/**
 * Sets module name.
 *
 * @param name
 *      Name of the module.
 */
static inline void RDBluetoothSetName(char *name) {

    char *preamble = "AT+NAME";		// Preamble
    for (uint8_t i = 0; preamble[i] != '\0'; i++) RDUARTSendChar(preamble[i]);
    for (uint8_t i = 0; name[i] != '\0'; i++) RDUARTSendChar(name[i]);
    _delay_ms(750);
}

/**
 * Sets module pin.
 *
 * @param pin
 *      Module pairing pin.
 */
static inline void RDBluetoothSetPin(char *pin) {

    char* preamble = "AT+PIN";  // Preamble
    for (uint8_t i = 0; preamble[i] != '\0'; i++) RDUARTSendChar(preamble[i]);
    for (uint8_t i = 0; pin[i] != '\0'; i++) RDUARTSendChar(pin[i]);
    _delay_ms(750);
}

/**
 * Sets module UART baud-rate. For supported baud-rates, see RDBluetoothConfig.
 *
 * @param baud
 *      See RDBluetoothConfig.
 */
static inline void RDBluetoothSetBR(char baud) {

    char* preamble = "AT+BAUD";		// Preamble.
    for (uint8_t i = 0; preamble[i] != '\0'; i++) RDUARTSendChar(preamble[i]);
    RDUARTSendChar(baud);
    _delay_ms(750);
}

/**
 * Pings module.
 */
static inline void RDBluetoothSendAT(void) {
    
    char *preamble = "AT";          // Preamble
    for (uint8_t i = 0; preamble[i] != '\0'; i++) RDUARTSendChar(preamble[i]);
    _delay_ms(200);
}

/**
 * Sends one byte of data.
 *
 * @param byte
 *      Data-byte to send.
 */
void RDBluetoothSendByte(char byte){
    
    RDUARTSendChar((uint8_t) byte);
}

/** 
 * Sends packet of given length.
 *
 * @param buffer
 *      Data-buffer to send.
 *
 * @param length
 *      Length of buffer (including null-terminator for strings).
 */
void RDBluetoothSendBuffer(char* buffer, uint16_t length) {
    
    uint16_t i = 0;
    
    for (i = 0; i < length; ++i) RDBluetoothSendByte(buffer[i]);
}

/**
 * Receives one byte of data.
 *
 * @return
 *      Data-byte
 */
char RDBluetoothReceiveByte(void){
    return (char) RDUARTGetChar();
}

/**
 * Check for "OK" response from module.
 *
 * @return
 *      1 if valid response,
 *      0 if invalid response.
 */
static inline uint8_t RDBluetoothCheckOk(void) {
    uint8_t i = 5;
    uint8_t okCheckStr = 0;
    while (!RDUARTAvailable() && i) {
        
        _delay_ms(100);
        --i;
    }
    if (i) {
        okCheckStr = RDBluetoothReceiveByte();
    } else {
        return 0;
    }
    return (okCheckStr == 'O') ? 1 : 0;
}

/** UNTESTED
 * Adjusts appropriate control pins to put module into configuration mode.
 */
static inline void RDBluetoothEnterConfigMode(void) {
    
    // Enable control pins
    DDRB |= BTPWR | KEYPIN;
    
    // Place Bluetooth in config mode
    PORTB |= BTPWR;			// Turn off module
    PORTB |= KEYPIN;		// Pull KEY high
    
    _delay_ms(100);
    PORTB &= ~BTPWR;		// Turn on module
}

/** UNTESTED
 * Adjusts appropriate control pins to restart module.
 */
static inline void RDBluetoothRestart(void) {
    PORTB &= ~KEYPIN;		// Pull KEY low
    PORTB |= BTPWR;			// Turn off module
    _delay_ms(100);
    PORTB &= ~BTPWR;		// Turn on module
}

/**
 * Returns UL baud-rate corresponding to designator.
 *
 * @param
 *      Baud-rate designator (see RDBluetoothSetBR header).
 *
 * @return
 *      Corresponding UL baud-rate. E.g. char "4" -> 9600UL
 */
static unsigned long RDBluetoothReturnBaudUL(char baud) {
    // Convert character to index value (0 : 12)
    uint8_t baudVal = (baud < 'A') ? baud - '0' : (baud - 'A') + 10;
    /*
     * 1 - 6  : 1200 * 2^(baudVal - 1)
     * 7 - 11 : 1200 * 2^(baudVal - 1) - 19200 * 2^(baudVal - 7)
     * 12     : 1382400
     */
    return (baudVal >= 12) ? 1382400 : 1200 * (1 << (baudVal - 1)) - (baudVal >= 7) * ( 19200 * (1 << (baudVal - 7)) );
}

/**
 * Queries module for set baud-rate and saves it to bluetoothBaud.
 */
static inline void RDBluetoothGetBaud(void) {
    char* baudSweep = "123456789ABC";
    uint8_t i = 0;
    //RDBluetoothEnterConfigMode();
    for (i = 0; baudSweep[i] != '\0'; ++i) {
        RDUARTInit(RDBluetoothReturnBaudUL(baudSweep[i]));		// Initialise UART
        RDBluetoothSendAT();				// Send ping
        bluetoothBaud = baudSweep[i];       // Store current test baud-rate
        if (RDBluetoothCheckOk()) {
            break;    // Check response
        }
    }
    //RDBluetoothRestart();
}

/**
 * Initialises UART to module's current baud-rate.
 *
 * @return
 *      Baud-rate detected from module.
 */
unsigned long RDBluetoothInit(void) {
    // Get current baud-rate
    RDBluetoothGetBaud();
    // Initialize UART with last baud-rate
    unsigned long baud = RDBluetoothReturnBaudUL(bluetoothBaud);
    RDUARTInit(baud);
    
    return baud;
}

/**
 * Renames device, sets pin, and changes current baud-rate. UART is reconfigured
 * automatically to the new baud-rate. Module is automatically placed and 
 * brought out of configuration mode.
 * 
 * @param name
 *      Name of the module.
 *
 * @param pin
 *      Module pairing pin.
 *
 * @param baud
 *      Corresponding baud-rate designator.
 *      (\a baud:	Baud-Rate),
 *      ('1':			1200),
 *      ('2':			2400),
 *      ('3':			4800),
 *      ('4':			9600),
 *      ('5':			19200),
 *      ('6':		    38400),
 *      ('7':			57600),
 *      ('8':			115200),
 *      ('9':			230400),
 *      ('A':			460800),
 *      ('B':			921600),
 *      ('C':			138240)
 */
void RDBluetoothConfig(char *name, char* pin, char baud) {
    
    RDBluetoothEnterConfigMode();
    
    _delay_ms(750);    
    // Configure
    RDBluetoothSetName(name);	// Set name
    RDBluetoothSetPin(pin);		// Set pin
    RDBluetoothSetBR(baud);		// Set baud rate
    
    RDBluetoothRestart();
    
    bluetoothBaud = baud;		// Update baud rate
    RDUARTInit( RDBluetoothReturnBaudUL(bluetoothBaud) );	// Reinitialise UART with new baud
}

#endif // RDBLUETOOTH_H_
