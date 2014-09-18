/* libRobotDev
 * RobotDevUtil.h
 * Purpose: Provides utility functions
 * Created: 25/07/2014
 * Author(s): Jerry Luck
 * Status: UNTESTED
 */ 

#ifndef ROBOTDEVUTIL_H_
#define ROBOTDEVUTIL_H_

//Bit operations
#define set_bit(a,p) a |= (1 << p)
#define clr_bit(a,p) a &= ~(1 << p)
#define get_bit(a,p) (((a)>>(p)) & 1)
#define tog_bit(a,p) a ^= (1 << p);

#endif /* ROBOTDEVUTIL_H_ */