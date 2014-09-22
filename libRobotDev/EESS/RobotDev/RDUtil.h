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
#define set_bit(a,p) ((a) |= (1 << (p)))
#define clr_bit(a,p) ((a) &= ~(1 << (p)))
#define get_bit(a,p) (((a)>>(p)) & 1)
#define tog_bit(a,p) ((a) ^= (1 << (p)))

#endif // RDUTIL_H_

