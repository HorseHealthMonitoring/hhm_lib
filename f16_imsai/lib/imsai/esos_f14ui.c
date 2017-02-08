/***********************************s********************************
 *
 * C code framework for ESOS user-interface (UI) service
 *
 *    requires the EMBEDDED SYSTEMS target rev. F14
 *
 * ****************************************************************/
#include "esos_f14ui.h"
#include "revF16.h"


#define MS_IN_A_SEC (1000)
#define THRESHOLD_RPG_IDLE_TIME (1000) //represents one second
#define DEFAULT_DOUBLE_PRESS_PERIOD_MS (2000)
#define RPG_TIMER_SAMPLE_RATE (3)
#define NUM_THAT_MAKES_COUNTER_EXCEED_ONE_SEC (334) //This is 334 because the timer occurs every 3MS meaning 3 * 334 = 1002 Therefore more than one second

#define DEFAULT_SLOW_RPG_THRESHOLD_TICKS_PER_SEC (2)
#define DEFAULT_FAST_RPG_THRESHOLD_TICKS_PER_SEC (8)

//Global Variables
int16_t i16_globalVelocity;

//Timer
ESOS_TMR_HANDLE RPGTimerHandle;
ESOS_TMR_HANDLE led1TimerHandle;
ESOS_TMR_HANDLE led2TimerHandle;
ESOS_TMR_HANDLE led3TimerHandle;

ESOS_USER_TIMER (led1Timer) {
	TOGGLE_LED1();
}

ESOS_USER_TIMER (led2Timer) {
	TOGGLE_LED2();
}

ESOS_USER_TIMER (led3Timer) {
	TOGGLE_LED3();
}

// PRIVATE DATA
_st_esos_uiF14Data_t _st_esos_uiF14Data;

// PRIVATE FUNCTIONS FOR RPG
inline void _esos_uiF14_setRPGCounter (int16_t i16_newValue) {
    _st_esos_uiF14Data.i16_RPGCounter = i16_newValue;
    return;
}

inline int16_t _esos_uiF14_getRPGCounter (void){
	return (_st_esos_uiF14Data.i16_RPGCounter);
}

inline void _esos_uiF14_setLastRPGCounter (int16_t i16_newValue) {
    _st_esos_uiF14Data.i16_lastRPGCounter = i16_newValue;
    return;
}

inline int16_t _esos_uiF14_getLastRPGCounter (void){
	return (_st_esos_uiF14Data.i16_lastRPGCounter);
}

inline BOOL _esos_uiF14_getRPGAHigh (void){
	return (_st_esos_uiF14Data.b_RPGAHigh);
}

inline void _esos_uiF14_setRPGAHigh (BOOL b_isHigh){
	_st_esos_uiF14Data.b_RPGAHigh = b_isHigh;
	return;
}

inline BOOL _esos_uiF14_getRPGBHigh (void){
	return (_st_esos_uiF14Data.b_RPGBHigh);
}

inline void _esos_uiF14_setRPGBHigh (BOOL b_isHigh){
	_st_esos_uiF14Data.b_RPGBHigh = b_isHigh;
	return;
}

//Velocity Getter
inline int16_t esos_uiF14_getRpgVelocity_i16(void) {
	return i16_globalVelocity;
}

//SW1 Setters
inline void esos_uiF14_setSW1Pressed(BOOL b_swVal) {
    _st_esos_uiF14Data.b_SW1Pressed = b_swVal;
    return;
}

inline void esos_uiF14_setSW1DoublePressed(BOOL b_swDoublePressed) {
    _st_esos_uiF14Data.b_SW1DoublePressed = b_swDoublePressed;
    return;
}

inline void esos_uiF14_setSW1State(SwitchState st_swState) {
	_st_esos_uiF14Data.st_SW1State = st_swState;
	return;
}

