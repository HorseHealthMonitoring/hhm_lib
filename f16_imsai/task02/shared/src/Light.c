#include "esos.h"
#include "esos_pic24.h"
#include "revF16.h"
#include "Light.h"

#define TURN_SIGNAL_DELAY (250)

// Timer handle and function called when
// the turn signal is displayed.
ESOS_TMR_HANDLE turnSignalTimerHandle;

ESOS_USER_TIMER(turnSignalTimer)
{
  TOGGLE_LED3();
}

// Turns off all lights other than Red
// and turns it on
inline void turnOnRedLight()
{
  if (esos_GetTimerHandle(turnSignalTimer) != ESOS_TMR_FAILURE)
  {
	   esos_UnregisterTimer(turnSignalTimerHandle);
  }
  TURN_OFF_LED3();
  TURN_OFF_LED2();
  TURN_ON_LED1();
}

// Turns off all lights other than Amber
// and turns it on
inline void turnOnAmberLight()
{
  if (esos_GetTimerHandle(turnSignalTimer) != ESOS_TMR_FAILURE)
  {
	esos_UnregisterTimer(turnSignalTimerHandle);
  }
  TURN_OFF_LED3();
  TURN_ON_LED2();
  TURN_OFF_LED1();
}

// Turns off all lights other than Green
// and turns it on
inline void turnOnGreenLight()
{
  if (esos_GetTimerHandle(turnSignalTimer) != ESOS_TMR_FAILURE)
  {
	esos_UnregisterTimer(turnSignalTimerHandle);
  }
  TURN_ON_LED3();
  TURN_OFF_LED2();
  TURN_OFF_LED1();
}

// Turns off all lights and begins blinking the
// green light
inline void turnOnBlinkGreen()
{
  TURN_OFF_LED2();
  TURN_OFF_LED1();
  if (esos_GetTimerHandle(turnSignalTimer) == ESOS_TMR_FAILURE)
  {
	   turnSignalTimerHandle = esos_RegisterTimer(turnSignalTimer, TURN_SIGNAL_DELAY);
     TOGGLE_LED3();
  }
}

// updates the LEDs on the board
// to the traffic light value passed in.
inline void updateLight(const Light light)
{
  if (light == Red)
    turnOnRedLight();
  else if (light == Amber)
    turnOnAmberLight();
  else if (light == Blink_Green)
	   turnOnBlinkGreen();
  else
    turnOnGreenLight();
}
