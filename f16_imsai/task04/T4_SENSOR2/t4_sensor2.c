//#includes go here. (First include the main esos.h file).
//All other includes are specific to this project.
#include "t4_sampleapp2_austin.h"


/************************************************************************
* User supplied defines
************************************************************************
*/

/************************************************************************
* User supplied functions
************************************************************************
*/

const uint16_t heartbeatTicks = 250;

/************************************************************************
* Globals
************************************************************************
*/

/****************************************************
*  user_init()
****************************************************
*/

void user_init(void) {
	config_esos_uiF14();
	esos_uiF14_flashLED3(heartbeatTicks);
	esos_RegisterTask(displayPot);
	esos_RegisterTask(sampleSW2);
	esos_RegisterTask(samplePotentiometer);
	esos_RegisterTask(sw3MenuScreen);
}//end of user_init
