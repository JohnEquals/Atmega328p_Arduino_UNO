// Header guards
#ifndef ATMEGA328P_ADC_H
#define ATMEGA328P_ADC_H

#include <stdint.h>

#define SET     1
#define RESET   0

/*
 *  Register Bitfield Definitions
*/
typedef struct{

    volatile uint8_t mux0    :1;
    volatile uint8_t mux1    :1;
    volatile uint8_t mux2    :1;
    volatile uint8_t mux3    :1;
    volatile uint8_t reserved    :1;
    volatile uint8_t adlar    :1;
    volatile uint8_t refs0    :1;
    volatile uint8_t refs1    :1;
}admux_regDef_t;

typedef struct{

    volatile uint8_t ADPS0    :1;
    volatile uint8_t ADPS1    :1;
    volatile uint8_t ADPS2    :1;
    volatile uint8_t ADIE    :1;
    volatile uint8_t ADIF    :1;
    volatile uint8_t ADATE    :1;
    volatile uint8_t ADSC    :1;
    volatile uint8_t ADEN    :1;
}adcsra_regDef_t;


typedef struct{
    volatile uint8_t ADCL;
    volatile uint8_t ADCH;  
}adcData_regDef_t;

// Register Addresses for Power Management and Sleep
#define PRR     (*((volatile uint8_t *)0x64))


// Register Addresses for ADC
#define ADMUX    (*((volatile uint8_t *)0x7c))
#define ADCSRA   (*((volatile uint8_t *)0x7a))
#define ADCL     (*((volatile uint8_t *)0x78))
#define ADCH     (*((volatile uint8_t *)0x79))
#define ADCSRB   (*((volatile uint8_t *)0x7b))
#define DIDR0    (*((volatile uint8_t *)0x7e))

// Register Objects
#define _ADMUX   (*((admux_regDef_t *)0x7c))
#define _ADCSRA   (*((adcsra_regDef_t *)0x7a))
#define _ADCL_AND_ADCH   (*((adcData_regDef_t *)0x78))

// Macros for bitfields
#define ADEN            7
#define MUX             0
#define REF_SELECT      6

#define PRADC           0
#define ADSC            6
#define ADPS            0

#define ADIF            4

// single ended input ADC channels
#define ADC0    0
#define ADC1    1
#define ADC2    2
#define ADC3    3
#define ADC4    4
#define ADC5    5
#define ADC6    6
#define ADC7    7
#define ADC8    8

// prescaler values
#define DIV_BY_128  7

// Voltage reference selection
#define AREF    0
#define AVCC    1

// TODO: Function(s) for processing ADC
// TODO: Function for setting up ADC
//  Need to make a general API
void adcConfig(void);

uint16_t adcConvert(void);
// TODO:  Cofigure ADC and write function to grab data from ADC data reg. 

#endif 