#ifndef BITOP_H
#define BITOP_H

/*! @file bitop.h
* Bit Operations Library
*/

/// Clear bit
#define clear_bit( reg, bitNumb )	((reg) &= ~(1 << (bitNumb)))

/// Set bit
#define set_bit( reg, bitNumb )		((reg) |= (1 << (bitNumb)))

/// Test bit
#define test_bit( reg, bitNumb )	((reg) & (1 << (bitNumb)))

///Change Bit
#define change_bit(ptr, position, val)	{if (val)  set_bit(ptr, position); else	clear_bit(ptr,position);}

#endif