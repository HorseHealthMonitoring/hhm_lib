//#includes go here. (First include the main esos.h file).
//All other includes are specific to this project.
#include "esos.h"
#include "esos_pic24.h"
#include "esos_pic24_sensor.h"
#include "all_generic.h"
#include "esos_comm.h"
#include "esos_comm_helpers.h"
#include "esos_f14ui.h"
#include "revF16.h"
#include <stdio.h>


/************************************************************************
* User supplied functions
************************************************************************
*/

const uint16_t heartbeatTicks = 250;

/************************************************************************
* Globals
************************************************************************
*/

char psz_sw1Option[] = "Press SW1: Display POT value once";
char psz_sw2Option[] = "Press SW2: Display POT value every second until you press SW2 again or SW1";
char psz_prompt[] = "Press either SW1 or SW2 to begin: ";
char psz_newLine[3] = {0x0D, 0x0A, 0};

BOOL b_flagToBeginTask = FALSE;

/************************************************************************
* User supplied defines
************************************************************************
*/

#define ONE_SEC (1000)

/************************************************************************
* ESOS_USER_TASK
************************************************************************
*/

ESOS_USER_TASK(t4_sensor1)
{
	static uint16_t u16_dataFromPot;
	static BOOL b_sw2PressedOnce = FALSE;
	ESOS_TASK_BEGIN();
	
	//Prints the prompt to the user
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw1Option);
	ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);   
	ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw2Option); 
	ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);   
	ESOS_TASK_WAIT_ON_SEND_STRING(psz_prompt);    
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	
	while (TRUE){
		
		//checks to see if sw1 is pressed then prints the POT value in 16bit hex once to the screen if it is true
		if (esos_uiF14_getSW1Pressed()){
			b_flagToBeginTask = FALSE;
			b_sw2PressedOnce = FALSE;
			ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(POT_CHANNEL, VREF);
			ESOS_TASK_WAIT_SENSOR_QUICK_READ(u16_dataFromPot);
			ESOS_SENSOR_CLOSE();
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_WAIT_ON_SEND_STRING("POT value: ");
			ESOS_TASK_WAIT_ON_SEND_UINT16_AS_HEX_STRING(u16_dataFromPot);
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM(); 
			
			ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();
			
			//Prints the prompt to the user
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw1Option);
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);   
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw2Option); 
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);   
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_prompt);  
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
		//checks to see if sw2 is pressed and if it has not been pressed before that point if so then it sets the flags to signal t4_sw2Task to begin
		else if (esos_uiF14_getSW2Pressed() && !b_sw2PressedOnce){
			b_flagToBeginTask = TRUE;
			b_sw2PressedOnce = TRUE;
			ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
		}
		//checks to see if sw2 is pressed and if it has been pressed before that point then clears the flags for t4_sw2Task to wait 
		else if (esos_uiF14_getSW2Pressed() && b_sw2PressedOnce){
			b_flagToBeginTask = FALSE;
			b_sw2PressedOnce = FALSE;
			ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
			
			//Prints the prompt to the user
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw1Option);
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);   
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw2Option); 
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);   
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_prompt);  
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
		ESOS_TASK_YIELD();
	}

	ESOS_TASK_END();
}

//This task waits for a flag to be set then samples the POT ADC and prints the value to the screen aproximately every second
ESOS_USER_TASK(t4_sw2Task){
	static uint16_t u16_dataFromPot;
	ESOS_TASK_BEGIN();
	
	while (TRUE){
		ESOS_TASK_WAIT_UNTIL(b_flagToBeginTask);
		ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(POT_CHANNEL, VREF);
		ESOS_TASK_WAIT_SENSOR_QUICK_READ(u16_dataFromPot);
		ESOS_SENSOR_CLOSE();
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("POT value: ");
		ESOS_TASK_WAIT_ON_SEND_UINT16_AS_HEX_STRING(u16_dataFromPot);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_TICKS(ONE_SEC);
	}
	
	ESOS_TASK_END();
}

/****************************************************
*  user_init()
****************************************************
*/

void user_init(void) {
  config_esos_uiF14();
  esos_RegisterTask(t4_sensor1);
  esos_RegisterTask(t4_sw2Task);
  esos_uiF14_flashLED3 (heartbeatTicks);
}//end of user_init



