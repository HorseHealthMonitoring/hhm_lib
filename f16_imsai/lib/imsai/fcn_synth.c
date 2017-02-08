#include "fcn_synth.h"
#include "esos.h"
#include "esos_pic24_irq.h"
#include "pic24_all.h"
#include "revF16.h"
#include "esos_pic24_spi.h"

// Global variables
static const uint16_t au16_sineValues[NUM_SAMPLES + 1] = 
{
	0, 100, 391, 844, 1415,  
	2048, 2475, 3251, 3704, 3995, 4095 
};

st_fcnSynthParams_t st_fcnSynthParams;
st_fcnSynthParams_t st_fcnSynthParamsB;


// User defined functions
uint32_t usToTicks(uint32_t u32_us, uint16_t u16_tmrPre) 
{
  	return ((uint32_t) (((uint64_t) u32_us) * ((uint64_t) FCY) / ((((uint64_t) u16_tmrPre) * (uint64_t) 1000000L))));
}

void fcn_synth_setAmplitude(uint8_t u8_amplitude3Point5)
{
	st_fcnSynthParams.u8_amplitude_3point5 = u8_amplitude3Point5;
	// The highest possible received value is 127 and the highest DAC value
	// is 4095 so ((4095 / 127) = 323) => use this to adjust the received value
	uint16_t u16_amplitude = ((uint16_t)u8_amplitude3Point5) * 323;
	// round if needed
	if ((u16_amplitude % 10) >= 5) {
		u16_amplitude += 10;
	}
	u16_amplitude /= 10;
	// saturate if over the maximum result
	if (u16_amplitude > 0xFFF) {
		u16_amplitude = 0xFFF;
	}

	st_fcnSynthParams.u16_amplitude = u16_amplitude;

	// calculate the step size that the triangle wave will increase by
	uint16_t u16_adjustedAmp = (u16_amplitude * 10) / NUM_SAMPLES;
	if ((u16_adjustedAmp % 10) >= 5)
		u16_adjustedAmp += 10;
	st_fcnSynthParams.u16_stepSize = (u16_adjustedAmp / 10);

	// adjust the sine wave values based off of the amplitude
	uint8_t u8_i = 0;
	uint16_t u16_percent = (uint16_t)((((uint32_t) u16_amplitude) * 100L) / (((uint32_t) au16_sineValues[NUM_SAMPLES])));
	uint32_t u32_sineScale = 1000L / ((uint32_t)u16_percent);
	for (u8_i = 0; u8_i < (NUM_SAMPLES + 1); ++u8_i) {
		st_fcnSynthParams.au16_sineValues[u8_i] = (uint16_t)(((((uint32_t)au16_sineValues[u8_i]) * 10L) / ((uint32_t)u32_sineScale))) ;
	}
}

void fcn_synth_setFreq(uint16_t u16_freq)
{
	st_fcnSynthParams.u16_frequency_Actual = u16_freq;
	st_fcnSynthParams.u32_frequency = ((uint32_t)u16_freq) * ((uint32_t)NUM_SAMPLES) * ((uint32_t)DIV);
}

void fcn_synth_setDutyCycle(uint8_t u8_dutyCycle)
{
	st_fcnSynthParams.u8_dutyCycle_Actual = u8_dutyCycle;
	if (u8_dutyCycle > 50)
	{
		uint8_t u8_dutyCycleOverFifty = u8_dutyCycle - 50;
		uint8_t u8_adjustedDuty = (100 /  u8_dutyCycleOverFifty);
		st_fcnSynthParams.u8_dutyCycle = (COUNTS_PER_CYCLE / 2) + (COUNTS_PER_CYCLE / u8_adjustedDuty);
	}
	else
	{
		uint8_t u8_adjustedDuty = (100 /  u8_dutyCycle);
		st_fcnSynthParams.u8_dutyCycle = COUNTS_PER_CYCLE / u8_adjustedDuty;
	}
}

void fcn_synth_setWaveForm(FncSynthMode e_mode) {
	st_fcnSynthParams.e_mode = e_mode;
}

uint8_t fcn_synth_getAmplitude() {
	return st_fcnSynthParams.u8_amplitude_3point5;
}

uint16_t fcn_synth_getFreq() {
	return st_fcnSynthParams.u16_frequency_Actual;
}

uint8_t fcn_synth_getDutyCycle() {
	return st_fcnSynthParams.u8_dutyCycle_Actual;
}

FncSynthMode fcn_synth_getWaveForm() {
	return st_fcnSynthParams.e_mode;
}


