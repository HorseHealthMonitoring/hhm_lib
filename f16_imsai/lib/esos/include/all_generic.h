/*
 * "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
 * All rights reserved.
 * (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
 * (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
 * (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */

// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
/** \file
 * \brief Embedded Systems Operating System (ESOS)
 * Definitions to make ESOS code more generic and portable.
 * \note This file is to be included in ESOS builds on all platforms.
*/

#pragma once

#include <stdint.h>
#ifdef  __linux
//#warning    Compiling for 32-bit Linux
#include "pc_generic.h"
#define HELLO_MSG "\n" __FILE__ ", built on " __DATE__ " at " __TIME__ "\n"
#else
//#warning    Compiling for Microchip PIC24 microcontrollers
#include "pic24_unions.h"
#endif


/**
 * Union structure to hold an uint8 (byte) to provide access to
 * 8-bit data with different "viewpoints" (or casts, if you will).
 * \note Works on machines that store their data little-endian.
 * If you use big-endian, the members will  have to be reordered!
 * \note Prefix for all UINT8 structs is <em>U8_</em>
 */
typedef union _UINT8 {
  /** uint8 viewed as an uint8 */
  uint8_t _uint8;
  /** uint8 viewed as an uint8 */
  uint8_t u8;
  struct {
    unsigned LSN: 4; /** Least significant nibble of the uint8 */
    unsigned MSN: 4;    /** Most signficant nibble of the uint8 */
  };
  struct {
    /** bit 0 (the LSb) of the uint8 */
unsigned b0:
    1;
    /** bit 1 of the uint8 */
unsigned b1:
    1;
    /** bit 2 of the uint8 */
unsigned b2:
    1;
    /** bit 3  of the uint8 */
unsigned b3:
    1;
    /** bit 4  of the uint8 */
unsigned b4:
    1;
    /** bit 5  of the uint8 */
unsigned b5:
    1;
    /** bit 6  of the uint8 */
unsigned b6:
    1;
    /** bit 7 (MSb) of the uint8 */
unsigned b7:
    1;
  };
} UINT8;

/**
 * Union structure to hold uint16s to provide access to
 * 16-bit data with different "viewpoints" (or casts, if you will).
 * \note Works on machines that store their
 * data little-endian.  If you use big-endian, the members will
 * have to be reordered!
 * \note Prefix for all UINT16 structs is <em>U16_</em>
 * \code
 * uint8_t      u8_a, u8_b;
 * UINT16     U16_y;
 *
 * U16_y._uint16 = 0xF00D;
 * u8_b = U16_y.u8Bytes[0];           // u8_b equals 0x0D
 * u8_a = U16_y.u8MSb;                // u8_a equals 0xF0
 * while (U16_y.b7);                  // falls through since bit 7 is 0 (FALSE)
 * \endcode
 */
typedef union _UINT16 {
  /** uint16 viewed as an uint16 */
  uint16_t _uint16;
  /** uint16 viewed as an uint16 */
  uint16_t u16;
  /** int16 viewed as an int16*/
  int16_t int16;
  /** int16 viewed as an int16*/
  int16_t i16;
  struct {
    /** LSB (uint8) of the uint16 */
    uint8_t u8Lsb;
    /** MSB (uint8) of the uint16 */
    uint8_t u8Msb;
  };
  struct {
    unsigned LSBLSN: 4;
    unsigned LSBMSN: 4;
    unsigned MSBLSN: 4;
    unsigned MSBMSN: 4;
  };
  struct {
    /** The uint16 viewed as an array of two (2) uint8s */
    uint8_t u8Bytes[2];
  };
  struct {
    /** bit 0 (the LSb) of the uint16 */
unsigned b0:
    1;
    /** bit 1 of the uint16 */
unsigned b1:
    1;
    /** bit 2 of the uint16 */
unsigned b2:
    1;
    /** bit 3 of the uint16 */
unsigned b3:
    1;
    /** bit 4 of the uint16 */
unsigned b4:
    1;
    /** bit 5 of the uint16 */
unsigned b5:
    1;
    /** bit 6 of the uint16 */
unsigned b6:
    1;
    /** bit 7 of the uint16 */
unsigned b7:
    1;
    /** bit 8 of the uint16 */
unsigned b8:
    1;
    /** bit 9 of the uint16 */
unsigned b9:
    1;
    /** bit 10 of the uint16 */
unsigned b10:
    1;
    /** bit 11 of the uint16 */
unsigned b11:
    1;
    /** bit 12 of the uint16 */
unsigned b12:
    1;
    /** bit 13 of the uint16 */
unsigned b13:
    1;
    /** bit 14 of the uint16 */
unsigned b14:
    1;
    /** bit 15 (the MSb) of the uint16 */
unsigned b15:
    1;
  };
} UINT16;

