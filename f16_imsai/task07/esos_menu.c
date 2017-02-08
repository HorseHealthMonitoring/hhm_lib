//Includes
#include "esos_menu.h"

// DEFINEs    
#define LCD_CUSTOM_LEFT_ARROW_LOCATION (0x01)
#define LCD_CUSTOM_UP_ARROW_LOCATION (0x02)
#define LCD_CUSTOM_DOWN_ARROW_LOCATION (0x03)

#define LCD_ARROW_LOCATION_ROW_0 (0)
#define LCD_ARROW_LOCATION_ROW_1 (1)
#define LCD_ARROW_LOCATION_COLUMN (7)

#define SUB_MENU_SINGLE_VALUE_TWO_CHOICE (0)
#define SUB_MENU_SINGLE_VALUE_NUMERICS (1)
#define SUB_MENU_DOUBLE_VALUE_NUMERICS (2)
#define SUB_MENU_SINGLE_VALUE_DATA_SELECTION (3)
#define SUB_MENU_STATIC_INFO_DISPLAY (4)

// PROTOTYPEs

// GLOBALs 
uint8_t menu_choice;
BOOL main_menu_flag = TRUE;
ESOS_SEMAPHORE(sem_updateMenu);
uint8_t u8_subMenuType;

typedef struct {
    uint8_t u8_numItems;
    uint8_t u8_choice;
    char (*ast_items)[2][8];
	uint8_t u8_subMenuArr[];
} menu_t; 

uint8_t main_menu_sub_menu_type = {
	SUB_MENU_STATIC_INFO_DISPLAY,
	SUB_MENU_SINGLE_VALUE_NUMERICS,
	SUB_MENU_SINGLE_VALUE_NUMERICS,
	SUB_MENU_SINGLE_VALUE_NUMERICS,
	SUB_MENU_STATIC_INFO_DISPLAY,
	SUB_MENU_SINGLE_VALUE_NUMERICS,
	SUB_MENU_STATIC_INFO_DISPLAY	
}

char main_menu[][2][8] = {
	{ "Set",
	"wvform"},
	{ "Set",
	  "freq" },
	{ "Set",
	  "ampltd" },
	{ "Set",
	  "duty" },
	{ "Read",
	  "LM60" },
	{ "Set",
	  "LEDs" },
	{ "About",
	  "..." }
    };
	
char wvform_menu[][2][8] = {
	
	{ "tri",
	  " " },
	{ "sine",
	  " " },
	{ "square",
	  " " }
};

char about_menu[][2][8] = {
	{ "Emily",
	  "eeg87" },
	{ "Tyler",
	  "tjg120" },
	{ "Chayne",
	  "ct446" },
	{ "Austin",
	  "ajh524" }
    };
	
	
char s_val_2_ch_menu[][2][8] = {
	{ "Exit? Y",
	  "N" },
    };
	
const uint8_t LCD_CUSTOM_LEFT_ARROW[8] = {0b00000,0b00001,0b00011,0b00111,0b00011,0b00001,0b00000};
const uint8_t LCD_CUSTOM_UP_ARROW[8] = {0b00100,0b01110,0b11111,0b00000,0b00000,0b00000,0b00000};
const uint8_t LCD_CUSTOM_DOWN_ARROW[8] = {0b00000,0b00000,0b00000,0b00000,0b11111,0b01110,0b00100};

//struct instance
menu_t menu;


inline void set_numItems(uint8_t numitems){
	menu.u8_numItems = numitems;
	return;
}

inline void set_choice(uint8_t choice){
	menu.u8_choice = choice;
	return;
}

inline void set_ast_items(char (*items)[2][8]){
	menu.ast_items = items;
	return;
}

const uint16_t heartbeatTicks = 500;

