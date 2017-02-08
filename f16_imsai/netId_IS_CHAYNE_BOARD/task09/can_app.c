//Include
#include "can_app.h"
#include "t6_app_austin.h"
#include "can_menu.h"
#include "CANTeamMembers.h"
#include "MyCanStats.h"
#include "can_temp.h"
#include "can_fcn_synth.h"

#define BROADCAST_TIMER_RATE (10000)
#define REMOVE_INACTIVE_USER_TIMER_RATE (90000)
#define TEAM_ID (2)
#define MEMBER_ID (2)
#define START_CONVERSION_COMMAND (0x51)
#define READ_TEMPERATURE_COMMAND (0xAA)
#define STOP_CONVERSION_COMMAND (0x22)

static uint32_t au32_ID_Time_LastHeard[10];
static uint16_t u16_global_adc_value;
static uint8_t gu8_celsius;

static BOOL gb_getTemperature = FALSE;

void initIdLastHeardvalues() {
	uint8_t u8_i;
	for(u8_i = 0; u8_i < NUM_OF_IDS; ++u8_i) {
		au32_ID_Time_LastHeard[u8_i] = 0;
	}
}

ESOS_USER_TASK(BroadcastID){
	static uint8_t buf[0] = {};

	ESOS_TASK_BEGIN();
	while (TRUE){
		ESOS_ECAN_SEND(calcMsgID(MY_ID) | CANMSG_TYPE_BEACON, buf, 0);
		ESOS_TASK_WAIT_TICKS(BROADCAST_TIMER_RATE);
	}
	ESOS_TASK_END();
}


ESOS_USER_TASK(ListenForBeacons){
	static uint8_t buf[3] = {0};
    static uint8_t u8_len;
    static uint16_t u16_can_id;
	static uint32_t u32_lastTime = 0;
	static uint32_t u32_currentTime = 0;
	uint8_t i,j;
	
	
	ESOS_TASK_BEGIN();
	esos_ecan_canfactory_subscribe(__pstSelf,CANMSG_TYPE_BEACON,CANMSG_TYPE_BEACON,MASKCONTROL_FIELD_NONZERO);
	while (TRUE){
		static MAILMESSAGE msg;
		
		if (ESOS_TASK_IVE_GOT_MAIL()) {
			ESOS_TASK_GET_NEXT_MESSAGE( &msg );
	        u16_can_id = msg.au16_Contents[0];
			
			//extract the team and member ID from the message
			uint16_t u16_teamID = stripTeamID(u16_can_id);
			uint16_t u16_memberID = stripMemberID(u16_can_id);
			uint8_t u8_arrayIndex = getArrayIndexFromTeamAndMemberIDs((uint8_t)u16_teamID, (uint8_t)u16_memberID);
			if (u8_arrayIndex != MY_ID) {
							
				au32_ID_Time_LastHeard[u8_arrayIndex] = 0;
				
				//add to menu
				addUserToMenu(u16_teamID,u16_memberID);
			}
		}

		uint8_t u8_i = 0;
		for (; u8_i < NUM_OF_IDS; ++u8_i) {
			if (u8_i == MY_ID)
				continue;
			au32_ID_Time_LastHeard[u8_i] += (u32_currentTime - u32_lastTime);
			if (au32_ID_Time_LastHeard[u8_i] > REMOVE_INACTIVE_USER_TIMER_RATE){
				uint8_t u8_teamID = aCANID_IDs[u8_i].u8_teamID;
				uint8_t u8_memberID = aCANID_IDs[u8_i].u8_memberID;
				removeUserFromMenu(u8_teamID, u8_memberID);
			}
		}

		u32_lastTime = esos_GetSystemTick();
		ESOS_TASK_WAIT_TICKS(100);
		u32_currentTime = esos_GetSystemTick();
		
	}
	ESOS_TASK_END();
}

//respond to a request for your temperature value
ESOS_USER_TASK(ListenForTempRequest){
	static uint8_t buf[3] = {0};
	static uint8_t u8_len;
    static uint16_t u16_can_id;
	static uint16_t u16_id = 0;
	u16_id = calcMsgID(MY_ID) | CANMSG_TYPE_TEMPERATURE1;
	
	ESOS_TASK_BEGIN();
	esos_ecan_canfactory_subscribe(__pstSelf,u16_id,u16_id,MASKCONTROL_EQUAL);
	while (TRUE){
		static MAILMESSAGE msg;
		
		//receive the request
		ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE( &msg );
		
		// send
		uint8_t adcBuf[2];
		adcBuf[0] = (uint8_t) u16_global_adc_value;
		adcBuf[1] = (uint8_t) (u16_global_adc_value >> 8);
		ESOS_ECAN_SEND(u16_id,adcBuf,2);
		
	}
	
	ESOS_TASK_END();
}

