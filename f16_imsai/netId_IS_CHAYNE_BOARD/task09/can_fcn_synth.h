#ifndef CAN_FCN_SYNTH_H_
#define CAN_FCN_SYNTH_H_
#include "fcn_synth.h"

typedef enum{
	Amp,
	Freq,
	Wvform,
	DtyCyc,
	LED
} UpdateType;

typedef struct{
	BOOL b_update;
	uint8_t u8_OtherID;
	uint16_t u16_data;
	UpdateType e_type;
} UpdateBoardParams;

FncSynthMode getWaveFormSettingForBoard(uint8_t u8_boardID);
uint16_t getFreqSettingForBoard(uint8_t u8_boardID);
uint8_t getAmpSettingForBoard(uint8_t u8_boardID);
uint8_t getDutyCycleSettingForBoard(uint8_t u8_boardID);
void updateBoard(uint8_t BoardID, uint16_t data, UpdateType Type);

void configCanFcnSynth();

#endif