/**
 * Returns the Least-Significant-Byte (LSB)
 * \param a UINT16 structure holding a 16-bit value
 * \returns LSB of the 16-bit value in the UINT16 struct
 * \hideinitializer
 */
#define LSB(a)      ((a).u8Bytes[0])
/**
 * Returns the Most-Significant-Byte (MSB)
 * \param a UINT16 structure holding a 16-bit value
 * \returns MSB of the 16-bit value in the UINT16 struct
 * \hideinitializer
 */
#define MSB(a)      ((a).u8Bytes[1])

/**
 * Union structure to hold an uint32 to provide access to
 * 32-bit data with different "viewpoints" (or casts, if you will).
 * \note Works on machines that store their
 * data little-endian.  If you use big-endian, the members will
 * have to be reordered!
 * \note Prefix for all UINT32 structs is <em>U32_</em>
 * \code
 * UINT32     U32_x;
 * uint16     u16_a;
 * uint8      u8_b;
 * UINT16     U16_y;
 *
 * U32_x._uint32 = 0xDEADBEEF;
 * u16_a = U32_x.u16LoWord;           // u16_a equals 0xBEEF
 * u8_b = U32_x.u8Bytes[0];           // u8_b equals 0xEF
 * u16_a = U32_x.u16Words[1];         // u16_a equals 0xDEAD
 * U16_y._uint16 = U32_x.u16LoWord;   // where's the "BEEF"?
 * u8_b = U16_y.uMSb;                 // u8_b equals 0xBE
 * while (U32_x.b31);                 // infinite loop
 * \endcode
 */
typedef union _UINT32 {
  /** uint32 viewed as an uint32 */
  uint32_t _uint32;
  /** uint32 viewed as an uint32 */
  uint32_t u32;
  struct {
    /** The LSB of the least-signficant uint16 in the 32-bit data */
    uint8_t u8LoLsb;
    /** The MSB of the least-signficant uint16 in the 32-bit data */
    uint8_t u8LoMsb;
    /** The LSB of the most-signficant uint16 in the 32-bit data */
    uint8_t u8HiLsb;
    /** The MSB of the most-signficant uint16 in the 32-bit data */
    uint8_t u8HiMsb;
  };
  struct {
    /** The least-significant uint16 in the 32-bit data */
    uint16_t u16LoWord;
    /** The most-significant uint16 in the 32-bit data */
    uint16_t u16HiWord;
  };
  struct {
    /** The uint32 viewed as an array of two (2) uint16s */
    uint16_t u16Words[2];
  };
  struct {
    /** The uint32 viewed as an array of four (4) uint8s */
    uint8_t u8Bytes[4];
  };
  struct {
unsigned b0:
    1;
    /** bit 1 (the LSb) of the uint32 */
unsigned b1:
    1;
    /** bit 2 of the uint32 */
unsigned b2:
    1;
    /** bit 3 of the uint32 */
unsigned b3:
    1;
    /** bit 4 of the uint32 */
unsigned b4:
    1;
    /** bit 5 of the uint32 */
unsigned b5:
    1;
    /** bit 6 of the uint32 */
unsigned b6:
    1;
    /** bit 7 of the uint32 */
unsigned b7:
    1;
    /** bit 8 of the uint32 */
unsigned b8:
    1;
    /** bit 9 of the uint32 */
unsigned b9:
    1;
    /** bit 10 of the uint32 */
unsigned b10:
    1;
    /** bit 11 of the uint32 */
unsigned b11:
    1;
    /** bit 12 of the uint32 */
unsigned b12:
    1;
    /** bit 13 of the uint32 */
unsigned b13:
    1;
    /** bit 14 of the uint32 */
unsigned b14:
    1;
    /** bit 15 of the uint32 */
unsigned b15:
    1;
    /** bit 16 of the uint32 */
unsigned b16:
    1;
    /** bit 17 of the uint32 */
unsigned b17:
    1;
    /** bit 18 of the uint32 */
unsigned b18:
    1;
    /** bit 19 of the uint32 */
unsigned b19:
    1;
    /** bit 20 of the uint32 */
unsigned b20:
    1;
    /** bit 21 of the uint32 */
unsigned b21:
    1;
    /** bit 22 of the uint32 */
unsigned b22:
    1;
    /** bit 23 of the uint32 */
unsigned b23:
    1;
    /** bit 24 of the uint32 */
unsigned b24:
    1;
    /** bit 25 of the uint32 */
unsigned b25:
    1;
    /** bit 26 of the uint32 */
unsigned b26:
    1;
    /** bit 27 of the uint32 */
unsigned b27:
    1;
    /** bit 28 of the uint32 */
unsigned b28:
    1;
    /** bit 29 of the uint32 */
unsigned b29:
    1;
    /** bit 30 of the uint32 */
unsigned b30:
    1;
    /** bit 31 (MSb) of the uint32 */
unsigned b31:
    1;
  };
} UINT32;

