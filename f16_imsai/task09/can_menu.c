//Include
#include "esos_menu_service.h"
#include "t6_app_austin.h"
#include "CANTeamMembers.h"
#include "MyCanStats.h"
#include "fcn_synth.h"
#include "can_temp.h"
#include "can_fcn_synth.h"

#define NUM_MENUS (9)

#define SET_TEAM_MENU_INDEX (1)
#define WAVEFORM_MENU_INDEX (2)
#define FREQ_MENU_INDEX (3)
#define AMP_MENU_INDEX (4)
#define DUTY_MENU_INDEX (5)
#define LM60_MENU_INDEX (6)
#define SET_LED_MENU_INDEX (7)
#define ABOUT_MENU_INDEX (8)

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
uint16_t gu16_ampltd_max = 0x69; // heh
uint8_t gu8_selectedUserIndex = MY_ID;
BOOL gb_runLM60 = FALSE;
uint8_t au8_menuToUserArrayIndexMapping[NUM_OF_IDS];

inline uint8_t getTargetBoard() {
	return gu8_selectedUserIndex;
}

const uint8_t LCD_CUSTOM_ROW1[8] = {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111};
const uint8_t LCD_CUSTOM_ROW2[8] = {0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW3[8] = {0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW4[8] = {0b00000,0b00000,0b00000,0b11111,0b11111,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW5[8] = {0b00000,0b00000,0b11111,0b11111,0b11111,0b11111,0b11111};
const uint8_t LCD_CUSTOM_ROW6[8] = {0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111};

//Define
#define NUM_ITEMS_MAIN_MENU (NUM_MENUS - 1)
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
void setTeam(uint8_t);

// Config Menus
// Declare the menu items specific for this task
menu_item_t main_menu_items[] = {
	{ .aac_items = {"Set", "Team"} },
	{ .aac_items = {"0 Set", "wvform"} },
	{ .aac_items = {"0 Set", "freq"} },
	{ .aac_items = {"0 Set", "ampltd"} },
	{ .aac_items = {"0 Set", "duty"} },
	{ .aac_items = {"0 Read", "LM60"} },
	{ .aac_items = {"0 Set", "LEDs"} },
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

menu_item_t ast_set_team[] ={
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

menu_t set_team_menu = {
	.u8_numItems = NUM_OF_IDS,
	.u8_subMenuType = SUB_MENU_SINGLE_VALUE_DATA_SELECTION,
	.ast_items = ast_set_team,
	.ast_select_data = lm60_user_select_data,
	.onSW3press = setTeam,
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

uint32_t fcn_convertStringToInteger(char* num){
	uint8_t  i, len;
	uint32_t result=0;
	len = strlen(num);
	for(i=0; i<len; i++){
		result = (result * 10) + ( num[i] - '0' );
	}
	return result;
}

// NOTE: although this takes in a 32 bit number, it caps at 99,999
void fcn_convertIntegerToString(uint32_t u16_number, char* pu8_string){
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

uint8_t convertDecimalStringTo3Point5Number(char* psz_str) {
	char ac_numString[7] = {0};
	ac_numString[0] = psz_str[0];

	uint8_t u8_amp3Point5 = (fcn_convertStringToInteger(ac_numString) << 5);
	memcpy(ac_numString, &(psz_str[2]), 5);

	uint32_t u32_bottom5BitsAsWholeNumber = fcn_convertStringToInteger(ac_numString);
	if (u32_bottom5BitsAsWholeNumber >= 50000) {
		u32_bottom5BitsAsWholeNumber -= 50000;
		u8_amp3Point5 |= 0b10000;
	}
	if (u32_bottom5BitsAsWholeNumber >= 25000) {
		u32_bottom5BitsAsWholeNumber -= 25000;
		u8_amp3Point5 |= 0b1000;
	}
	if (u32_bottom5BitsAsWholeNumber >= 12500) {
		u32_bottom5BitsAsWholeNumber -= 12500;
		u8_amp3Point5 |= 0b100;
	}
	if (u32_bottom5BitsAsWholeNumber >= 6250) {
		u32_bottom5BitsAsWholeNumber -= 6250;
		u8_amp3Point5 |= 0b10;
	}
	if (u32_bottom5BitsAsWholeNumber >= 3125) {
		u32_bottom5BitsAsWholeNumber -= 3125;
		u8_amp3Point5 |= 0b1;
	}
	return u8_amp3Point5;
}

int8_t convertMvToDegreesC(uint16_t u16_pointOneMvReading) {
    int16_t i16_adjustedPointOneMvReading = u16_pointOneMvReading - TEMP_SENSOR_TENTH_MV_OFFSET;
    int8_t i8_degreesCelsius = (int8_t)(i16_adjustedPointOneMvReading / TEMP_SENSOR_TENTH_MV_PER_DEGREES_C);
    return i8_degreesCelsius;
}

void confirmWvformMenu(uint8_t u8_wvformChoice){
	if (u8_wvformChoice == 0){
		if (gu8_selectedUserIndex == MY_ID) {
			duty_menu.b_disabled = TRUE;
			fcn_synth_setWaveForm(Triangle);
		}
		updateBoard(gu8_selectedUserIndex, Triangle, Wvform);
	} 
	else if(u8_wvformChoice == 1){
		if (gu8_selectedUserIndex == MY_ID) {
			duty_menu.b_disabled = TRUE;
			fcn_synth_setWaveForm(Sine);
		}
		updateBoard(gu8_selectedUserIndex, Sine, Wvform);
	}
	else if(u8_wvformChoice == 2){
		if (gu8_selectedUserIndex == MY_ID) {
			duty_menu.b_disabled = FALSE;
			fcn_synth_setWaveForm(Square);
		}
		updateBoard(gu8_selectedUserIndex, Square, Wvform);
	}
	
}

void confirmFrequencyMenu(uint8_t u8_freqNumIndex){
	uint16_t temp = fcn_convertStringToInteger(freq_menu.aac_numeric_data[u8_freqNumIndex]);
	if (gu8_selectedUserIndex == MY_ID) {
		fcn_synth_setFreq(temp);
	}
	updateBoard(gu8_selectedUserIndex, temp, Freq);
}

void confirmAmplitudeMenu(uint8_t u8_ampChoice){
	uint8_t temp = convertDecimalStringTo3Point5Number(ampltd_menu.aac_numeric_data[0]);
	if (gu8_selectedUserIndex == MY_ID) {
		fcn_synth_setAmplitude(temp);
	}
	updateBoard(gu8_selectedUserIndex, temp, Amp);
}

void confirmDutyMenu(uint8_t u8_dutyVal){
	uint16_t temp = fcn_convertStringToInteger(duty_menu.aac_numeric_data[u8_dutyVal]);
	if (gu8_selectedUserIndex == MY_ID) {
		fcn_synth_setDutyCycle(temp);
	}
	updateBoard(gu8_selectedUserIndex, temp, DtyCyc);
}

void confirmLEDMenu(uint8_t u8_ledVal){
	if (gu8_selectedUserIndex == MY_ID) {
		fcn_setLEDValue(u8_ledVal);
	} 
	updateBoard(gu8_selectedUserIndex, u8_ledVal, LED);
}

void convertIntegerTo3Point5String(uint8_t u8_3Point5Val, char* psz_str) {
	uint8_t u8_whole = (u8_3Point5Val & 0b11100000) >> 5;

	char ac_tempString[8] = {0};
	fcn_convertIntegerToString(u8_whole, ac_tempString);
	psz_str[0] = ac_tempString[0];
	psz_str[1] = '.';

	// clear everything back to 0
	memset(ac_tempString, 0, 8);

	uint8_t u8_decimalPart = (u8_3Point5Val & 0b11111);
	uint32_t u32_decimalValueAsWholeNumber = 0;
	if (u8_decimalPart & 0b10000)
		u32_decimalValueAsWholeNumber += 100000l / 2;
	if (u8_decimalPart & 0b1000)
		u32_decimalValueAsWholeNumber += 100000l / 4;
	if (u8_decimalPart & 0b100)
		u32_decimalValueAsWholeNumber += 100000l / 8;
	if (u8_decimalPart & 0b10)
		u32_decimalValueAsWholeNumber += 100000l / 16;
	if (u8_decimalPart & 0b1)
		u32_decimalValueAsWholeNumber += 100000l / 32;

	fcn_convertIntegerToString(u32_decimalValueAsWholeNumber, ac_tempString);
	uint8_t u8_i = 0;
	uint8_t u8_lenOfNumber = strlen(ac_tempString);
	for(; u8_i < 5; ++u8_i) {
		if (u8_i < (5 - u8_lenOfNumber)) {
			psz_str[u8_i + 2] = '0';
		} else {
			psz_str[u8_i + 2] = ac_tempString[u8_i];
		}
	}
}

void goToSubMenu(uint8_t u8_choice){
	switch(u8_choice)
	{
		case WAVEFORM_MENU_INDEX - 1:
		{
			FncSynthMode e_tempMode = getWaveFormSettingForBoard(gu8_selectedUserIndex);
			if (e_tempMode == Triangle){
				wvform_menu.u8_choice = 0;
			} 
			else if(e_tempMode == Sine){
				wvform_menu.u8_choice = 1;
			}
			else if(e_tempMode == Square){
				wvform_menu.u8_choice = 2;	
			}
			break;
		}
		case FREQ_MENU_INDEX - 1:
		{
			uint16_t u16_freq = getFreqSettingForBoard(gu8_selectedUserIndex);
			fcn_convertIntegerToString(u16_freq, freq_menu.aac_numeric_data[0]);
			break;
		}
		case AMP_MENU_INDEX - 1:
		{
			uint8_t u8_amp = getAmpSettingForBoard(gu8_selectedUserIndex);
			printf("amp: %d\n", u8_amp);
			convertIntegerTo3Point5String(u8_amp, ampltd_menu.aac_numeric_data[0]);
			break;
		}
		case DUTY_MENU_INDEX - 1:
		{
			uint8_t u8_dutyCucle = getDutyCycleSettingForBoard(gu8_selectedUserIndex);
			fcn_convertIntegerToString(u8_dutyCucle, duty_menu.aac_numeric_data[0]);
			break;
		}
		case LM60_MENU_INDEX - 1:
		{
			updateMenu();
			gb_runLM60 = TRUE;
			startGettingTemp();
			break;
		}
		case SET_LED_MENU_INDEX - 1:
		{
			setLED_menu.u8_choice  = getLEDNumericSettingForBoard(gu8_selectedUserIndex);
			break;
		}
		
		default: break;
	}
}

void startLM60(uint8_t u8_choice) {
	gb_runLM60 = TRUE;
}

void stopLM60(uint8_t u8_choice){
	// ignore what's passed in since
	// it doesn't matter for this
	gb_runLM60 = FALSE;
	stopGettingTemp();
}

void setTeam(uint8_t u8_choice) {
	gu8_selectedUserIndex = au8_menuToUserArrayIndexMapping[u8_choice];

	// change all main menu options other than 
	// the first and last to display the chosen value
	uint8_t u8_i;
	for(u8_i = 1; u8_i < (NUM_MENUS - 2); ++u8_i) {
		main_menu_items[u8_i].aac_items[0][0] = u8_choice + '0';
	}
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
	uint8_t u8_amp3Point5 = convertDecimalStringTo3Point5Number(ampltd_menu.aac_numeric_data[0]);

	if (esos_uiF14_isRpgTurningCW()){
		if (u8_amp3Point5 < gu16_ampltd_max){
			u8_amp3Point5 += 1;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (u8_amp3Point5 > gu16_ampltd_min){
			u8_amp3Point5 -= 1;
		}
	}
	convertIntegerTo3Point5String(u8_amp3Point5, ampltd_menu.aac_numeric_data[0]);
}

void setAmplRPGturningFast(uint8_t index){
	uint8_t u8_amp3Point5 = convertDecimalStringTo3Point5Number(ampltd_menu.aac_numeric_data[0]);

	if (esos_uiF14_isRpgTurningCW()){
		if (u8_amp3Point5 < gu16_ampltd_max){
			if ((gu16_ampltd_max - u8_amp3Point5) < 10)
				u8_amp3Point5 = gu16_ampltd_max;
			else
				u8_amp3Point5 += 10;
		}
	}
	else if (esos_uiF14_isRpgTurningCCW()){
		if (u8_amp3Point5 > gu16_ampltd_min){
			if ((u8_amp3Point5 - gu16_ampltd_min) < 10)
				u8_amp3Point5 = gu16_ampltd_min;
			else
				u8_amp3Point5 -= 10;
		}
	}
	convertIntegerTo3Point5String(u8_amp3Point5, ampltd_menu.aac_numeric_data[0]);
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
		if ((aCANID_IDs[u8_index].u8_teamID == u8_teamId) && ((aCANID_IDs[u8_index].u8_memberID == u8_memberId))) {
			break;
		}
	}
	if (u8_index == NUM_OF_IDS) {
		return 0;
	} 
	uint8_t u8_indexToSearchFor = u8_index;
	for (u8_index = 0; u8_index < NUM_OF_IDS; ++u8_index) {
		if (au8_menuToUserArrayIndexMapping[u8_index] == u8_indexToSearchFor) {
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

static char apsz_netIds[NUM_OF_IDS][9];

//user_init()
void configCanMenu(void){
	uint8_t u8_idNum;
	uint8_t u8_index = 1;

	for(u8_idNum = 0; u8_idNum < NUM_OF_IDS; ++u8_idNum) {
		if (u8_idNum == MY_ID) {
			apsz_netIds[u8_idNum][0] = '0'; 
			strcpy(&(apsz_netIds[u8_idNum][1]), aCANID_IDs[u8_idNum].psz_netID);
			lm60_user_select_data[0].ac_data = apsz_netIds[u8_idNum];
			lm60_user_select_data[0].b_disabled = FALSE;
			au8_menuToUserArrayIndexMapping[0] = MY_ID;
		} else {
			apsz_netIds[u8_idNum][0] = u8_index + '0';
			strcpy(&(apsz_netIds[u8_idNum][1]), aCANID_IDs[u8_idNum].psz_netID);
			lm60_user_select_data[u8_index].ac_data = apsz_netIds[u8_idNum];
			lm60_user_select_data[u8_index].b_disabled = TRUE;
			au8_menuToUserArrayIndexMapping[u8_index] = u8_idNum;
			++u8_index;
		}
	}

	//Configure the hardware
	config_esos_uiF14();
	config_fcn_synth();
	esos_pic24_configI2C1(400);

	esos_lcd44780_configDisplay();
	esos_lcd44780_init();
	x[0] = &main_menu;
	x[SET_TEAM_MENU_INDEX] = &set_team_menu;
	x[WAVEFORM_MENU_INDEX] = &wvform_menu;
	x[FREQ_MENU_INDEX] = &freq_menu;
	x[AMP_MENU_INDEX] = &ampltd_menu;
	x[DUTY_MENU_INDEX] = &duty_menu;
	x[LM60_MENU_INDEX] = &LM60_menu;
	x[SET_LED_MENU_INDEX] = &setLED_menu;
	x[ABOUT_MENU_INDEX] = &about_menu;
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