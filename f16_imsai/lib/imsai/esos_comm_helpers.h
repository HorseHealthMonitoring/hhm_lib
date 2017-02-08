#ifndef ESOS_COMM_HELPERS_H_
#define ESOS_COMM_HELPERS_H_
#include "esos_comm.h"
#include "esos_pic24_sensor.h"

ESOS_CHILD_TASK( __esos_OutUint16AsHexString, uint16_t);
ESOS_CHILD_TASK( __esos_OutPointOneMvAsMv, uint16_t);
ESOS_CHILD_TASK( __esos_OutSensorProcessType, esos_sensor_process_t);

#define   ESOS_TASK_WAIT_ON_SEND_UINT16_AS_HEX_STRING( u16_out)           \
            ESOS_TASK_SPAWN_AND_WAIT( (ESOS_TASK_HANDLE)&__stChildTaskTx, __esos_OutUint16AsHexString,(u16_out))

#define   ESOS_TASK_WAIT_ON_SEND_POINT_ONE_MV_AS_MV(u16_pointOneMv)           \
            ESOS_TASK_SPAWN_AND_WAIT( (ESOS_TASK_HANDLE)&__stChildTaskTx, __esos_OutPointOneMvAsMv,(u16_pointOneMv))

#define   ESOS_TASK_WAIT_ON_SEND_SENSOR_PROCESS_TYPE(e_sensorProcesstype)		\
            ESOS_TASK_SPAWN_AND_WAIT( (ESOS_TASK_HANDLE)&__stChildTaskTx, __esos_OutSensorProcessType,(e_sensorProcesstype))


#endif //ESOS_COMM_HELPERS_H_
