#ifndef ATMEGA328P_GPIO_H
#define ATMEGA328P_GPIO_H

#include<stdint.h>
// Register addresses

#define PORTB               ((volatile uint8_t*)0x25)
#define DDRB                ((volatile uint8_t*)0x24)
#define PINB                ((volatile uint8_t*)0x23)

#define PORTC               ((volatile uint8_t*)0x28)
#define DDRC                ((volatile uint8_t*)0x27)
#define PINC                ((volatile uint8_t*)0x26)

#define PORTD               ((volatile uint8_t*)0x2b)
#define DDRD                ((volatile uint8_t*)0x2a)
#define PIND                ((volatile uint8_t*)0x29)

// general port parameter for programming APIs
#define PORT_B      0
#define PORT_C      1
#define PORT_D      2

// pin mode
#define OUTPUT_PIN  0
#define INPUT_PIN   1

void pinConfig(uint8_t pinNum, uint8_t port, uint8_t pinMode);
void togglePin(uint8_t pinNum, uint8_t port);

#endif