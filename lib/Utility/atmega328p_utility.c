#include <stdint.h>
#include "atmega328p_utility.h"
#include "atmega328p_TIM.h"

void my_delay(void){
	// Note: int is 16 bits on ATMEGA328P (8-bit mcu)
	// Clock freq: 16 MHz (via external oscillator on Arduino UNO)
	// time of 1 clock cycle  =  1/16M = 62.5 ns
	for(volatile uint16_t i = 0; i<65535;i++);
}

// TODO: Implement precise delay API
//void my_precise_delay(uint16_t timeInMilli){}


// Assumes vIn to be in the range of [0,5] volts
uint8_t floatToString(char* data, uint8_t len, float vIn){
    
	if((vIn > 5.0f)||(vIn < 0.0f))
		return 1;
		
	uint16_t temp = vIn * 1000;    // since the input voltage is between 0V-5V, we have decimals.  Preserve to 3 significant digits
	// temp should be between 0 and 5000.  We start with a divisor of 1000 because we want to extract values left to right
	uint16_t divisor = 1000; 
	uint16_t digit = 0;
	for (uint8_t i = 0; i < len - 1; i++){
		if(i == 1){
			*data++ = '.';
			continue;
		}else{
		    if(divisor > 0){
		    digit = temp / divisor;  // extract leftmost digit
			*data++= (char)(digit+48);  
            temp = temp - digit*divisor;  // update temp without the extracted digit
            divisor /= 10;    // update divisor 
		    }
		}
	}
	*data = '\0';
	return 0;
}


