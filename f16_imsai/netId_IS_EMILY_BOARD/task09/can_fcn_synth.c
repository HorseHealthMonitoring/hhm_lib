#include "can_fcn_synth.h"
#include "esos.h"
#include "esos_ecan.h"
#include "CANTeamMembers.h"
#include "MyCanStats.h"
#include "esos_mail.h"
#include "esos_ecan_imsai.h"

#define FCN_SYNTH_ADVERTISE_PERIOD (5000)
#define ESOS_TASK_WAIT_TICKS_OR_UNTIL(u32_duration, b_condition) do { \
		__pstSelf->u32_savedTick = esos_GetSystemTick();     \
	   __pstSelf->u32_waitLen = (u32_duration);             \
	   ESOS_TASK_WAIT_UNTIL(__esos_hasTickDurationPassed(__pstSelf->u32_savedTick, __pstSelf->u32_waitLen) || b_condition ); \
	} while(0)
		

UpdateBoardParams UpdateOtherBoard;
st_fcnSynthParams_t ast_otherBoardFcnSynthParams[NUM_OF_IDS];
static BOOL gb_advertiseFcnSynth = FALSE;

void updateBoard(uint8_t BoardID, uint16_t data, UpdateType Type){
	if (BoardID == MY_ID){
		gb_advertiseFcnSynth = TRUE;
		return;
	}
	else{
		UpdateOtherBoard.b_update = TRUE;
		UpdateOtherBoard.u8_OtherID = BoardID;
		UpdateOtherBoard.u16_data = data;
		UpdateOtherBoard.e_type = Type;
	}
}

FncSynthMode getWaveFormSettingForBoard(uint8_t u8_boardID) {
	if (u8_boardID == MY_ID) {
		return fcn_synth_getWaveForm();
	} else {
		return ast_otherBoardFcnSynthParams[u8_boardID].e_mode;
	}
}

uint16_t getFreqSettingForBoard(uint8_t u8_boardID) {
	if (u8_boardID == MY_ID) {
		return fcn_synth_getFreq();
	} else {
		return (uint16_t)ast_otherBoardFcnSynthParams[u8_boardID].u32_frequency;
	}
}

uint8_t getAmpSettingForBoard(uint8_t u8_boardID) {
	if (u8_boardID == MY_ID) {
		return fcn_synth_getAmplitude();
	} else {
		return ast_otherBoardFcnSynthParams[u8_boardID].u8_amplitude_3point5;
	}
}

uint8_t getDutyCycleSettingForBoard(uint8_t u8_boardID) {
	if (u8_boardID == MY_ID) {
		return fcn_synth_getDutyCycle();
	} else {
		return ast_otherBoardFcnSynthParams[u8_boardID].u8_dutyCycle;
	}
}

uint8_t getLEDNumericSettingForBoard(uint8_t u8_boardID){
	if (u8_boardID == MY_ID){
		return fcn_getNumericLEDValue();
	}else{
		printf("led value: %d", ast_otherBoardFcnSynthParams[u8_boardID].u8_ledValue);
		return ast_otherBoardFcnSynthParams[u8_boardID].u8_ledValue;
	}
}

ESOS_USER_TASK (FcnSynthAdvertiser) {
	static uint8_t au8_buf[2] = {0};
	ESOS_TASK_BEGIN();
	while(TRUE) {
		
		// waveform
		au8_buf[0] = (uint8_t)fcn_synth_getWaveForm_B();
		ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_WAVEFORM), au8_buf, 1);

		// duty cycle
		au8_buf[0] = fcn_synth_getDutyCycle_B();
		ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_DUTYCYCLE), au8_buf, 1);


		// frequency
		*( ((uint16_t*)au8_buf) ) = fcn_synth_getFreq_B();
		ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_FREQUENCY), au8_buf, 2);

		// amplitude
		au8_buf[0] = fcn_synth_getAmplitude_B();
		ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_AMPLITUDE), au8_buf, 1);
		
		//LEDs
		au8_buf[0] = fcn_getNumericLEDValue();
		ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_LEDS),au8_buf,1);

		ESOS_TASK_WAIT_TICKS_OR_UNTIL(FCN_SYNTH_ADVERTISE_PERIOD, gb_advertiseFcnSynth);
		gb_advertiseFcnSynth = FALSE;
	}

	ESOS_TASK_END();
}   