void fcn_synth_setAmplitude_B(uint16_t u8_amplitude3Point5)
{
	st_fcnSynthParamsB.u8_amplitude_3point5 = u8_amplitude3Point5;
	// The highest possible received value is 127 and the highest DAC value
	// is 4095 so ((4095 / 127) = 323) => use this to adjust the received value
	uint16_t u16_amplitude = ((uint16_t)u8_amplitude3Point5) * 323;
	// round if needed
	if ((u16_amplitude % 10) >= 5) {
		u16_amplitude += 10;
	}
	u16_amplitude /= 10;
	// saturate if over the maximum result
	if (u16_amplitude > 0xFFF) {
		u16_amplitude = 0xFFF;
	}

	st_fcnSynthParamsB.u16_amplitude = u16_amplitude;

	// calculate the step size that the triangle wave will increase by
	uint16_t u16_adjustedAmp = (u16_amplitude * 10) / NUM_SAMPLES;
	if ((u16_adjustedAmp % 10) >= 5)
		u16_adjustedAmp += 10;
	st_fcnSynthParamsB.u16_stepSize = (u16_adjustedAmp / 10);

	// adjust the sine wave values based off of the amplitude
	uint8_t u8_i = 0;
	uint16_t u16_percent = (uint16_t)((((uint32_t) u16_amplitude) * 100L) / (((uint32_t) au16_sineValues[NUM_SAMPLES])));
	uint32_t u32_sineScale = 1000L / ((uint32_t)u16_percent);
	for (u8_i = 0; u8_i < (NUM_SAMPLES + 1); ++u8_i) {
		st_fcnSynthParamsB.au16_sineValues[u8_i] = (uint16_t)(((((uint32_t)au16_sineValues[u8_i]) * 10L) / ((uint32_t)u32_sineScale))) ;
	}
}

void fcn_synth_setFreq_B(uint16_t u16_freq)
{
	st_fcnSynthParamsB.u16_frequency_Actual = u16_freq;
	st_fcnSynthParamsB.u32_frequency = ((uint32_t)u16_freq) * ((uint32_t)NUM_SAMPLES) * ((uint32_t)DIV);
}

void fcn_synth_setDutyCycle_B(uint8_t u8_dutyCycle){
	st_fcnSynthParamsB.u8_dutyCycle_Actual = u8_dutyCycle;
	if (u8_dutyCycle > 50)
	{
		uint8_t u8_dutyCycleOverFifty = u8_dutyCycle - 50;
		uint8_t u8_adjustedDuty = (100 /  u8_dutyCycleOverFifty);
		st_fcnSynthParamsB.u8_dutyCycle = (COUNTS_PER_CYCLE / 2) + (COUNTS_PER_CYCLE / u8_adjustedDuty);
	}
	else
	{
		uint8_t u8_adjustedDuty = (100 /  u8_dutyCycle);
		st_fcnSynthParamsB.u8_dutyCycle = COUNTS_PER_CYCLE / u8_adjustedDuty;
	}
}

void fcn_synth_setWaveForm_B(FncSynthMode e_mode){
	st_fcnSynthParamsB.e_mode = e_mode;
}

void fcn_setLEDValue(uint8_t u8_numericLEDValue){
	if (u8_numericLEDValue == 0){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3Off();
	}
	if (u8_numericLEDValue == 1){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3On();
	}
	if (u8_numericLEDValue == 2){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3Off();
	}
	if (u8_numericLEDValue == 3){
		esos_uiF14_turnLED1Off();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3On();
	}
	if (u8_numericLEDValue == 4){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3Off();
	}
	if (u8_numericLEDValue == 5){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2Off();
		esos_uiF14_turnLED3On();
	}
	if (u8_numericLEDValue == 6){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3Off();
	}
	if (u8_numericLEDValue == 7){
		esos_uiF14_turnLED1On();
		esos_uiF14_turnLED2On();
		esos_uiF14_turnLED3On();
	}
	st_fcnSynthParams.u8_ledValue = u8_numericLEDValue;
}
uint8_t fcn_synth_getAmplitude_B() {
	return st_fcnSynthParamsB.u8_amplitude_3point5;
}

uint16_t fcn_synth_getFreq_B() {
	return st_fcnSynthParamsB.u16_frequency_Actual;
}

uint8_t fcn_synth_getDutyCycle_B() {
	return st_fcnSynthParamsB.u8_dutyCycle_Actual;
}

FncSynthMode fcn_synth_getWaveForm_B() {
	return st_fcnSynthParamsB.e_mode;
}

uint8_t fcn_getNumericLEDValue(){
	return st_fcnSynthParams.u8_ledValue;
}