// Now, some macros to help navigate these structures.....
/**
 * Returns the Least-Significant-WORD (uint16) of a UINT32 structure
 * \param a UINT32 structure holding a 32-bit value
 * \returns LSW of the 32-bit value in the UINT32 struct
 * \hideinitializer
 */
#define LOWER_WORD(a)   ((a).u16Words[0])
/**
 * Returns the Most-Significant-WORD (uint16) of a UINT32 structure
 * \param a UINT32 structure holding a 32-bit value
 * \returns MSW of the 32-bit value in the UINT32 struct
 * \hideinitializer
 */
#define UPPER_WORD(a)   ((a).u16Words[1])
/**
 * Returns the Least-Significant-BYTE (uint8) of a UINT32 structure
 * \param a UINT32 structure holding a 32-bit value
 * \returns Byte 0 (LSB) of the 32-bit value in the UINT32 struct
 * \hideinitializer
 */
#define LOWER_LSB(a)    ((a).u8Bytes[0])
/**
 * Returns the 2nd most least-Significant-BYTE (uint8) of a UINT32 structure
 * \param a UINT32 structure holding a 32-bit value
 * \returns Byte 1 of the 32-bit value in the UINT32 struct
 * \hideinitializer
 */
#define LOWER_MSB(a)    ((a).u8Bytes[1])
/**
 * Returns the 3nd most least-Significant-BYTE (uint8) of a UINT32 structure
 * \param a UINT32 structure holding a 32-bit value
 * \returns Byte 2 of the 32-bit value in the UINT32 struct
 * \hideinitializer
 */
#define UPPER_LSB(a)    ((a).u8Bytes[2])
/**
 * Returns the Most-Significant-BYTE (uint8) of a UINT32 structure
 * \param a UINT32 structure holding a 32-bit value
 * \returns Byte 3 (MSB) of the 32-bit value in the UINT32 struct
 * \hideinitializer
 */
#define UPPER_MSB(a)    ((a).u8Bytes[3])

/**
 * A single bit quantity. Takes on the value TRUE or FALSE.
 * \hideinitializer
 */
typedef enum _BOOL {
  /** False, not true, off, zero */
  FALSE = 0,
  /** True, not false, on, one */
  TRUE
} BOOL;

/** another way to say \ref TRUE */
#define OK              TRUE
/** another way to say \ref FALSE */
#define FAIL            FALSE

#define ON        TRUE
#define OFF       FALSE

#define HIGH      TRUE
#define LOW       FALSE

#define ENABLE      TRUE
#define DISABLE     FALSE

