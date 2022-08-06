#ifndef DHT11_H
#define DHT11_H

#include "Atmega328p_TIM.h"

typedef struct{
	uint8_t humidity_decimal;
	uint8_t humidity_integral;
	uint8_t temperature_decimal;
	uint8_t temperature_integral;
	uint8_t checksum;
    uint8_t dht11_error;
	//bool last_data_valid;
}dht_data_t;

typedef struct{
    dht_data_t data;
}dht_handle_t;

void dht11_init(void);
void dht11_readData(dht_handle_t * h_dht11);

#endif