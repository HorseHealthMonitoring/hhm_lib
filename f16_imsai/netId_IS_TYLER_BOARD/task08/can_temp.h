#ifndef CAN_TEMP_H_
#define CAN_TEMP_H_
#include "esos.h"

uint8_t getCeliusReadingOfTargetBoard();
void setTargetBoard(uint8_t);
void stopGettingTemp();

#endif //CAN_TEMP_H_