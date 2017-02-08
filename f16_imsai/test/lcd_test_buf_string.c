#include "esos.h"
//#include "esos_comm.h"
#include "esos_lcd44780.h"
#include "esos_f14ui.h"

ESOS_USER_TASK (lcdTest)
{
	ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
	static const uint8_t u8_data_array[4] = {0x30, 0x40, 0x50, 0x60}; 
	static const uint8_t u8_data_array_clear[4] = {' ', ' ',  ' ', ' '};
	static const uint8_t u8_y = 48;
	static uint8_t u8_row = 0;
	static uint8_t u8_col = 3;
	static BOOL b_showCursor = TRUE;
	static uint8_t u8_zRow = 0;
	static uint8_t u8_zCol = 7;
	esos_lcd44780_setCursorDisplay(b_showCursor);
	while(TRUE)
	{
		esos_lcd44780_setCursor(u8_row, u8_col);
		//esos_lcd44780_writeChar(u8_zRow, u8_zCol, u8_y);
		esos_lcd44780_writeBuffer(u8_row, u8_col, u8_data_array, 4);
		esos_uiF14_toggleLED1();
		ESOS_TASK_WAIT_TICKS(1000);
		esos_lcd44780_writeBuffer(u8_row, u8_col, u8_data_array_clear, 4);
		u8_row = !u8_row;
		ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
	}

	ESOS_TASK_END();
}

void user_init(void)
{
	config_esos_uiF14();
	esos_uiF14_flashLED3(250);
	esos_lcd44780_configDisplay();
	esos_lcd44780_init();
	esos_RegisterTask(lcdTest);
}