inline void esos_uiF14_setSW1DoublePressPeriod(uint16_t u16_period){
	_st_esos_uiF14Data.u16_SW1DoublePressPeriod = u16_period;
	return;
}

//SW2 Setters
inline void esos_uiF14_setSW2Pressed(BOOL b_swVal) {
    _st_esos_uiF14Data.b_SW2Pressed = b_swVal;
    return;
}

inline void esos_uiF14_setSW2DoublePressed(BOOL b_swDoublePressed) {
    _st_esos_uiF14Data.b_SW2DoublePressed = b_swDoublePressed;
    return;
}

inline void esos_uiF14_setSW2State(SwitchState st_swState) {
	_st_esos_uiF14Data.st_SW2State = st_swState;
	return;
}

inline void esos_uiF14_setSW2DoublePressPeriod(uint16_t u16_period){
	_st_esos_uiF14Data.u16_SW2DoublePressPeriod = u16_period;
	return;
}

//SW3 Setters
inline void esos_uiF14_setSW3Pressed(BOOL b_swVal) {
    _st_esos_uiF14Data.b_SW3Pressed = b_swVal;
    return;
}

inline void esos_uiF14_setSW3DoublePressed(BOOL b_swDoublePressed) {
    _st_esos_uiF14Data.b_SW3DoublePressed = b_swDoublePressed;
    return;
}

inline void esos_uiF14_setSW3State(SwitchState st_swState) {
	_st_esos_uiF14Data.st_SW3State = st_swState;
	return;
}

inline void esos_uiF14_setSW3DoublePressPeriod(uint16_t u16_period){
	_st_esos_uiF14Data.u16_SW3DoublePressPeriod = u16_period;
	return;
}

//SW1 PRIVATE Getters
inline SwitchState esos_uiF14_getSW1State(void) {
  	return (_st_esos_uiF14Data.st_SW1State);
}

//SW2 PRIVATE Getters
inline SwitchState esos_uiF14_getSW2State(void) {
  	return (_st_esos_uiF14Data.st_SW2State);
}

//SW3 PRIVATE Getters
inline SwitchState esos_uiF14_getSW3State(void) {
  	return (_st_esos_uiF14Data.st_SW3State);
}

// PUBLIC SWITCH FUNCTIONS
//SW1
inline BOOL esos_uiF14_getSW1Pressed (void) {
    return (_st_esos_uiF14Data.b_SW1Pressed==TRUE);
}

inline BOOL esos_uiF14_getSW1Released (void) {
    return (_st_esos_uiF14Data.b_SW1Pressed==FALSE);
}

inline BOOL esos_uiF14_getSW1DoublePressed (void) {
	BOOL b_doublePressed = _st_esos_uiF14Data.b_SW1DoublePressed;
    _st_esos_uiF14Data.b_SW1DoublePressed = FALSE;
	return b_doublePressed;
}

inline uint16_t esos_uiF14_getSW1DoublePressPeriod (void) {
    return (_st_esos_uiF14Data.u16_SW1DoublePressPeriod);
}

//SW2
inline BOOL esos_uiF14_getSW2Pressed (void) {
    return (_st_esos_uiF14Data.b_SW2Pressed==TRUE);
}

inline BOOL esos_uiF14_getSW2Released (void) {
    return (_st_esos_uiF14Data.b_SW2Pressed==FALSE);
}

inline BOOL esos_uiF14_getSW2DoublePressed (void) {
	BOOL b_doublePressed = _st_esos_uiF14Data.b_SW2DoublePressed;
	_st_esos_uiF14Data.b_SW2DoublePressed = FALSE;
	return b_doublePressed;
}

inline uint16_t esos_uiF14_getSW2DoublePressPeriod (void) {
    return (_st_esos_uiF14Data.u16_SW2DoublePressPeriod);
}

//SW3
inline BOOL esos_uiF14_getSW3Pressed (void) {
    return (_st_esos_uiF14Data.b_SW3Pressed==TRUE);
}

