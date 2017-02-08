#include "can_temp.h"
#include "esos.h"
#include "esos_ecan.h"
#include "CANTeamMembers.h"
#include "MyCanStats.h"

static uint8_t gu8_celsius;
static uint8_t gu8_boardId;

uint8_t getCeliusReadingOfTargetBoard() {
	return gu8_celsius;
}

void setTargetBoard(uint8_t u8_boardId) {
	if (u8_boardId < NUM_OF_IDS) {
		gu8_boardId = u8_boardId;
	}
}


ESOS_USER_TASK (readTemp) {
	ESOS_TASK_BEGIN();

	while(TRUE) {
		if (gu8_boardId == MY_ID) {
			// read the lm60 and store in gu8_celsius
		} else {
			// set it to the temperature from target board
		}
		ESOS_TASK_WAIT_TICKS(200);
	}

	ESOS_TASK_END();
}

