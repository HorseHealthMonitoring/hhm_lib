// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
#include "esos.h"
#include "esos_pic24.h"
#include "revF16.h"
#include "Light.h"
#include "rushHour.h"

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
const uint8_t u8_numRushHourLightTransitions = 6;

BOOL b_lastYieldWasRushHour;

/*
 * An ESOS task that is used to determine the light values for the trafficLight
 * after each transition.
 */
ESOS_USER_TASK(scheduleLight) {
  /*arrays that specify the amount of time between each transition for each operation mode*/
  static const uint8_t au8_trafficLightTimingSchedule[] = { 10, 3, 10, 3 };
  static const uint8_t au8_trafficLightRushHourTimingSchedule[] = {30, 3, 1, 30, 3, 1};

  static const Light ast_northSouthLightValues[] = { Red, Red, Green, Amber };
  static const Light ast_eastWestLightValues[] = { Green, Amber, Red, Red };

  static const Light ast_eastWestRushHourLightValues[] = {Green, Amber, Red, Red, Red, Red};
  static const Light ast_northSouthRushHourLightValues[] = {Red, Red, Red, Green, Amber, Red};

  static uint8_t u8_scheduleIndex = 0;

  ESOS_TASK_BEGIN();
  while (TRUE) {
	uint8_t u8_cycleTime = b_lastYieldWasRushHour ? au8_trafficLightRushHourTimingSchedule[u8_scheduleIndex] : au8_trafficLightTimingSchedule[u8_scheduleIndex];
    ESOS_TASK_WAIT_TICKS(SECONDS_TO_TICKS(u8_cycleTime));
	BOOL b_isRushHour = IS_RUSH_HOUR();
  // adjust based on the current mode and previous mode
	if (b_isRushHour != b_lastYieldWasRushHour){ // if it changed after the last transition
		if(!b_lastYieldWasRushHour){
			if(u8_scheduleIndex >= (u8_numLightTransitions/2)){
				u8_scheduleIndex += (u8_numRushHourLightTransitions - u8_numLightTransitions) /2;
			}
		} else{
			if(u8_scheduleIndex >= (u8_numLightTransitions/2)){
				u8_scheduleIndex += (u8_numLightTransitions - u8_numRushHourLightTransitions) / 2;
			}
		}
	}
	++u8_scheduleIndex;
  // now we check if the index went outside of bounds of the array
  // and update the light value for the N/S and E/W
	if (b_isRushHour){
		if (u8_scheduleIndex >= u8_numRushHourLightTransitions){
			u8_scheduleIndex = 0;
		}
		st_northSouthLightValue = ast_northSouthRushHourLightValues[u8_scheduleIndex];
		st_eastWestLightValue = ast_eastWestRushHourLightValues[u8_scheduleIndex];
	}else{
		if (u8_scheduleIndex >= u8_numLightTransitions){
			u8_scheduleIndex = 0 ;
		}
		st_northSouthLightValue = ast_northSouthLightValues[u8_scheduleIndex];
		st_eastWestLightValue = ast_eastWestLightValues[u8_scheduleIndex];
	}
	b_lastYieldWasRushHour = b_isRushHour;
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
  configSW1();

  // We need to set this in user init so that the scheduling task
  // will have it before it begins
  b_lastYieldWasRushHour = IS_RUSH_HOUR();

  // Now we set the initial light values based off of the mode we begin in
  if (IS_RUSH_HOUR()) {
    st_northSouthLightValue = Red;
    st_eastWestLightValue = Green;
  } else {
    st_northSouthLightValue = Red;
    st_eastWestLightValue = Green;
  }

  // user_init() should register at least one user task
  esos_RegisterTask(scheduleLight);
  esos_RegisterTask(trafficLight);
} // end user_init()
