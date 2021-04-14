// Blink LED Program
#include<stdint.h>

#define PORTD               (*((volatile uint8_t*)0x2b))
#define DDRD                (*((volatile uint8_t*)0x2a))
#define PIND                (*((volatile uint8_t*)0x29))
#define PIN5                ((uint8_t)5)

void my_delay(void){
	// Note: int is 16 bits on ATMEGA328P (8-bit mcu)
	// Clock freq: 16 MHz (via external oscillator on Arduino UNO)
	// time of 1 clock cycle  =  1/16M = 62.5 ns
	for(volatile uint32_t i = 0; i<65535;i++);
}

void pinConfig(uint8_t pinNum){
    DDRD |=  (1<<pinNum);
    PORTD |= (1<<pinNum);   
}

void togglePin(uint8_t pinNum){
		// Setting PIND will toggle PORTD
        PIND |= (1 <<pinNum);
}


int main(void) {

    pinConfig(PIN5);
	while(1){ 
		my_delay();
		togglePin(PIN5); 
	}

	return 0;
}