void configTimer2() 
{
	// Ensure that Timer2 configured as separate timers.
	T2CONbits.T32 = 0;     // 32-bit mode off
	// T2CON set like this for documentation purposes.
	// Could be replaced by T2CON = 0x0030
	T2CON = T2_ON | T2_IDLE_CON | T2_GATE_OFF
	      | T2_SOURCE_INT
	      | T2_PS_1_64 ;  //results in T2CON= 0x0030
	TMR2  = 0;                       //clear timer2 value

	/* set T2's priority to be the highest possible for a PIC24 user IRQ
	** User IRQs (if used) will be a lower IRQ priority, so the tick will
	** get serviced in a timely manner
	*/
	_T2IE = 1;                       // enable the interrupt
	ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T2);
	T2CONbits.TON = 1;               //turn on the timer

}

void configTimer3() 
{
	// T2CON set like this for documentation purposes.
	// Could be replaced by T2CON = 0x0030
	T3CON = T3_ON | T3_IDLE_CON | T3_GATE_OFF
	      | T3_SOURCE_INT
	      | T3_PS_1_64 ;  //results in T2CON= 0x0030
	TMR3  = 0;                       //clear timer2 value

	/* set T2's priority to be the highest possible for a PIC24 user IRQ
	** User IRQs (if used) will be a lower IRQ priority, so the tick will
	** get serviced in a timely manner
	*/
	_T3IE = 1;                       // enable the interrupt
	ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T3);
	T3CONbits.TON = 1;               //turn on the timer

}

void config_fcn_synth(void) 
{
	configTimer2();

	fcn_synth_setFreq(64);
	fcn_synth_setAmplitude(0x69);
	fcn_synth_setDutyCycle(50);
	fcn_synth_setWaveForm(Square);
	
	ESOS_REGISTER_PIC24_USER_INTERRUPT( ESOS_IRQ_PIC24_T2, ESOS_USER_IRQ_LEVEL1, _T2Interrupt);
 	ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T2);

 	configTimer3();

	fcn_synth_setFreq_B(64);
	fcn_synth_setAmplitude_B(0x69);
	fcn_synth_setDutyCycle_B(50);
	fcn_synth_setWaveForm_B(Square);
	
	ESOS_REGISTER_PIC24_USER_INTERRUPT( ESOS_IRQ_PIC24_T3, ESOS_USER_IRQ_LEVEL1, _T3Interrupt);
 	ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T3);
	
	configDAC();
}


// Timer interrupt
ESOS_USER_INTERRUPT( ESOS_IRQ_PIC24_T2 ) 
{
	static BOOL b_goingUp = TRUE;
	static uint16_t u16_value = 0;
	static FncSynthMode e_mode;
	static uint16_t u16_amplitude;
	static uint16_t u16_stepSize;
	static uint8_t u8_dutyCycle;
	static uint8_t u8_counter = 0;
	static BOOL b_cycleFinished = TRUE;
	static uint16_t* pu16_sineValues;
	static uint16_t u32_frequency;

	ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T2);    //clear the interrupt bit

	// We need to prevent the UI from changing everything 
	// randomly in the middle of a cycle. Let the waveform
	// finish and then use the new fields.
	if (b_cycleFinished) 
	{
		e_mode = st_fcnSynthParams.e_mode;
		if (e_mode == Triangle)
			b_goingUp = TRUE;
		u16_amplitude = st_fcnSynthParams.u16_amplitude;
		u16_stepSize = st_fcnSynthParams.u16_stepSize;
		u8_dutyCycle = st_fcnSynthParams.u8_dutyCycle;
		pu16_sineValues = st_fcnSynthParams.au16_sineValues;
		if (u32_frequency != st_fcnSynthParams.u32_frequency)
		{
			u32_frequency = st_fcnSynthParams.u32_frequency;
			PR2 = (uint16_t) usToTicks(FREQ_TO_US(u32_frequency), getTimerPrescale(T2CONbits));
		}
		b_cycleFinished = FALSE;
	}

	switch (e_mode)
	{
		// make a triangle wave
		case Triangle:
		{
			if (b_goingUp) 
			{
				if (u8_counter >= (NUM_SAMPLES))
					b_goingUp = FALSE;
				else
					u16_value = (u16_stepSize * ((uint16_t)u8_counter));
			} 
			else
			{
				if (u8_counter == (COUNTS_PER_CYCLE - 1))
				{
					b_goingUp = TRUE;
					b_cycleFinished = TRUE;
				}
				else
					u16_value = (u16_amplitude) - (u16_stepSize * ((uint16_t)(u8_counter - 10)));
			}
			break;
		}
		// make a square wave
		case Square:
		{
			if (u8_counter >= u8_dutyCycle)
				b_goingUp = FALSE;
			else
				b_goingUp = TRUE;

			if (b_goingUp)
				u16_value = u16_amplitude;
			else
				u16_value = 0;

			if (u8_counter == (NUM_SAMPLES - 1))
				b_cycleFinished = TRUE;
			break;
		}
		// make a sine wave
		case Sine:
		{
			if (b_goingUp) 
			{
				if (u8_counter >= 10)
					b_goingUp = FALSE;
				uint8_t u8_index = u8_counter;
				u16_value = pu16_sineValues[u8_index];
			}
			else
			{
				if (u8_counter >= (COUNTS_PER_CYCLE- 1))
				{
					b_goingUp = TRUE;
					b_cycleFinished = TRUE;
				}
				uint8_t u8_index = 20 - u8_counter;
				u16_value = pu16_sineValues[u8_index];
			}
		}
		default: break;
	}
	if (++u8_counter == COUNTS_PER_CYCLE)
	{
		u8_counter = 0;
	}
	writeDac(u16_value);

} //end INT1 ISR