ESOS_USER_TASK( OtherBoardFcnSynthListener ) {
	static MAILMESSAGE msg;
	static uint16_t Local_ID;
	static uint8_t au8_buf[2];
	static uint8_t length;
	ESOS_TASK_BEGIN();
	const uint16_t u16_mask = CANMSG_TYPE_WAVEFORM | CANMSG_TYPE_FREQUENCY | CANMSG_TYPE_AMPLITUDE | CANMSG_TYPE_DUTYCYCLE | CANMSG_TYPE_LEDS;
	esos_ecan_canfactory_subscribe(__pstSelf, u16_mask, u16_mask, MASKCONTROL_FIELD_NONZERO);
	while(TRUE) {
		if (UpdateOtherBoard.b_update){
			UpdateOtherBoard.b_update = FALSE;
			switch (UpdateOtherBoard.e_type){
				case Amp:{
					au8_buf[0] = UpdateOtherBoard.u16_data;
					length = 1;
					Local_ID = calcMsgID(UpdateOtherBoard.u8_OtherID) | CANMSG_TYPE_AMPLITUDE;
					break;
				}
				case Freq:{
					*((uint16_t*)au8_buf) = UpdateOtherBoard.u16_data;
					length = 2;
					Local_ID = calcMsgID(UpdateOtherBoard.u8_OtherID) | CANMSG_TYPE_FREQUENCY;
					break;
				}
				case Wvform:{
					au8_buf[0] = UpdateOtherBoard.u16_data;
					length = 1;
					Local_ID = calcMsgID(UpdateOtherBoard.u8_OtherID) | CANMSG_TYPE_WAVEFORM;
					break;
				}
				case DtyCyc:{
					au8_buf[0] = UpdateOtherBoard.u16_data;
					length = 1;
					Local_ID = calcMsgID(UpdateOtherBoard.u8_OtherID) | CANMSG_TYPE_DUTYCYCLE;
					break;
				}
				case LED:{
					au8_buf[0] = UpdateOtherBoard.u16_data;
					length = 1;
					Local_ID = calcMsgID(UpdateOtherBoard.u8_OtherID) | CANMSG_TYPE_LEDS;
					break;
				}
				default:{
					UpdateOtherBoard.b_update = FALSE;
					continue;
				}
				}
			ESOS_ECAN_SEND(Local_ID, au8_buf, length);
		}
		
		//check to see if we need to update stuff from other boards
		while(ESOS_TASK_IVE_GOT_MAIL()){	
			ESOS_TASK_GET_NEXT_MESSAGE( &msg );
			uint16_t u16_can_id = msg.au16_Contents[0];
			
			//extract the team and member ID from the message
			uint16_t u16_teamID = stripTeamID(u16_can_id);
			uint16_t u16_memberID = stripMemberID(u16_can_id);
			uint8_t u8_arrayIndex = getArrayIndexFromTeamAndMemberIDs((uint8_t)u16_teamID, (uint8_t)u16_memberID);

			uint8_t u8_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof( uint16_t );

			// if it's a data request another task will handle it
			if (u8_len == 0) {
				continue;
			}
			
			// use the message ID to determine what information was sent
			switch (u16_can_id & typeIDMask)
			{
				case CANMSG_TYPE_WAVEFORM: 
				{
					if (msg.au8_Contents[2] < 3) {
						if (u8_arrayIndex == MY_ID) {
							fcn_synth_setWaveForm_B((FncSynthMode)msg.au8_Contents[2]);
							gb_advertiseFcnSynth = TRUE;
						} else {
							ast_otherBoardFcnSynthParams[u8_arrayIndex].e_mode = (FncSynthMode)msg.au8_Contents[2];
						}
					}
					break;
				}
				case CANMSG_TYPE_DUTYCYCLE:
				{
					if (msg.au8_Contents[2] <= 100) {
						if (u8_arrayIndex == MY_ID) {
							fcn_synth_setDutyCycle_B(msg.au8_Contents[2]);
							gb_advertiseFcnSynth = TRUE;
						} else {
							ast_otherBoardFcnSynthParams[u8_arrayIndex].u8_dutyCycle = msg.au8_Contents[2];
						}
					}
					break;
				}
				case CANMSG_TYPE_FREQUENCY:
				{
					if (u8_len >= 2) {
						uint16_t u16_freq = msg.au16_Contents[1];
						if ((u16_freq >= 64) && (u16_freq <= 2047)) {
							if (u8_arrayIndex == MY_ID) {
								fcn_synth_setFreq_B(u16_freq);
								gb_advertiseFcnSynth = TRUE;
							} else {
								ast_otherBoardFcnSynthParams[u8_arrayIndex].u32_frequency = u16_freq;
							}
						}
					}
					break;
				}
				case CANMSG_TYPE_AMPLITUDE:
				{
					uint8_t u8_ampltd3Point5Format = msg.au8_Contents[2];
					if (u8_arrayIndex == MY_ID) {
						fcn_synth_setAmplitude_B(u8_ampltd3Point5Format);
						gb_advertiseFcnSynth = TRUE;
					} else {
						ast_otherBoardFcnSynthParams[u8_arrayIndex].u8_amplitude_3point5 = u8_ampltd3Point5Format;
					}
					break;
				
				case CANMSG_TYPE_LEDS:
				{
					uint8_t u8_ledValueFromCAN = msg.au8_Contents[2];
					if (u8_arrayIndex == MY_ID) {
						printf("led from can: %d\n", u8_ledValueFromCAN);
						fcn_setLEDValue(u8_ledValueFromCAN);
						gb_advertiseFcnSynth = TRUE;
					} else {
						ast_otherBoardFcnSynthParams[u8_arrayIndex].u8_ledValue = u8_ledValueFromCAN;
					}
					break;
				}
				default: break;
			}
			ESOS_TASK_YIELD();
		}
	}
	ESOS_TASK_WAIT_TICKS(50);
	}
	ESOS_TASK_END();
}

void configCanFcnSynth(void) {
	esos_RegisterTask(FcnSynthAdvertiser);
	esos_RegisterTask(OtherBoardFcnSynthListener);
}