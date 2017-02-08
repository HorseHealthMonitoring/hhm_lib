/*******************************************************************
 *
 * C header file for ESOS user-interface (UI) service
 *
 *    requires the EMBEDDED SYSTEMS target rev. F14
 *
 * ****************************************************************/

#ifndef   ESOS_UIF14_H
#define ESOS_UIF14_H

#include "esos.h"

// STRUCTURES
typedef enum
{
  SwitchReleased,
  SwitchPressedOnce,
  SwitchReleasedAfterPress,
  SwitchPressedTwice,
  SwitchDoublePressed
} SwitchState;

typedef struct {
    BOOL b_SW1Pressed;
    BOOL b_SW1DoublePressed;
    BOOL b_SW2Pressed;
    BOOL b_SW2DoublePressed;
    BOOL b_SW3Pressed;
    BOOL b_SW3DoublePressed;

    uint16_t u16_SW1DoublePressPeriod;
    uint16_t u16_SW2DoublePressPeriod;
    uint16_t u16_SW3DoublePressPeriod;

    SwitchState st_SW1State;
    SwitchState st_SW2State;
    SwitchState st_SW3State;

    BOOL b_RPGAHigh;
    BOOL b_RPGBHigh;

    BOOL b_LED1On;
    uint16_t u16_LED1FlashPeriod;
    BOOL b_LED2On;
    uint16_t u16_LED2FlashPeriod;
    BOOL b_LED3On;
    uint16_t u16_LED3FlashPeriod;

    int16_t i16_RPGCounter;
    int16_t i16_lastRPGCounter;

	  uint16_t u16_rpgSlowVelocityLimit;
	  uint16_t u16_rpgFastVelocityLimit;
} _st_esos_uiF14Data_t;

// DEFINEs and CONSTANTs
#define __ESOS_TICKS_TO_MS(x)           (x/1)
#define __ESOS_MS_TO_TICKS(x)           (x*1)
#define __ESOS_UIF14_UI_PERIOD_MS       10
#define NUM_CLICKS_PER_REV (12)

// PRIVATE FUNCTION PROTOTYPES

// int16_t _esos_uiF14_getRpgCounter (void);
// inline void _esos_uiF14_setRpgCounter (int16_t);
//
// int16_t _esos_uiF14_getLastRpgCounter (void);
// inline void _esos_uiF14_setLastRpgCounter (int16_t);
//
// BOOL _esos_uiF14_getRPGAHigh (void);
// void _esos_uiF14_setRPGAHigh (BOOL);
//
// BOOL _esos_uiF14_getRPGBHigh (void);
// void _esos_uiF14_setRPGBHigh (BOOL);

ESOS_USER_TASK (__esos_uiF14_task);

// PUBLIC API FUNCTION PROTOTYPES
//SW1
inline BOOL esos_uiF14_getSW1Pressed (void);
inline BOOL esos_uiF14_getSW1Released (void);
inline BOOL esos_uiF14_getSW1DoublePressed (void);
inline void esos_uiF14_setSW1DoublePressPeriod(uint16_t);

//SW2
inline BOOL esos_uiF14_getSW2Pressed (void);
inline BOOL esos_uiF14_getSW2Released (void);
inline BOOL esos_uiF14_getSW2DoublePressed (void);
inline void esos_uiF14_setSW2DoublePressPeriod(uint16_t);

//SW3
inline BOOL esos_uiF14_getSW3Pressed (void);
inline BOOL esos_uiF14_getSW3Released (void);
inline BOOL esos_uiF14_getSW3DoublePressed (void);
inline void esos_uiF14_setSW3DoublePressPeriod(uint16_t);

//LED1
inline BOOL esos_uiF14_isLED1On (void);
inline BOOL esis_uiF14_isLED1Off (void);
inline void esos_uiF14_turnLED1On (void);
inline void esos_uiF14_turnLED1Off (void);
inline void esos_uiF14_toggleLED1 (void);
inline void esos_uiF14_flashLED1 (uint16_t);

//LED2
inline BOOL esos_uiF14_isLED2On (void);
inline BOOL esis_uiF14_isLED2Off (void);
inline void esos_uiF14_turnLED2On (void);
inline void esos_uiF14_turnLED2Off (void);
inline void esos_uiF14_toggleLED2 (void);
inline void esos_uiF14_flashLED2 (uint16_t);

