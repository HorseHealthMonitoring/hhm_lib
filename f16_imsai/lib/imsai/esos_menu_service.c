//Include
#include "esos_menu_service.h"

//Defines
#define LCD_CUSTOM_LEFT_ARROW_LOCATION (0x00)
#define LCD_CUSTOM_UP_ARROW_LOCATION (0x01)
#define LCD_CUSTOM_DOWN_ARROW_LOCATION (0x02)

#define LCD_ARROW_LOCATION_ROW_0 (0)
#define LCD_ARROW_LOCATION_ROW_1 (1)
#define LCD_ARROW_LOCATION_COLUMN (7)

//Globals

uint8_t gu8_menu_choice;
uint8_t gu8_num_menus;
BOOL gb_main_menu_flag = TRUE;
BOOL gb_update_menu_flag = TRUE;
BOOL gb_select_flag = FALSE; 

const uint8_t LCD_CUSTOM_LEFT_ARROW[8] = {0b00000,0b00001,0b00011,0b00111,0b00011,0b00001,0b00000};
const uint8_t LCD_CUSTOM_UP_ARROW[8] = {0b00100,0b01110,0b11111,0b00000,0b00000,0b00000,0b00000};
const uint8_t LCD_CUSTOM_DOWN_ARROW[8] = {0b00000,0b00000,0b00000,0b00000,0b11111,0b01110,0b00100};

uint8_t getActiveSelectDataCount(const menu_t* p_menu) {
	uint8_t u8_activeCount = 0;
	uint8_t u8_index = 0;
	for(u8_index = 0; u8_index < p_menu->u8_numItems; ++u8_index) {
		if ( !(p_menu->ast_select_data[u8_index].b_disabled) ) {
			++u8_activeCount;
		}
	}
	return u8_activeCount;
}

uint8_t getIndexOfNextActiveSelectDataGoingDown(const menu_t* p_menu) {
	uint8_t u8_index;
	for (u8_index = (p_menu->u8_choice + 1); u8_index < p_menu->u8_numItems; ++u8_index) {
		if (!(p_menu->ast_select_data[u8_index].b_disabled)) {
			return u8_index;
		}
	}
	return u8_index;
}

uint8_t getIndexOfNextActiveSelectDataGoingUp(const menu_t* p_menu) {
	uint8_t u8_index;
	if (p_menu->u8_choice == 0) {
		return 0;
	}
	for (u8_index = (p_menu->u8_choice - 1); u8_index > 0; --u8_index) {
		if (!(p_menu->ast_select_data[u8_index].b_disabled)) {
			return u8_index;
		}
	}
	if (!(p_menu->ast_select_data[u8_index].b_disabled)) {
		return 0;
	} else {
		return p_menu->u8_choice;
	}
}

BOOL nextActiveSelectDatagoingDownExists(const menu_t* p_menu) {
	return getIndexOfNextActiveSelectDataGoingDown(p_menu) < p_menu->u8_numItems;
}

BOOL nextActiveSelectDatagoingUpExists(const menu_t* p_menu) {
	return getIndexOfNextActiveSelectDataGoingUp(p_menu) != p_menu->u8_choice;
}


void updateMenu()
{
	gb_update_menu_flag = FALSE;
}

void setArrowsCustomChars(void) {
	esos_lcd44780_setCustomChar(LCD_CUSTOM_LEFT_ARROW_LOCATION, LCD_CUSTOM_LEFT_ARROW);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_UP_ARROW_LOCATION, LCD_CUSTOM_UP_ARROW);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_DOWN_ARROW_LOCATION, LCD_CUSTOM_DOWN_ARROW);
}

//Structure pointer
menu_t** gp_menu_list;

//Global Structure
menu_t* gp_menu;