// Timer interrupt
ESOS_USER_INTERRUPT( ESOS_IRQ_PIC24_T3 ) 
{
	static BOOL b_goingUp = TRUE;
	static uint16_t u16_value = 0;
	static FncSynthMode e_mode;
	static uint16_t u16_amplitude;
	static uint16_t u16_stepSize;
	static uint8_t u8_dutyCycle;
	static uint8_t u8_counter = 0;
	static BOOL b_cycleFinished = TRUE;
	static uint16_t* pu16_sineValues;
	static uint16_t u32_frequency;

	ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T3);    //clear the interrupt bit

	// We need to prevent the UI from changing everything 
	// randomly in the middle of a cycle. Let the waveform
	// finish and then use the new fields.
	if (b_cycleFinished) 
	{
		e_mode = st_fcnSynthParamsB.e_mode;
		if (e_mode == Triangle)
			b_goingUp = TRUE;
		u16_amplitude = st_fcnSynthParamsB.u16_amplitude;
		u16_stepSize = st_fcnSynthParamsB.u16_stepSize;
		u8_dutyCycle = st_fcnSynthParamsB.u8_dutyCycle;
		pu16_sineValues = st_fcnSynthParamsB.au16_sineValues;
		if (u32_frequency != st_fcnSynthParamsB.u32_frequency)
		{
			u32_frequency = st_fcnSynthParamsB.u32_frequency;
			PR3  = (uint16_t) usToTicks(FREQ_TO_US(u32_frequency), getTimerPrescale(T3CONbits));
		}
		b_cycleFinished = FALSE;
	}

	switch (e_mode)
	{
		// make a triangle wave
		case Triangle:
		{
			if (b_goingUp) 
			{
				if (u8_counter >= (NUM_SAMPLES))
					b_goingUp = FALSE;
				else
					u16_value = (u16_stepSize * ((uint16_t)u8_counter));
			} 
			else
			{
				if (u8_counter == (COUNTS_PER_CYCLE - 1))
				{
					b_goingUp = TRUE;
					b_cycleFinished = TRUE;
				}
				else
					u16_value = (u16_amplitude) - (u16_stepSize * ((uint16_t)(u8_counter - 10)));
			}
			break;
		}
		// make a square wave
		case Square:
		{
			if (u8_counter >= u8_dutyCycle)
				b_goingUp = FALSE;
			else
				b_goingUp = TRUE;

			if (b_goingUp)
				u16_value = u16_amplitude;
			else
				u16_value = 0;

			if (u8_counter == (NUM_SAMPLES - 1))
				b_cycleFinished = TRUE;
			break;
		}
		// make a sine wave
		case Sine:
		{
			if (b_goingUp) 
			{
				if (u8_counter >= 10)
					b_goingUp = FALSE;
				uint8_t u8_index = u8_counter;
				u16_value = pu16_sineValues[u8_index];
			}
			else
			{
				if (u8_counter >= (COUNTS_PER_CYCLE- 1))
				{
					b_goingUp = TRUE;
					b_cycleFinished = TRUE;
				}
				uint8_t u8_index = 20 - u8_counter;
				u16_value = pu16_sineValues[u8_index];
			}
		}
		default: break;
	}
	if (++u8_counter == COUNTS_PER_CYCLE)
	{
		u8_counter = 0;
	}
	writeDac_B(u16_value);

} //end INT1 ISR


