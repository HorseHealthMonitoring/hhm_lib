//#includes go here. (First include the main esos.h file).
//All other includes are specific to this project.
#include "lcd_app_lm60m.h"

/************************************************************************
* User supplied defines
************************************************************************
*/
#define DISPLAY_TEMPERATURE (ESOS_USER_FLAG_D)
#define DISPLAY_TEMPERATURE_ON (esos_IsUserFlagSet(ESOS_USER_FLAG_D))

#define LCD_CUSTOM_ONE_LINE_LOCATION (0x01)
#define LCD_CUSTOM_TWO_LINE_LOCATION (0x02)
#define LCD_CUSTOM_THREE_LINE_LOCATION (0x03)
#define LCD_CUSTOM_FOUR_LINE_LOCATION (0x04)
#define LCD_CUSTOM_FIVE_LINE_LOCATION (0x05)
#define LCD_CUSTOM_SIX_LINE_LOCATION (0x06)
#define LCD_CUSTOM_SEVEN_LINE_LOCATION (0x07)
#define LCD_BLOCK (0xFF)

/************************************************************************
* User supplied functions
************************************************************************
*/

int8_t convertMvToDegreesC(uint16_t u16_pointOneMvReading)
{
    int16_t i16_adjustedPointOneMvReading = u16_pointOneMvReading - TEMP_SENSOR_TENTH_MV_OFFSET;
    int8_t i8_degreesCelsius = (int8_t)(i16_adjustedPointOneMvReading / TEMP_SENSOR_TENTH_MV_PER_DEGREES_C);
    return i8_degreesCelsius;
}

