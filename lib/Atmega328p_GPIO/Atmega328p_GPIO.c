#include <stdint.h>
#include "Atmega328p_GPIO.h"
#include <stdbool.h>
#include "avr\io.h"
/*
 * 		Toggling pins:  writing 1 to PINx will toggle value of PORTxn
 *
 * 		| DDxn | PORTxn | PUD |  Mode
 *		|   0  |    0   |  NC | Input, tri-state
 * 	    |   0  |    1   |  0  | Input, sources current if pulled low externally  
 *      |   0  |    1   |  1  | Input, tri-state
 *      |   1  |    0   |  NC | output low (sink current)
 * 	    |   1  |    1   |  NC | output HIGH(source current)
 */

void pinConfig(uint8_t pinNum, uint8_t port, uint8_t pinMode, _Bool pullupEnabled)
{
	if(port == PORT_B)
	{
		if(pinMode == OUTPUT_PIN)
		{
			DDRB  |= (1<<pinNum);
			//*PORTB |= (1<<pinNum); 
		}
		else
		{
			DDRB &=  ~(1<<pinNum);
		}
	}
	else if(port == PORT_C)
	{
		if(pinMode == OUTPUT_PIN)
		{
			DDRC |=  (1<<pinNum);
			//*PORTC |= (1<<pinNum); 
		}
		else
			DDRC &=  ~(1<<pinNum);
	}
	else if(port == PORT_D)
	{
		if(pinMode == OUTPUT_PIN)
		{
			DDRD |=  (1<<pinNum);
			//*PORTD |= (1<<pinNum); 
		}else
			DDRD &=  ~(1<<pinNum);
	}

	if(pinMode == INPUT_PIN)
	{
		if(pullupEnabled)
		{
			if(port == PORT_B)
			{
				PORTB |= (1<<pinNum); 
			}else if(port == PORT_C)
			{
				PORTC |= (1<<pinNum); 
			}else if(port == PORT_D)
			{
				PORTD |= (1<<pinNum); 
			}  
		}
		else
		{
			if(port == PORT_B)
			{
				PORTB &= ~(1<<pinNum); 
			}else if(port == PORT_C)
			{
				PORTC &= ~(1<<pinNum); 
			}else if(port == PORT_D)
			{
				PORTD &= ~(1<<pinNum); 
			}  
		}  
	}
}


void togglePin(uint8_t pinNum, uint8_t port){
		// Setting PIND will toggle PORTD
	if(port == PORT_B){
		PINB |= (1 <<pinNum); 
	}else if(port == PORT_C){
		PINC |= (1 <<pinNum); 
	}else if(port == PORT_D){
		PIND |= (1 <<pinNum); 
	}   
}

void gpio_setPinState(uint8_t pinNum, uint8_t port, uint8_t pinState)
{
	if(pinState == HIGH)
	{
		if(port == PORT_B){
			PORTB |= (1 <<pinNum); 
		}else if(port == PORT_C){
			PORTC |= (1 <<pinNum); 
		}else if(port == PORT_D){
			PORTD |= (1 <<pinNum); 
		}   
	}
	else if(pinState == LOW)
	{
		if(port == PORT_B){
			PORTB &= ~(1 <<pinNum); 
		}else if(port == PORT_C){
			PORTC &= ~(1 <<pinNum); 
		}else if(port == PORT_D){
			PORTD &= ~(1 <<pinNum); 
		}   
	}
}

uint8_t gpio_getPinState(uint8_t pinNum, uint8_t port)
{
	// use only for input pins
	uint8_t state; // dummy value 
	if(port == PORT_B)
	{
		state = PINB & (1 <<pinNum); 
	}
	else if(port == PORT_C)
	{
		state = PINC & (1 <<pinNum); 
		state &= ~(1 << 7);  // mask out bit7 to generate the correct state value
	}
	else if(port == PORT_D)
	{
		state = PIND & (1 <<pinNum); 
	}   
	state = ((state == 0)?LOW:HIGH);
	return state;
}