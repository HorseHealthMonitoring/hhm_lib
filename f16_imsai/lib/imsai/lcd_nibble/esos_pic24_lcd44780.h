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

#ifndef ESOS_pic24_LCD_H
#define ESOS_pic24_LCD_H

/**
 * \addtogroup ESOS_Task_LCD_Service
 * @{
 */
 
 /** \file
 *  \brief This file contains routines which configure and
 *  use an LCD interface on the Microchip PIC24 MCUs.
 */

/* I N C L U D E S **********************************************************/
#include "pic24_all.h"
#include "all_generic.h"

/* HW-SPECIFIC MACROS *******************************************************/
#define  __esos_lcd44780_hw_config() __esos_lcd44780_pic24_config()
#define esos_hw_config_char_lcd() __esos_pic24_config_char_lcd()
#define  __esos_lcd44780_hw_setDataPins(u8_data) __esos_lcd44780_pic24_setDataPins(u8_data)
#define  __esos_lcd44780_hw_getDataPins() __esos_lcd44780_pic24_getDataPins()
#define  __esos_lcd44780_hw_configDataPinsAsInput() __esos_lcd44780_pic24_configDataPinsAsInput()
#define  __esos_lcd44780_hw_configDataPinsAsOutput() __esos_lcd44780_pic24_configDataPinsAsOutput()

#define  __ESOS_LCD44780_HW_SET_E_HIGH() (_LATD10 = 1)           
#define  __ESOS_LCD44780_HW_SET_E_LOW() (_LATD10 = 0)                   
#define  __ESOS_LCD44780_HW_SET_RW_READ() (_LATD11 = 1)               
#define  __ESOS_LCD44780_HW_SET_RW_WRITE() (_LATD11 = 0)             
#define  __ESOS_LCD44780_HW_SET_RS_REGISTERS() (_LATC12 = 0)     
#define  __ESOS_LCD44780_HW_SET_RS_DATA() (_LATC12 = 1)               

#define  __ESOS_LCD44780_HW_SET_D0() (_LATE0 = 1)             
#define  __ESOS_LCD44780_HW_CLEAR_D0() (_LATE0 = 0)            
#define  __ESOS_LCD44780_HW_GET_D0() (_RE0)           
#define  __ESOS_LCD44780_HW_SET_D1() (_LATE1 = 1)             
#define  __ESOS_LCD44780_HW_CLEAR_D1() (_LATE1 = 0)            
#define  __ESOS_LCD44780_HW_GET_D1() (_RE1)             
#define  __ESOS_LCD44780_HW_SET_D2() (_LATE2 = 1)              
#define  __ESOS_LCD44780_HW_CLEAR_D2() (_LATE2 = 0)            
#define  __ESOS_LCD44780_HW_GET_D2() (_RE2)             
#define  __ESOS_LCD44780_HW_SET_D3() (_LATE3 = 1)             
#define  __ESOS_LCD44780_HW_CLEAR_D3() (_LATE3 = 0)            
#define  __ESOS_LCD44780_HW_GET_D3() (_RE3)             
#define  __ESOS_LCD44780_HW_SET_D4() (_LATE4 = 1)             
#define  __ESOS_LCD44780_HW_CLEAR_D4() (_LATE4 = 0)           
#define  __ESOS_LCD44780_HW_GET_D4() (_RE4)             
#define  __ESOS_LCD44780_HW_SET_D5() (_LATE5 = 1)             
#define  __ESOS_LCD44780_HW_CLEAR_D5() (_LATE5 = 0)           
#define  __ESOS_LCD44780_HW_GET_D5() (_RE5)             
#define  __ESOS_LCD44780_HW_SET_D6() (_LATE6 = 1)             
#define  __ESOS_LCD44780_HW_CLEAR_D6() (_LATE6 = 0)           
#define  __ESOS_LCD44780_HW_GET_D6() (_RE6)             
#define  __ESOS_LCD44780_HW_SET_D7() (_LATE7 = 1)             
#define  __ESOS_LCD44780_HW_CLEAR_D7() (_LATE7 = 0)            
#define  __ESOS_LCD44780_HW_GET_D7() (_RE7)            

/* P R I V A T E   P R O T O T Y P E S *****************************************/
void __esos_lcd44780_pic24_config ( void );
void __esos_lcd44780_pic24_setDataPins( uint8_t u8_data);
uint8_t __esos_lcd44780_pic24_getDataPins( void );
void __esos_lcd44780_pic24_configDataPinsAsInput( void );
void __esos_lcd44780_pic24_configDataPinsAsOutput( void );
void __esos_pic24_config_char_lcd( void );

#endif
