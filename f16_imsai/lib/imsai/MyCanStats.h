#ifndef MYCANSTATS_H_
#define MYCANSTATS_H_

#include "CANTeamMembers.h"

#ifdef IS_JORDAN_BOARD
#define MY_ID             0 // Look above for your array index
#elif IS_TYLER_BOARD
#define MY_ID             1
#elif IS_EMILY_BOARD
#define MY_ID             2
#elif IS_WILL_BOARD
#define MY_ID             3
#elif IS_CALEB_BOARD
#define MY_ID             4
#elif IS_AUSTIN_BOARD
#define MY_ID             5
#elif IS_BRIAN_BOARD
#define MY_ID             6
#elif IS_CHAYNE_BOARD
#define MY_ID             7
#elif IS_DREW_BOARD
#define MY_ID             8
#elif IS_BRUCE_BOARD
#define MY_ID             9
#else 
#define MY_ID 			  0
#endif


#define MY_NAME           (aCANID_IDs[MY_ID].psz_name)
#define MY_NETID          (aCANID_IDs[MY_ID].psz_netID)
#define MY_TEAM_ID        (aCANID_IDs[MY_ID].u8_teamID)
#define MY_MEMBER_ID      (aCANID_IDs[MY_ID].u8_memberID)
#define MY_MSG_ID(type)   ((MY_TEAM_ID << TEAM_ID_SHIFT_AMOUNT) | (MY_MEMBER_ID << MEMBER_ID_SHIFT_AMOUNT) | (type & typeIDMask))

#endif //MYCANSTATS_H_