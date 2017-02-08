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

/**
 * \addtogroup ESOS_Task_LCD_Service
 * @{
 */
 
/*** I N C L U D E S *************************************************/
#include "esos_pic24_lcd44780.h"
#include "pic24_all.h"
#include "all_generic.h"

/*** T H E   C O D E *************************************************/
void __esos_lcd44780_pic24_config ( void ){
    
}

void __esos_lcd44780_pic24_setDataPins( uint8_t u8_data) {
	// get bit 0
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D0();
	else
		__ESOS_LCD44780_HW_CLEAR_D0();
	
	// bit 1
	u8_data >>= 1;
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D1();
	else
		__ESOS_LCD44780_HW_CLEAR_D1();

	// bit 2
	u8_data >>= 1;
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D2();
	else
		__ESOS_LCD44780_HW_CLEAR_D2();

	// bit 3
	u8_data >>= 1;
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D3();
	else
		__ESOS_LCD44780_HW_CLEAR_D3();

	// bit 4
	u8_data >>= 1;
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D4();
	else
		__ESOS_LCD44780_HW_CLEAR_D4();

	// bit 5
	u8_data >>= 1;
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D5();
	else
		__ESOS_LCD44780_HW_CLEAR_D5();

	// bit 6
	u8_data >>= 1;
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D6();
	else
		__ESOS_LCD44780_HW_CLEAR_D6();

	// bit 7
	u8_data >>= 1;
	if (u8_data & 0x01)
		__ESOS_LCD44780_HW_SET_D7();
	else
		__ESOS_LCD44780_HW_CLEAR_D7();
}

uint8_t __esos_lcd44780_pic24_getDataPins( void ) {
	uint8_t u8_data = 0;
	if (__ESOS_LCD44780_HW_GET_D0()){
		u8_data += 1;
	}
	if (__ESOS_LCD44780_HW_GET_D1()){
		u8_data += 2;
	}
	if (__ESOS_LCD44780_HW_GET_D2()){
		u8_data += 4;
	}
	if (__ESOS_LCD44780_HW_GET_D3()){
		u8_data += 8;
	}
	if (__ESOS_LCD44780_HW_GET_D4()){
		u8_data += 16;
	}
	if (__ESOS_LCD44780_HW_GET_D5()){
		u8_data += 32;
	}
	if (__ESOS_LCD44780_HW_GET_D6()){
		u8_data += 64;
	}
	if (__ESOS_LCD44780_HW_GET_D7()){
		u8_data += 128;
	}
	return u8_data;
}

void __esos_lcd44780_pic24_configDataPinsAsInput( void ) {
	CONFIG_RE0_AS_DIG_INPUT();
	CONFIG_RE1_AS_DIG_INPUT();
	CONFIG_RE2_AS_DIG_INPUT();
	CONFIG_RE3_AS_DIG_INPUT();
	CONFIG_RE4_AS_DIG_INPUT();
	CONFIG_RE5_AS_DIG_INPUT();
	CONFIG_RE6_AS_DIG_INPUT();
	CONFIG_RE7_AS_DIG_INPUT();
	
}

void __esos_lcd44780_pic24_configDataPinsAsOutput( void ) {
	CONFIG_RD10_AS_DIG_OUTPUT(); //SET LCDE
	CONFIG_RD11_AS_DIG_OUTPUT(); //SET LCDRW
	CONFIG_RC12_AS_DIG_OUTPUT(); //SET LCDRS
	CONFIG_RE0_AS_DIG_OUTPUT();
	CONFIG_RE1_AS_DIG_OUTPUT();
	CONFIG_RE2_AS_DIG_OUTPUT();
	CONFIG_RE3_AS_DIG_OUTPUT();
	CONFIG_RE4_AS_DIG_OUTPUT();
	CONFIG_RE5_AS_DIG_OUTPUT();
	CONFIG_RE6_AS_DIG_OUTPUT();
	CONFIG_RE7_AS_DIG_OUTPUT();
	
}

void __esos_pic24_config_char_lcd( void ){
	
	
}
