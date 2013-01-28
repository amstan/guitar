#ifndef BITOP_H
#define BITOP_H

/*! @file bitop.h
 * Bit Operations Library
 */

/// Clear bits
#define clear_bits(reg, bitmask)          ((reg) &= ~(bitmask))
/// Clear bit
#define clear_bit(reg, bitNum)            (clear_bits(reg,(1 << (bitNum))))

/// Set bits
#define set_bits(reg, bitmask)            ((reg) |= (bitmask))
/// Set bit
#define set_bit(reg, bitNum)              (set_bits(reg,(1 << (bitNum))))

/// Toggle bits
#define toggle_bits(reg, bitmask)         ((reg) ^= (bitmask))
/// Toggle bit
#define toggle_bit(reg, bitNum)           (toggle_bits(reg,(1 << (bitNum))))

/// Test bit
#define test_bit(reg, bitNum)             (((reg) & (1 << (bitNum)))!=0)

/// Change Bits
#define change_bits(reg, bitmask, val)    {clear_bits(reg,bitmask); set_bits(reg,(val)&bitmask);}
/// Change Bit
#define change_bit(reg, bitNum, val)      {if (val) set_bit(reg, bitNum); else clear_bit(reg,bitNum);}

#endif