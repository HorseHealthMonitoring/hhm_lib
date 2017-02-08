#include "t6_app_austin.h"

#define GET_UINT8_FROM_HEX_DIGIT(u8_hex) ((IS_DIGIT(u8_hex)) ? (u8_hex - '0') : (u8_hex - 'A' + 10))

#define IS_CARRIAGE_RETURN(u8_c) (u8_c == 0x0D)
#define IS_DIGIT(u8_c) (u8_c >= '0' && u8_c <= '9')
#define IS_HEX(u8_c) (IS_DIGIT(u8_c) || (u8_c >= 'A' && u8_c <= 'F'))
#define IS_START(u8_c) (u8_c == 'S')
#define IS_STOP(u8_c) (u8_c == 'P')
#define IS_SELECT_BUS(u8_c) (u8_c == 'G')
#define IS_DELAY(u8_c) (u8_c == 'L')
#define IS_ECHO(u8_c) (u8_c == 'T')
#define IS_WRITE(u8_c) (u8_c == 'W')
#define IS_READ(u8_c) (u8_c == 'R')
#define IS_SELECT_SPEED(u8_c) (u8_c == 'J')
#define IS_SEND_NACK(u8_c) (u8_c == 'N')
#define IS_WAIT_FOR_EOL(u8_c) (u8_c == ':')

#define DELAY_INPUT_LEN (4)
#define MAX_COMMANDS_DURING_WAIT_FOR_EOL (128)

#define IMSAIREADNI2C1(u8_readData, u16_cnt) do { \
		ESOS_ALLOCATE_CHILD_TASK(th_childHandle); \
    	ESOS_TASK_SPAWN_AND_WAIT(th_childHandle, imsaiReadNI2C1, &u8_readData, u16_cnt); \
	} while(0)

#define WRITE_I2C_DATA(u8_address, u8_data) do { \
	__PIC24_I2C1_PUT(I2C_WADDR(u8_address)); \
	__PIC24_I2C1_PUT(u8_data); \
	} while(0)

BOOL isReadAddress(uint8_t u8_x) {
  uint8_t u8_data;
  u8_data = u8_x & 0x01;
  if (u8_data == 1) return(TRUE);
  else return(FALSE);
}

static const uint8_t au8_checkAgainst[2] = {'1', '4'};

typedef enum
{
	idle,
	waitForAddress,
	waitForWrite,
	waitForRead,
	delay,
	echo,
	waitForTimeSensitive,
	waitForSelectBus,
	waitForSetSpeed
} e_state;

/*
Task Name: getUint8FromHexString
Purpose: This function receives a Hex Character from the serial port and converts it to uint8 base information and returns it.
*/
uint8_t getUint8FromHexString(const uint8_t* pu8_hex)
{
	uint8_t u8_top = GET_UINT8_FROM_HEX_DIGIT(pu8_hex[0]);
	uint8_t u8_bottom = GET_UINT8_FROM_HEX_DIGIT(pu8_hex[1]);
	return (u8_top << 4) + u8_bottom;
}

/*
Task Name: getUint8FromHexStringTwo
Purpose: This function receives a Hex Character from the serial port and converts it to uint8 base information and returns it.
Intended Operation: Does the same thing as getUint8FromHexString but just works on the third and fourth part of the array.
*/
uint8_t getUint8FromHexStringTwo(const uint8_t* pu8_hex)
{
	uint8_t u8_top = GET_UINT8_FROM_HEX_DIGIT(pu8_hex[2]);
	uint8_t u8_bottom = GET_UINT8_FROM_HEX_DIGIT(pu8_hex[3]);
	return (u8_top << 4) + u8_bottom;
}

/*
Task Name: getHexStringFromUint8
Purpose: This function receives a uint8 and converts it to an ASCII string.
*/
void getHexStringFromUint8(uint8_t u8_data, uint8_t* pu8_hex)
{
	uint8_t u8_top = u8_data >> 4;
	uint8_t u8_bottom = u8_data & 0x0F;
	pu8_hex[0] = (u8_top > 9) ? ((u8_top - 10) + 'A') : (u8_top + '0');
	pu8_hex[1] = (u8_bottom > 9) ? ((u8_bottom - 10) + 'A') : (u8_bottom + '0');
}

