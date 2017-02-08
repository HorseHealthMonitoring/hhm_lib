#include "esos.h"
#include "esos_comm.h"
#include "esos_lcd44780.h"
#include "esos_f14ui.h"

char psz_newLine[3] = {0x0D, 0x0A, 0};

ESOS_USER_TASK (lcdTest)
{
	ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
	static const uint8_t u8_y = 0x30;
	static uint8_t u8_row = 0x00;
	static uint8_t u8_col = 0x00;
	static BOOL b_showCursor = TRUE;
	static uint8_t u8_zRow = 0;
	static uint8_t u8_zCol = 7;
	esos_lcd44780_setCursorDisplay(b_showCursor);
	while(TRUE)
	{
		esos_lcd44780_setCursor(u8_row, u8_col);
		esos_lcd44780_writeChar(u8_zRow, u8_zCol, u8_y);
		esos_uiF14_toggleLED1();
		ESOS_TASK_WAIT_TICKS(1000);
		esos_lcd44780_writeChar(u8_zRow, u8_zCol, ' ');
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("row: ");
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_row);
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_WAIT_ON_SEND_STRING("col: ");
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_col);
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		u8_row += 1;
		u8_col += 1;
		if (u8_row >= ESOS_LCD44780_MEM_HEIGHT)
			u8_row = 0;
		if (u8_col >= ESOS_LCD44780_MEM_WIDTH)
			u8_col = 0;
		++u8_zRow;
		--u8_zCol;
		if (u8_zRow >= ESOS_LCD44780_MEM_HEIGHT)
			u8_zRow = 0;
		if (u8_zCol >= ESOS_LCD44780_MEM_WIDTH)
			u8_zCol = 7;
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