//Read your own temp sensor and update the global var
ESOS_USER_TASK(UpdateMyTemp){
	static uint8_t buf[4];
	static uint16_t u16_data;
	static BOOL b_readFromLM60 = FALSE;
	static uint8_t u8_counter = 0;
	static uint8_t u8_address = 0x90;
	static uint8_t u8_readData = 0;

	static uint16_t u16_id = 0;
	u16_id = calcMsgID(MY_ID) | CANMSG_TYPE_TEMPERATURE1;
	
	ESOS_TASK_BEGIN();

	__PIC24_I2C1_START();
	__PIC24_I2C1_PUT(I2C_WADDR(u8_address));
	__PIC24_I2C1_PUT(0xAC);
	__PIC24_I2C1_PUT(0b00000001);
	__PIC24_I2C1_STOP();

	while (TRUE){
		if (b_readFromLM60)
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(TEMP_SENSOR_CHANNEL, VREF);
			ESOS_TASK_WAIT_SENSOR_READ(u16_data, ESOS_SENSOR_ONE_SHOT, ESOS_SENSOR_FORMAT_VOLTAGE);
	  		ESOS_SENSOR_CLOSE();
			
			u16_global_adc_value = u16_data;
		}
		else
		{
			//do operations to read from 1631
			__PIC24_I2C1_START();
			__PIC24_I2C1_PUT(I2C_WADDR(u8_address));
			__PIC24_I2C1_PUT(START_CONVERSION_COMMAND);
			__PIC24_I2C1_STOP();
			ESOS_TASK_WAIT_TICKS(1);
			__PIC24_I2C1_START();
			__PIC24_I2C1_PUT(I2C_WADDR(u8_address));
			__PIC24_I2C1_PUT(READ_TEMPERATURE_COMMAND);
			__PIC24_I2C1_RSTART();
			__PIC24_I2C1_PUT(I2C_RADDR(u8_address));
			uint8_t u8_i = 0;
			static uint8_t u8_cel;
			static uint16_t u16_x = 0;
			for (u8_i = 0; u8_i < 1; u8_i++)
			{
				ESOS_TASK_WAIT_ON_GETI2C1(&u8_readData, I2C_ACK);
				u8_cel = u8_readData;
				u16_x = ((uint16_t)u8_readData) << 8;
			}
			ESOS_TASK_WAIT_ON_GETI2C1(&u8_readData, I2C_NAK);
			u16_x += u8_readData;
			__PIC24_I2C1_STOP();
			if (u8_cel < 80) {
				printf("temp: %d\n, temp hex: 0x%04x", u8_cel, u16_x);
				u16_global_adc_value = (((uint16_t)u8_cel) * TEMP_SENSOR_TENTH_MV_PER_DEGREES_C) + TEMP_SENSOR_TENTH_MV_OFFSET;
			}
		}
		if (++u8_counter > 60)
		{
			b_readFromLM60 = !b_readFromLM60;
			u8_counter = 0;
		}
		ESOS_TASK_WAIT_TICKS(500); //wait 5 seconds before alternating to the other temp reader.
	}
	ESOS_TASK_END();
}

//determine if the menu s requesting a read from your temp or someone else's
ESOS_USER_TASK(PickTempSensor){
	static uint16_t u16_temp;
	static uint16_t u16_id;
	static uint8_t u8_firstChar; 
  	static uint8_t u8_secondChar;
	static uint8_t u8_celsius;
	static uint8_t u8_len;
	uint16_t u16_bufMSB;
	uint16_t u16_bufLSB;
	
	uint8_t buf[0];
	ESOS_TASK_BEGIN();
	while (TRUE){
		static MAILMESSAGE msg;
		ESOS_TASK_WAIT_UNTIL(gb_getTemperature);
		if (getTargetBoard() == MY_ID) {
			// read the lm60 and store in gu8_celsius
			u16_temp = u16_global_adc_value;
			
		} else {
			// set it to the temperature from target board
			u16_id = calcMsgID(getTargetBoard()) | CANMSG_TYPE_TEMPERATURE1;
			esos_ecan_canfactory_subscribe(__pstSelf,u16_id,u16_id,MASKCONTROL_EQUAL);
			ESOS_ECAN_SEND(u16_id,buf,0);
			ESOS_TASK_WAIT_UNTIL(ESOS_TASK_IVE_GOT_MAIL() || !gb_getTemperature);
			if (gb_getTemperature) {
				ESOS_TASK_GET_NEXT_MESSAGE( &msg );
			
				u16_temp = msg.au16_Contents[0];
				u8_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof( uint16_t );
				memcpy( buf, &(msg.au8_Contents[ sizeof( uint16_t ) ]), u8_len );
				u16_bufLSB = buf[0];
				u16_bufMSB = (buf[1]<<8);
				u16_temp = u16_bufMSB+u16_bufLSB; 
			}
		}
		//convert the input from the temperature sensor to celisus in 8bits.
		if (gb_getTemperature) {
			u8_celsius = (uint8_t)(convertMvToDegreesC(u16_temp));
			gu8_celsius = u8_celsius;
		}

		ESOS_TASK_WAIT_TICKS(200);
	}
	
	ESOS_TASK_END();
	
}

uint8_t getCeliusReadingOfTargetBoard() {
	return gu8_celsius;
}

void stopGettingTemp() {
	gb_getTemperature = FALSE;
}

void startGettingTemp() {
	gb_getTemperature = TRUE;
}



void user_init() {
	configCanFcnSynth();

	__esos_ecan_hw_config_ecan();
	CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);
	configCanMenu();
	esos_RegisterTask( CANFactory);
	esos_RegisterTask(BroadcastID);
	esos_RegisterTask(ListenForBeacons);
	esos_RegisterTask(ListenForTempRequest);
	esos_RegisterTask(UpdateMyTemp);
	esos_RegisterTask(PickTempSensor);

}
