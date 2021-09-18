#ifndef ATMEGA328P_UTILITY_H
#define ATMEGA328P_UTILITY_H

#include <stdint.h>

uint8_t floatToString(char* data, uint8_t len, float vIn);
void my_delay(void);

#endif