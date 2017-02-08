#ifndef T4_SAMPLEAPP2_AUSTIN_H_
#define T4_SAMPLEAPP2_AUSTIN_H_

#include "esos.h"
#include "esos_pic24_sensor.h"
#include "revF16.h"
#include "esos_comm.h"
#include "esos_f14ui.h"
#include "all_generic.h"
#include "esos_comm_helpers.h"
#include "esos_pic24.h"

ESOS_USER_TASK(displayPot);
ESOS_USER_TASK(sampleSW2);
ESOS_USER_TASK(samplePotentiometer);
ESOS_USER_TASK(sw3MenuScreen);

#endif // T4_SAMPLEAPP2_AUSTIN_H_