ESOS_USER_TASK(heartbeat_LED) {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    LED3 = !LED3;
    ESOS_TASK_WAIT_TICKS( heartbeatTicks );
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end heartbeat_LED task

/*
 * An ESOS task to update the LCD44780 structure based on the selected
 * menu choice.
 */
ESOS_USER_TASK ( display_menu_task ) { 
	//Setting the custom characters for the LCD screen for up, down and left arrows
	esos_lcd44780_setCustomChar(LCD_CUSTOM_LEFT_ARROW_LOCATION, LCD_CUSTOM_LEFT_ARROW);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_UP_ARROW_LOCATION, LCD_CUSTOM_UP_ARROW);
	esos_lcd44780_setCustomChar(LCD_CUSTOM_DOWN_ARROW_LOCATION, LCD_CUSTOM_DOWN_ARROW);
    
	ESOS_TASK_BEGIN();
	
	//initially set the display to main_menu
    set_numItems(7);//MAGIC NUMBERSSSS
	set_choice(0);
	set_ast_items(main_menu);
	
    while (TRUE) {
        esos_lcd44780_clearScreen();
		//menu items
		esos_lcd44780_writeString(0,0,menu.ast_items[menu.u8_choice][0]);
		esos_lcd44780_writeString(1,0,menu.ast_items[menu.u8_choice][1]);
		
		//Arrows
		if (menu.u8_choice == 0){//first menu element
			esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_1, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_DOWN_ARROW_LOCATION);
		}
		else if (menu.u8_choice == (menu.u8_numItems - 1)){//last menu element
			esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_0, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_UP_ARROW_LOCATION);
		}
		else{//all elements that aren't the first or last
			esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_0, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_UP_ARROW_LOCATION);
			esos_lcd44780_writeChar(LCD_ARROW_LOCATION_ROW_1, LCD_ARROW_LOCATION_COLUMN ,LCD_CUSTOM_DOWN_ARROW_LOCATION);
		}
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

/*
 * An ESOS task to update the menu choice based on switch activity.
 */