/** An uninitialized pointer */
#define NULLPTR         0
/** An unitialized index value */
#define NULLIDX         0xFF
/** Mask to represent bit 0 (the LSb) */
#define BIT0            0x0001
/** Mask to represent bit 1 */
#define BIT1            0x0002
/** Mask to represent bit 2 */
#define BIT2            0x0004
/** Mask to represent bit 3 */
#define BIT3            0x0008
/** Mask to represent bit 4 */
#define BIT4            0x0010
/** Mask to represent bit 5 */
#define BIT5            0x0020
/** Mask to represent bit 6 */
#define BIT6            0x0040
/** Mask to represent bit 7 (the MSb of a uint8) */
#define BIT7            0x0080
/** Mask to represent bit 8 (the LSb of the most-significant byte in an uint8)*/
#define BIT8            0x0100
/** Mask to represent bit 9 */
#define BIT9            0x0200
/** Mask to represent bit 10 */
#define BIT10           0x0400
/** Mask to represent bit 11 */
#define BIT11           0x0800
/** Mask to represent bit 12 */
#define BIT12           0x1000
/** Mask to represent bit 13 */
#define BIT13           0x2000
/** Mask to represent bit 14 */
#define BIT14           0x4000
/** Mask to represent bit 15 (the MSb of an uint16) */
#define BIT15           0x8000

/**
 * Sets the bits of a variable according to a mask
 * \param var variable containing the bits to "set"
 * \param mask varable with <em>ONE</em>s in the locations where you want to <em>set</em> bits
 * \hideinitializer
 */
#define BIT_SET_MASK(var, mask)             ((var) |= (mask))
/**
 * Clear the bits of a variable according to a mask
 * \param var variable containing the bits to "clear"
 * \param mask varable with <em>ONE</em>s in the locations where you want to <em>clear</em> bits
 * \hideinitializer
 */
#define BIT_CLEAR_MASK(var, mask)           ((var) &= (~(mask)))
/**
 * Toggle the bits of a variable according to a mask
 * \param var variable containing the bits to "toggle"
 * \param mask varable with <em>ONE</em>s in the locations where you want to <em>toggle</em> bits
 * \hideinitializer
 */
#define BIT_TOGGLE_MASK(var, mask)          ((var) ^= (mask))
/**
 * Determine if one or more bits are set in a variable
 * \param var variable containing the bits to test
 * \param mask varable with <em>ONE</em>s in the locations where you want to check for <em>set</em> bits
 * \retval TRUE if one or more of the mask bits are set in the variable
 * \retval FALSE if none of the mask bits are set
 * \note Does <em>NOT</em> test to see if you variable has ALL mask bits set
 * \hideinitializer
 */
#define IS_BIT_SET_MASK(var, mask)          (((var) & (mask)))
/**
 * Determine if one or more bits are cleared in a variable
 * \param var variable containing the bits to test
 * \param mask varable with <em>ONE</em>s in the locations where you want to check for <em>cleared</em> bits
 * \retval TRUE if one or more of the mask bits are cleared in the variable
 * \retval FALSE if none of the mask bits are cleared
 * \note Does <em>NOT</em> test to see if you variable has ALL mask bits cleared
 * \hideinitializer
 */
#define IS_BIT_CLEAR_MASK(var, mask)        ((~(var) & (mask)))

/**
 * Sets a single bit in a variable
 * \param var variable
 * \param bitnum Number (0-?) of the bit that should be <em>set</em>
 * \hideinitializer
 */
#define BIT_SET(var, bitnum)             ((var) |= (1 << (bitnum)))
/**
 * Clears a single bit in a variable
 * \param var variable
 * \param bitnum Number (0-?) of the bit that should be <em>cleared</em>
 * \hideinitializer
 */
#define BIT_CLEAR(var, bitnum)           ((var) &= (~(1 << (bitnum))))
/**
 * Toggle a single bit in a variable
 * \param var variable
 * \param bitnum Number (0-?) of the bit that should be <em>toggleed</em>
 * \hideinitializer
 */
#define BIT_TOGGLE(var, bitnum)          ((var) ^= (1 << (bitnum)))
/**
 * Determine if a bit is set in a variable
 * \param var variable containing the bits to test
 * \param bitnum Number (0-?) of the bit to test for being <em>set</em>
 * \retval TRUE if the bit is set
 * \retval FALSE otherwise
 * \hideinitializer
 */
#define IS_BIT_SET(var, bitnum)          ((var) & (1 << (bitnum)))
/**
 * Determine if a bit is cleared in a variable
 * \param var variable containing the bits to test
 * \param bitnum Number (0-?) of the bit to test for being <em>cleared</em>
 * \retval TRUE if the bit is clear
 * \retval FALSE otherwise
 * \hideinitializer
 */
#define IS_BIT_CLEAR(var, bitnum)        (~(var) & ((1 << (bitnum))))