/*
Task Name: i2c_service
Purpose: This task is a state machine that allows the user start an i2c device, write and read, stop the i2c device, and delay the
operations of the machine as well as echo what the user inputs.
*/
ESOS_USER_TASK(i2c_service)
{
	static e_state currentState = idle;
	static e_state prevState = idle;
	static uint8_t u8_hexCount = 0;
	static uint8_t au8_hexString[4] = {0};
	static uint8_t u8_address = 0;
	static uint8_t u8_data = 0;
	static uint8_t u8_writeData = 0;
	static uint8_t u8_readData = 0;
	static uint8_t u8_waitForEOLBufLen = 0;
	static uint8_t u8_waitForEOLBufIndex = 0;
	static uint8_t au8_bufferData[128] = {0};
	static uint8_t au8_readHexOutput[4] = {0};
	static uint8_t u8_first = 0;
	static uint8_t u8_second = 0;
	static uint16_t u16_delayData = 0;
	static uint8_t au8_checkData[2] = {0};
	static uint8_t u8_checkDataIndex = 0;
	static uint16_t u16_fKHz = 0;

	ESOS_TASK_BEGIN();

	while (TRUE)
	{
		//Idle state: In this state the user can start the i2c service, echo, delay, conduct a wait to EOL input sequence
		//select the bus as well as select the speed of the bus.
		if (currentState == idle)
		{
			u8_address = 0;
			u8_data = 0;
			u8_writeData = 0;
			u8_readData = 0;

			if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
			{
				//assigining u8_data to what's in the buffer
				//deleting the entry afterwards and restructuring the buffer.
				u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
				if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
				{
					u8_waitForEOLBufIndex = 0;
					u8_waitForEOLBufLen = 0;
				}
			}
			else
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			  	ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}

			if (IS_START(u8_data))
			{
				__PIC24_I2C1_START();
				prevState = currentState;
				currentState = waitForAddress;
			}
			else if (IS_WAIT_FOR_EOL(u8_data))
			{
				prevState = currentState;
				currentState = waitForTimeSensitive;
			}
			else if (IS_ECHO(u8_data))
			{
				prevState = currentState;
				currentState = echo;
			}
			else if (IS_DELAY(u8_data))
			{
				prevState = currentState;
				currentState = delay;
			}
			else if (IS_SELECT_BUS(u8_data))
			{
				prevState = currentState;
				currentState = waitForSelectBus;
			}
			else if (IS_SEND_NACK(u8_data))
			{
				I2C1CONbits.ACKDT = I2C_NAK;
			}
			else if (IS_SELECT_SPEED(u8_data))
			{
				au8_checkData[0] = 0;
				au8_checkData[1] = 0;
				while (!((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1])))
				{
					if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
					{
					//assigining u8_data to what's in the buffer
					//deleting the entry afterwards and restructuring the buffer.
						u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
						if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
						{
						u8_waitForEOLBufIndex = 0;
						u8_waitForEOLBufLen = 0;
						}
					}
					else
					{
						ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				  		ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
				    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
					}
					au8_checkData[u8_checkDataIndex++] = u8_data;
					if ((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1]))
					{
						u8_checkDataIndex = 0;
						prevState = currentState;
						currentState = waitForSetSpeed;
					}
					else if (u8_checkDataIndex > 1)
					{
						u8_checkDataIndex = 0;
						break;
					}
				}
			}
			else
			{
				prevState = currentState;
				currentState = idle;
			}
		}
		
		//Wait For Address: waits for the user to input a certain address and diverts
		//The user to the read or write state.
		else if (currentState == waitForAddress)
		{
			if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
			{
				//assigining u8_data to what's in the buffer
				//deleting the entry afterwards and restructuring the buffer.
				u8_address = au8_bufferData[u8_waitForEOLBufIndex++];
				if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
				{
					u8_waitForEOLBufIndex = 0;
					u8_waitForEOLBufLen = 0;
				}
			}
			else
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			  	ESOS_TASK_WAIT_ON_GET_UINT8(u8_address);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}

			if (IS_START(u8_address))
			{
				__PIC24_I2C1_RSTART();
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForAddress;
			}
			else if (IS_STOP(u8_address))
			{
				__PIC24_I2C1_STOP();
				u8_hexCount = 0;
				prevState = currentState;
				currentState = idle;
			}
			else if (IS_WAIT_FOR_EOL(u8_address))
			{
				prevState = currentState;
				currentState = waitForTimeSensitive;
				u8_hexCount = 0;
			}
			else if (IS_ECHO(u8_address))
			{
				prevState = currentState;
				currentState = echo;
				u8_hexCount = 0;
			}
			else if (IS_DELAY(u8_address))
			{
				prevState = currentState;
				currentState = delay;
				u8_hexCount = 0;
			}
			else if (IS_SELECT_BUS(u8_address))
			{
				prevState = currentState;
				currentState = waitForSelectBus;
				u8_hexCount = 0;
			}
			else if (IS_SEND_NACK(u8_address))
			{
				I2C1CONbits.ACKDT = I2C_NAK;
			}
			else if (IS_SELECT_SPEED(u8_address))
			{
				au8_checkData[0] = 0;
				au8_checkData[1] = 0;
				while (!((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1])))
				{
					if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
					{
					//assigining u8_data to what's in the buffer
					//deleting the entry afterwards and restructuring the buffer.
						u8_address = au8_bufferData[u8_waitForEOLBufIndex++];
						if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
						{
						u8_waitForEOLBufIndex = 0;
						u8_waitForEOLBufLen = 0;
						}
					}
					else
					{
						ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				  		ESOS_TASK_WAIT_ON_GET_UINT8(u8_address);
				    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
					}
					au8_checkData[u8_checkDataIndex++] = u8_address;
					if ((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1]))
					{
						u8_checkDataIndex = 0;
						prevState = currentState;
						currentState = waitForSetSpeed;
					}
					else if (u8_checkDataIndex > 1)
					{
						u8_checkDataIndex = 0;
						break;
					}
				}
			}
			else if (IS_HEX(u8_address))
			{
				au8_hexString[u8_hexCount++] = u8_address;
				if (u8_hexCount == 2)
				{
					u8_address = getUint8FromHexString(au8_hexString);
					u8_hexCount = 0;
					currentState = (isReadAddress(u8_address)) ? waitForRead : waitForWrite;
					if (currentState == waitForWrite)
					{
						__PIC24_I2C1_PUT(I2C_WADDR(u8_address));
					}
					else
					{
						__PIC24_I2C1_PUT(I2C_RADDR(u8_address));
					}
				}
				else
				{
					prevState = currentState;
					currentState = waitForAddress;
				}
			}
		}
		
		//Wait For Write: state where the user continually writes until entering another i2c device command.
		else if (currentState == waitForWrite)
		{
			if (isReadAddress(u8_address))
			{
				u8_address = u8_address - 1;
			}
			else
			{
				u8_address = u8_address;
			}

			if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
			{
				//assigining u8_data to what's in the buffer
				//deleting the entry afterwards and restructuring the buffer.
				u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
				if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
				{
					u8_waitForEOLBufIndex = 0;
					u8_waitForEOLBufLen = 0;
				}
			}
			else
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			  	ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}

			if (IS_START(u8_data))
			{
				__PIC24_I2C1_RSTART();
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForAddress;
			}
			else if (IS_STOP(u8_data))
			{
				__PIC24_I2C1_STOP();
				prevState = currentState;
				currentState = idle;
				u8_hexCount = 0;
			}
			else if (IS_READ(u8_data))
			{
				__PIC24_I2C1_RSTART();
				__PIC24_I2C1_PUT(I2C_RADDR(u8_address));
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForRead;
			}
			else if (IS_WAIT_FOR_EOL(u8_data))
			{
				prevState = currentState;
				currentState = waitForTimeSensitive;
				u8_hexCount = 0;
			}
			else if (IS_DELAY(u8_data))
			{
				prevState = currentState;
				currentState = delay;
				u8_hexCount = 0;
			}
			else if (IS_ECHO(u8_data))
			{
				prevState = currentState;
				currentState = echo;
				u8_hexCount = 0;
			}
			else if (IS_SELECT_BUS(u8_data))
			{
				prevState = currentState;
				currentState = waitForSelectBus;
				u8_hexCount = 0;
			}
			else if (IS_SEND_NACK(u8_data))
			{
				I2C1CONbits.ACKDT = I2C_NAK;
			}
			else if (IS_SELECT_SPEED(u8_data))
			{
				au8_checkData[0] = 0;
				au8_checkData[1] = 0;
				while (!((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1])))
				{
					if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
					{
					//assigining u8_data to what's in the buffer
					//deleting the entry afterwards and restructuring the buffer.
						u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
						if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
						{
						u8_waitForEOLBufIndex = 0;
						u8_waitForEOLBufLen = 0;
						}
					}
					else
					{
						ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				  		ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
				    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
					}
					au8_checkData[u8_checkDataIndex++] = u8_data;
					if ((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1]))
					{
						u8_checkDataIndex = 0;
						prevState = currentState;
						currentState = waitForSetSpeed;
					}
					else if (u8_checkDataIndex > 1)
					{
						u8_checkDataIndex = 0;
						break;
					}
				}
			}
			else if (IS_HEX(u8_data))
			{
				au8_hexString[u8_hexCount++] = u8_data;
				if (u8_hexCount == 2)
				{
					u8_data = getUint8FromHexString(au8_hexString);
					__PIC24_I2C1_PUT(u8_data);
					u8_hexCount = 0;
					prevState = currentState;
					currentState = waitForWrite;
				}
				else
				{
					prevState = currentState;
					currentState = waitForWrite;
				}
			}
		}
	
		//Wait For Read: Waits until the user inputs how many bytes they wish to read.
		else if (currentState == waitForRead)
		{
			if (!isReadAddress(u8_address))
			{
				u8_address = u8_address + 1;
			}
			else
			{
				u8_address = u8_address;
			}

			if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
			{
				//assigining u8_data to what's in the buffer
				//deleting the entry afterwards and restructuring the buffer.
				u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
				if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
				{
					u8_waitForEOLBufIndex = 0;
					u8_waitForEOLBufLen = 0;
				}
			}
			else
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			  	ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}

			if (IS_START(u8_data))
			{
				__PIC24_I2C1_RSTART();
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForAddress;
			}
			else if (IS_STOP(u8_data))
			{
				__PIC24_I2C1_STOP();
				prevState = currentState;
				currentState = idle;
				u8_hexCount = 0;
			}
			else if (IS_WRITE(u8_data))
			{
				__PIC24_I2C1_RSTART();
				__PIC24_I2C1_PUT(I2C_WADDR(u8_address));
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForWrite;
			}
			else if (IS_WAIT_FOR_EOL(u8_data))
			{
				prevState = currentState;
				currentState = waitForTimeSensitive;
				u8_hexCount = 0;
			}
			else if (IS_DELAY(u8_data))
			{
				prevState = currentState;
				currentState = delay;
				u8_hexCount = 0;
			}
			else if (IS_SELECT_BUS(u8_data))
			{
				prevState = currentState;
				currentState = waitForSelectBus;
				u8_hexCount = 0;
			}
			else if (IS_ECHO(u8_data))
			{
				prevState = currentState;
				currentState = echo;
				u8_hexCount = 0;
			}
			else if (IS_SEND_NACK(u8_data))
			{
				I2C1CONbits.ACKDT = I2C_NAK;
			}
			else if (IS_SELECT_SPEED(u8_data))
			{
				au8_checkData[0] = 0;
				au8_checkData[1] = 0;
				while (!((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1])))
				{
					if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
					{
					//assigining u8_data to what's in the buffer
					//deleting the entry afterwards and restructuring the buffer.
						u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
						if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
						{
						u8_waitForEOLBufIndex = 0;
						u8_waitForEOLBufLen = 0;
						}
					}
					else
					{
						ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				  		ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
				    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
					}
					au8_checkData[u8_checkDataIndex++] = u8_data;
					if ((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1]))
					{
						u8_checkDataIndex = 0;
						prevState = currentState;
						currentState = waitForSetSpeed;
					}
					else if (u8_checkDataIndex > 1)
					{
						u8_checkDataIndex = 0;
						break;
					}
				}
			}
			else if (IS_HEX(u8_data))
			{
				au8_hexString[u8_hexCount++] = u8_data;
				if (u8_hexCount == 2)
				{
					u8_data = getUint8FromHexString(au8_hexString);
					static uint8_t u8_i;
					for(u8_i = 0; u8_i < (u8_data - 1); ++u8_i) // read w/ ack until next to last byte
					{
						ESOS_TASK_WAIT_ON_GETI2C1(&u8_readData, I2C_ACK);
						ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
						getHexStringFromUint8(u8_readData, au8_readHexOutput);
						ESOS_TASK_WAIT_ON_SEND_UINT8(au8_readHexOutput[0]);
						ESOS_TASK_WAIT_ON_SEND_UINT8(au8_readHexOutput[1]);
						ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
					}
					ESOS_TASK_WAIT_ON_GETI2C1(&u8_readData, I2C_NAK); // get the last byte and signal nack
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					getHexStringFromUint8(u8_readData, au8_readHexOutput);
					ESOS_TASK_WAIT_ON_SEND_UINT8(au8_readHexOutput[0]);
					ESOS_TASK_WAIT_ON_SEND_UINT8(au8_readHexOutput[1]);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
					u8_hexCount = 0;
					prevState = currentState;
					currentState = waitForRead;
				}
				else
				{
					prevState = currentState;
					currentState = waitForRead;
				}
			}
		}
		
		//Delay: Allows the user to input how long they wish to delay the operations of the device.
		else if (currentState == delay)
		{
			//expecting 16 bits so I read here real quick to see if the user wants to change states
			if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
			{
				//assigining u8_data to what's in the buffer
				//deleting the entry afterwards and restructuring the buffer.
				u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
				if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
				{
					u8_waitForEOLBufIndex = 0;
					u8_waitForEOLBufLen = 0;
				}
			}
			else
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			  	ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}
			if (IS_START(u8_data))
			{
				__PIC24_I2C1_RSTART();
				prevState = currentState;
				currentState = waitForAddress;
				u8_hexCount = 0;
			}
			else if (IS_STOP(u8_data))
			{
				__PIC24_I2C1_STOP();
				prevState = currentState;
				currentState = idle;
				u8_hexCount = 0;
			}
			else if (IS_WRITE(u8_data))
			{
				__PIC24_I2C1_RSTART();
				__PIC24_I2C1_PUT(I2C_WADDR(u8_address));
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForWrite;
			}
			else if (IS_READ(u8_data))
			{
				__PIC24_I2C1_RSTART();
				__PIC24_I2C1_PUT(I2C_RADDR(u8_address));
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForRead;
			}
			else if (IS_WAIT_FOR_EOL(u8_data))
			{
				prevState = currentState;
				currentState = waitForTimeSensitive;
				u8_hexCount = 0;
			}
			else if (IS_ECHO(u8_data))
			{
				prevState = currentState;
				currentState = echo;
				u8_hexCount = 0;
			}
			else if (IS_SELECT_BUS(u8_data))
			{
				prevState = currentState;
				currentState = waitForSelectBus;
				u8_hexCount = 0;
			}
			else if (IS_SEND_NACK(u8_data))
			{
				I2C1CONbits.ACKDT = I2C_NAK;
			}
			else if (IS_SELECT_SPEED(u8_data))
			{
				au8_checkData[0] = 0;
				au8_checkData[1] = 0;
				while (!((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1])))
				{
					if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
					{
					//assigining u8_data to what's in the buffer
					//deleting the entry afterwards and restructuring the buffer.
						u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
						if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
						{
						u8_waitForEOLBufIndex = 0;
						u8_waitForEOLBufLen = 0;
						}
					}
					else
					{
						ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				  		ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
				    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
					}
					au8_checkData[u8_checkDataIndex++] = u8_data;
					if ((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1]))
					{
						u8_checkDataIndex = 0;
						prevState = currentState;
						currentState = waitForSetSpeed;
					}
					else if (u8_checkDataIndex > 1)
					{
						u8_checkDataIndex = 0;
						break;
					}
				}
			}
			else if (IS_HEX(u8_data))
			{
				au8_hexString[u8_hexCount++] = u8_data;
				if (u8_hexCount == 4)
				{
					u8_first = getUint8FromHexString(au8_hexString);
					u8_second = getUint8FromHexStringTwo(au8_hexString);
					u16_delayData = u8_first;
					u16_delayData = u16_delayData << 8;
					u16_delayData = u16_delayData + u8_second;
					ESOS_TASK_WAIT_TICKS(u16_delayData);
					u8_hexCount = 0;
					prevState = currentState;
					currentState = delay;
				}
				else
				{
					prevState = currentState;
					currentState = delay;
				}
			}
		}
		
		//Echo: Echos back over the serial port what the user enters.
		else if (currentState == echo)
		{
			//im expecting the user to send 2 bytes after sending the echo command
			if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
			{
				//assigining u8_data to what's in the buffer
				//deleting the entry afterwards and restructuring the buffer.
				u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
				if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
				{
					u8_waitForEOLBufIndex = 0;
					u8_waitForEOLBufLen = 0;
				}
			}
			else
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			  	ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}
			//Do i need to check if they want to change states or should I just force an echo?
			if (IS_START(u8_data))
			{
				__PIC24_I2C1_RSTART();
				prevState = currentState;
				currentState = waitForAddress;
				u8_hexCount = 0;
			}
			else if (IS_STOP(u8_data))
			{
				__PIC24_I2C1_STOP();
				prevState = currentState;
				currentState = idle;
				u8_hexCount = 0;
			}
			else if (IS_WRITE(u8_data))
			{	
				__PIC24_I2C1_RSTART();
				__PIC24_I2C1_PUT(I2C_WADDR(u8_address));
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForWrite;
			}
			else if (IS_READ(u8_data))
			{
				__PIC24_I2C1_RSTART();
				__PIC24_I2C1_PUT(I2C_RADDR(u8_address));
				u8_hexCount = 0;
				prevState = currentState;
				currentState = waitForRead;
			}
			else if (IS_WAIT_FOR_EOL(u8_data))
			{
				prevState = currentState;
				currentState = waitForTimeSensitive;
				u8_hexCount = 0;
			}
			else if (IS_DELAY(u8_data))
			{
				prevState = currentState;
				currentState = delay;
				u8_hexCount = 0;
			}
			else if (IS_SELECT_BUS(u8_data))
			{
				prevState = currentState;
				currentState = waitForSelectBus;
				u8_hexCount = 0;
			}
			else if (IS_SEND_NACK(u8_data))
			{
				I2C1CONbits.ACKDT = I2C_NAK;
			}
			else if (IS_SELECT_SPEED(u8_data))
			{
				au8_checkData[0] = 0;
				au8_checkData[1] = 0;
				while (!((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1])))
				{
					if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
					{
					//assigining u8_data to what's in the buffer
					//deleting the entry afterwards and restructuring the buffer.
						u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
						if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
						{
						u8_waitForEOLBufIndex = 0;
						u8_waitForEOLBufLen = 0;
						}
					}
					else
					{
						ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				  		ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
				    	ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
					}
					au8_checkData[u8_checkDataIndex++] = u8_data;
					if ((au8_checkData[0] == au8_checkAgainst[0]) && (au8_checkData[1] == au8_checkAgainst[1]))
					{
						u8_checkDataIndex = 0;
						prevState = currentState;
						currentState = waitForSetSpeed;
					}
					else if (u8_checkDataIndex > 1)
					{
						u8_checkDataIndex = 0;
						break;
					}
				}
			}
			else if (IS_HEX(u8_data))
			{
				au8_hexString[u8_hexCount++] = u8_data;
				if (u8_hexCount == 4)
				{
					u8_first = getUint8FromHexString(au8_hexString);
					u8_second = getUint8FromHexStringTwo(au8_hexString);
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_UINT8(u8_first);
					ESOS_TASK_WAIT_ON_SEND_UINT8(u8_second);
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
					u8_hexCount = 0;
					prevState = currentState;
					currentState = echo;
				}
				else
				{
					prevState = currentState;
					currentState = echo;
				}
			}
		}

		//Wait For Time Sensitive: Allows the user to initiate the time sensitive state
		//allowing the user to place commands in a buffer. Executes upon leaving the state.
		else if (currentState == waitForTimeSensitive)
		{
			do
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

				if (IS_CARRIAGE_RETURN(u8_data))
				{
					break;
				}

				au8_bufferData[u8_waitForEOLBufIndex++] = u8_data;
				u8_waitForEOLBufLen = u8_waitForEOLBufLen + 1;

			} while (u8_waitForEOLBufLen != MAX_COMMANDS_DURING_WAIT_FOR_EOL);

			u8_waitForEOLBufIndex = 0;
			currentState = prevState;
		}

		else if (currentState == waitForSelectBus)
		{
			//__PIC24_I2C1_STOP(); **Will be implemented when more buses are available**
			prevState = currentState;
			currentState = idle;
		}

		else if (currentState == waitForSetSpeed)
		{
			if (u8_waitForEOLBufIndex < u8_waitForEOLBufLen) //set buffer tracker
			{
				//assigining u8_data to what's in the buffer
				//deleting the entry afterwards and restructuring the buffer.
				u8_data = au8_bufferData[u8_waitForEOLBufIndex++];
				if (u8_waitForEOLBufIndex >= u8_waitForEOLBufLen)
				{
					u8_waitForEOLBufIndex = 0;
					u8_waitForEOLBufLen = 0;
				}
			}
			else
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			  	ESOS_TASK_WAIT_ON_GET_UINT8(u8_data);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}

			au8_hexString[u8_hexCount++] = u8_data;
			if (u8_hexCount == 2)
			{
				u8_first = getUint8FromHexString(au8_hexString);
				u16_fKHz = u8_first;
				__PIC24_I2C1_STOP();
				esos_pic24_configI2C1(u16_fKHz);
				u8_hexCount = 0;
				prevState = currentState;
				currentState = idle;
			}
			else
			{
				prevState = currentState;
				currentState = waitForSetSpeed;
			}
		}
	}
	ESOS_TASK_END();
}
