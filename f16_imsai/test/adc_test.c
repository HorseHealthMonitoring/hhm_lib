#include "esos.h"
#include "esos_pic24_sensor.h"
#include "revF16.h"
#include "esos_comm_helpers.h"
#include "esos_comm.h"

char psz_newLine[3] = {0x0D, 0x0A, 0};

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
        ESOS_TASK_SPAWN_AND_WAIT(th_child, _WAIT_ON_AVAILABLE_SENSOR, ESOS_SENSOR_CH02, VREF);

		    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_STRING("AFTER");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        ESOS_ALLOCATE_CHILD_TASK(th_child2);
        ESOS_TASK_SPAWN_AND_WAIT(th_child2, _WAIT_SENSOR_READ, &u16_data, ESOS_SENSOR_ONE_SHOT, ESOS_SENSOR_FORMAT_VOLTAGE);

		    ESOS_SENSOR_CLOSE();

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_STRING("AFTER ALL");

        ESOS_TASK_WAIT_ON_SEND_POINT_ONE_MV_AS_MV(u16_data);
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