//LED3
inline BOOL esos_uiF14_isLED3On (void);
inline BOOL esis_uiF14_isLED3Off (void);
inline void esos_uiF14_turnLED3On (void);
inline void esos_uiF14_turnLED3Off (void);
inline void esos_uiF14_toggleLED3 (void);
inline void esos_uiF14_flashLED3 (uint16_t);

//COLOR LEDs
inline void esos_uiF14_turnRedLEDOn (void);
inline void esos_uiF14_turnRedLEDOff (void);
inline void esos_uiF14_turnGreenLEDOn (void);
inline void esos_uiF14_turnGreenLEDOff (void);
inline void esos_uiF14_turnYellowLEDOn (void);
inline void esos_uiF14_turnYellowLEDOff (void);

//RPG
inline int16_t esos_uiF14_getRpgValue_i16 (void);
inline BOOL esos_uiF14_isRpgTurning (void);
inline BOOL esos_uiF14_isRpgTurningSlow (void);
inline BOOL esos_uiF14_isRpgTurningMedium (void);
inline BOOL esos_uiF14_isRpgTurningFast (void);
inline BOOL esos_uiF14_isRpgTurningCW (void);
inline BOOL esos_uiF14_isRpgTurningCCW (void);

void config_esos_uiF14();	//config hardware and service software structures for use

int16_t esos_uiF14_getRpgVelocity_i16 (void);

//VELOCITY FUNCTION PROTOTYPES
//Medium velocity is set between the limits of fast and slow velocity
uint16_t esos_uiF14_getSlowVelocityLimit (void);
uint16_t esos_uiF14_getFastVelocityLimit (void);

void esos_uiF14_setRPGSlowVelocityLimit( uint16_t u16_limit );
void esos_uiF14_setRPGFastVelocityLimit( uint16_t u16_limit );

// PUBLIC API ESOS TASK MACROS

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED()              ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW1Pressed() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED()             ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW1Released() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED_AND_RELEASED() do {            \
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED();           \
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();          \
                          } while (0)
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_DOUBLE_PRESSED()       ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW1DoublePressed() )

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED()              ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW2Pressed() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED()             ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW2Released() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED_AND_RELEASED() do {            \
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED();           \
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();          \
                          } while (0)
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_DOUBLE_PRESSED()       ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW2DoublePressed() )

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED()              ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW3Pressed() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED()             ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW3Released() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED_AND_RELEASED() do {            \
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();           \
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();          \
                          } while (0)
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_DOUBLE_PRESSED()       ESOS_TASK_WAIT_UNTIL( esos_uiF14_getSW3DoublePressed() )

#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS()          	  ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurning() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_CW()             ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCW() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_CCW()            ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCCW() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_SLOW()			  ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningSlow() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_SLOW_CW()        ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCW() && esos_uiF14_isRpgTurningSlow() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_SLOW_CCW()       ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCCW() && esos_uiF14_isRpgTurningSlow() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM()         ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningMedium() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CW()      ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCW() && esos_uiF14_isRpgTurningMedium() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CCW()     ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCCW() && esos_uiF14_isRpgTurningMedium() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST()           ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningFast() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CW()        ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCW() && esos_uiF14_isRpgTurningFast() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CCW()       ESOS_TASK_WAIT_UNTIL( esos_uiF14_isRpgTurningCCW() && esos_uiF14_isRpgTurningFast() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_REV(NumberOfRevolutions)           do { \
                            int16_t i16_ticks = _esos_uiF14_getRPGCounter(); \
                            ESOS_TASK_WAIT_UNTIL((_esos_uiF14_getRPGCounter() >= (i16_ticks + (NUM_CLICKS_PER_REV * NumberOfRevolutions)) || (_esos_uiF14_getRPGCounter() <= (i16_ticks - (NUM_CLICKS_PER_REV * NumberOfRevolutions)) ); \
                          } while(0)
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_CW_REV(NumberOfRevolutions)        do { \
                            int16_t i16_ticks = _esos_uiF14_getRPGCounter(); \
                            ESOS_TASK_WAIT_UNTIL((_esos_uiF14_getRPGCounter() >= (i16_ticks + (NUM_CLICKS_PER_REV * NumberOfRevolutions))); \
                          } while(0)
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_CCW_REV(NumberOfRevolutions)       do { \
                            int16_t i16_ticks = _esos_uiF14_getRPGCounter(); \
                            ESOS_TASK_WAIT_UNTIL((_esos_uiF14_getRPGCounter() <= (i16_ticks - (NUM_CLICKS_PER_REV * NumberOfRevolutions))); \
                          } while(0)


#endif    // ESOS_UIF14_H
