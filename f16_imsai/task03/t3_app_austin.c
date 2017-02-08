//#includes go here. (First include the main esos.h file).
//All other includes are specific to this project.
#include "esos.h"
#include "esos_pic24.h"
#include "t3_app_austin.h"
#include "all_generic.h"
#include "esos_comm.h"
#include "esos_f14ui.h"
#include <stdio.h>

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
//Different strings to send out to user.

//Main screen
char psz_doublePress[] = "(1) Double Press Periods";
char psz_rpgThresholds[] = "(2) RPG Thresholds";
char psz_prompt[] = "Enter a number (1 or 2) to change hardware periods or thresholds: ";

//Button Double Press Periods Screen Main
char psz_sw1[] = "(1) SW1";
char psz_sw2[] = "(2) SW2";
char psz_sw3[] = "(3) SW3";
char psz_promptDoublePress[] = "Enter a number (1 - 3) to change the double press period: ";

//Button Double Press Periods Screen 2
char psz_promptDoublePress1[] = "Enter a number (0 - 9) to change the double press period of SW1: ";
char psz_promptDoublePress2[] = "Enter a number (0 - 9) to change the double press period of SW2: ";
char psz_promptDoublePress3[] = "Enter a number (0 - 9) to change the double press period of SW3: ";

//RPG Thresholds Screen Main
char psz_slow[] = "(1) Slow";
char psz_medium[] = "(2) Medium";
char psz_fast[] = "(3) Fast";
char psz_promptRpgThresholds[] = "Enter a number (1 - 3) to change the different threshold speeds: ";

//RPG Thresholds Screen 2
char psz_promptRpgThresholds1[] = "Enter a number (0 - 9) to change the threshold of Slow: ";
char psz_promptRpgThresholds2Minimum[] = "Enter a number (0 - 9) to change the minimum threshold of Medium: ";
char psz_promptRpgThresholds2Maximum[] = "Enter a number (0 - 9) to change the maximum threshold of Medium: ";
char psz_promptRpgThresholds3[] = "Enter a number (0 - 9) to change the threshold of Fast: ";

char psz_done[9] = {' ', 'D', 'o', 'n', 'e', '.', 0x0D, 0x0A, 0};
char psz_newLine[3] = {0x0D, 0x0A, 0};

ESOS_CHILD_TASK(doublePress_child);
ESOS_CHILD_TASK(doublePressTwo_child, uint8_t);
ESOS_CHILD_TASK(rpgThresholds_child);
ESOS_CHILD_TASK(rpgThresholdsTwo_child, uint8_t);

/************************************************************************
* ESOS tasks
************************************************************************
*/

ESOS_USER_TASK(t3_appUI)
/*
An ESOS user task to host the user interface application.
Intended operation: prints options to the user's screen, receives input from the
					user that selects a certain option (either to update the swtiches
					or the RPG thresholds) and then creates the corresponding child task.
There are two child tasks total.
*/
{
  static uint8_t u8_mainScreenChoice;
  static ESOS_TASK_HANDLE th_child;

	ESOS_TASK_BEGIN();
	while (TRUE)
	{

      	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      	ESOS_TASK_WAIT_ON_SEND_STRING(psz_doublePress);
      	ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
      	ESOS_TASK_WAIT_ON_SEND_STRING(psz_rpgThresholds);
	    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
	    ESOS_TASK_WAIT_ON_SEND_STRING(psz_prompt);
	    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	    do {
	        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
          	ESOS_TASK_WAIT_ON_GET_UINT8(u8_mainScreenChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
        } while((u8_mainScreenChoice < '1') || (u8_mainScreenChoice > '2'));

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
	    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	    if (u8_mainScreenChoice == '1')
	    {
	    	ESOS_ALLOCATE_CHILD_TASK(th_child);
        	ESOS_TASK_SPAWN_AND_WAIT(th_child, doublePress_child);
	    }

	    else
	    {
	    	ESOS_ALLOCATE_CHILD_TASK(th_child);
	    	ESOS_TASK_SPAWN_AND_WAIT(th_child, rpgThresholds_child);
	    }

	}// end of while
   	ESOS_TASK_END();
}// end of t3_app_ui task

ESOS_CHILD_TASK(doublePress_child)
/*
An ESOS child task to host the button press options for the user interface application.
Intended operation: prints button press options to the user's screen, receives input from the
					user that selects a certain option (either to update switch 1, 2 or 3)
					and then creates the corresponding child task.
There is 1 child task total.
*/
{
	static uint8_t u8_doublePressChoice;
	static ESOS_TASK_HANDLE th_child;

    ESOS_TASK_BEGIN();

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw1);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw2);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_sw3);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptDoublePress);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    do {
    	ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    	ESOS_TASK_WAIT_ON_GET_UINT8(u8_doublePressChoice);
    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    } while ((u8_doublePressChoice < '1') || (u8_doublePressChoice > '3'));

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    ESOS_ALLOCATE_CHILD_TASK(th_child);
    ESOS_TASK_SPAWN_AND_WAIT(th_child, doublePressTwo_child, u8_doublePressChoice);

    ESOS_TASK_END();
} //end of doublePress_child