inline BOOL esos_uiF14_getSW3Released (void) {
  	return (_st_esos_uiF14Data.b_SW3Pressed==FALSE);
}

inline BOOL esos_uiF14_getSW3DoublePressed (void) {
	BOOL b_doublePressed = _st_esos_uiF14Data.b_SW3DoublePressed;
	_st_esos_uiF14Data.b_SW3DoublePressed = FALSE;
	return b_doublePressed;
}

inline uint16_t esos_uiF14_getSW3DoublePressPeriod (void) {
    return (_st_esos_uiF14Data.u16_SW3DoublePressPeriod);
}

// PUBLIC LED FUNCTIONS
//LED1
inline BOOL esos_uiF14_isLED1On (void) {
    return (_st_esos_uiF14Data.b_LED1On==TRUE);
}

inline BOOL esos_uiF14_isLED1Off (void) {
    return (_st_esos_uiF14Data.b_LED1On==FALSE);
}

inline uint16_t esos_uiF14_getLED1FlashPeriod(void) {
	return (_st_esos_uiF14Data.u16_LED1FlashPeriod);
}

inline void esos_uiF14_turnLED1On (void) {
    _st_esos_uiF14Data.b_LED1On = TRUE;
	esos_uiF14_flashLED1(0);
    return;
}

inline void esos_uiF14_turnLED1Off (void) {
    _st_esos_uiF14Data.b_LED1On = FALSE;
	esos_uiF14_flashLED1(0);
    return;
}

inline void esos_uiF14_toggleLED1 (void) {
    _st_esos_uiF14Data.b_LED1On ^= 1;
	esos_uiF14_flashLED1(0);
    return;
}

void esos_uiF14_flashLED1( uint16_t u16_period) {
	_st_esos_uiF14Data.u16_LED1FlashPeriod = u16_period;
	if (u16_period == 0){
		if (esos_GetTimerHandle(led1Timer) != ESOS_TMR_FAILURE) {
			esos_UnregisterTimer(led1TimerHandle);
		}
	} 
	else {
		TURN_ON_LED1();
		led1TimerHandle = esos_RegisterTimer(led1Timer, _st_esos_uiF14Data.u16_LED1FlashPeriod);
	}
	return;
}

//LED2
inline BOOL esos_uiF14_isLED2On (void) {
    return (_st_esos_uiF14Data.b_LED2On==TRUE);
}

inline BOOL esos_uiF14_isLED2Off (void) {
    return (_st_esos_uiF14Data.b_LED2On==FALSE);
}

inline uint16_t esos_uiF14_getLED2FlashPeriod(void) {
	return (_st_esos_uiF14Data.u16_LED2FlashPeriod);
}

inline void esos_uiF14_turnLED2On (void) {
    _st_esos_uiF14Data.b_LED2On = TRUE;
	esos_uiF14_flashLED2(0);
    return;
}

inline void esos_uiF14_turnLED2Off (void) {
    _st_esos_uiF14Data.b_LED2On = FALSE;
	esos_uiF14_flashLED2(0);
    return;
}

inline void esos_uiF14_toggleLED2 (void) {
    _st_esos_uiF14Data.b_LED2On ^= 1;
	esos_uiF14_flashLED2(0);
    return;
}

void esos_uiF14_flashLED2( uint16_t u16_period) {
	_st_esos_uiF14Data.u16_LED2FlashPeriod = u16_period;
	if (u16_period == 0){
		if (esos_GetTimerHandle(led2Timer) != ESOS_TMR_FAILURE) {
			esos_UnregisterTimer(led2TimerHandle);
		}
	} 
	else {
		TURN_ON_LED2();
		led2TimerHandle = esos_RegisterTimer(led2Timer, _st_esos_uiF14Data.u16_LED2FlashPeriod);
	}
	return;
}

//LED3
inline BOOL esos_uiF14_isLED3On (void) {
    return (_st_esos_uiF14Data.b_LED3On==TRUE);
}

