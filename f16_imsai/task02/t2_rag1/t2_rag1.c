// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
#include "esos.h"
#include "esos_pic24.h"
#include "revF16.h"
#include "Light.h"

/************************************************************************
 * User supplied defines
 ************************************************************************
 */
#define CONFIG_RED_LIGHT() CONFIG_LED1()
#define CONFIG_AMBER_LIGHT() CONFIG_LED2()
#define CONFIG_GREEN_LIGHT() CONFIG_LED3()

#define NORTH_SOUTH_ACTIVE() (SW3_IS_NOT_DEPRESSED())

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

/************************************************************************
 * Globals
 ************************************************************************
 */
Light st_northSouthLightValue = Red;
Light st_eastWestLightValue = Green;
const uint8_t u8_numLightTransitions = 4;



/*
 * An ESOS task that is used to determine the light values for the trafficLight
 * after each transition.
 */
ESOS_USER_TASK(scheduleLight) {
  static const uint8_t au8_trafficLightTimingSchedule[] = { 10, 3, 10, 3 };
  static uint8_t u8_index = 0;
  static const Light ast_northSouthLightValues[] = { Red, Red, Green, Amber };
  static const Light ast_eastWestLightValues[] = { Green, Amber, Red, Red };
  ESOS_TASK_BEGIN();
  while (TRUE) {
    ESOS_TASK_WAIT_TICKS(SECONDS_TO_TICKS(au8_trafficLightTimingSchedule[u8_index]));
    ++u8_index;
    if (u8_index >= u8_numLightTransitions)
      u8_index = 0;
    st_northSouthLightValue = ast_northSouthLightValues[u8_index];
    st_eastWestLightValue = ast_eastWestLightValues[u8_index];
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end light scheduling task

/*
 * An ESOS task that is used to change the light displayed
 */
ESOS_USER_TASK(trafficLight) {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    if (NORTH_SOUTH_ACTIVE())
      updateLight(st_northSouthLightValue);
    else
      updateLight(st_eastWestLightValue);

    ESOS_TASK_YIELD();
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end light scheduling task

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init(void) {
  // Set up hardware
  CONFIG_LED1();
  CONFIG_LED2();
  CONFIG_LED3();

  configSW3();

  // user_init() should register at least one user task
  esos_RegisterTask(scheduleLight);
  esos_RegisterTask(trafficLight);
} // end user_init()
