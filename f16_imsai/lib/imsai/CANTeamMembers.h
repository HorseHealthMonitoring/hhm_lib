#ifndef EMBEDDED_LAB_H
#define	EMBEDDED_LAB_H

#include <string.h>
#include <stdint.h>

typedef struct
{
    char *psz_name;
    char *psz_netID;
    uint8_t u8_teamID;
    uint8_t u8_memberID;
} CAN_ID;

#define NUM_OF_IDS 10 // 8 Students + 1 TA + 1 Professor

#define teamIDMask   0b0000011100000000   // eight teams
#define memberIDMask 0b0000000011100000   // eight members
#define typeIDMask   0b0000000000011111   // 32 possible MSGs
#define TEAM_ID_SHIFT_AMOUNT   8
#define MEMBER_ID_SHIFT_AMOUNT 5
#define TYPE_ID_SHIFT_AMOUNT   0

// Message Types
#define CANMSG_TYPE_POLL           0
#define CANMSG_TYPE_BEACON         1
#define CANMSG_TYPE_TEMPERATURE1   2
#define CANMSG_TYPE_TEMPERATURE2   3
#define CANMSG_TYPE_WAVEFORM       4
#define CANMSG_TYPE_POTENTIOMETER  5
#define CANMSG_TYPE_FREQUENCY      6
#define CANMSG_TYPE_AMPLITUDE      7
#define CANMSG_TYPE_DUTYCYCLE      8
#define CANMSG_TYPE_LEDS           9

#define calcMsgID(u8_arrayIndex) (aCANID_IDs[u8_arrayIndex].u8_teamID << TEAM_ID_SHIFT_AMOUNT) | (aCANID_IDs[u8_arrayIndex].u8_memberID << MEMBER_ID_SHIFT_AMOUNT)

// CAN ID Table
extern const CAN_ID aCANID_IDs[NUM_OF_IDS];

inline uint8_t stripTeamID(uint16_t u16_MSG_ID);

inline uint8_t stripMemberID(uint16_t u16_MSG_ID);

inline uint8_t stripTypeID(uint16_t u16_MSG_ID);

int8_t getArrayIndexFromMsgID(uint16_t u16_MSG_ID);

uint8_t getArrayIndexFromTeamAndMemberIDs(uint8_t u8_teamID, uint8_t u8_memberID);

int8_t getNameFromMsgID(uint16_t u16_MSG_ID, char *psz_name, uint8_t u8_len);

#endif	/* EMBEDDED_LAB_H */