//Tasks
ESOS_USER_TASK ( display_menu_task ) { 
	//Setting the custom characters for the LCD screen for up, down and left arrows
    setArrowsCustomChars();
	ESOS_TASK_BEGIN();
	
    while (TRUE) {
		ESOS_TASK_WAIT_UNTIL(!gb_update_menu_flag);
        esos_lcd44780_clearScreen();
		
		//checks for a menu that has more than one static information display so it can place arrows appropriately 
		if (gb_main_menu_flag || (gp_menu->u8_subMenuType == SUB_MENU_STATIC_INFO_DISPLAY)){
			//menu items
			esos_lcd44780_writeString(0,0,gp_menu->ast_items[gp_menu->u8_choice].aac_items[0]);
			esos_lcd44780_writeString(1,0,gp_menu->ast_items[gp_menu->u8_choice].aac_items[1]);
			//UP and DOWN Arrows
			if (gp_menu->u8_numItems > 1){
				if ((gp_menu->u8_choice == 0)){//first menu element
					esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_1, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_DOWN_ARROW_LOCATION);
				}
				else if (gp_menu->u8_choice == (gp_menu->u8_numItems - 1)){//last menu element
					esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_0, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_UP_ARROW_LOCATION);
				}
				else{//all elements that aren't the first or last
					esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_0, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_UP_ARROW_LOCATION);
					esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_1, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_DOWN_ARROW_LOCATION);
				}
			}
		}

		else if (gp_menu->u8_subMenuType == SUB_MENU_SINGLE_VALUE_TWO_CHOICE){
			esos_lcd44780_writeString(0,0,gp_menu->ast_items[0].aac_items[0]);
			esos_lcd44780_writeString(0,0,gp_menu->ast_items[1].aac_items[0]);
			if (gb_select_flag){
				esos_lcd44780_writeChar(gp_menu->u8_choice, LCD_ARROW_LOCATION_COLUMN, LCD_CUSTOM_LEFT_ARROW_LOCATION);
			}			
		}
		
		else if (gp_menu->u8_subMenuType == SUB_MENU_SINGLE_VALUE_NUMERICS){
			esos_lcd44780_writeString(0,0,gp_menu->aac_numeric_data[0]);
			if (gb_select_flag){
				esos_lcd44780_writeChar(gp_menu->u8_choice, LCD_ARROW_LOCATION_COLUMN, LCD_CUSTOM_LEFT_ARROW_LOCATION);
			}
		}
		else if (gp_menu->u8_subMenuType == SUB_MENU_DOUBLE_VALUE_NUMERICS){
			esos_lcd44780_writeString(0,0,gp_menu->aac_numeric_data[0]);
			esos_lcd44780_writeString(0,0,gp_menu->aac_numeric_data[1]);
			if (gb_select_flag){
				esos_lcd44780_writeChar(gp_menu->u8_choice, LCD_ARROW_LOCATION_COLUMN, LCD_CUSTOM_LEFT_ARROW_LOCATION);
			}
		}
		else if (gp_menu->u8_subMenuType == SUB_MENU_SINGLE_VALUE_DATA_SELECTION){
			esos_lcd44780_writeString(0,0,gp_menu->ast_items[0].aac_items[0]);
			esos_lcd44780_writeString(1,0,gp_menu->ast_select_data[gp_menu->u8_choice].ac_data);
		}
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK ( selection_handler ) {
    ESOS_TASK_BEGIN();
    
    while ( TRUE ) {
		
		if (gb_main_menu_flag){
			if (esos_uiF14_isRpgTurningCW()) {
				if(gp_menu->u8_choice < (gp_menu->u8_numItems - 1)){
					gp_menu->u8_choice = gp_menu->u8_choice + 1;
					ESOS_TASK_WAIT_TICKS(1100);				
				}
			} else if (esos_uiF14_isRpgTurningCCW()) {
				if(gp_menu->u8_choice > 0){
					gp_menu->u8_choice = gp_menu->u8_choice - 1;
					ESOS_TASK_WAIT_TICKS(1100);				
				}
			}
		
			//updates the flags to set the sub menu based on the current choice
			if (esos_uiF14_getSW3Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
				if (gp_menu->onSW3press != 0)
					gp_menu->onSW3press(gp_menu->u8_choice);
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = FALSE;
				gu8_menu_choice = gp_menu->u8_choice;
			}
		}
		//Decide how the switches will control the submenu
		//Single Val 2 choice
		else if(!gb_main_menu_flag && (gp_menu->u8_subMenuType == SUB_MENU_SINGLE_VALUE_TWO_CHOICE)){
			if (esos_uiF14_getSW1Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
				gb_select_flag = TRUE;
				gp_menu->u8_choice = 0;
			}
			//SW2 selects second option
			if (esos_uiF14_getSW2Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW2Released());
				gb_select_flag = TRUE;
				gp_menu->u8_choice = 1;
			}
			//SW3 confirm and go back to main menu
			if (esos_uiF14_getSW3Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());				
				gp_menu->onSW3press(gp_menu->u8_choice);
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = TRUE;
				gb_select_flag = FALSE;
			}
			//RPG picks which one is selected
			if (esos_uiF14_isRpgTurningCW() && gp_menu->u8_choice == 0) {
				gb_select_flag = TRUE;
				gp_menu->u8_choice = 1;			
			}
			else if (esos_uiF14_isRpgTurningCCW() && gp_menu->u8_choice == 1) {
				gb_select_flag = TRUE;
				gp_menu->u8_choice = 0;			
			}
		}
		//Single Numeric
		else if(!gb_main_menu_flag && (gp_menu->u8_subMenuType == SUB_MENU_SINGLE_VALUE_NUMERICS)){
			static uint16_t u16_rpgTicksLastUpdate = 0;

			if (esos_uiF14_getSW1Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
				gb_select_flag = TRUE;
				gp_menu->u8_choice = 0;
			}
			if (esos_uiF14_getSW3Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());				
				gp_menu->onSW3press(gp_menu->u8_choice);
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = TRUE;
				gb_select_flag = FALSE;
			}
			if (esos_uiF14_isRpgTurning() && gb_select_flag){
				if (esos_uiF14_isRpgTurningFast())
					gp_menu->onRPGturnFast(gp_menu->u8_choice);
				else
					gp_menu->onRPGturn(gp_menu->u8_choice);
				ESOS_TASK_WAIT_TICKS(1000);
			}
		}
		//Double Numeric
		else if(!gb_main_menu_flag && (gp_menu->u8_subMenuType == SUB_MENU_DOUBLE_VALUE_NUMERICS)){
			if (esos_uiF14_getSW1Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
				gb_select_flag = TRUE;
				gp_menu->u8_choice = 0;
			}
			if (esos_uiF14_getSW2Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW2Released());
				gb_select_flag = TRUE;
				gp_menu->u8_choice = 1;
			}
			if (esos_uiF14_getSW3Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());				
				gp_menu->onSW3press(gp_menu->u8_choice);
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = TRUE;
				gb_select_flag = FALSE;
			}
			if (esos_uiF14_isRpgTurning() && gb_select_flag){
				if (esos_uiF14_isRpgTurningFast())
					gp_menu->onRPGturnFast(gp_menu->u8_choice);
				else
					gp_menu->onRPGturn(gp_menu->u8_choice);
				ESOS_TASK_WAIT_TICKS(1100);
			}
		}
		//Single Data selection
		else if(!gb_main_menu_flag && (gp_menu->u8_subMenuType == SUB_MENU_SINGLE_VALUE_DATA_SELECTION)){
			uint8_t u8_activeDataSelectCount = getActiveSelectDataCount(gp_menu);
			if ((u8_activeDataSelectCount == 0)) {
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = TRUE;
			} else if (gp_menu->ast_select_data[gp_menu->u8_choice].b_disabled) {
				if (nextActiveSelectDatagoingDownExists(gp_menu)) {
					uint8_t u8_nextActiveMenuChoice = getIndexOfNextActiveSelectDataGoingDown(gp_menu);
					gp_menu->u8_choice = u8_nextActiveMenuChoice;
				} else if (nextActiveSelectDatagoingUpExists(gp_menu)) {
					uint8_t u8_nextActiveMenuChoice = getIndexOfNextActiveSelectDataGoingUp(gp_menu);
					gp_menu->u8_choice = u8_nextActiveMenuChoice;
				}
			} else {
				if (esos_uiF14_getSW3Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());				
					gp_menu->onSW3press(gp_menu->u8_choice);
					if (gp_menu->pst_menuAfterChoice != 0) {
						gb_update_menu_flag = FALSE;
						gp_menu = gp_menu->pst_menuAfterChoice;
					} else {
						gb_update_menu_flag = TRUE;
						gb_main_menu_flag = TRUE;
					}
				}
				if (esos_uiF14_isRpgTurningCW() && nextActiveSelectDatagoingDownExists(gp_menu)){
					uint8_t u8_nextActiveMenuChoice = getIndexOfNextActiveSelectDataGoingDown(gp_menu);
					gp_menu->u8_choice = u8_nextActiveMenuChoice;
					ESOS_TASK_WAIT_TICKS(1100);
				}
				if (esos_uiF14_isRpgTurningCCW() && nextActiveSelectDatagoingUpExists(gp_menu)){
					uint8_t u8_nextActiveMenuChoice = getIndexOfNextActiveSelectDataGoingUp(gp_menu);
					gp_menu->u8_choice = u8_nextActiveMenuChoice;
					ESOS_TASK_WAIT_TICKS(1100);
				}
			}
		}
		//Static Display
		else if(!gb_main_menu_flag && (gp_menu->u8_subMenuType == SUB_MENU_STATIC_INFO_DISPLAY)){
			if (esos_uiF14_isRpgTurningCW()) {
				
				if(gp_menu->u8_choice < (gp_menu->u8_numItems - 1)){
					gp_menu->u8_choice = gp_menu->u8_choice + 1;
					ESOS_TASK_WAIT_TICKS(1100);				
				}
			} else if (esos_uiF14_isRpgTurningCCW()) {
				if(gp_menu->u8_choice > 0){
					gp_menu->u8_choice = gp_menu->u8_choice - 1;
					ESOS_TASK_WAIT_TICKS(1100);				
				}
			}
		
			//updates the flags to set the sub menu based on the current choice
			if (esos_uiF14_getSW3Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
				if (gp_menu->onSW3press != 0)
					gp_menu->onSW3press(0);
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = TRUE;
			}
			if (esos_uiF14_getSW1Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = TRUE;
			}
			if (esos_uiF14_getSW2Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW2Released());
				gb_update_menu_flag = TRUE;
				gb_main_menu_flag = TRUE;
			}
		}
			
        
		ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(set_menu_task){
	ESOS_TASK_BEGIN();
	while(TRUE){
		ESOS_TASK_WAIT_UNTIL(gb_update_menu_flag);
		if (gb_main_menu_flag){
			gp_menu = gp_menu_list[0];
		}
		else if ((gu8_menu_choice + 1) < gu8_num_menus) {
			gp_menu = gp_menu_list[gu8_menu_choice+1];
		}
		gb_update_menu_flag = FALSE;
	}
	ESOS_TASK_END();
}

//Functions
void setMenu(menu_t** menus, uint8_t numMenus){
	gp_menu_list = menus;
	gu8_num_menus = numMenus;
}
