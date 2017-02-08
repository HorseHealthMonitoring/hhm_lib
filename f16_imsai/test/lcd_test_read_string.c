#include "esos.h"
#include "esos_comm.h"
#include "esos_lcd44780.h"
#include "esos_f14ui.h"

char psz_newLine[3] = {0x0D, 0x0A, 0};

ESOS_USER_TASK (lcdTest)
{
	ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
	static const uint8_t u8_y = 48;
	static const uint8_t u8_z = 49;
	static const uint8_t u8_a = 50;
	static uint8_t u8_row = 0x00;
	static uint8_t u8_col = 0x00;
	static BOOL b_showCursor = TRUE;
	static uint8_t u8_zRow = 0;
	static uint8_t u8_zCol = 5;
	static uint8_t u8_data[4] = {0};
	esos_lcd44780_setCursorDisplay(b_showCursor);
	while(TRUE)
	{
		esos_lcd44780_setCursor(u8_row, u8_col);
		esos_lcd44780_writeChar(u8_zRow, u8_zCol, u8_y);
		esos_lcd44780_writeChar(u8_zRow, u8_zCol + 1, u8_z);
		esos_lcd44780_writeChar(u8_zRow, u8_zCol + 2, u8_a);
		esos_uiF14_toggleLED1();
		esos_lcd44780_getBuffer(u8_zRow, u8_zCol, u8_data, 3);
		ESOS_TASK_WAIT_TICKS(1000);
		esos_lcd44780_writeChar(u8_zRow, u8_zCol, ' ');
		esos_lcd44780_writeChar(u8_zRow, u8_zCol + 1, ' ');
		esos_lcd44780_writeChar(u8_zRow, u8_zCol + 2, ' ');
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(u8_data);
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		u8_row += 1;
		u8_col += 1;
		if (u8_row >= ESOS_LCD44780_MEM_HEIGHT)
			u8_row = 0;
		if ((u8_col + 2) >= ESOS_LCD44780_MEM_WIDTH)
			u8_col = 0;
		++u8_zRow;
		--u8_zCol;
		if (u8_zRow >= ESOS_LCD44780_MEM_HEIGHT)
			u8_zRow = 0;
		if (u8_zCol >= ESOS_LCD44780_MEM_WIDTH)
			u8_zCol = 5;
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