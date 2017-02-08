#ifndef T4_SENSOR3_APP_H_
#define T4_SENSOR3_APP_H_

#include "esos.h"
#include "esos_pic24_sensor.h"
#include "revF16.h"
#include "esos_comm.h"
#include "esos_f14ui.h"
#include "all_generic.h"
#include "esos_comm_helpers.h"

ESOS_USER_TASK(displayTemp);
ESOS_USER_TASK(sampleSW2);
ESOS_USER_TASK(sampleTemp);
ESOS_USER_TASK(sw3MenuScreen);

#endif // T4_SENSOR3_APP_H_
