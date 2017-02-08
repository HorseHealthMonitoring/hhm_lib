//Include files
#include "lcd_app_lm60m.h"
#include "lcd_app_pot.h"

/************************************************************************
* User supplied defines
************************************************************************
*/

/************************************************************************
* User supplied functions
************************************************************************
*/

/************************************************************************
* Globals
************************************************************************
*/
const uint16_t heartbeatTicks = 500;//50% duty cycle, period of 1 second

/****************************************************
*  user_init()
****************************************************
*/
void user_init(void){
	//Configure Hardware
	config_esos_uiF14();
	esos_lcd44780_configDisplay();
	esos_lcd44780_init();
	esos_uiF14_flashLED3(heartbeatTicks);
	
	//register user tasks
	esos_RegisterTask(t5_lcdPot);
	esos_RegisterTask(t5_lcdLM60M);
	esos_RegisterTask(sampleSW3);
}//end user_init
