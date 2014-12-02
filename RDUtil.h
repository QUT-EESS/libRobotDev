/* 
 * libRobotDev
 * RDUtil.h
 * Purpose: Provides utility functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson
 * Status: UNTESTED
 */ 

#ifndef RDUTIL_H_
#define RDUTIL_H_

//Bit operations
// Set specified pin of specified port to 1
#define set_bit(port,pin) ((port) |= (1 << (pin)))

// Set specified pin of specified port to 0
#define clr_bit(port,pin) ((port) &= ~(1 << (pin)))

// Read specified pin of specified port
#define get_bit(port,pin) (((port)>>(pin)) & 1)

// Toggle specified pin of specified port
#define tog_bit(port,pin) ((port) ^= (1 << (pin)))

#endif // RDUTIL_H_