ESOS_USER_TASK ( selection_handler ) {
    ESOS_TASK_BEGIN();
    while ( TRUE ) {
		if (main_menu_flag){
			if (esos_uiF14_getSW3Pressed()){
				ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
				ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
				main_menu_flag = FALSE;
				menu_choice = menu.u8_choice;
				u8_subMenuType = menu.u8_subMenuArr[menu.u8_choice];
			}
		}
		else{
			//Single Val 2 choice
			if(u8_subMenuType == SUB_MENU_SINGLE_VALUE_TWO_CHOICE){
				//SW1 selects first option
				if (esos_uiF14_getSW1Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
					menu.u8_choice = 0;
				}
				//SW2 selects second option
				if (esos_uiF14_getSW2Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW2Released());
					menu.u8_choice = 1;
				}
				//SW3 confirm and go back to main menu
				if (esos_uiF14_getSW3Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
					ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
					main_menu_flag = TRUE;
				}
				//RE picks which one is selected
				if (esos_uiF14_isRpgTurningCW() && menu_choice == 0) {
					menu.u8_choice = 1;			
				}
				else if (esos_uiF14_isRpgTurningCCW() && menu_choice == 1) {
						menu.u8_choice = 0;			
				}
			}
			//Single Numeric
			else if(u8_subMenuType == SUB_MENU_SINGLE_VALUE_NUMERICS){
				//SW1 select value
				if (esos_uiF14_getSW1Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
					menu.u8_choice = 0;
				}
				//SW3 confirm and go back to main menu
				if (esos_uiF14_getSW3Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
					ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
					main_menu_flag = TRUE;
				}
				//RE change value between upper and lower limits
				if (esos_uiF14_isRpgTurningCW()) {
					if(esos_uiF14_isRpgTurningFast())
					{
						//upper limit
						if(menu.numericValue[0] < (menu.numericValue[2])){
							menu.numericValue[0] = menu.numericValue[0] + menu.numericValue[5];
							ESOS_TASK_WAIT_TICKS(1000);				
						}
					}
					else{
						if(menu.numericValue[0] < (menu.numericValue[2])){
							menu.numericValue[0] = menu.numericValue[0] + menu.numericValue[4];
							ESOS_TASK_WAIT_TICKS(1000);				
						}
					}			
				}
				else if (esos_uiF14_isRpgTurningCCW() ) {
					if(esos_uiF14_isRpgTurningFast())
					{
						//lower limit
						if(menu.numericValue[0] > menu.numericValue[3]){
							menu.numericValue[0] = menu.numericValue[0] - menu.numericValue[5];
							ESOS_TASK_WAIT_TICKS(1000);				
						}
					}
					else{
						if(menu.numericValue[0] > menu.numericValue[3]){
							menu.numericValue[0] = menu.numericValue[0] + menu.numericValue[4];
							ESOS_TASK_WAIT_TICKS(1000);				
						}		
					}
				}

			}
			//Double Numeric
			else if(u8_subMenuType == SUB_MENU_DOUBLE_VALUE_NUMERICS){
				//SW1 select value
				if (esos_uiF14_getSW1Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
					menu.u8_choice = 0;
				}
				//SW2 select value
				if (esos_uiF14_getSW2Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW2Released());
					menu.u8_choice = 0;
				}
				//SW3 confirm and go back to main menu
				if (esos_uiF14_getSW3Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
					ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
					main_menu_flag = TRUE;
				}
				//RE change value between upper and lower limits
				if (esos_uiF14_isRpgTurningCW()) {
					if(esos_uiF14_isRpgTurningFast())
					{
						if(menu.numericValue < (menu.upperLimit)){
							menu.numericValue = menu.numericValue + (menu.upperLimit * .1);
							ESOS_TASK_WAIT_TICKS(1000);				
						}
					}
					else{
						if(menu.u8_choice < (menu.u8_numItems - 1)){
						menu.u8_choice = menu.u8_choice + 1;
						ESOS_TASK_WAIT_TICKS(1000);				
						}
					}
							
				}
				else if (esos_uiF14_isRpgTurningCCW() ) {
					if(esos_uiF14_isRpgTurningFast())
					{
						if(menu.numericValue > menu.lowerLimit){
							menu.numericValue = menu.numericValue - (menu.upperLimit * .1);
							ESOS_TASK_WAIT_TICKS(1000);				
						}
					}
					else{
						if(menu.u8_choice > 0){
							menu.u8_choice = menu.u8_choice - 1;
							ESOS_TASK_WAIT_TICKS(1000);				
						}		
					}
				}
			}
			//Single Data selection
			else if(u8_subMenuType == SUB_MENU_SINGLE_VALUE_DATA_SELECTION){
				//SW1 select value
				if (esos_uiF14_getSW1Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
					menu.u8_choice = 0;
				}
				//SW2 select value
				if (esos_uiF14_getSW2Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW2Released());
					menu.u8_choice = 1;
				}
				//SW3 confirm and go back to main menu
				if (esos_uiF14_getSW3Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
					ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
					main_menu_flag = TRUE;
				}
				//TODO:Predefine the list to choose from
				//Clockwise moves down, counterclockwise moves up 
				if (esos_uiF14_isRpgTurningCW()) {
					if(menu.u8_choice < (menu.u8_numItems - 1)){
						menu.u8_choice = menu.u8_choice + 1;
						ESOS_TASK_WAIT_TICKS(1000);				
					}
				} else if (esos_uiF14_isRpgTurningCCW()) {
					if(menu.u8_choice > 0){
						menu.u8_choice = menu.u8_choice - 1;
						ESOS_TASK_WAIT_TICKS(1000);				
					}
				}
			}
			//Static Display
			else if(u8_subMenuType == SUB_MENU_STATIC_INFO_DISPLAY){
				//SW1 confirm and go back to main menu
				if (esos_uiF14_getSW1Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Released());
					ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
					main_menu_flag = TRUE;
				}
				//SW2 confirm and go back to main menu
				if (esos_uiF14_getSW2Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW2Released());
					ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
					main_menu_flag = TRUE;
				}
				//SW3 confirm and go back to main menu
				if (esos_uiF14_getSW3Pressed()){
					ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW3Released());
					ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 1);
					main_menu_flag = TRUE;
				}
				//Clockwise moves down, counterclockwise moves up
				if (esos_uiF14_isRpgTurningCW()) {
					if(menu.u8_choice < (menu.u8_numItems - 1)){
						menu.u8_choice = menu.u8_choice + 1;
						ESOS_TASK_WAIT_TICKS(1000);				
					}
				} else if (esos_uiF14_isRpgTurningCCW()) {
					if(menu.u8_choice > 0){
						menu.u8_choice = menu.u8_choice - 1;
						ESOS_TASK_WAIT_TICKS(1000);				
					}
				}
			}
		}			
	}
        
    ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK ( set_menu_task ) {
	ESOS_TASK_BEGIN();
	
	while(TRUE){
		//Main menu
		ESOS_TASK_WAIT_SEMAPHORE(sem_updateMenu, 1);
		if (main_menu_flag){
			set_numItems(7);//MAGIC NUMBERSSSS...fix it laters
			set_choice(0);
			set_ast_items(main_menu);
		}
		
		ESOS_SIGNAL_SEMAPHORE(sem_updateMenu, 0);
		
	}
	ESOS_TASK_END();
}


//user_init()
void user_init(void){
	//Configure the hardware
	config_esos_uiF14();
	esos_uiF14_flashLED3(heartbeatTicks);
	esos_lcd44780_configDisplay();
	esos_lcd44780_init();
	
	//Register the user tasks
	ESOS_INIT_SEMAPHORE(sem_updateMenu, 0);
	esos_RegisterTask(heartbeat_LED);
	esos_RegisterTask(display_menu_task);
	esos_RegisterTask(set_menu_task);
	esos_RegisterTask( selection_handler);
}
