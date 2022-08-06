// Header guards
#ifndef ATMEGA328P_ADC_H
#define ATMEGA328P_ADC_H

#include <stdint.h>

#define SET     1
#define RESET   0

// macros for ADC resolution range
#define ADC_MAX_VALUE   (1023U)
#define ADC_MIN_VALUE   (0U)

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
#define AREF    0   // sets voltage ref to AREF with internal V_ref turned off.  We can input some voltage to AREF pin to serve as voltage reference.
#define AVCC    1   // AVCC = 5V voltage ref (can measure AREF voltage to confirm)
#define INTERNAL_VOLTAGE_1_1 3   // sets to internal voltage = 1.1V (confirm by measuring AREF voltage)

// TODO: Function(s) for processing ADC
// TODO: Function for setting up ADC
//  Need to make a general API
void adcConfig(void);

uint16_t adcConvert(void);

uint16_t adcValueToVoltage(uint16_t adcValue);
// TODO:  Cofigure ADC and write function to grab data from ADC data reg. 

#endif 