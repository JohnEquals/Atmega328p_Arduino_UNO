#ifndef ATMEGA328P_TIM_H
#define ATMEGA328P_TIM_H

#include <Atmega328p_ADC.h>

// TIM0 Register handles
#define TCCR0A  ((volatile uint8_t*)0x44u) //Timer/Counter Control Register A
#define TCCR0B  ((volatile uint8_t*)0x45u)
#define OCR0A   ((volatile uint8_t*)0x47u)
#define OCR0B   ((volatile uint8_t*)0x48u)
#define TCNT0   ((volatile uint8_t*)0x46u)
// Timer/Counter1 Register Handles
#define TCCR1A  ((volatile uint8_t *)0x80u)
#define TCCR1B  ((volatile uint8_t *)0x81u)
#define OCR1A   ((volatile uint16_t *)0x8Au)
#define TCNT1   ((volatile uint16_t *)0x84u)
#define ICR1    ((volatile uint16_t *)0x86u)

// TCCR1A bitfield definitions
#define COM1A 		6
#define	COM1B 		4
#define WGM11_10   	0

// TCCR1B bitfield definitions
#define WGM13_12   	3
#define CS12_10		0

// COM1A/COM1B modes
#define TOGGLE_OC1A_ON_MATCH	1  // WGM13:0 = 9 or 11 only
#define CLEAR_OC1_UP_SET_OC1_DOWN		2
#define SET_OC1_UP_CLEAR_OC1_DOWN		3

//WGM1 bits modes
// following updates OCR1x at BOTTOM
#define	PWM_PHA_AND_FREQ_CORRECT_TOP_ICR1		0
#define	PWM_PHA_AND_FREQ_CORRECT_TOP_OCR1A		1
#define PWM_FAST_TOP_ICR1		2
#define PWM_FAST_TOP_OCR1A		3

// clock prescale
#define NO_PRESCALE	1
#define DIV_BY_8		2
#define DIV_BY_64		3
#define DIV_BY_256		4
#define DIV_BY_1024	5
		


// following updates OCR1x at TOP
#define	 PWM_PHA_CORRECT_TOP_ICR		2
#define	 PWM_PHA_CORRECT_TOP_OCR1A		3

/*
 * bitfield definitions
 */

// PRR
#define PRTIM0 5u

// TCCR0A
#define COM0A1_0    6u  // Compare Match Output A Mode
#define COMOB1_0    4u  
#define WGM01_0     0

// TCCR0B
#define FOC0A   7u
#define FOC0B   6u
#define WGM02   3u
#define CS02_0  0


// COM0A modes:
//  For PWM Phase correct mode, these are the COM0A/B modes:
#define NORM_OP   0    // Normal port operation, OC0A/B disconnected.
// if WGM02 = 0 -> normal operations.  WGM02 = 1 -> toggle OC0A on compare match
#define OC0A_ON_COMPARE_MATCH  1u       // for OC0A only
#define CLEAR_UPCOUNT_SET_DOWNCOUNT 2u  // clear & set refer to OC0A/B
#define CLEAR_DOWNCOUNT_SET_UPCOUNT 3u

// CS0 values
#define CLK_NO_PRESCALE     1U
#define CLK_PRESCALE_BY_8   2U
#define CLK_PRESCALE_BY_64  3U
#define CLK_PRESCALE_BY_256 4U
#define CLK_PRESCALE_BY_1024 4U

// API Prototypes
void tim0_config(void);
void setPWMFreq(uint8_t maxVal);
void tim1_config(void);
void controlServoAngle( uint16_t adcValue);
void set_tim1_as_fast_pwm(void);

#endif