inline BOOL esos_uiF14_isLED3Off (void) {
    return (_st_esos_uiF14Data.b_LED3On==FALSE);
}

inline uint16_t esos_uiF14_getLED3FlashPeriod(void) {
	return (_st_esos_uiF14Data.u16_LED3FlashPeriod);
}

inline void esos_uiF14_turnLED3On (void) {
    _st_esos_uiF14Data.b_LED3On = TRUE;
	esos_uiF14_flashLED3(0);
    return;
}

inline void esos_uiF14_turnLED3Off (void) {
    _st_esos_uiF14Data.b_LED3On = FALSE;
	esos_uiF14_flashLED3(0);
    return;
}

inline void esos_uiF14_toggleLED3 (void) {
    _st_esos_uiF14Data.b_LED3On ^= 1;
	esos_uiF14_flashLED3(0);
    return;
}

inline void esos_uiF14_flashLED3( uint16_t u16_period) {
	_st_esos_uiF14Data.u16_LED3FlashPeriod = u16_period;
	if (u16_period == 0) {
		if (esos_GetTimerHandle(led3Timer) != ESOS_TMR_FAILURE) {
			esos_UnregisterTimer(led3TimerHandle);
		}
	} 
	else {
		TURN_ON_LED3();
		led3TimerHandle = esos_RegisterTimer(led3Timer, _st_esos_uiF14Data.u16_LED3FlashPeriod);
	}
	return;
}

//Red, Yellow, Green Functions
inline void esos_uiF14_turnRedLEDOn (void){
	esos_uiF14_turnLED1On();
	return;
}

inline void esos_uiF14_turnRedLEDOff (void){
	esos_uiF14_turnLED1Off();
	return;
}

inline void esos_uiF14_turnYellowLEDOn (void){
	esos_uiF14_turnLED2On();
	return;
}

inline void esos_uiF14_turnYellowLEDOff (void){
	esos_uiF14_turnLED2Off();
	return;
}

inline void esos_uiF14_turnGreenLEDOn (void){
	esos_uiF14_turnLED3On();
	return;
}

inline void esos_uiF14_turnGreenLEDOff (void){
	esos_uiF14_turnLED3Off();
	return;
}

// PUBLIC RPG FUNCTIONS

inline int16_t esos_uiF14_getRpgValue_i16 ( void ) {
    return _st_esos_uiF14Data.i16_RPGCounter;
}

inline BOOL esos_uiF14_isRpgTurning ( void ) {
    return (esos_uiF14_getRpgVelocity_i16() != 0);
}

inline BOOL esos_uiF14_isRpgTurningSlow( void ) {
	int16_t i16_velocity = esos_uiF14_getRpgVelocity_i16();
	if (i16_velocity < 0){
		return esos_uiF14_isRpgTurning() && (int32_t)(-1 * i16_velocity) < (int32_t)_st_esos_uiF14Data.u16_rpgSlowVelocityLimit;
	}
	else{
		return esos_uiF14_isRpgTurning() &&  (int32_t)i16_velocity < (int32_t)_st_esos_uiF14Data.u16_rpgSlowVelocityLimit;
	}
}

inline BOOL esos_uiF14_isRpgTurningMedium( void ) {
	int16_t i16_velocity = esos_uiF14_getRpgVelocity_i16();
	if (i16_velocity < 0){
		return esos_uiF14_isRpgTurning() && ((int32_t)(-1 * i16_velocity) >= (int32_t)_st_esos_uiF14Data.u16_rpgSlowVelocityLimit) && ((int32_t)(-1 * i16_velocity) <= (int32_t)_st_esos_uiF14Data.u16_rpgFastVelocityLimit);
	}
	else{
		return esos_uiF14_isRpgTurning() && ((int32_t)i16_velocity >= (int32_t)_st_esos_uiF14Data.u16_rpgSlowVelocityLimit) && ((int32_t)i16_velocity <= (int32_t)_st_esos_uiF14Data.u16_rpgFastVelocityLimit);
	}
}

