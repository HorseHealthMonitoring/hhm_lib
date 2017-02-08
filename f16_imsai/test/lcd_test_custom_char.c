#include "esos.h"
#include "esos_comm.h"
#include "esos_lcd44780.h"
#include "esos_f14ui.h"

char psz_newLine[3] = {0x0D, 0x0A, 0};

const uint8_t LCD_CUSTOM_I[8] = {0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110, 0b00000};
const uint8_t LCD_CUSTOM_M[8] = {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001, 0b00000};
const uint8_t LCD_CUSTOM_S[8] = {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110, 0b00000};
const uint8_t LCD_CUSTOM_A[8] = {0b01110, 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b00000};

#define LCD_CUSTOM_I_LOCATION (0x01)
#define LCD_CUSTOM_M_LOCATION (0x02)
#define LCD_CUSTOM_S_LOCATION (0x03)
#define LCD_CUSTOM_A_LOCATION (0x04)


ESOS_USER_TASK (lcdTest)
{
	ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
	esos_lcd44780_setCustomChar(1, LCD_CUSTOM_I);
	esos_lcd44780_setCustomChar(2, LCD_CUSTOM_M);
	esos_lcd44780_setCustomChar(3, LCD_CUSTOM_S);
	esos_lcd44780_setCustomChar(4, LCD_CUSTOM_A);
	ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
	static const uint8_t u8_y = 0x30;
	static uint8_t u8_row = 0x00;
	static uint8_t u8_col = 0x00;
	static BOOL b_showCursor = TRUE;
	static uint8_t u8_zRow = 0;
	static uint8_t u8_zCol = 2;
	static uint8_t u8_data[6] = {LCD_CUSTOM_I_LOCATION, LCD_CUSTOM_M_LOCATION, LCD_CUSTOM_S_LOCATION, LCD_CUSTOM_A_LOCATION, LCD_CUSTOM_I_LOCATION, 0};
	static uint8_t u8_dataClear[6] = {' ', ' ', ' ', ' ', ' ', 0};
	esos_lcd44780_setCursorDisplay(b_showCursor);
	while(TRUE)
	{
		esos_lcd44780_setCursor(u8_row, u8_col);
		esos_lcd44780_writeString(u8_zRow, u8_zCol, u8_data);
		esos_uiF14_toggleLED1();
		ESOS_TASK_WAIT_TICKS(1000);
		esos_lcd44780_writeString(u8_zRow, u8_zCol, u8_dataClear);
        ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent());
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
			u8_zCol = 2;
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