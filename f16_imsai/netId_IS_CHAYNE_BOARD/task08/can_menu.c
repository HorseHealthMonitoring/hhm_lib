//Include
#include "esos_menu_service.h"
#include "t6_app_austin.h"
#include "CANTeamMembers.h"
#include "MyCanStats.h"
#include "fcn_synth.h"
#include "can_temp.h"

#define NUM_MENUS (8)
#define LM60_CHOICE (5)
#define AMP_CHOICE (3)

#define LCD_CUSTOM_ROW1_LOCATION (0x00)
#define LCD_CUSTOM_ROW2_LOCATION (0x01)
#define LCD_CUSTOM_ROW3_LOCATION (0x02)
#define LCD_CUSTOM_ROW4_LOCATION (0x03)
#define LCD_CUSTOM_ROW5_LOCATION (0x04)
#define LCD_CUSTOM_ROW6_LOCATION (0x05)

//Global
const uint16_t heartbeatTicks = 500;
uint16_t gu16_freq_min = 64;
uint16_t gu16_freq_max = 2047;
uint16_t gu16_duty_min = 0;
uint16_t gu16_duty_max = 100;
uint16_t gu16_ampltd_min = 0;
uint16_t gu16_ampltd_max = 33;
BOOL gb_runLM60 = FALSE;

