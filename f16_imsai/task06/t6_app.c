#include "t6_app_austin.h"
#include "esos.h"
#include "esos_f14ui.h"

void user_init(void){
	//Configure Hardware
	config_esos_uiF14();
	esos_uiF14_flashLED3(500);
	esos_pic24_configI2C1(400);
	
	//register user tasks
	esos_RegisterTask(i2c_service);
}//end user_init