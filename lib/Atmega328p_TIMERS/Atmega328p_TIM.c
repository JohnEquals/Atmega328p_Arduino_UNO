#include "Atmega328p_TIM.h"
#include "UART.h"

#include <stdint.h>

void tim0_config(void){

    // turn on TIM0 
    PRR &= ~(1u<<PRTIM0);
    
    /*
     *  Configure for PWM Phase Correct Mode
     */
    // set waveform Generation mode to PWM Phase Correct
    // WGM2:0 = 1 for top = 0xFF, WGM2:0 = 5 for top = OCRA 
    // We set TOP = OCRA because we want to change OCRA to the value
    // we get from ADC.
    // Note: the raw ADC value we get is 16-bit but OCRA is only 
    // 8-bits...so shift ADC value to right by 8 bits to truncate only if its greater than 255? 
    *TCCR0A |= 1u<<WGM01_0;
    // use this to set TOP = OCRA
    *TCCR0B |= 1<<WGM02;

    // Set Compare Match Output A mode
    *TCCR0A |= 1<<COM0A1_0;
    
    // Set Compare Match Output B mode
    /*
    *TCCR0A &= ~(3<<COM0B1_0);
    *TCCR0A |= CLEAR_DOWNCOUNT_SET_UPCOUNT<<COM0B1_0;
    */

    // for PWM, clear these bitfields
    *TCCR0B &= ~(1<<FOC0A);
    *TCCR0B &= ~(1<<FOC0B);
    
    // Set clock source
    *TCCR0B |= (5u<<CS02_0);   
}

void setPWMFreq(uint8_t maxVal){
    if(maxVal > *TCNT0)
        *OCR0A = maxVal;
}

// TODO: implement 16-bit timer

void tim1_config(void){
    uint8_t len;
    unsigned char buffer[40];
	// config for Phase & Frequency Correct PWM mode, inverted
	// use ICR1A to set TOP value
	// use OCR1A to store the new ADC values.
	// toggle OC1A on compare match
	*TCCR1A = 1 << COM1A;
	//  system clk should be set to 1 MHz.  
	*TCCR1B = (2<< WGM13_12) | ( NO_PRESCALE << CS12_10);

	// config OC1A pin as output mode
    // Note:  D9 pin header on ARDUINO UNO REV3 - PB1 pin on ATMEGA328P - OCR1A

	// set TOP value to 19999 (20000 counts) to set a 50 Hz PWM 
    // signal given that timer clk is 1MHz
	// TOP value should always be set to N – 1 where N is number of counts you want
	// to generate a   (TIM_CLK_FREQ / N)  frequency signal.
    *ICR1 = 20000u;
    //*OCR1A = 4000u;
    len = sprintf((char*)buffer,"TCCR1A: %ud\nICR1: %ud\nTCCR1B: %ud\n",TCCR1A,ICR1,TCCR1B);
	sendData(buffer, len);
}

void set_tim1_as_fast_pwm(void){

    *TCCR1A |= 2<<WGM11_10 | 2<<COM1A;
    *TCCR1B |= 3<<WGM13_12 | 3<<CS12_10;
    *ICR1 = 19999;

    *OCR1A = 9000; 
}

void controlServoAngle( uint16_t adcValue){
	
	// need to put adcValue into slope intercept formula and figure out what the critical values are for OCR1A to put the servo in 0 and 180 angles.  
	if( adcValue > *TCNT1)
		*OCR1A = adcValue;
}
