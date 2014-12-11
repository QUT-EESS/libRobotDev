/* 
 * libRobotDev
 * RDUtil.h
 * Purpose: Provides utility functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson
 * Status: UNTESTED
 */ 

#ifndef RDUTIL_H_
/**
 * Robot Development Utilities Header.
 */
#define RDUTIL_H_

/******************
 * Bit operations *
 ******************/

/**
 * Set specified pin of specified port to 1.
 */
#define set_bit(port,pin) ((port) |= (1 << (pin)))

/**
 * Clear specified pin of specified port.
 */
#define clr_bit(port,pin) ((port) &= ~(1 << (pin)))

/**
 * Read specified pin of specified port.
 */
#define get_bit(port,pin) (((port)>>(pin)) & 1)

/**
 * Toggle specified pin of specified port.
 */
#define tog_bit(port,pin) ((port) ^= (1 << (pin)))

#endif // RDUTIL_H_