/************************************************************************
* Globals
************************************************************************
*/
const uint8_t LCD_CUSTOM_ONE_LINE[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111};
const uint8_t LCD_CUSTOM_TWO_LINE[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111};
const uint8_t LCD_CUSTOM_THREE_LINE[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111};
const uint8_t LCD_CUSTOM_FOUR_LINE[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111};
const uint8_t LCD_CUSTOM_FIVE_LINE[8] = {0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
const uint8_t LCD_CUSTOM_SIX_LINE[8] = {0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
const uint8_t LCD_CUSTOM_SEVEN_LINE[8] = {0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};

char psz_newLine[3] = { 0x0D, 0x0A, 0 };

//mult-line dec
const uint8_t au8_tempCharList[8] = {LCD_CUSTOM_ONE_LINE_LOCATION, LCD_CUSTOM_TWO_LINE_LOCATION, LCD_CUSTOM_THREE_LINE_LOCATION, LCD_CUSTOM_FOUR_LINE_LOCATION, LCD_CUSTOM_FIVE_LINE_LOCATION, LCD_CUSTOM_SIX_LINE_LOCATION, LCD_CUSTOM_SEVEN_LINE_LOCATION, LCD_BLOCK};

/************************************************************************
* ESOS tasks
************************************************************************
*/
ESOS_USER_TASK(sampleSW3)
/*
Task Name: sampleSW3
Purpose: This task samples input from SW3 and raises a flag to allow the pot to begin printing every 1 second.
Intendend operation: Upon the press of SW3, this task will raise a user flag that allows the application to print.
					 The flag will be lowered when SW3 pressed, and will not be raised again until SW3 is pressed again.
*/
{
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		//Wait until SW3 is pressed.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();
		//Set the flag for the app to begin printing to the LCD.
		esos_SetUserFlag(DISPLAY_TEMPERATURE);
		//Wait until SW3 is released.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();
		//Wait until SW3 pressed.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();
		//Clear the flag, stopping the application from printing to LCD.
		esos_ClearUserFlag(DISPLAY_TEMPERATURE);
		//Wait until SW3 is pressed again.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();
	}// end of while
	ESOS_TASK_END();
}// end sampleSW3 task

ESOS_USER_TASK(t5_lcdLM60M)
{
	static const uint8_t u8_L = 0x4C;
	static const uint8_t u8_M = 0x4D;
	static const uint8_t u8_six = 0x36;
	static const uint8_t u8_zero = 0x30;
	static const uint8_t u8_C = 0x43;
	static uint16_t u16_data;
	static uint8_t u8_celsius;
	static uint8_t u8_row;
	static uint8_t u8_column;
	static uint8_t u8_thermRow;
	static uint8_t u8_thermColumn;
	static uint8_t u8_tempIndex = 0;

	ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
	esos_lcd44780_setCustomChar(LCD_CUSTOM_ONE_LINE_LOCATION, LCD_CUSTOM_ONE_LINE);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_TWO_LINE_LOCATION, LCD_CUSTOM_TWO_LINE);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_THREE_LINE_LOCATION, LCD_CUSTOM_THREE_LINE);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_FOUR_LINE_LOCATION, LCD_CUSTOM_FOUR_LINE);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_FIVE_LINE_LOCATION, LCD_CUSTOM_FIVE_LINE);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_SIX_LINE_LOCATION, LCD_CUSTOM_SIX_LINE);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_SEVEN_LINE_LOCATION, LCD_CUSTOM_SEVEN_LINE);
	ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());

	while(TRUE)
	{
		ESOS_TASK_WAIT_UNTIL(DISPLAY_TEMPERATURE_ON);

		ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(TEMP_SENSOR_CHANNEL, VREF);
		ESOS_TASK_WAIT_SENSOR_READ(u16_data, ESOS_SENSOR_ONE_SHOT, ESOS_SENSOR_FORMAT_VOLTAGE);
  		ESOS_SENSOR_CLOSE();

  		u8_celsius = (uint8_t)(convertMvToDegreesC(u16_data));

  		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
  		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_celsius); //for testing purposes
  		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		
		esos_lcd44780_clearScreen();

  		uint8_t u8_firstChar = u8_celsius / 10; 
  		uint8_t u8_secondChar = u8_celsius % 10;
  		u8_firstChar = 0x30 + u8_firstChar;
  		u8_secondChar = 0x30 + u8_secondChar;

		//Write 'LM60' to the screen
  		u8_row = 0;
  		u8_column = 0;
		esos_lcd44780_writeChar(u8_row, u8_column, u8_L);
		u8_column = 1;
		esos_lcd44780_writeChar(u8_row, u8_column, u8_M);
		u8_column = 2;
		esos_lcd44780_writeChar(u8_row, u8_column, u8_six);
		u8_column = 3;
		esos_lcd44780_writeChar(u8_row, u8_column, u8_zero);

		u8_row = 1;
		u8_column = 0;
		esos_lcd44780_writeChar(u8_row, u8_column, u8_firstChar);
		u8_column = 1;
		esos_lcd44780_writeChar(u8_row, u8_column, u8_secondChar);
		u8_column = 2;
		esos_lcd44780_writeChar(u8_row, u8_column, u8_C);

		if ((u8_celsius >= 20) && (u8_celsius < 28))
		{
			u8_tempIndex = u8_celsius - 20;
			u8_thermRow = 1;
			u8_thermColumn = 7;
			esos_lcd44780_writeChar(u8_thermRow, u8_thermColumn, au8_tempCharList[u8_tempIndex]);
		}
		else if ((u8_celsius >= 28) && (u8_celsius < 36))
		{
			u8_tempIndex = 7;
			u8_thermRow = 1;
			u8_thermColumn = 7;
			esos_lcd44780_writeChar(u8_thermRow, u8_thermColumn, au8_tempCharList[u8_tempIndex]);
			u8_tempIndex = u8_celsius - 28;
			u8_thermRow = 0;
			u8_thermColumn = 7;
			esos_lcd44780_writeChar(u8_thermRow, u8_thermColumn, au8_tempCharList[u8_tempIndex]);
		}
		else
		{
			u8_tempIndex = 7;
			u8_thermRow = 1;
			u8_thermColumn = 7;
			esos_lcd44780_writeChar(u8_thermRow, u8_thermColumn, au8_tempCharList[u8_tempIndex]);
			u8_thermRow = 0;
			u8_thermColumn = 7;
			esos_lcd44780_writeChar(u8_thermRow, u8_thermColumn, au8_tempCharList[u8_tempIndex]);
		}// end of if-else
		ESOS_TASK_WAIT_TICKS(100); // for testing purposes
	}// end of while
	ESOS_TASK_END();
}// end of t5_lcdLM60M task