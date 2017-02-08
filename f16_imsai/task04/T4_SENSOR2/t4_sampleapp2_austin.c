//#includes go here. (First include the main esos.h file).
//All other includes are specific to this project.
#include "t4_sampleapp2_austin.h"

/************************************************************************
* User supplied defines
************************************************************************
*/

#define SW3_MENU (ESOS_USER_FLAG_E) //This flag is set when the menu screen is upon on the user's console.
#define PRINT_POT_VOLTAGE (ESOS_USER_FLAG_F) //This flag is set when the pot is required to print its voltage every second.
#define SW3_MENU_ON (esos_IsUserFlagSet(SW3_MENU)) //This #define notes when the menu is on.
#define PRINT_POT_VOLTAGE_ON (esos_IsUserFlagSet(PRINT_POT_VOLTAGE)) //This flag denotes when the pot is printing every second.

/************************************************************************
* User supplied functions
************************************************************************
*/

/************************************************************************
* Globals
************************************************************************
*/

uint8_t processType = ESOS_SENSOR_ONE_SHOT;
char psz_done[9] = { ' ', 'D', 'o', 'n', 'e', '.', 0x0D, 0x0A, 0 };
char psz_newLine[3] = { 0x0D, 0x0A, 0 };

/************************************************************************
* ESOS tasks
************************************************************************
*/

ESOS_USER_TASK(displayPot)
/*
Task Name: displayPot
Purpose: This task displays the potentiometer reading in 16 bit hex.
Intendend operation: Upon the press of SW1, this task will display the potentiometer reading in 16 bit hex.
					 The reading will be based upon the processing type the user has chosen.
*/
{
	static uint16_t u16_data;
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		//Wait until SW1 is pressed and the user menu is not on.
		ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Pressed() && !SW3_MENU_ON);

		//Configure the potentiometer and read from the potentiometer based upon the process type.
		ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(POT_CHANNEL, VREF);
		ESOS_TASK_WAIT_SENSOR_READ(u16_data, processType, ESOS_SENSOR_FORMAT_BITS);
  		ESOS_SENSOR_CLOSE();

  		//Print the required data.
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_SENSOR_PROCESS_TYPE(processType);
		ESOS_TASK_WAIT_ON_SEND_UINT16_AS_HEX_STRING(u16_data);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		//Wait until the user releases SW1 before doing anything else.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();
	}// end of while
	ESOS_TASK_END();
} // end displayPot task

ESOS_USER_TASK(sampleSW2)
/*
Task Name: sampleSW2
Purpose: This task samples input from SW2 and raises a flag to allow the pot to begin printing every 1 second.
Intendend operation: Upon the press of SW2, this task will raise a user flag that allows the application to print the potentiometer voltage every 1 second.
					 The flag will be lowered when SW1 or SW2 is pressed, and will not be raised again until SW2 is pressed again.
*/
{
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		//Wait until the user menu screen is off.
		ESOS_TASK_WAIT_UNTIL(!SW3_MENU_ON);
		//Wait until SW2 is pressed.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED();
		//Set the flag for the app to begin printing the potentiometer every 1 second.
		esos_SetUserFlag(PRINT_POT_VOLTAGE);
		//Wait until SW2 is released.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
		//Wait until SW1 or SW2 is pressed.
		ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Pressed() || esos_uiF14_getSW2Pressed());
		//Clear the flag, stopping the application from printing every 1 second.
		esos_ClearUserFlag(PRINT_POT_VOLTAGE);
		//Wait until SW2 is pressed again.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
	}// end of while
	ESOS_TASK_END();
}// end sampleSW2 task