inline BOOL esos_uiF14_isRpgTurningFast( void ) {
	int16_t i16_velocity = esos_uiF14_getRpgVelocity_i16();
	if (i16_velocity < 0){
		return esos_uiF14_isRpgTurning() && ((int32_t)(-1 * i16_velocity) > (int32_t)_st_esos_uiF14Data.u16_rpgFastVelocityLimit);
	}
	else{
		return esos_uiF14_isRpgTurning() && ((int32_t)i16_velocity > (int32_t)_st_esos_uiF14Data.u16_rpgFastVelocityLimit);
	}
}

inline BOOL esos_uiF14_isRpgTurningCW( void ) {
  	return (esos_uiF14_getRpgVelocity_i16() > 0);
}

inline BOOL esos_uiF14_isRpgTurningCCW( void ) {
  	return (esos_uiF14_getRpgVelocity_i16() < 0);
}

inline int16_t esos_uiF14_getRPGVelocity_i16( void ) {
	return i16_globalVelocity;
}

inline int16_t esos_uiF14_getRPGSlowVelocityLimit( void ) {
	return _st_esos_uiF14Data.u16_rpgSlowVelocityLimit;
}

inline void esos_uiF14_setRPGSlowVelocityLimit( uint16_t u16_limit ){
	_st_esos_uiF14Data.u16_rpgSlowVelocityLimit = u16_limit;
	return;
}

inline int16_t esos_uiF14_getRPGFastVelocityLimit( void ) {
	return _st_esos_uiF14Data.u16_rpgFastVelocityLimit;
}

inline void esos_uiF14_setRPGFastVelocityLimit( uint16_t u16_limit ){
	_st_esos_uiF14Data.u16_rpgFastVelocityLimit = u16_limit;
	return;
}


//RPG Timer
ESOS_USER_TIMER(RPGTimer){
	static uint32_t lastTimeClickOccurs = 0;
	static BOOL b_tickFinished = FALSE; //becomes true when the RPG signals are both high
	uint32_t currentTimeClickOccurs;
	//Read pins
	_esos_uiF14_setRPGAHigh (RPGA_IS_HIGH());
	_esos_uiF14_setRPGBHigh (RPGB_IS_HIGH());

	//Check for click to occur 
	//The rpg click creates a signal where one line goes low before the other depending on the direction it was
	// turned therefore this if statement makes sure the last tick has finished (meaning both signals went back high)
	// then calculate the velocity based on the direction and time between clicks
	if (b_tickFinished && !_esos_uiF14_getRPGAHigh() && _esos_uiF14_getRPGBHigh()){
		if (lastTimeClickOccurs == 0){
			lastTimeClickOccurs = esos_GetSystemTick();
		} 
		else {
			currentTimeClickOccurs = esos_GetSystemTick();
			if ((currentTimeClickOccurs - lastTimeClickOccurs) > THRESHOLD_RPG_IDLE_TIME){
				lastTimeClickOccurs = currentTimeClickOccurs;
			} 
			else{
				_esos_uiF14_setLastRPGCounter(_esos_uiF14_getRPGCounter());
				_esos_uiF14_setRPGCounter(_esos_uiF14_getRPGCounter() + 1);
				i16_globalVelocity = MS_IN_A_SEC / (currentTimeClickOccurs - lastTimeClickOccurs);
				lastTimeClickOccurs = currentTimeClickOccurs;
			}
		}
		b_tickFinished = FALSE;
	} 
	else if(b_tickFinished && _esos_uiF14_getRPGAHigh() && !_esos_uiF14_getRPGBHigh()){
		if (lastTimeClickOccurs == 0){
			lastTimeClickOccurs = esos_GetSystemTick();
		} 
		else {
			currentTimeClickOccurs = esos_GetSystemTick();
			if ((currentTimeClickOccurs - lastTimeClickOccurs) > THRESHOLD_RPG_IDLE_TIME){
				lastTimeClickOccurs = currentTimeClickOccurs;
			} 
			else {
				_esos_uiF14_setLastRPGCounter(_esos_uiF14_getRPGCounter());
				_esos_uiF14_setRPGCounter(_esos_uiF14_getRPGCounter() - 1);
				i16_globalVelocity = (~(MS_IN_A_SEC / (currentTimeClickOccurs - lastTimeClickOccurs))) + 1 ;
				lastTimeClickOccurs = currentTimeClickOccurs;
			}
		}
		b_tickFinished = FALSE;
	}
	else {
		if (_esos_uiF14_getRPGAHigh() && _esos_uiF14_getRPGBHigh()) {
			b_tickFinished = TRUE;
		}
		if (__ESOS_TICKS_TO_MS(esos_GetSystemTick() - lastTimeClickOccurs) >= MS_IN_A_SEC){
			i16_globalVelocity = 0; //this sets the velocity to 0 since the RPG hasn't turned in over one second
		}
	}
}

