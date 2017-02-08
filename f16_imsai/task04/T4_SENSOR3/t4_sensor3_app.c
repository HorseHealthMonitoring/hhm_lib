//#includes go here. (First include the main esos.h file).
//All other includes are specific to this project.
#include "t4_sensor3_app.h"

/************************************************************************
* User supplied defines
************************************************************************
*/

#define SW3_MENU (ESOS_USER_FLAG_E)
#define PRINT_TEMP (ESOS_USER_FLAG_F)
#define SW3_MENU_ON (esos_IsUserFlagSet(SW3_MENU))
#define PRINT_TEMP_ON (esos_IsUserFlagSet(PRINT_TEMP))

/************************************************************************
* User supplied functions
************************************************************************
*/

int8_t convertMvToDegreesC(uint16_t u16_pointOneMvReading)
{
    int16_t i16_adjustedPointOneMvReading = u16_pointOneMvReading - TEMP_SENSOR_TENTH_MV_OFFSET;
    int8_t i8_degreesCelsius = (int8_t)(i16_adjustedPointOneMvReading / TEMP_SENSOR_TENTH_MV_PER_DEGREES_C);
    return i8_degreesCelsius;
}

/************************************************************************
* Globals
************************************************************************
*/
uint8_t u8_processType = ESOS_SENSOR_ONE_SHOT; /*Type of processing to do on ADC. Defaults to just one sample.*/
char psz_done[9] = { ' ', 'D', 'o', 'n', 'e', '.', 0x0D, 0x0A, 0 };
char psz_newLine[3] = { 0x0D, 0x0A, 0 };

/************************************************************************
* ESOS tasks
************************************************************************
*/

/*
 * An ESOS task that displays the adc reading in hex for the temperature sensor.
 * Only displays when switch 1 is pressed.
 */
ESOS_USER_TASK(displayTemp)
{
	static uint16_t u16_data;
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Pressed() && !SW3_MENU_ON);

		ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(TEMP_SENSOR_CHANNEL, VREF);
		ESOS_TASK_WAIT_SENSOR_READ(u16_data, u8_processType, ESOS_SENSOR_FORMAT_BITS);
  	ESOS_SENSOR_CLOSE();

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_SENSOR_PROCESS_TYPE(u8_processType);
		ESOS_TASK_WAIT_ON_SEND_UINT16_AS_HEX_STRING(u16_data);
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();
	}// end of while
	ESOS_TASK_END();
} // end displayPot task

ESOS_USER_TASK(sampleSW2)
{
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		ESOS_TASK_WAIT_UNTIL(!SW3_MENU_ON);
		ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED();
		esos_SetUserFlag(PRINT_TEMP);
		ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
		ESOS_TASK_WAIT_UNTIL(esos_uiF14_getSW1Pressed() || esos_uiF14_getSW2Pressed());
		esos_ClearUserFlag(PRINT_TEMP);
		ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
	}// end of while
	ESOS_TASK_END();
}// end sampleSW2 task

/*
 * An ESOS task that sample the tempurature sensor and displays the tempurature
 * every second. Begins when switch 2 is pressed.
 */
ESOS_USER_TASK(sampleTemp)
{
	static uint16_t u16_data;
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(TEMP_SENSOR_CHANNEL, VREF);
		ESOS_TASK_WAIT_SENSOR_READ(u16_data, u8_processType, ESOS_SENSOR_FORMAT_VOLTAGE);
        ESOS_SENSOR_CLOSE();

		ESOS_TASK_WAIT_UNTIL(PRINT_TEMP_ON && !SW3_MENU_ON);
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_SENSOR_PROCESS_TYPE(u8_processType);
		int8_t i8_celsius = convertMvToDegreesC(u16_data);
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING((uint8_t)i8_celsius);
		ESOS_TASK_WAIT_ON_SEND_STRING(" degrees celsius");
		ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_TICKS(1000);
	}// end of while
	ESOS_TASK_END();
}// end samplePotentiometer task

/*
 * An ESOS task that displays a prompt and allows the user to change the way the application
 * is processing the ADC readings. This blocks all other tasks.
 */
ESOS_USER_TASK(sw3MenuScreen)
{
	static uint8_t u8_firstChoice;
	static uint8_t u8_secondChoice;
	ESOS_TASK_BEGIN();
	while (TRUE)
	{
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();
		esos_SetUserFlag(SW3_MENU);

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
		ESOS_TASK_WAIT_ON_SEND_STRING("Choose an option (1-5) to change processing modes.");
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		do
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    		ESOS_TASK_WAIT_ON_GET_UINT8(u8_firstChoice);
    		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		} while ((u8_firstChoice < '1') || (u8_firstChoice > '5'));

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    	ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
    	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    	if (u8_firstChoice == '1')
    	{
    		u8_processType = ESOS_SENSOR_ONE_SHOT;
    	}
    	else
    	{
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
			ESOS_TASK_WAIT_ON_SEND_STRING("Choose an option (1-6) to change processing modes.");
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			do
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
	    		ESOS_TASK_WAIT_ON_GET_UINT8(u8_secondChoice);
	    		ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			} while ((u8_secondChoice < '1') || (u8_secondChoice > '6'));

			u8_secondChoice = (uint8_t)(u8_secondChoice - '0');

			if (u8_firstChoice == '2')
			{
				u8_processType = (0x00) + u8_secondChoice;
			}
			else if (u8_firstChoice == '3')
			{
				u8_processType = (0x20) + u8_secondChoice;
			}
			else if (u8_firstChoice == '4')
			{
				u8_processType = (0x40) + u8_secondChoice;
			}
			else
			{
				u8_processType = (0x80) + u8_secondChoice;
			}

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	    	ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
	    	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	    }

		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	    ESOS_TASK_WAIT_ON_SEND_STRING(psz_done);
	    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

		esos_ClearUserFlag(SW3_MENU);
		ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();

	}// end of while
	ESOS_TASK_END();
}// end of sw3MenuScreen
