// Blink LED Program
#include <stdio.h>
#include <stdint.h>
#include <Atmega328p_ADC.h>
#include <Atmega328p_GPIO.h>
#include <Atmega328p_TIM.h>
#include <UART.h>
#include <atmega328p_utility.h>

#include <avr/power.h>	

// Register definitions for system clock registers (TODO: Port these over to appropriate library files to configure clock settings)
//#define CLKPR	(*((volatile uint8_t *)0x61))

// PCn pin can be used as ADC
#define PIN6                6
#define PIN5                5
#define PIN1				1
#define PIN0				0

#define VREF_5V				5
#define VREF_3V3			3	

int main(void) { 
	// Default clock speed for Arduino UNO is 16 MHz.  We use this API from avrlib to set system 
	// prescale value to 16 which makes our system clock to all peripherals as 1 MHz
	clock_prescale_set(clock_div_16);

	unsigned char buffer[40];
	//PC0 for ADC0
	pinConfig(PIN0, PORT_C, INPUT_PIN);
	//PD6 for OC0A
	//pinConfig(PIN6, PORT_D, OUTPUT_PIN);
	adcConfig();
	
	txConfig();
	setTxMode(ENABLE);

	// PB1 for OC1A
	pinConfig(PIN1, PORT_B, OUTPUT_PIN);

	tim1_config();
	uint16_t adcVal;
	uint8_t len;
	while(1){ 
		adcVal = adcConvert();
		len = sprintf((char*)buffer,"The ADC value is: %d\n",adcVal);
		sendData(buffer, len);
		// TODO:  Need to fix resolution of ADC.  Only makes multiples of 256
		// Also, because of this, its not giving different PWM speeds since our 
		// PWM function only takes 8 bit values.  Maybe change to 16-bit timer?
		//controlServoAngle(adcVal);
		//i = (i + 1)%3;
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		//togglePin(PIN1, PORT_B);
		// Print the voltage readings from ADC (need to implement floating point in software)
		//voltageIn = (adcVal* VREF_5V)/1024; // equation to convert adc conversion value to the voltage 
		//len = sprintf((char*)buffer,"The voltage is: %4.5f\n",voltageIn);
	}
	return 0;
}