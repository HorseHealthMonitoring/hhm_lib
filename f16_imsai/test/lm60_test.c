#include "esos.h"
#include "esos_pic24_sensor.h"
#include "esos_comm_helpers.h"
#include "revF16.h"
#include "esos_comm.h"

char psz_newLine[3] = {0x0D, 0x0A, 0};

int8_t convertMvToDegreesC(uint16_t pointOneMvReading)
{
    int16_t adjustedPointOneMvReading = pointOneMvReading - TEMP_SENSOR_TENTH_MV_OFFSET;
    int8_t degreesCelsius = (int8_t)(adjustedPointOneMvReading / TEMP_SENSOR_TENTH_MV_PER_DEGREES_C);
    return degreesCelsius;
}

ESOS_USER_TASK(testAdc)
{
    static ESOS_TASK_HANDLE th_child;
	  static ESOS_TASK_HANDLE th_child2;
    static uint16_t u16_data;
    ESOS_TASK_BEGIN();
    while(TRUE)
    {
		    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("BEFORE");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


        ESOS_ALLOCATE_CHILD_TASK(th_child);
        ESOS_TASK_SPAWN_AND_WAIT(th_child, _WAIT_ON_AVAILABLE_SENSOR, TEMP_SENSOR_CHANNEL, VREF);

		    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_STRING("AFTER");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        ESOS_ALLOCATE_CHILD_TASK(th_child2);
        ESOS_TASK_SPAWN_AND_WAIT(th_child2, _WAIT_SENSOR_READ, &u16_data, ESOS_SENSOR_ONE_SHOT, ESOS_SENSOR_FORMAT_VOLTAGE);

		    ESOS_SENSOR_CLOSE();

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_UINT16_AS_HEX_STRING(u16_data);
        //int8_t cel = convertMvToDegreesC(u16_data);
        //ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(cel); // should be safe since I doubt we'll go outside of +-127
        //ESOS_TASK_WAIT_ON_SEND_STRING("degrees celsius");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_TICKS(1000);
    }
    ESOS_TASK_END();
}

void user_init (void)
{
    esos_RegisterTask(testAdc);
}