ESOS_CHILD_TASK(doublePressTwo_child, uint8_t u8_doublePressChoice)
/*
An ESOS child task to host the final choices for button press times for the user interface application.
Intended operation: prints options to the user's screen, receives input from the
					user that selects a certain option (1 - 9 for different thresholds) 
					and then creates the corresponding child task.
*/
{
	static uint8_t u8_doublePressTwoChoice;
	static uint8_t u8_periodIndex = 0;
	static const uint16_t au16_doublePressPeriodChoices[] = {100, 200, 300, 400, 500, 600, 700, 800, 900};

	ESOS_TASK_BEGIN();

	if (u8_doublePressChoice == '1')
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1): 100 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2): 200 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3): 300 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4): 400 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5): 500 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(6): 600 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(7): 700 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(8): 800 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(9): 900 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptDoublePress1);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(u8_doublePressTwoChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_doublePressTwoChoice < '1') || (u8_doublePressTwoChoice > '9'));

		u8_periodIndex = (uint8_t)(u8_doublePressTwoChoice - '0');

    	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    	ESOS_TASK_WAIT_ON_SEND_UINT8(u8_periodIndex); //debug code;
    	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		esos_uiF14_setSW1DoublePressPeriod(au16_doublePressPeriodChoices[u8_periodIndex - 1]);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	}

	else if (u8_doublePressChoice == '2')
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1): 100 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2): 200 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3): 300 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4): 400 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5): 500 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(6): 600 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(7): 700 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(8): 800 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(9): 900 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptDoublePress2);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(u8_doublePressTwoChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_doublePressTwoChoice < '1') || (u8_doublePressTwoChoice > '9'));

		u8_periodIndex = (uint8_t)(u8_doublePressTwoChoice - '0');
		esos_uiF14_setSW2DoublePressPeriod(au16_doublePressPeriodChoices[u8_periodIndex - 1]);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	}

	else
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1): 100 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2): 200 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3): 300 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4): 400 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5): 500 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(6): 600 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(7): 700 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(8): 800 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(9): 900 ms");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptDoublePress3);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(u8_doublePressTwoChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_doublePressTwoChoice < '1') || (u8_doublePressTwoChoice > '9'));

		u8_periodIndex = (uint8_t)(u8_doublePressTwoChoice - '0');
		esos_uiF14_setSW3DoublePressPeriod(au16_doublePressPeriodChoices[u8_periodIndex - 1]);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	}

	ESOS_TASK_END();

}//end of doublePressTwo_child