ESOS_USER_TASK(samplePotentiometer)
/*
Task Name: samplePotentiometer
Purpose: This task samples input from the potentiometer and prints its voltage in mV every 1 second.
Intendend operation: Upon the flag PRINT_POT_VOLTAGE_ON being raised and the user menu being off, 
					 this task will begin printing the potentiometer voltage in mV every 1 second.
*/
{
	static uint16_t u16_data;
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		//Configure the potentiometer according to the user's preferred processing mode and sample.
		ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(POT_CHANNEL, VREF);
		ESOS_TASK_WAIT_SENSOR_READ(u16_data, processType, ESOS_SENSOR_FORMAT_VOLTAGE);
        ESOS_SENSOR_CLOSE();

        //Wait until PRINT_POT_VOLTAGE_ON flag is up and the user menu is off.
		ESOS_TASK_WAIT_UNTIL(PRINT_POT_VOLTAGE_ON && !SW3_MENU_ON);
		//Print required data.
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_SENSOR_PROCESS_TYPE(processType);
		ESOS_TASK_WAIT_ON_SEND_POINT_ONE_MV_AS_MV(u16_data);
		ESOS_TASK_WAIT_ON_SEND_STRING(" mv");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		//Wait 1 second (1000 ms).
		ESOS_TASK_WAIT_TICKS(1000);
	}// end of while
	ESOS_TASK_END();
}// end samplePotentiometer task

ESOS_USER_TASK(sw3MenuScreen)
/*
Task Name: sw3MenuScreen
Purpose: This task prints a menu screen for the user to read, takes input from the user, and alters the processing mode of the hardware based
		 upon the user's input.
Intendend operation: Upon press of the SW3, the task will raise the menu screen flag, preventing all other tasks from printing to the screen.
					 The user will input the options they desire and then the task will update the processing mode. After this,
					 the task will lower the menu screen flag and all other tasks can resume printing to the screen.
*/
{
	static uint8_t u8_firstChoice;
	static uint8_t u8_secondChoice;
	//static unsigned 8bit integer variables to hold the user choices.
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		//Wait until SW3 is pressed then raise the flag.
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();
		esos_SetUserFlag(SW3_MENU);

		//Print the screen.
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1) ONE SHOT");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2) AVERAGE");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3) MINIMUM");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4) MAXIMUM");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5) MEDIAN");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("Choose an option (1-5) to change processing modes: ");
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		//While the choice is not between 1-5, keep asking for the choice.
		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    		ESOS_TASK_WAIT_ON_GET_UINT8(u8_firstChoice);
    		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_firstChoice < '1') || (u8_firstChoice > '5'));

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    	ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    	//Evaluate the choices before moving on to the next menu screen.
    	if (u8_firstChoice == '1')
    	{
    		processType = ESOS_SENSOR_ONE_SHOT;
    	}
    	else
    	{
    		//Print the screen.
	    	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_STRING("(1) TWO");
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_WAIT_ON_SEND_STRING("(2) FOUR");
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_WAIT_ON_SEND_STRING("(3) EIGHT");
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_WAIT_ON_SEND_STRING("(4) SIXTEEN");
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_WAIT_ON_SEND_STRING("(5) THIRTY-TWO");
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_WAIT_ON_SEND_STRING("(6) SIXTY-FOUR");
			ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
			ESOS_TASK_WAIT_ON_SEND_STRING("Choose an option (1-6) to change processing modes: ");
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			//While the second choice is not between 1-6, keep asking.
			do
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
	    		ESOS_TASK_WAIT_ON_GET_UINT8(u8_secondChoice);
	    		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			} while ((u8_secondChoice < '1') || (u8_secondChoice > '6'));

			//Convert the second choice to an actual int instead of a char.
			u8_secondChoice = (uint8_t)(u8_secondChoice - '0');

			//Based upon the user's choices, select the correct processing type.
			if (u8_firstChoice == '2')
			{
				processType = (0x00) + u8_secondChoice;
			}
			else if (u8_firstChoice == '3')
			{
				processType = (0x20) + u8_secondChoice;
			}
			else if (u8_firstChoice == '4')
			{
				processType = (0x40) + u8_secondChoice;
			}
			else
			{
				processType = (0x80) + u8_secondChoice;
			}

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	    	ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
	    	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	    }

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	    ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
	    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	    //Clear the menu screen flag to allow everyone to print again and wait until the user releases SW3.
		esos_ClearUserFlag(SW3_MENU);
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();

	}// end of while
	ESOS_TASK_END();
}// end of sw3MenuScreen
