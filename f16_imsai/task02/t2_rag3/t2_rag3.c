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
#define TURN_SIGNAL_TRIGGERED() (SW2_IS_DEPRESSED())

#define NUM_OPERATION_MODES (4)

/************************************************************************
 * User supplied enums and structs
 ************************************************************************
 */

typedef enum
{
    Normal,
    RushHour,
    NormalWithTurnLight,
    RushHourWithTurnLight
} SchedulingMode;

/************************************************************************
 * Globals
 ************************************************************************
 */
Light st_northSouthLightValue; /*The current color the North/South light is displaying*/
Light st_eastWestLightValue; /*The current color the East/West light is displaying*/

const uint8_t NUM_LIGHT_TRANSITIONS = 4; /*Number of transitions that a light operating in normal mode has*/
const uint8_t NUM_RUSH_HOUR_LIGHT_TRANSITIONS = 6; /*Number of transitions that a light operating in rush hour mode has*/
const uint8_t NUM__LIGHT_TRANSITIONS_WITH_TURN_LIGHT = 6; /*Number of transitions that a light operating in normal mode with a left turn signal has*/
const uint8_t NUM_RUSH_HOUR_LIGHT_TRANSITIONS_WITH_TURN_LIGHT = 8; /*Number of transitions that a light operating in rush hour mode with a left turn signal hashas*/

SchedulingMode st_lastOperationMode; /*the mode of operation the traffic light was in after the last transition*/

/*arrays that specify the color light to display after each transition for each operation mode*/
static const Light ast_northSouthLightValues[] = { Red, Red, Green, Amber };
static const Light ast_eastWestLightValues[] = { Green, Amber, Red, Red };
static const Light ast_eastWestRushHourLightValues[] = {Green, Amber, Red, Red, Red, Red};
static const Light ast_northSouthRushHourLightValues[] = {Red, Red, Red, Green, Amber, Red};
static const Light ast_northSouthLightValuesWithTurnSignal[] = { Red, Red, Red, Blink_Green, Green, Amber };
static const Light ast_eastWestLightValuesWithTurnSignal[] = { Blink_Green, Green, Amber, Red, Red, Red };
static const Light ast_northSouthRushHourLightValuesWithTurnSignal[] = { Red, Red, Red, Red, Blink_Green, Green, Amber, Red };
static const Light ast_eastWestRushHourLightValuesWithTurnSignal[] = { Blink_Green, Green, Amber, Red, Red, Red, Red, Red };

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

 /*
  * A function that determines the current operation mode based on whether
  * rush hour is enabled and turn signal is enabled.
  */
SchedulingMode getCurrentSchedulingMode()
{
 BOOL b_isInRushHourMode = IS_RUSH_HOUR();
 BOOL b_turnLightIsTriggered = TURN_SIGNAL_TRIGGERED();
 if (b_isInRushHourMode)
 {
   if (b_turnLightIsTriggered)
     return RushHourWithTurnLight;
   else
     return RushHour;
 }
 else
 {
   if (b_turnLightIsTriggered)
     return NormalWithTurnLight;
   else
     return Normal;
 }
}

/*
 * Determines the current light value for the North South traffic light based
 * on the current operating mode and how many transitions the light has went
 * through so far.
 */
Light getNorthSouthLightValueByOperationMode(SchedulingMode st_mode, uint8_t u8_lightIndex)
{
  switch(st_mode)
  {
    case Normal: return ast_northSouthLightValues[u8_lightIndex];
    case RushHour: return ast_northSouthRushHourLightValues[u8_lightIndex];
    case NormalWithTurnLight: return ast_northSouthLightValuesWithTurnSignal[u8_lightIndex];
    case RushHourWithTurnLight: return ast_northSouthRushHourLightValuesWithTurnSignal[u8_lightIndex];
    default: return Red; // This should really never happen
  }
}

/*
 * Determines the current light value for the East West traffic light based
 * on the current operating mode and how many transitions the light has went
 * through so far.
 */