ESOS_CHILD_TASK(rpgThresholds_child)
/*
An ESOS child task to host the RPG threshold choices for the user interface application.
Intended operation: prints options to the user's screen, receives input from the
					user that selects a certain option (either to update the slow, medium and fast)
					and then creates the corresponding child task.
There is 1 child tasks total.
*/
{
	static ESOS_TASK_HANDLE th_child;
	static uint8_t u8_rpgThresholdsChoice;

	ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_slow);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_medium);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_fast);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptRpgThresholds);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    do {
    	ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    	ESOS_TASK_WAIT_ON_GET_UINT8(u8_rpgThresholdsChoice);
    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    } while ((u8_rpgThresholdsChoice < '1') || (u8_rpgThresholdsChoice > '3'));

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    ESOS_ALLOCATE_CHILD_TASK(th_child);
    ESOS_TASK_SPAWN_AND_WAIT(th_child, rpgThresholdsTwo_child, u8_rpgThresholdsChoice);

	ESOS_TASK_END();
}// end of rpgThresholds_child

ESOS_CHILD_TASK(rpgThresholdsTwo_child, uint8_t u8_rpgThresholdsChoice)
/*
An ESOS child task to host the final choices for RPG thresholds for the user interface application.
Intended operation: prints options to the user's screen, receives input from the
					user that selects a certain option (1 - 9 for different thresholds) 
					and then creates the corresponding child task.
*/
{
	static uint8_t u8_rpgThresholdsTwoChoice;
	static uint8_t u8_thresholdIndex = 0;
	static const uint16_t au16_rpgThresholdChoices[] = {2, 3, 4, 5, 6, 7, 8, 9, 10};

	ESOS_TASK_BEGIN();

		if (u8_rpgThresholdsChoice == '1')
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1): 2 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2): 3 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3): 4 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4): 5 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5): 6 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(6): 7 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(7): 8 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(8): 9 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(9): 10 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptRpgThresholds1);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(u8_rpgThresholdsTwoChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_rpgThresholdsTwoChoice < '1') || (u8_rpgThresholdsTwoChoice > '9'));

		u8_thresholdIndex = (uint8_t)(u8_rpgThresholdsTwoChoice - '0');
		esos_uiF14_setRPGSlowVelocityLimit(au16_rpgThresholdChoices[u8_thresholdIndex - 1]);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	}

	else if (u8_rpgThresholdsChoice == '2')
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1): 2 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2): 3 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3): 4 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4): 5 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5): 6 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(6): 7 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(7): 8 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(8): 9 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(9): 10 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptRpgThresholds2Minimum);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(u8_rpgThresholdsTwoChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_rpgThresholdsTwoChoice < '1') || (u8_rpgThresholdsTwoChoice > '9'));

		u8_thresholdIndex = (uint8_t)(u8_rpgThresholdsTwoChoice - '0');
		esos_uiF14_setRPGSlowVelocityLimit(au16_rpgThresholdChoices[u8_thresholdIndex - 1]);

    	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1): 2 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2): 3 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3): 4 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4): 5 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5): 6 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(6): 7 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(7): 8 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(8): 9 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(9): 10 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptRpgThresholds2Maximum);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(u8_rpgThresholdsTwoChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_rpgThresholdsTwoChoice < '1') || (u8_rpgThresholdsTwoChoice > '9'));

		u8_thresholdIndex = (uint8_t)(u8_rpgThresholdsTwoChoice - '0');
		esos_uiF14_setRPGFastVelocityLimit(au16_rpgThresholdChoices[u8_thresholdIndex - 1]);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	}

	else
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING("(1): 2 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(2): 3 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(3): 4 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(4): 5 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(5): 6 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(6): 7 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(7): 8 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(8): 9 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING("(9): 10 clicks/sec");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_promptRpgThresholds3);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			ESOS_TASK_WAIT_ON_GET_UINT8(u8_rpgThresholdsTwoChoice);
			ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_rpgThresholdsTwoChoice < '1') || (u8_rpgThresholdsTwoChoice > '9'));

		u8_thresholdIndex = (uint8_t)(u8_rpgThresholdsTwoChoice - '0');
		esos_uiF14_setRPGFastVelocityLimit(au16_rpgThresholdChoices[u8_thresholdIndex - 1]);

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	}

	ESOS_TASK_END();
}//end rpgThresholdsTwo_child
