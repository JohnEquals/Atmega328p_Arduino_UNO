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

#define GPIO_PORT_B     0
#define GPIO_PORT_C     1
#define GPIO_PORT_D     2
// pin mode
#define OUTPUT_PIN  0
#define INPUT_PIN   1

// pin states
#define LOW     (0)
#define HIGH    (1)

// pullup modes
#define PULLUP_ENABLED  (1)
#define PULLUP_DISABLED (0)

// GPIO to Arduino headers mapping
/*  
 *  Digital Pins
 *  RX <-> D0 <-> PD0
 *  TX <-> D1 <-> PD1
 *  D2 <-> PD2
 *  D3 <-> PD3
 *  D4 <-> PD4
 *  D5 <-> PD5
 *  D6 <-> PD6
 *  D7 <-> PD7
 *  D8 <-> PB0
 *  D9 <-> PB1
 *  D10 <-> PB2
 *  D11 <-> PB3
 *  D12 <-> PB4
 *  D13 <->  PB5
 * 
 *  Analog Pins
 *  A0 <-> PC0 
 *  A1 <-> PC1
 *  A2 <-> PC2
 *  A3 <-> PC3
 *  A4 <-> PC4
 *  A5 <-> PC5
 */
void pinConfig(uint8_t pinNum, uint8_t port, uint8_t pinMode, _Bool pullupEnabled);
void togglePin(uint8_t pinNum, uint8_t port);
void gpio_setPinState(uint8_t pinNum, uint8_t port, uint8_t pinState);
uint8_t gpio_getPinState(uint8_t pinNum, uint8_t port);

#endif