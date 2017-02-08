#ifndef ESOS_MENU_SERVICE_H_	
#define ESOS_MENU_SERVICE_H_

#include "esos.h"
#include "esos_pic24_sensor.h"
#include "revF16.h"
#include "esos_comm.h"
#include "esos_f14ui.h"
#include "all_generic.h"
#include "esos_comm_helpers.h"
#include "esos_pic24.h"
#include "esos_lcd44780.h"

void updateMenu(void);
void setArrowsCustomChars(void);

typedef struct menu_t menu_t;

//Structures
typedef struct {
	const char* ac_data;
	BOOL b_disabled;
} menu_select_data_t;

typedef struct {
	char aac_items[2][8];
	BOOL b_disabled;
} menu_item_t;

struct menu_t{
    uint8_t u8_numItems;
    uint8_t u8_choice;
    menu_item_t *ast_items;
	uint8_t u8_subMenuType;
	char (*aac_numeric_data)[8];
	menu_select_data_t* ast_select_data;
	void (*onRPGturn)(uint8_t);
	void (*onRPGturnFast)(uint8_t);
	void (*onSW3press)(uint8_t);
	BOOL b_disabled;
	menu_t* pst_menuAfterChoice;
}; 

void setMenu(menu_t**, uint8_t);
ESOS_USER_TASK ( display_menu_task );
ESOS_USER_TASK ( selection_handler );
ESOS_USER_TASK(set_menu_task);

//Defines
#define SUB_MENU_SINGLE_VALUE_TWO_CHOICE (0)
#define SUB_MENU_SINGLE_VALUE_NUMERICS (1)
#define SUB_MENU_DOUBLE_VALUE_NUMERICS (2)
#define SUB_MENU_SINGLE_VALUE_DATA_SELECTION (3)
#define SUB_MENU_STATIC_INFO_DISPLAY (4)

#endif