// UIF14 INITIALIZATION FUNCTION
void config_esos_uiF14() {
	// setup your UI implementation
	//Configure UI hardware
	CONFIG_LED1();
	CONFIG_LED2();
	CONFIG_LED3();

	configSW3();
	configSW2();
	configSW1();

	configRPGA();
	configRPGB();

	_esos_uiF14_setRPGCounter(0);
	_esos_uiF14_setLastRPGCounter(0);

	// initialize switch state for Double Pressed FSM.
	// It's safe to assume released since if it's pressed
	// the state machine will just instantly transition.
	esos_uiF14_setSW1State(SwitchReleased);
	esos_uiF14_setSW2State(SwitchReleased);
	esos_uiF14_setSW3State(SwitchReleased);

	esos_uiF14_turnLED1Off();
	esos_uiF14_turnLED2Off();
	esos_uiF14_turnLED3Off();

	esos_uiF14_setSW1Pressed(SW1_IS_DEPRESSED());
	esos_uiF14_setSW2Pressed(SW2_IS_DEPRESSED());
	esos_uiF14_setSW3Pressed(SW3_IS_DEPRESSED());

	esos_uiF14_setSW1DoublePressed(FALSE);
	esos_uiF14_setSW2DoublePressed(FALSE);
	esos_uiF14_setSW3DoublePressed(FALSE);

	esos_uiF14_setSW1DoublePressPeriod(DEFAULT_DOUBLE_PRESS_PERIOD_MS);
	esos_uiF14_setSW2DoublePressPeriod(DEFAULT_DOUBLE_PRESS_PERIOD_MS);
	esos_uiF14_setSW3DoublePressPeriod(DEFAULT_DOUBLE_PRESS_PERIOD_MS);

	esos_uiF14_setRPGSlowVelocityLimit(DEFAULT_SLOW_RPG_THRESHOLD_TICKS_PER_SEC);
	esos_uiF14_setRPGFastVelocityLimit(DEFAULT_FAST_RPG_THRESHOLD_TICKS_PER_SEC);

	//Configure UI Service
	esos_RegisterTimer(RPGTimer, RPG_TIMER_SAMPLE_RATE);
	esos_RegisterTask( __esos_uiF14_task );
}


// util
inline BOOL isSWDoublePressTimerExpired(uint32_t u32_ticksAtFirstPress, uint16_t (*fn_getDoublePressPeriod)(void)){
  	return (__ESOS_TICKS_TO_MS(esos_GetSystemTick() - u32_ticksAtFirstPress) > fn_getDoublePressPeriod());
}

