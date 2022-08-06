#ifndef ATMEGA328P_TIM_H
#define ATMEGA328P_TIM_H

#include "Atmega328p_ADC.h"

// TIM0 Register handles
#define TCCR0A  ((volatile uint8_t*)0x44u) //Timer/Counter Control Register A
#define TCCR0B  ((volatile uint8_t*)0x45u)
#define OCR0A   ((volatile uint8_t*)0x47u)
#define OCR0B   ((volatile uint8_t*)0x48u)
#define TCNT0   ((volatile uint8_t*)0x46u)
// Timer/Counter1 Register Handles
#define TCCR1A  ((volatile uint8_t*)0x80u)
#define TCCR1B  ((volatile uint8_t*)0x81u)
#define OCR1B   ((volatile uint16_t*)0x8Au)
#define OCR1A   ((volatile uint16_t*)0x88u)
#define TCNT1   ((volatile uint16_t*)0x84u)
#define ICR1    ((volatile uint16_t*)0x86u)

#define OCR1A_n     (OCR1A)
#define TCCR1A_n    (TCCR1A)
#define TCCR1B_n    (TCCR1B)

// TCCR1A bitfield definitions
#define COM1A 		6
#define	COM1B 		4
#define WGM11_10   	0

// TCCR1B bitfield definitions
#define WGM13_12   	3
#define CS12_10		0

// COM1A/COM1B modes
#define TOGGLE_OC1A_ON_MATCH	        1  // WGM13:0 = 9 or 11 only
#define CLEAR_OC1_UP_SET_OC1_DOWN		2
#define SET_OC1_UP_CLEAR_OC1_DOWN		3

// Timer1 Waveform Generation Modes (bitfield values for WGMx1_0)
#define MODE_0  (0U)  // Normal; TOP = 0xFFFF
#define MODE_1  (1U)  // PWM, phase correct, 8-bit; TOP = 0x00FF
#define MODE_2  (2U)  // PWM, phase correct, 9-bit; TOP = 0x01FF
#define MODE_3  (3U)  // PWM, phase correct, 10-bit; TOP = 0x03FF
#define MODE_4  (4U)  // CTC; TOP = OCR1A
#define MODE_5  (5U)  // Fast PWM, 8-bit; TOP = 0x00FF
#define MODE_6  (6U)  // Fast PWM, 9-bit; TOP = 0x01FF
#define MODE_7  (7U)  // Fast PWM, 10-bit; TOP = 0x03FF
#define MODE_8  (8U)  // PWM, phase and frequency correct; TOP = ICR1
#define MODE_9  (9U)  // PWM, phase and frequency correct; TOP = OCR1A
#define MODE_10 (10U) // PWM, phase correct; TOP = ICR1
#define MODE_11 (11U) // PWM, phase correct; TOP = OCR1A
#define MODE_12 (12U) // CTC; TOP = ICR1

#define MODE_14 (14U) // Fast PWM; TOP = ICR1
#define MODE_15 (15U) // Fast PWM; TOP = IOCR1A

#define WGM_BITS_1_0_MSK (0x3U) // use this mask to extract bits 1-0 from above WGM bitfield values
#define WGM_BITS_3_2_MSK (0xCU) // use this mask to extract bits 3-2 from above WGM bitfield values

// bitfield values for WGMx3_2
#define MODES_3_0   (0U)
#define MODES_7_4   (1U)
#define MODES_11_8  (2U)
#define MODES_15_12 (3U)
// Timer1 Channel
#define TIM_CHANNEL_A   (0)
#define TIM_CHANNEL_B   (1U)

//WGM1 bits modes
// following updates OCR1x at BOTTOM
#define	PWM_PHA_AND_FREQ_CORRECT_TOP_ICR1		0U
#define	PWM_PHA_AND_FREQ_CORRECT_TOP_OCR1A		1U
#define PWM_FAST_TOP_ICR1		2U
#define PWM_FAST_TOP_OCR1A		3U

// clock prescale
#define NO_PRESCALE	    1U
#define DIV_BY_8		2U
#define DIV_BY_64		3U
#define DIV_BY_256		4U
#define DIV_BY_1024	    5U
		


// following updates OCR1x at TOP
#define	 PWM_PHA_CORRECT_TOP_ICR		2U
#define	 PWM_PHA_CORRECT_TOP_OCR1A		3U

/*
 * bitfield definitions
 */

// PRR - controls power to peripheral
#define PRTIM0 (5U)
#define PRTIM1 (3U)

// TCCRnA where n = 0,1 
#define COMA1_0    6u  // Compare Match Output A Mode
#define COMB1_0    4u  
#define WGM1_0     0U

// TCCR0B
#define FOC0A   7u
#define FOC0B   6u
#define WGM02   3u
#define CS02_0  0U


// TCCR1B
#define ICNC1       (7U)
#define ICES1       (6U)
#define WGM1_3_2    (3u)
#define CS1_2_0     (0U)

// COM0A modes:
//  For PWM Phase correct mode, these are the COM0A/B modes:
#define NORM_OP   0U    // Normal port operation, OC0A/B disconnected.
// if WGM02 = 0 -> normal operations.  WGM02 = 1 -> toggle OC0A on compare match
#define TOGGLE_OCA_ON_COMPARE  (1U)       // for OC0A only
#define TOGGLE_ON_COMPARE      (1U)
#define CLEAR_UPCOUNT_SET_DOWNCOUNT 2u  // clear & set refer to OC0A/B
#define CLEAR_DOWNCOUNT_SET_UPCOUNT 3u

// Clock Select bit (CS0/CS1) values
#define CLK_NO_PRESCALE      1U
#define CLK_PRESCALE_BY_8    2U
#define CLK_PRESCALE_BY_64   3U
#define CLK_PRESCALE_BY_256  4U
#define CLK_PRESCALE_BY_1024 5U

// Timer1 range
#define TIMER1_MAX_VALUE (65535U)
#define TIMER1_MIN_VALUE (0U)

#define PWM_DUTY_CYCLE_10   (100U)  // 10% duty cycle, multiplied by 10 
#define PWM_DUTY_CYCLE_7_5  (75U)  // 7.5% duty cycle, multiplied by 10
#define PWM_DUTY_CYCLE_5    (50U)  // 5.0% duty cycle, multiplied by 10 

typedef struct 
{
    uint16_t waveform_gen_mode   :4;
    uint16_t channel             :1;  
    uint16_t compare_output_mode :2;
    uint16_t clock_prescale      :3;   
    uint16_t reserved            :6;
    uint16_t pwm_freq              ;
    uint8_t duty_cycle             ;
}tim1_config_handle_t;


// Timer1 Macros
#define TIM1_MAX_TIMER_TICKS (32767U) // Valid for TIM1 CLK Freq = 2 MHz.  Equal to UINT16_MAX/2 

// API Prototypes
void tim0_config(void);
void setPWMFreq(uint8_t maxVal);
void tim1_config(void);
void controlServoAngle( uint16_t adcValue);
void set_tim1_as_fast_pwm(void);

void tim1_init(tim1_config_handle_t htim1);
void tim1_set_counter_max_value(uint16_t counter_max_value, tim1_config_handle_t htim1);
void pwm_set_duty_cycle(uint8_t duty_cycle);

#endif