Light getEastWestLightValueByOperationMode(SchedulingMode st_mode, uint8_t u8_lightIndex)
{
  switch(st_mode)
  {
    case Normal: return ast_eastWestLightValues[u8_lightIndex];
    case RushHour: return ast_eastWestRushHourLightValues[u8_lightIndex];
    case NormalWithTurnLight: return ast_eastWestLightValuesWithTurnSignal[u8_lightIndex];
    case RushHourWithTurnLight: return ast_eastWestRushHourLightValuesWithTurnSignal[u8_lightIndex];
    default: return Red; // This should really never happen
  }
}

/*
 * A function that determines how the index passed in should be adjusted based on
 * the previous operation mode and the current operation mode. Returns the
 * mapped index value.
 */
uint8_t getSchedulingIndexFromPreviousAndCurrentModes(SchedulingMode st_beforeWait, SchedulingMode st_afterWait, uint8_t u8_currentIndex)
{
 switch(st_beforeWait)
 {
   case Normal:
   {
     switch(st_afterWait)
     {
       case RushHour:
       {
         if(u8_currentIndex >= (NUM_LIGHT_TRANSITIONS/2))
   				return u8_currentIndex + 1;
         else
           return u8_currentIndex;
       }
       case NormalWithTurnLight:
       {
         if (u8_currentIndex < (NUM_LIGHT_TRANSITIONS / 2)) // below half way
           return u8_currentIndex + 1;
         else
           return u8_currentIndex + 2;
       }
       case RushHourWithTurnLight:
       {
         if (u8_currentIndex < ((NUM_RUSH_HOUR_LIGHT_TRANSITIONS_WITH_TURN_LIGHT / 2) - 1))
           return u8_currentIndex + 1;
         else
           return u8_currentIndex + 2;
       }
       default: return u8_currentIndex;
     }
   }
   case RushHour:
   {
     switch(st_afterWait)
     {
       case Normal: return (u8_currentIndex >= (NUM_LIGHT_TRANSITIONS / 2)) ? u8_currentIndex - 1 : u8_currentIndex;
       case NormalWithTurnLight:
       {
         if (u8_currentIndex == (NUM__LIGHT_TRANSITIONS_WITH_TURN_LIGHT / 2) || u8_currentIndex == 0)
           return u8_currentIndex + 1;
         else
           return u8_currentIndex;
       }
       case RushHourWithTurnLight: return (u8_currentIndex >= (NUM_RUSH_HOUR_LIGHT_TRANSITIONS / 2)) ? u8_currentIndex + 1 : u8_currentIndex;
       default: return u8_currentIndex;
     }
   }
   case NormalWithTurnLight:
   {
     switch(st_afterWait)
     {
       case Normal:
       {
         if (u8_currentIndex < (NUM__LIGHT_TRANSITIONS_WITH_TURN_LIGHT / 2))
           return u8_currentIndex - 1;
         else
           return u8_currentIndex - 2;
       }
       case RushHour:
       {
         if (u8_currentIndex == (NUM__LIGHT_TRANSITIONS_WITH_TURN_LIGHT / 2) || u8_currentIndex == 0)
           return u8_currentIndex - 1;
         else
           return u8_currentIndex;
       }
       case RushHourWithTurnLight:
       {
         if (u8_currentIndex >= (NUM__LIGHT_TRANSITIONS_WITH_TURN_LIGHT/2))
           return u8_currentIndex + 1;
         else
           return u8_currentIndex;
       }
       default: return u8_currentIndex;
     }
   }
   case RushHourWithTurnLight:
   {
     switch(st_afterWait)
     {
       case Normal:
       {
         if (u8_currentIndex < ((NUM_RUSH_HOUR_LIGHT_TRANSITIONS_WITH_TURN_LIGHT / 2) - 1))
           return u8_currentIndex - 1;
         else
           return u8_currentIndex - 2;
       }
       case RushHour:
       {
         if (u8_currentIndex < (NUM_RUSH_HOUR_LIGHT_TRANSITIONS_WITH_TURN_LIGHT / 2))
           return u8_currentIndex - 1;
         else
           return u8_currentIndex - 2;
       }
       case NormalWithTurnLight:
       {
         if (u8_currentIndex >= (NUM__LIGHT_TRANSITIONS_WITH_TURN_LIGHT/2))
           return u8_currentIndex - 1;
         else
           return u8_currentIndex;
       }
       default: return u8_currentIndex;
     }
   }
   default: return u8_currentIndex;
 }
}

