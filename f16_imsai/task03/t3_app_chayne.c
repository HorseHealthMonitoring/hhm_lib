#include "esos.h"
#include "esos_pic24.h"
#include "t3_app_chayne.h"
#include "all_generic.h"
#include "esos_comm.h"
#include "esos_f14ui.h"

#define RPG_MEDIUM_TURNING_LED2_BLINK_PERIOD_MS (500)
#define RPG_FAST_TURNING_LED2_BLINK_PERIOD_MS (100)
#define RPG_MEDIUM_TURNING_LED2_BLINK_PERIOD_TICKS (__ESOS_MS_TO_TICKS(RPG_MEDIUM_TURNING_LED2_BLINK_PERIOD_MS))
#define RPG_FAST_TURNING_LED2_BLINK_PERIOD_TICKS (__ESOS_MS_TO_TICKS(RPG_FAST_TURNING_LED2_BLINK_PERIOD_MS))

#define BLINK_QUICKLY_PERIOD_MS (100)
#define BLINK_QUICKLY_PERIOD_TICKS (__ESOS_MS_TO_TICKS(BLINK_QUICKLY_PERIOD_MS))

char psz_CRNL[3] = {0x0D, 0x0A, 0};
/*
 * Task that listens to the RPG and updates led2Controller
 * accordingly.
 */
ESOS_USER_TASK(led2Controller)
{
  ESOS_TASK_BEGIN();
  while (TRUE)
  {
    if (esos_uiF14_isRpgTurningSlow())
    {
      ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      if (esos_uiF14_isRpgTurningCW())
      {
        ESOS_TASK_WAIT_ON_SEND_STRING( "SLOW >> CLOCKWISE" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
      }
      else
      {
        ESOS_TASK_WAIT_ON_SEND_STRING( "SLOW >> COUNTER CLOCKWISE" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
      }
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

      esos_uiF14_turnLED2On();
      ESOS_TASK_WAIT_UNTIL( !esos_uiF14_isRpgTurningSlow() );
    }
    else if (esos_uiF14_isRpgTurningMedium())
    {
      ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      if (esos_uiF14_isRpgTurningCW())
      {
        ESOS_TASK_WAIT_ON_SEND_STRING( "MEDIUM >> CLOCKWISE" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
      }
      else
      {
        ESOS_TASK_WAIT_ON_SEND_STRING( "MEDIUM >> COUNTER CLOCKWISE" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
      }
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

      esos_uiF14_flashLED2(RPG_MEDIUM_TURNING_LED2_BLINK_PERIOD_TICKS);
      ESOS_TASK_WAIT_UNTIL( !esos_uiF14_isRpgTurningMedium() );
    }
    else if (esos_uiF14_isRpgTurningFast())
    {
      ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      if (esos_uiF14_isRpgTurningCW())
      {
        ESOS_TASK_WAIT_ON_SEND_STRING( "FAST >> CLOCKWISE" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
      }
      else
      {
        ESOS_TASK_WAIT_ON_SEND_STRING( "FAST >> COUNTER CLOCKWISE" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
      }
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

      esos_uiF14_flashLED2(RPG_FAST_TURNING_LED2_BLINK_PERIOD_TICKS);
      ESOS_TASK_WAIT_UNTIL( !esos_uiF14_isRpgTurningFast() );
    }
    esos_uiF14_turnLED2Off();
    ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS();
  }
  ESOS_TASK_END();
}

ESOS_CHILD_TASK(blinkLed1Rapidly, uint8_t);

/*
 * Task to listen to the switches and update
 * LED1 accordingly.
 */
ESOS_USER_TASK(led1Controller)
{
  static ESOS_TASK_HANDLE th_blinkRapidlyChild;
  static BOOL b_sw1DoublePressed = FALSE;
  static BOOL b_sw2DoublePressed = FALSE;

  static const uint8_t u8_ledNumTimesToBlink = 3;

  ESOS_TASK_BEGIN();
  while (TRUE)
  {
    if (esos_uiF14_getSW3Pressed())
    {
      if (esos_uiF14_getSW1Pressed())
      {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING( "SW1 Pressed" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        esos_uiF14_turnLED1On();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();
        esos_uiF14_turnLED1Off();
      }
      else if (b_sw1DoublePressed)
      {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING( "SW1 Double Pressed" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        ESOS_ALLOCATE_CHILD_TASK(th_blinkRapidlyChild);
        ESOS_TASK_SPAWN_AND_WAIT(th_blinkRapidlyChild, blinkLed1Rapidly, u8_ledNumTimesToBlink);
      }
    }
    else
    {

      if (esos_uiF14_getSW2Pressed())
      {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING( "SW2 Pressed" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        esos_uiF14_turnLED1On();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
        esos_uiF14_turnLED1Off();
      }
      else if (b_sw2DoublePressed)
      {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING( "SW2 Double Pressed" );
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        ESOS_ALLOCATE_CHILD_TASK(th_blinkRapidlyChild);
        ESOS_TASK_SPAWN_AND_WAIT(th_blinkRapidlyChild, blinkLed1Rapidly, u8_ledNumTimesToBlink);
      }
    }

    ESOS_TASK_WAIT_UNTIL (
      (esos_uiF14_getSW3Pressed() && ( esos_uiF14_getSW1Pressed() || (b_sw1DoublePressed = esos_uiF14_getSW1DoublePressed()) )) ||
      (!esos_uiF14_getSW3Pressed() && ( esos_uiF14_getSW2Pressed() || (b_sw2DoublePressed = esos_uiF14_getSW2DoublePressed()) ))
    );
  }
  ESOS_TASK_END();
}

/*
 * Task to blink an LED the number of times passed in.
 * Led choice value must be from 1 - 3. This number corresponds to
 * LEDs 1, 2, and 3.
 */
ESOS_CHILD_TASK(blinkLed1Rapidly, uint8_t u8_timesToBlink) {
  static uint8_t u8_blinkCount;

  ESOS_TASK_BEGIN();
  for (u8_blinkCount = 0; u8_blinkCount < u8_timesToBlink; ++u8_blinkCount)
  {
      esos_uiF14_turnLED1On();
      ESOS_TASK_WAIT_TICKS(BLINK_QUICKLY_PERIOD_TICKS);
      esos_uiF14_turnLED1Off();
      ESOS_TASK_WAIT_TICKS(BLINK_QUICKLY_PERIOD_TICKS);
  }

  ESOS_TASK_END();
}
