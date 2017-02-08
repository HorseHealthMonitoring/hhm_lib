#ifndef FCN_SYNTH_H_
#define FCN_SYNTH_H_
#include "esos.h"

// Defines
#define NUM_SAMPLES (10)
#define COUNTS_PER_CYCLE (DIV * NUM_SAMPLES)
#define DIV (2)
#define FREQ_TO_US(u16_freq) (1000000L/((uint32_t)u16_freq))

typedef enum 
{
	Sine,
	Triangle,
	Square
} FncSynthMode;

typedef struct 
{
	FncSynthMode e_mode;
	uint32_t u16_amplitude; // NOTE: this should be a 12 bit number where 0xFFF is the maximum
	uint8_t u8_amplitude_3point5;
	uint16_t u32_frequency;
	uint16_t u16_frequency_Actual;
	uint8_t u8_dutyCycle;
	uint8_t u8_dutyCycle_Actual;
	uint16_t u16_stepSize;
	uint16_t au16_sineValues[NUM_SAMPLES + 1];
	uint8_t u8_ledValue;
} st_fcnSynthParams_t ;

void config_fcn_synth(void);
void fcn_synth_setAmplitude(uint8_t);
void fcn_synth_setFreq(uint16_t);
void fcn_synth_setDutyCycle(uint8_t);
void fcn_synth_setWaveForm(FncSynthMode);

uint8_t fcn_synth_getAmplitude();
uint16_t fcn_synth_getFreq();
uint8_t fcn_synth_getDutyCycle();
FncSynthMode fcn_synth_getWaveForm();

uint8_t fcn_synth_getAmplitude_B();
uint16_t fcn_synth_getFreq_B();
uint8_t fcn_synth_getDutyCycle_B();
FncSynthMode fcn_synth_getWaveForm_B();

#endif