/*
 * An ESOS task that is used to determine the light values for the trafficLight
 * after each transition.
 */
ESOS_USER_TASK(scheduleLight) {
  /*arrays that specify the amount of time between each transition for each operation mode*/
  static const uint8_t au8_trafficLightTimingSchedule[] = { 10, 3, 10, 3 };
  static const uint8_t au8_trafficLightRushHourTimingSchedule[] = {30, 3, 1, 30, 3, 1};
  static const uint8_t au8_trafficLightTimingScheduleWithTurnSignal[] = { 10, 10, 3, 10, 10, 3 };
  static const uint8_t au8_trafficLightRushHourTimingScheduleWithTurnSignal[] = { 10, 30, 3, 1, 10, 30, 3, 1 };

  static uint8_t u8_scheduleIndex = 0; /*the index to all of the various arrays this task uses.*/

  ESOS_TASK_BEGIN();

  while (TRUE) {

  // We need to determine how long to yield for based on the operation mode
  uint8_t u8_cycleTime;
  if (st_lastOperationMode == Normal)
    u8_cycleTime = au8_trafficLightTimingSchedule[u8_scheduleIndex];
  else if (st_lastOperationMode == RushHour)
    u8_cycleTime = au8_trafficLightRushHourTimingSchedule[u8_scheduleIndex];
  else if (st_lastOperationMode == NormalWithTurnLight)
    u8_cycleTime = au8_trafficLightTimingScheduleWithTurnSignal[u8_scheduleIndex];
  else
    u8_cycleTime = au8_trafficLightRushHourTimingScheduleWithTurnSignal[u8_scheduleIndex];

  ESOS_TASK_WAIT_TICKS(SECONDS_TO_TICKS(u8_cycleTime));

  // Get the current mode the traffic light is in
  SchedulingMode st_operationMode = getCurrentSchedulingMode();

  // Update the index based on the previous and current mode.
  u8_scheduleIndex = getSchedulingIndexFromPreviousAndCurrentModes(st_lastOperationMode, st_operationMode, u8_scheduleIndex);
  ++u8_scheduleIndex;

  // Determine what color the light should be based on the operation mode
  // and adjust the index back to zero if it has been incremented past the end of the array
  st_northSouthLightValue = getNorthSouthLightValueByOperationMode(st_operationMode, u8_scheduleIndex);
  st_eastWestLightValue = getEastWestLightValueByOperationMode(st_operationMode, u8_scheduleIndex);

  // update the index if it has went outside the bounds of the array it is using
  if (st_operationMode == Normal && u8_scheduleIndex >= NUM_LIGHT_TRANSITIONS)
      u8_scheduleIndex = 0;
  else if (st_operationMode == RushHour && u8_scheduleIndex >= NUM_RUSH_HOUR_LIGHT_TRANSITIONS)
      u8_scheduleIndex = 0;
  else if (st_operationMode == NormalWithTurnLight && u8_scheduleIndex >= NUM_RUSH_HOUR_LIGHT_TRANSITIONS)
      u8_scheduleIndex = 0;
  else if (u8_scheduleIndex >= NUM_RUSH_HOUR_LIGHT_TRANSITIONS_WITH_TURN_LIGHT)
      u8_scheduleIndex = 0;

  // Save off the last mode it operated under
  st_lastOperationMode = st_operationMode;
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
    // Yield so that the other task can run
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
  configSW2();
  configSW1();

  // We need to set this in user init so that the scheduling task
  // will have it before it begins
  st_lastOperationMode = getCurrentSchedulingMode();

  // Now we set the initial light values based off of the mode we begin in
  st_northSouthLightValue = getNorthSouthLightValueByOperationMode(st_lastOperationMode, 0);
  st_eastWestLightValue = getEastWestLightValueByOperationMode(st_lastOperationMode, 0);

  // user_init() should register at least one user task
  esos_RegisterTask(scheduleLight);
  esos_RegisterTask(trafficLight);
} // end user_init()
