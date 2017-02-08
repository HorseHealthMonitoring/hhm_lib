//#includes go here. (First include the main esos.h file).
//All other includes are specific to this project.
#include "esos.h"
#include "esos_pic24.h"
#include "t3_app_austin.h"
#include "esos_f14ui.h"
#include "t3_app_chayne.h"
#include "esos_pic24_rs232.h"


/************************************************************************
* User supplied defines
************************************************************************
*/

/************************************************************************
* User supplied functions
************************************************************************
*/

const uint16_t heartbeatTicks = 500;

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
  esos_RegisterTask(led2Controller);
  esos_RegisterTask(led1Controller);
  esos_RegisterTask(t3_appUI);
  esos_uiF14_flashLED3 (heartbeatTicks);
}//end of user_init
