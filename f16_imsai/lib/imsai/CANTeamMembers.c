#include "CANTeamMembers.h"

const CAN_ID aCANID_IDs[NUM_OF_IDS] = {
	//"Name",         "netID",    teamID,    memberID,   // Array Index
	 {"JT Freeman",   "jtf177",   1,         1 },        // 0
	 {"TJ Gilmore",   "tjg120",   2,         1 },        // 1
	 {"EE Gorman",    "eeg87",    2,         2 },        // 2
	 {"WB Gray",      "wbj63",    1,         2 },        // 3
	 {"CN Hamill",    "cnh190",   1,         3 },        // 4
	 {"AJ Heath",     "ajh524",   2,         3 },        // 5
	 {"BC Joyner",    "bcj162",   1,         4 },        // 6
	 {"CM Thrash",    "ct446",    2,         4 },        // 7
	 {"RA Taylor",    "rat56",    7,         1 },        // 8
	 {"JW Bruce",     "jwbruce",  7,         7 }         // 9
};


/*inline uint16_t calcMsgID(uint8_t u8_arrayIndex)
{
    return (aCANID_IDs[u8_arrayIndex].u8_teamID << TEAM_ID_SHIFT_AMOUNT) | (aCANID_IDs[u8_arrayIndex].u8_memberID << MEMBER_ID_SHIFT_AMOUNT);
}*/

inline uint8_t stripTeamID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & teamIDMask)    >> TEAM_ID_SHIFT_AMOUNT;
}

inline uint8_t stripMemberID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & memberIDMask)  >> MEMBER_ID_SHIFT_AMOUNT;
}

inline uint8_t stripTypeID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & typeIDMask)    >> TYPE_ID_SHIFT_AMOUNT;
}

int8_t getArrayIndexFromMsgID(uint16_t u16_MSG_ID)
{
    int8_t i8_arrayIndex = -1; // Default - Not Found (-1)
    uint8_t u8_arrayIndex;
	
    for (u8_arrayIndex = 0; u8_arrayIndex < NUM_OF_IDS; u8_arrayIndex++)
	{
        if (calcMsgID(u8_arrayIndex) == (u16_MSG_ID & (teamIDMask | memberIDMask)))
		{
            i8_arrayIndex = u8_arrayIndex;
            break;
        }
    }
	
    return i8_arrayIndex;
}

uint8_t getArrayIndexFromTeamAndMemberIDs(uint8_t u8_teamID, uint8_t u8_memberID) {
    uint8_t u8_index;
    for(u8_index = 0; u8_index < NUM_OF_IDS; ++u8_index) {
        if ((aCANID_IDs[u8_index].u8_teamID == u8_teamID) &&  ((aCANID_IDs[u8_index].u8_memberID == u8_memberID))) {
            return u8_index;
        }
    }
    return 0;
}

int8_t getNameFromMsgID(uint16_t u16_MSG_ID, char *psz_name, uint8_t u8_len)
{
    int8_t i8_arrayIndex;
	
    i8_arrayIndex = getArrayIndexFromMsgID(u16_MSG_ID);
    if (i8_arrayIndex != -1) // If the Message ID was found
	{
        strncpy(psz_name, aCANID_IDs[i8_arrayIndex].psz_name, u8_len);
        return 0;
    }
	
    return -1;
}