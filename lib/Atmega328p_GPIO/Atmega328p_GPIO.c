#include <stdint.h>
#include "Atmega328p_GPIO.h"


void pinConfig(uint8_t pinNum, uint8_t port, uint8_t pinMode){
	if(port == PORT_B){
		if(pinMode == OUTPUT_PIN){
			*DDRB |=  (1<<pinNum);
			*PORTB |= (1<<pinNum); 
		}else
			*DDRB &=  ~(1<<pinNum);
	}else if(port == PORT_C){
		if(pinMode == OUTPUT_PIN){
			*DDRC |=  (1<<pinNum);
			*PORTC |= (1<<pinNum); 
		}else
			*DDRC &=  ~(1<<pinNum);
	}else if(port == PORT_D){
		if(pinMode == OUTPUT_PIN){
			*DDRD |=  (1<<pinNum);
			*PORTD |= (1<<pinNum); 
		}else
			*DDRD &=  ~(1<<pinNum);
	}    
}


void togglePin(uint8_t pinNum, uint8_t port){
		// Setting PIND will toggle PORTD
	if(port == PORT_B){
		*PINB |= (1 <<pinNum); 
	}else if(port == PORT_C){
		*PINC |= (1 <<pinNum); 
	}else if(port == PORT_D){
		*PIND |= (1 <<pinNum); 
	}   
}