const uint8_t LCD_CUSTOM_ROW1[8] = {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111};
const uint8_t LCD_CUSTOM_ROW2[8] = {0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW3[8] = {0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW4[8] = {0b00000,0b00000,0b00000,0b11111,0b11111,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW5[8] = {0b00000,0b00000,0b11111,0b11111,0b11111,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW6[8] = {0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111};

//Define
#define NUM_ITEMS_MAIN_MENU (7)
#define CHOICE_DEFAULT_ZERO (0)

//Function prototypes
void confirmWvformMenu(uint8_t);
void confirmFrequencyMenu(uint8_t);
void confirmAmplitudeMenu(uint8_t);
void confirmDutyMenu(uint8_t);
void confirmLEDMenu(uint8_t);
void setFreqRPGturning(uint8_t);
void setFreqRPGturningFast(uint8_t);
void setDutyRPGturning(uint8_t);
void setDutyRPGturningFast(uint8_t);
void setAmplRPGturning(uint8_t);
void setAmplRPGturningFast(uint8_t);
void goToSubMenu(uint8_t);
void startLM60(uint8_t);
void stopLM60(uint8_t);

// Config Menus
// Declare the menu items specific for this task
menu_item_t main_menu_items[] = {
	{ .aac_items = {"Set", "wvform"} },
	{ .aac_items = {"Set", "freq"} },
	{ .aac_items = {"Set", "ampltd"} },
	{ .aac_items = {"Set", "duty"} },
	{ .aac_items = {"Read", "LM60"} },
	{ .aac_items = {"Set", "LEDs"} },
	{ .aac_items = {"About", "..."} }
};
	

menu_item_t about_menu_items[] = {
	{ .aac_items = {"Emily", "eeg87"} },
	{ .aac_items = {"Tyler", "tjg120"} },
	{ .aac_items = {"Chayne", "ct446"} },
	{ .aac_items = {"Austin", "ajh524"} }
};

menu_item_t ampltd_ast_data[] = {
	{ .aac_items = {"ampltd", ""} }
};


//select data for wvform menu
menu_item_t wvform_ast_data[] ={
	{ .aac_items = {"wvform", ""} },
	{ .aac_items = {"wvform", ""} },
	{ .aac_items = {"wvform", ""} }
};

menu_item_t lm60User_ast_data[] ={
	{ .aac_items = {"netId", ""} }
};

menu_select_data_t wvform_select_data[] ={
	{ .ac_data = "tri", .b_disabled = FALSE },
	{ .ac_data = "sine", .b_disabled = FALSE },
	{ .ac_data = "square", .b_disabled = FALSE }
};

menu_select_data_t lm60_user_select_data[NUM_OF_IDS];

char ampltd_numeric_data[][8] ={
	{ "0.0" }
};

menu_item_t lm60_ast_data[] = {
	{ .aac_items = { "LM60", "00" } }
};

// Numeric data
// current value, low value, high value, reg turn, fast turn
char freq_numeric_data[2][8] = {"64", ""};
char duty_numeric_data[2][8] = {"0", ""};
menu_select_data_t setLED_select_data[8] = { 
	{ .ac_data = "0", .b_disabled = FALSE },
	{ .ac_data = "1", .b_disabled = FALSE },
	{ .ac_data = "2", .b_disabled = FALSE },
	{ .ac_data = "3", .b_disabled = FALSE },
	{ .ac_data = "4", .b_disabled = FALSE },
	{ .ac_data = "5", .b_disabled = FALSE },
	{ .ac_data = "6", .b_disabled = FALSE },
	{ .ac_data = "7", .b_disabled = FALSE }
};


// Populated menu structures	
menu_t main_menu = {
	.u8_numItems = NUM_ITEMS_MAIN_MENU,
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.ast_items = main_menu_items,
	.onSW3press = goToSubMenu,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};
	
menu_t wvform_menu = {
	.u8_numItems = 3,
	.u8_subMenuType = SUB_MENU_SINGLE_VALUE_DATA_SELECTION,
	.ast_items = wvform_ast_data,
	.ast_select_data = wvform_select_data,
	.onSW3press = confirmWvformMenu,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};

menu_t freq_menu = {
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.u8_subMenuType = SUB_MENU_SINGLE_VALUE_NUMERICS,
	.aac_numeric_data = freq_numeric_data,
	.onSW3press = confirmFrequencyMenu,
	.onRPGturn = setFreqRPGturning,
	.onRPGturnFast = setFreqRPGturningFast,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};

menu_t ampltd_menu = {
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.u8_subMenuType = SUB_MENU_SINGLE_VALUE_NUMERICS,
	.aac_numeric_data = ampltd_numeric_data,
	.onSW3press = confirmAmplitudeMenu,
	.onRPGturn = setAmplRPGturning,
	.onRPGturnFast = setAmplRPGturningFast,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};

menu_t duty_menu = {
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.u8_subMenuType = SUB_MENU_SINGLE_VALUE_NUMERICS,
	.aac_numeric_data = duty_numeric_data,
	.onSW3press = confirmDutyMenu,
	.onRPGturn = setDutyRPGturning,
	.onRPGturnFast = setDutyRPGturningFast,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};

menu_t LM60_menu = {
	.u8_numItems = 1,
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.u8_subMenuType = SUB_MENU_STATIC_INFO_DISPLAY,
	.ast_items = lm60_ast_data,
	.onSW3press = stopLM60,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};

menu_t LM60BoardChoice_menu = {
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.u8_numItems = NUM_OF_IDS,
	.u8_subMenuType = SUB_MENU_SINGLE_VALUE_DATA_SELECTION,
	.ast_items = lm60User_ast_data,
	.ast_select_data = lm60_user_select_data,
	.onSW3press = startLM60,
	.b_disabled = FALSE,
	//.pst_menuAfterChoice = 0
	.pst_menuAfterChoice = &LM60_menu
};

menu_t setLED_menu = {
	.u8_numItems = 8,
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.u8_subMenuType = SUB_MENU_SINGLE_VALUE_DATA_SELECTION,
	.ast_select_data = setLED_select_data,
	.onSW3press = confirmLEDMenu,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};

menu_t about_menu = {
	.u8_numItems = 4,
	.u8_choice = CHOICE_DEFAULT_ZERO,
	.ast_items = about_menu_items,
	.u8_subMenuType = SUB_MENU_STATIC_INFO_DISPLAY,
	.b_disabled = FALSE,
	.pst_menuAfterChoice = 0
};

menu_t** menu_list; 
uint8_t u8_length = 8;


// Functions

int8_t convertMvToDegreesC(uint16_t u16_pointOneMvReading)
{
    int16_t i16_adjustedPointOneMvReading = u16_pointOneMvReading - TEMP_SENSOR_TENTH_MV_OFFSET;
    int8_t i8_degreesCelsius = (int8_t)(i16_adjustedPointOneMvReading / TEMP_SENSOR_TENTH_MV_PER_DEGREES_C);
    return i8_degreesCelsius;
}

uint16_t fcn_convertStringToInteger(char* num){
	uint8_t  i, len;
	uint16_t result=0;
	len = strlen(num);
	for(i=0; i<len; i++){
		result = (result * 10) + ( num[i] - '0' );
	}
	return result;
}

void confirmWvformMenu(uint8_t u8_wvformChoice){
	if (u8_wvformChoice == 0){
		duty_menu.b_disabled = TRUE;
		fcn_synth_setWaveForm(Triangle);
	} 
	else if(u8_wvformChoice == 1){
		duty_menu.b_disabled = TRUE;
		fcn_synth_setWaveForm(Sine);
	}
	else if(u8_wvformChoice == 2){
		duty_menu.b_disabled = FALSE;
		fcn_synth_setWaveForm(Square);
	}
}

void confirmFrequencyMenu(uint8_t u8_freqNumIndex){
	uint16_t temp = fcn_convertStringToInteger(freq_menu.aac_numeric_data[u8_freqNumIndex]);
	fcn_synth_setFreq(temp);
}

void confirmAmplitudeMenu(uint8_t u8_ampChoice){
	static const uint8_t U8_AMPLITUDE_SCALAR = 124;

	char ac_numString[5];
	ac_numString[0] = ampltd_menu.aac_numeric_data[0][0];
	ac_numString[1] = ampltd_menu.aac_numeric_data[0][2];
	ac_numString[2] = 0;

	uint16_t temp = fcn_convertStringToInteger(ac_numString);

	fcn_synth_setAmplitude(U8_AMPLITUDE_SCALAR * temp);
}

void confirmDutyMenu(uint8_t u8_dutyVal){
	uint16_t temp = fcn_convertStringToInteger(duty_menu.aac_numeric_data[u8_dutyVal]);
	fcn_synth_setDutyCycle(temp);
}

void confirmLEDMenu(uint8_t u8_ledVal){
	if (u8_ledVal == 0){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3Off();
	}
	if (u8_ledVal == 1){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3On();
	}
	if (u8_ledVal == 2){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3Off();
	}
	if (u8_ledVal == 3){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3On();
	}
	if (u8_ledVal == 4){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3Off();
	}
	if (u8_ledVal == 5){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3On();
	}
	if (u8_ledVal == 6){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3Off();
	}
	if (u8_ledVal == 7){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3On();
	}
}

void goToSubMenu(uint8_t u8_choice){
	if (u8_choice == (LM60_CHOICE - 1))
	{
		// updateMenu();
		// gb_runLM60 = TRUE;
	}
	else if (u8_choice == (AMP_CHOICE - 1))
	{
		// // register custom characters
		// esos_lcd44780_setCustomChar(LCD_CUSTOM_ROW1_LOCATION, LCD_CUSTOM_ROW1);
		// esos_lcd44780_setCustomChar(LCD_CUSTOM_ROW2_LOCATION, LCD_CUSTOM_ROW2);
		// esos_lcd44780_setCustomChar(LCD_CUSTOM_ROW3_LOCATION, LCD_CUSTOM_ROW3);
		// esos_lcd44780_setCustomChar(LCD_CUSTOM_ROW4_LOCATION, LCD_CUSTOM_ROW4);
		// esos_lcd44780_setCustomChar(LCD_CUSTOM_ROW5_LOCATION, LCD_CUSTOM_ROW5);
		// esos_lcd44780_setCustomChar(LCD_CUSTOM_ROW6_LOCATION, LCD_CUSTOM_ROW6);
	}
}

void startLM60(uint8_t u8_choice) {
	const char* psz_netIdChosen = LM60BoardChoice_menu.ast_select_data[u8_choice].ac_data;
	uint8_t u8_userIndex = 0;
	for (u8_userIndex = 0; u8_userIndex < NUM_OF_IDS; ++u8_userIndex) {
		if (psz_netIdChosen == aCANID_IDs[u8_userIndex].psz_netID) {
			setTargetBoard(u8_userIndex);
		}
	}
	gb_runLM60 = TRUE;
}

void stopLM60(uint8_t u8_choice){
	// ignore what's passed in since
	// it doesn't matter for this
	gb_runLM60 = FALSE;
	stopGettingTemp();
}

void fcn_convertIntegerToString(uint16_t u16_number, char* pu8_string){
	  uint8_t      u8_cIndex;
	  uint8_t      u8_digit;
	  uint16_t     u16_tmp;

	  u8_cIndex = 0;
	  u8_digit = 0;
	  if (u16_number > 9999)
		pu8_string[u8_digit++] = '0' + (u16_number / 10000);
	  if (u16_number > 999)
		pu8_string[u8_digit++] = '0' + ((u16_number % 10000) / 1000);
	  if (u16_number > 99)
		pu8_string[u8_digit++] = '0' + ((u16_number % 1000) / 100);
	  if (u16_number > 9)
	  	pu8_string[u8_digit++] = '0' + ((u16_number % 100) / 10);
	  pu8_string[u8_digit++] = '0' + (u16_number % 10);
	  pu8_string[u8_digit++] = 0;
}

//RPG turning functions
void setFreqRPGturning(uint8_t index){
	uint16_t temp = fcn_convertStringToInteger(freq_menu.aac_numeric_data[index]);
	if (esos_uiF14_isRpgTurningCW()){
		if (temp < gu16_freq_max){
			temp += 1;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (temp > gu16_freq_min){
			temp -= 1;
		}
	}
	fcn_convertIntegerToString(temp, freq_menu.aac_numeric_data[index]);
}

void setFreqRPGturningFast(uint8_t index){
	uint16_t temp = fcn_convertStringToInteger(freq_menu.aac_numeric_data[index]);
	if (esos_uiF14_isRpgTurningCW()){
		if (temp < gu16_freq_max){
			if ((gu16_freq_max - temp) < 50)
				temp = gu16_freq_max;
			else
				temp += 50;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (temp > gu16_freq_min){
			if ((temp - gu16_freq_min) < 50)
				temp = gu16_freq_min;
			else
				temp -= 50;
		}
	}
	fcn_convertIntegerToString(temp, freq_menu.aac_numeric_data[index]);
}

void setAmplRPGturning(uint8_t index){
	char ac_numString[5];
	ac_numString[0] = ampltd_menu.aac_numeric_data[index][0];
	ac_numString[1] = ampltd_menu.aac_numeric_data[index][2];
	ac_numString[2] = 0;

	uint16_t temp = fcn_convertStringToInteger(ac_numString);
	if (esos_uiF14_isRpgTurningCW()){
		if (temp < gu16_ampltd_max){
			temp += 1;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (temp > gu16_ampltd_min){
			temp -= 1;
		}
	}
	fcn_convertIntegerToString(temp, ac_numString);
	ampltd_menu.aac_numeric_data[index][0] = ac_numString[0];
	ampltd_menu.aac_numeric_data[index][1] = '.';
	ampltd_menu.aac_numeric_data[index][2] = ac_numString[1];
}

void setAmplRPGturningFast(uint8_t index){
	char ac_numString[5];
	ac_numString[0] = ampltd_menu.aac_numeric_data[index][0];
	ac_numString[1] = ampltd_menu.aac_numeric_data[index][2];
	ac_numString[2] = 0;

	uint16_t temp = fcn_convertStringToInteger(ac_numString);
	if (esos_uiF14_isRpgTurningCW()){
		if (temp < gu16_ampltd_max){
			if ((gu16_ampltd_max - temp) < 10)
				temp = gu16_ampltd_max;
			else
				temp += 10;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (temp > gu16_ampltd_min){
			if ((temp - gu16_ampltd_min) < 10)
				temp = gu16_ampltd_min;
			else
				temp -= 10;
		}
	}
	fcn_convertIntegerToString(temp, ac_numString);
	ampltd_menu.aac_numeric_data[index][0] = ac_numString[0];
	ampltd_menu.aac_numeric_data[index][1] = '.';
	ampltd_menu.aac_numeric_data[index][2] = ac_numString[1];
}

void setDutyRPGturning(uint8_t index){
	uint16_t temp = fcn_convertStringToInteger(duty_menu.aac_numeric_data[index]);
	if (esos_uiF14_isRpgTurningCW()){
		if (temp < gu16_duty_max){
			temp += 1;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (temp > gu16_duty_min){
			temp -= 1;
		}
	}
	fcn_convertIntegerToString(temp, duty_menu.aac_numeric_data[index]);
}

void setDutyRPGturningFast(uint8_t index){
	uint16_t temp = fcn_convertStringToInteger(duty_menu.aac_numeric_data[index]);
	if (esos_uiF14_isRpgTurningCW()){
		if (temp < gu16_duty_max){
			if ((gu16_duty_max - temp) < 10)
				temp = gu16_duty_max;
			else
				temp += 10;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (temp > gu16_duty_min){
			if ((temp - gu16_duty_min) < 10)
				temp = gu16_duty_min;
			else
				temp -= 10;
		}
	}
	fcn_convertIntegerToString(temp, duty_menu.aac_numeric_data[index]);
}

menu_select_data_t* getUserMenuEntryByTeamAndMemberId(uint8_t u8_teamId, uint8_t u8_memberId) {
	uint8_t u8_index;
	for(u8_index = 0; u8_index < NUM_OF_IDS; ++u8_index) {
		if ((aCANID_IDs[u8_index].u8_teamID == u8_teamId) &&  ((aCANID_IDs[u8_index].u8_memberID == u8_memberId))) {
			return &(lm60_user_select_data[u8_index]);
		}
	}
	return 0;
}

void addUserToMenu(uint8_t u8_teamId, uint8_t u8_memberId) {
	menu_select_data_t* pst_userMenuEntry = getUserMenuEntryByTeamAndMemberId(u8_teamId, u8_memberId);
	if (pst_userMenuEntry != 0) {
		pst_userMenuEntry->b_disabled = FALSE;
	}
}

void removeUserFromMenu(uint8_t u8_teamId, uint8_t u8_memberId) {
	menu_select_data_t* pst_userMenuEntry = getUserMenuEntryByTeamAndMemberId(u8_teamId, u8_memberId);
	if (pst_userMenuEntry != 0) {
		pst_userMenuEntry->b_disabled = TRUE;
	}
}

menu_t* x[NUM_MENUS];

ESOS_USER_TASK(display_temp){
	static uint16_t u16_data = 0;
	static uint8_t u8_celsius = 0;
	ESOS_TASK_BEGIN();
	while (TRUE){
		
		ESOS_TASK_WAIT_UNTIL(gb_runLM60);

		u8_celsius = getCeliusReadingOfTargetBoard();
  		uint8_t u8_firstChar = u8_celsius / 10; 
  		uint8_t u8_secondChar = u8_celsius % 10;
  		//convert the first and second digits to the correlating location values hosted in the LCDs memory.
  		u8_firstChar = 0x30 + u8_firstChar;
  		u8_secondChar = 0x30 + u8_secondChar;

  		// update the screen to reflect the temperature
		LM60_menu.ast_items[0].aac_items[1][0] = u8_firstChar;
		LM60_menu.ast_items[0].aac_items[1][1] = u8_secondChar;
		updateMenu();

		ESOS_TASK_WAIT_TICKS(200);
	}
	ESOS_TASK_END();
}

ESOS_USER_TASK (addRemoveMembers) {
	ESOS_TASK_BEGIN();

	static uint8_t u8_i = 0;
	static BOOL b_goingUp = TRUE;
	while(TRUE) {
		if (u8_i == MY_ID) {
			if (b_goingUp)
				++u8_i;
			else
				--u8_i;
		}
		if (b_goingUp) {
			addUserToMenu(aCANID_IDs[u8_i].u8_teamID, aCANID_IDs[u8_i].u8_memberID);
		} else {
			removeUserFromMenu(aCANID_IDs[u8_i].u8_teamID, aCANID_IDs[u8_i].u8_memberID);
		}
		if (b_goingUp)
			++u8_i;
		else
			--u8_i;
		ESOS_TASK_WAIT_TICKS(10000);
		if (u8_i >= NUM_OF_IDS) {
			u8_i = NUM_OF_IDS - 1;
			b_goingUp = FALSE;
		} else if (u8_i == 0) {
			b_goingUp = TRUE;
		}
	}

	ESOS_TASK_END();
}


//user_init()
void configCanMenu(void){
	uint8_t u8_idNum;
	for(u8_idNum = 0; u8_idNum < NUM_OF_IDS; ++u8_idNum) {
		lm60_user_select_data[u8_idNum].ac_data = aCANID_IDs[u8_idNum].psz_netID;
		if (u8_idNum == MY_ID) {
			lm60_user_select_data[u8_idNum].b_disabled = FALSE;
		} else {
			lm60_user_select_data[u8_idNum].b_disabled = TRUE;
		}
	}

	//Configure the hardware
	config_esos_uiF14();
	config_fcn_synth();
	esos_pic24_configI2C1(400);


	esos_lcd44780_configDisplay();
	esos_lcd44780_init();
	x[0] = &main_menu;
	x[1] = &wvform_menu;
	x[2] = &freq_menu;
	x[AMP_CHOICE] = &ampltd_menu;
	x[4] = &duty_menu;
	//x[LM60_CHOICE] = &LM60_menu;
	x[LM60_CHOICE] = &LM60BoardChoice_menu;
	x[6] = &setLED_menu;
	x[7] = &about_menu;
	menu_list = x;
	//Initialize the structures in the menu service
	setMenu(menu_list, u8_length);
	
	// //Register the user tasks
	esos_RegisterTask(display_menu_task);
	esos_RegisterTask(selection_handler);
	esos_RegisterTask(set_menu_task);
	esos_RegisterTask(display_temp);
	esos_RegisterTask(i2c_service);
	// esos_RegisterTask(addRemoveMembers);
}
//Things to remember
/*-change aac_numeric_data to a string for printing and back to an int for use
  -  */