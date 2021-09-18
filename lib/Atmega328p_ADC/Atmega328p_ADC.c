#include "Atmega328p_ADC.h"

// TODO: Function for setting up ADC
//  Need to make a general API
void adcConfig(void){

    uint8_t temp = 0;
    //uint8_t bSuccess = 0;

    // Set-up analog pin for ADC
    // set AVCC as voltage ref for ADC
    // On Arduino UNO board, external cap is connected from
    // AREF pin to GND.
    temp |= AVCC<<REF_SELECT;
    // set analog pin as ADC input
    // TODO: pass ADC pin as parameter to API
    temp |= ADC0 << MUX; 
    ADMUX = temp;
    // set prescaler to 128
    // Since input clk source is 8 MHz,
    // in order to get ADC clock value between 50k-200kHz,
    // we need at least a division factor of 128.  
    ADCSRA |= DIV_BY_128 << ADPS;
    // Enable ADC
    ADCSRA |= SET << ADEN;
}

uint16_t adcConvert(void){

    uint16_t val = 0;
    // Start conversion
    PRR &= ~(SET << PRADC);
    ADCSRA |= SET << ADSC;
    // Wait until ADC conversion finishes
    while(ADCSRA & (1 << ADSC)){}
    // extract the converted result
    val |= (uint16_t)ADCL;
    val |= ((uint16_t)(ADCH & 0x03)) << 8;

    while(ADCSRA & (1 << ADSC)){}
    val |= (uint16_t)ADCL;
    val |= ((uint16_t)(ADCH & 0x03)) << 8;
    return val;
}