SwitchState updateSwState(SwitchState st_state, uint32_t* u32_ticksOfLastPress, BOOL b_swVal, void (*fn_setDoublePressed)(BOOL), uint16_t (*fn_getDoublePressPeriod)(void)){
	BOOL b_timerExpired = isSWDoublePressTimerExpired(*u32_ticksOfLastPress, fn_getDoublePressPeriod);
	switch(st_state){
		case SwitchReleased:{
			if (b_swVal){
				st_state = SwitchPressedOnce;
				*u32_ticksOfLastPress = esos_GetSystemTick();
			}
			break;
		}
		case SwitchPressedOnce:{
			if (!b_swVal){
				st_state = (b_timerExpired) ? SwitchReleased : SwitchReleasedAfterPress;
			}
			break;
		}
		case SwitchReleasedAfterPress:{
			if (b_swVal){
				if (b_timerExpired){
					*u32_ticksOfLastPress = esos_GetSystemTick();
					st_state = SwitchPressedOnce;
				}
				else{
					st_state = SwitchPressedTwice;
				}
			}
			break;
		}
		case SwitchPressedTwice:{
			if (!b_swVal){
				if (!b_timerExpired){
					fn_setDoublePressed(TRUE);
				}
				st_state = SwitchReleased;
			}
			break;
		}
		default: st_state = SwitchReleased; break; // should never really happen.
	}
	return st_state;
}

// UIF14 task to manage user-interface
ESOS_USER_TASK( __esos_uiF14_task ){
	static uint32_t u32_sw1TicksAtFirstPress;
	static uint32_t u32_sw2TicksAtFirstPress;
	static uint32_t u32_sw3TicksAtFirstPress;
  	ESOS_TASK_BEGIN();
  	while(TRUE) {
		// UI Stuff
		esos_uiF14_setSW1Pressed(SW1_IS_DEPRESSED());
		esos_uiF14_setSW2Pressed(SW2_IS_DEPRESSED());
		esos_uiF14_setSW3Pressed(SW3_IS_DEPRESSED());

		SwitchState st_sw1State = esos_uiF14_getSW1State();
		SwitchState st_sw2State = esos_uiF14_getSW2State();
		SwitchState st_sw3State = esos_uiF14_getSW3State();

		st_sw1State = updateSwState(st_sw1State, &u32_sw1TicksAtFirstPress, esos_uiF14_getSW1Pressed(), esos_uiF14_setSW1DoublePressed, esos_uiF14_getSW1DoublePressPeriod);
		st_sw2State = updateSwState(st_sw2State, &u32_sw2TicksAtFirstPress, esos_uiF14_getSW2Pressed(), esos_uiF14_setSW2DoublePressed, esos_uiF14_getSW2DoublePressPeriod);
		st_sw3State = updateSwState(st_sw3State, &u32_sw3TicksAtFirstPress, esos_uiF14_getSW3Pressed(), esos_uiF14_setSW3DoublePressed, esos_uiF14_getSW3DoublePressPeriod);

		esos_uiF14_setSW1State(st_sw1State);
		esos_uiF14_setSW2State(st_sw2State);
		esos_uiF14_setSW3State(st_sw3State);

		if ((esos_GetTimerHandle(led1Timer) == ESOS_TMR_FAILURE)) {
			if (esos_uiF14_isLED1On()) {
				TURN_ON_LED1();
			} 
			else {
				TURN_OFF_LED1();
			}
		}

		if ((esos_GetTimerHandle(led2Timer) == ESOS_TMR_FAILURE)) {
			if (esos_uiF14_isLED2On()) {
				TURN_ON_LED2();
			} 
			else {
				TURN_OFF_LED2();
			}
		}

		if ((esos_GetTimerHandle(led3Timer) == ESOS_TMR_FAILURE)) {
			if (esos_uiF14_isLED3On()) {
				TURN_ON_LED3();
			} 
			else {
				TURN_OFF_LED3();
			}
		}
		ESOS_TASK_WAIT_TICKS( __ESOS_UIF14_UI_PERIOD_MS );
  	}
  	ESOS_TASK_END();
}
