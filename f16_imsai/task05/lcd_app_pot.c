//Includes
#include "lcd_app_pot.h"

/************************************************************************
* User supplied defines
************************************************************************
*/
#define POT_DISPLAY_ROW (0)
#define POT_DISPLAY_COLUMN (0)
#define BOTTOM_ROW (1)
#define LEFTMOST_COLUMN (0)
#define RIGHTMOST_COLUMN (7)
#define SLIDER_BAR (0b11111111)

#define IS_LCD_TEMP_ACTIVE (esos_IsUserFlagSet(ESOS_USER_FLAG_D))

/************************************************************************
* User supplied functions
************************************************************************
*/
void convertUINT16toHEXstring(uint16_t u16_x, char* ac_hexString){
	uint8_t        u8_c;

	ac_hexString[0] = '0';
	ac_hexString[1] = 'x';
	u8_c = (u16_x >> 8);
	ac_hexString[2] = __esos_u8_GetMSBHexCharFromUint8(u8_c);
	ac_hexString[3] = __esos_u8_GetLSBHexCharFromUint8(u8_c);
	u8_c = (uint8_t)u16_x;
	ac_hexString[4] = __esos_u8_GetMSBHexCharFromUint8(u8_c);
	ac_hexString[5] = __esos_u8_GetLSBHexCharFromUint8(u8_c);
	ac_hexString[6] = 0;
}

/************************************************************************
* ESOS tasks
************************************************************************
*/
ESOS_USER_TASK(t5_lcdPot){
	
	static uint16_t u16_potValue;
	uint8_t u8_potSliderColumn;
	char ac_hexString[7];
	
	ESOS_TASK_BEGIN();
	
	
	while (TRUE){
	
		ESOS_TASK_WAIT_UNTIL(!IS_LCD_TEMP_ACTIVE);
		//get the current ADC value from the potentiometer
		ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(POT_CHANNEL, VREF);
		ESOS_TASK_WAIT_SENSOR_QUICK_READ(u16_potValue);
		ESOS_SENSOR_CLOSE();
		
		esos_lcd44780_clearScreen();
		
		//write the potentiometer value as hex string to the LCD screen (line 1)
		convertUINT16toHEXstring(u16_potValue, ac_hexString);
		esos_lcd44780_writeString (POT_DISPLAY_ROW, POT_DISPLAY_COLUMN, ac_hexString);

		
		//set row and column values and print the slider bar		
		if (u16_potValue == 0x0000){//if pot value is minimum
			//slider is on far left
			esos_lcd44780_writeChar (BOTTOM_ROW, LEFTMOST_COLUMN, SLIDER_BAR);
		}
			
		else if (u16_potValue == 0x0FFF){//else if pot value is maximum
			//slider is on far right
			esos_lcd44780_writeChar (BOTTOM_ROW, RIGHTMOST_COLUMN, SLIDER_BAR);
		}
		
		else{
			//slider is proportional to to the pot value
			if (u16_potValue > 0 && u16_potValue < 511){// 0 < x < 511.875, column 1
				u8_potSliderColumn = 0;
			}
			else if (u16_potValue >= 512 && u16_potValue <= 1023){// 511.875 < x < 1023.75, column 2
				u8_potSliderColumn = 1;
			}
			else if (u16_potValue >= 1024 && u16_potValue <= 1535){// 1023.75 < x < 1535.625, column 3
				u8_potSliderColumn = 2;
			}
			else if (u16_potValue >= 1536 && u16_potValue <= 2047){// 1535.625 < x < 2047.5, column 4
				u8_potSliderColumn = 3;
			}
			else if (u16_potValue >= 2048 && u16_potValue <= 2559){// 2047.5 < x < 2559.375, column 5
				u8_potSliderColumn = 4;
			}
			else if (u16_potValue >= 2560 && u16_potValue <= 3071){// 2559.375 < x < 3071.25, column 6
				u8_potSliderColumn = 5;
			}
			else if (u16_potValue >= 3072 && u16_potValue <= 3583){// 3071.25 < x < 3583.125, column 7
				u8_potSliderColumn = 6;
			}
			else if (u16_potValue >= 3584 && u16_potValue <= 4095){// 3583.125 < x < 4095, column 8
				u8_potSliderColumn =7;
			}
			
			esos_lcd44780_writeChar (BOTTOM_ROW, u8_potSliderColumn, SLIDER_BAR);
			
		}
		ESOS_TASK_WAIT_TICKS(100);
	}
	ESOS_TASK_END();
	
}

