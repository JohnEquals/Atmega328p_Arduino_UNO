#include "Atmega328p_TIM.h"
#include "UART.h"

#include <stdint.h>
#include <stdio.h>
#include <formulas.h>

#define GET_PRESCALE_VALUE(cs_bits)     ( \
                                        (cs_bits==CLK_NO_PRESCALE)?1U:\
                                        (cs_bits==CLK_PRESCALE_BY_8)?8U:\
                                        (cs_bits==CLK_PRESCALE_BY_64)?64U:\
                                        (cs_bits==CLK_PRESCALE_BY_256)?256U:\
                                        (cs_bits==CLK_PRESCALE_BY_1024)?1024U:0\
                                        )

// assertions for PWM modes

#define IS_NORMAL_MODE(waveform_gen_mode)           (waveform_gen_mode == MODE_0)

#define IS_CTC_MODE(waveform_gen_mode)              (\
                                                    (waveform_gen_mode == MODE_4)||\
                                                    (waveform_gen_mode == MODE_12)\
                                                    )         

#define IS_PHASE_CORRECT_MODE(waveform_gen_mode)    (\
                                                    (waveform_gen_mode == MODE_1)||\
                                                    (waveform_gen_mode == MODE_2)||\
                                                    (waveform_gen_mode == MODE_3)||\
                                                    (waveform_gen_mode == MODE_10)||\
                                                    (waveform_gen_mode == MODE_11)\
                                                    )

#define IS_PHASE_AND_FREQ_CORRECT_MODE(waveform_gen_mode)   (\
                                                            (waveform_gen_mode == MODE_8)||\
                                                            (waveform_gen_mode == MODE_9)\
                                                            )                                                 
                                    
#define IS_FAST_MODE(waveform_gen_mode)             (\
                                                    (waveform_gen_mode == MODE_5)||\
                                                    (waveform_gen_mode == MODE_6)||\
                                                    (waveform_gen_mode == MODE_7)||\
                                                    (waveform_gen_mode == MODE_14)||\
                                                    (waveform_gen_mode == MODE_15)\
                                                    )

#define ASSERT_TIMER1_MODE(waveform_gen_mode)       (\
                                                    IS_NORMAL_MODE(waveform_gen_mode)||\
                                                    IS_CTC_MODE(waveform_gen_mode)||\
                                                    IS_PHASE_CORRECT_MODE(waveform_gen_mode)||\
                                                    IS_PHASE_AND_FREQ_CORRECT_MODE(waveform_gen_mode)||\
                                                    IS_FAST_MODE(waveform_gen_mode)\
                                                    )

#define ASSERT_TIMER1_PRESCALER(prescaler)          (\
                                                    (prescaler == NO_PRESCALE)||\
                                                    (prescaler == DIV_BY_8)||\
                                                    (prescaler == DIV_BY_64)||\
                                                    (prescaler == DIV_BY_256)||\
                                                    (prescaler == DIV_BY_1024)\
                                                    )

#ifndef F_CPU
#define F_CPU   (16000000L) 
#endif

#define ASSERT_PWM_FREQUENCY(frequency)             ((frequency >= 0)||(frequency <= F_CPU))
    

#define ENABLE_TIM1    (PRR &= ~(1<< PRTIM1))
// choose prescale value such that TIM1 CLK frequency is 2 MHz
#define DISABLE_TIM1     (PRR |= (1<< PRTIM1))  

#define RESET_TCNT1     (*TCNT1 = 0)
#define RESET_OCR1B     (*OCR1B = 0)
#define RESET_OCR1A    (*OCR1A = 0)

#define TOP_IS_OCR_VALUE(timer_mode)    ((timer_mode == MODE_4)||\
                                        (timer_mode == MODE_9)||\
                                        (timer_mode == MODE_11))

#define TOP_IS_ICR_VALUE(timer_mode)    ((timer_mode == MODE_8)||\
                                        (timer_mode == MODE_10)||\
                                        (timer_mode == MODE_14))

#define SERVO_MOTOR_ANGLE_MAX	180
#define SERVO_MOTOR_ANGLE_MIN 	0

#define SERVO_DUTY_CYCLE_MIN 5
#define SERVO_DUTY_CYCLE_MAX 10

static uint8_t len_t;
static unsigned char buffer_t[40];


// global variables
uint16_t previous_ocr1a_val = 0;

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
    *TCCR0A |= 1u<<WGM1_0;
    // use this to set TOP = OCRA
    *TCCR0B |= 1<<WGM02;

    // Set Compare Match Output A mode
    *TCCR0A |= 1<<COMA1_0;
    
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
    len_t = sprintf((char*)buffer_t,"TCCR1A: %u\nICR1: %u\nTCCR1B: %u\n",*TCCR1A,*ICR1,*TCCR1B);
	sendData(buffer_t, len_t);
}

uint16_t pwm_frequency_to_output_compare_reg(uint16_t pwn_freq_in_hertz, tim1_config_handle_t htim1)
{
    uint16_t top_value = 0;
    /*
    for(uint8_t i = 1; i<=5;i++)
    {
        len_t = sprintf((char*)buffer_t,"cs_bits: %u\nprescale: %u\n",i, GET_PRESCALE_VALUE(i));
        sendData(buffer_t, len_t);
    }
    */

    // verify the mode is correct and the frequency and prescale values are correct
    if( ASSERT_TIMER1_MODE(htim1.waveform_gen_mode) && ASSERT_PWM_FREQUENCY(pwn_freq_in_hertz) && ASSERT_TIMER1_PRESCALER(htim1.clock_prescale))
    {
        uint32_t temp = 0;
        // convert tim1 clock prescaler from bitfield code to actual value
        uint8_t prescaler_val = GET_PRESCALE_VALUE(htim1.clock_prescale);
        
        // fast mode vs phase correct modes use different formulas for TOP value
        if(IS_FAST_MODE(htim1.waveform_gen_mode))
        {
            temp = F_CPU/(prescaler_val * pwn_freq_in_hertz) - 1;
        }
        else if(IS_PHASE_AND_FREQ_CORRECT_MODE(htim1.waveform_gen_mode)||IS_PHASE_CORRECT_MODE(htim1.waveform_gen_mode))
        {
            temp = F_CPU/(prescaler_val * pwn_freq_in_hertz*2);
        }
        if(temp > 0)
        {
            if((TIMER1_MIN_VALUE >= top_value)&&(top_value <= TIMER1_MAX_VALUE))
            {
                top_value = (uint16_t)temp;
            }
        }
    }
    return top_value;

}

void set_tim1_as_fast_pwm(void){

    *TCCR1A |= 2<<WGM11_10 | 2<<COM1A;
    *TCCR1B |= 3<<WGM13_12 | 3<<CS12_10;
    *ICR1 = 19999;

    *OCR1A = 9000; 
}


void set_pwm_duty_cycle(tim1_config_handle_t htim1)
{
    /*
        if(TOP_IS_ICR_VALUE(htim1.waveform_gen_mode))
        {
            
        }
        else if(TOP_IS_ICR_VALUE(htim1.waveform_gen_mode))
        {

        }
    */
   
    uint16_t adc_val = 0;  // read ADC value

    uint16_t angle = map(adc_val, SERVO_MOTOR_ANGLE_MIN, SERVO_MOTOR_ANGLE_MAX, ADC_MIN_VALUE, ADC_MAX_VALUE);

    uint16_t duty_cycle = map(angle, SERVO_DUTY_CYCLE_MIN, SERVO_DUTY_CYCLE_MAX, SERVO_MOTOR_ANGLE_MIN, SERVO_MOTOR_ANGLE_MAX);

    uint16_t ocb1_val = duty_cycle*(*OCR1A);
    *OCR1B = ocb1_val;

    // UPDATE OCB1 REGISTER

    // delay
}

void controlServoAngle( uint16_t adcValue){
	
	// need to put adcValue into slope intercept formula and figure out what the critical values are for OCR1A to put the servo in 0 and 180 angles.  
	if( adcValue > *TCNT1)
		*OCR1A = adcValue;
}

// PB2 <- OC1B , PB1 <- OC1A
void tim1_init(tim1_config_handle_t htim1)
{   
    uint8_t len;
    unsigned char buffer[40];
    //len = sprintf((char*)buffer,"TCCR1B: %u\n",*TCCR1B & ((uint16_t)7U));
	//sendData(buffer, len);


    // disable if currently enabled
    *TCCR1B = 0;
    RESET_TCNT1;  // need to clear counter on re
    RESET_OCR1B;
    RESET_OCR1A;  

    uint8_t temp_reg = 0;

    // set waveform generator mode
    temp_reg |= ( (htim1.waveform_gen_mode & WGM_BITS_1_0_MSK) << WGM11_10); 

    // set compare output mode 
    if(htim1.channel == TIM_CHANNEL_A)
    {
        temp_reg |= (htim1.compare_output_mode << COM1A);
    }
    else if(htim1.channel == TIM_CHANNEL_B)
    {
        temp_reg |= (htim1.compare_output_mode << COM1B);
    }

    *TCCR1A = temp_reg; 

    // clear temp reg for configuring next register
    temp_reg = 0; 
    len = sprintf((char*)buffer,"temp reg before TCCB: %u\n",temp_reg);
	sendData(buffer, len);
    // set waveform generator mode
    if((MODE_0 <= htim1.waveform_gen_mode)&&(htim1.waveform_gen_mode <= MODE_3))
    {
       temp_reg |= (MODES_3_0 <<WGM13_12);
    }
    else if((MODE_4 <= htim1.waveform_gen_mode)&&(htim1.waveform_gen_mode <= MODE_7))
    {
       temp_reg |= (MODES_7_4 <<WGM13_12);
    }
    else if((MODE_8 <= htim1.waveform_gen_mode)&&(htim1.waveform_gen_mode <= MODE_11)) 
    {
       temp_reg |= (MODES_11_8 <<WGM13_12);
    }
    else if((MODE_12 <= htim1.waveform_gen_mode)&&(htim1.waveform_gen_mode <= MODE_15)) 
    {
       temp_reg |= (MODES_15_12 <<WGM13_12);
    }
    len = sprintf((char*)buffer,"temp reg before INIT TCCB: %u\n",temp_reg);
	sendData(buffer, len);

    // set clock prescaler

    temp_reg |= (htim1.clock_prescale << CS12_10);

    *TCCR1B = temp_reg;

    // set period of PWM
    //*OCR1A = pwm_frequency_to_output_compare_reg(htim1.pwm_freq, htim1);
    *OCR1A = 1250U;

    previous_ocr1a_val = *OCR1A;
    // set duty cycle of PWM to 50% initially
    *OCR1B = *OCR1A/2;

    len = sprintf((char*)buffer,"Initializing TIM1\nTCCR1A: %u\nTCCR1B: %u\nOCR1A: %u\nOCR1B: %u\n",*TCCR1A,*TCCR1B,*OCR1A,*OCR1B);
	sendData(buffer, len);
}

// for pwm, sets the period
void tim1_set_counter_max_value(uint16_t counter_max_value, tim1_config_handle_t htim1)
{
    uint8_t len;
    unsigned char buffer[40];
    if( TOP_IS_OCR_VALUE(htim1.waveform_gen_mode) )
    {
        if(htim1.channel == TIM_CHANNEL_A)
        {
            *OCR1A = counter_max_value;
        }
        else if(htim1.channel == TIM_CHANNEL_B)
        {
            *OCR1B = counter_max_value;
        }
    }
    else if(TOP_IS_ICR_VALUE(htim1.waveform_gen_mode))
    {
        *ICR1 = counter_max_value;
    }
    len = sprintf((char*)buffer,"OCR1A: %u\nOCR1B: %u\nICR1: %u\n",*OCR1A, *OCR1B, *ICR1);
	sendData(buffer, len);
}


void pwm_set_duty_cycle(uint8_t duty_cycle)
{
    uint16_t denominator = 1000U; // 100 multiplied by 10
    if(duty_cycle == PWM_DUTY_CYCLE_10)
    {
        *OCR1B = (previous_ocr1a_val/denominator)*duty_cycle;
    }
    else if(duty_cycle == PWM_DUTY_CYCLE_7_5)
    {
        *OCR1B = (previous_ocr1a_val/denominator)*duty_cycle;
    }
    else if(duty_cycle == PWM_DUTY_CYCLE_5)
    {
        *OCR1B = (previous_ocr1a_val/denominator)*duty_cycle;
    }
}



// 

void microsec_timer_init(void)
{
    *TCCR1A = 0;  

    *TCCR1B = 0;
    *TCCR1B = 0; // timer disable initially

}

// Assuming that system clock frequency is 16MHz and timer1 clock speed is 2MHz
void usec_delay(uint16_t time_in_microseconds)
{   
    // Assuming that system clock frequency is 16MHz and timer1 clock speed is 2MHz
    if(time_in_microseconds > TIM1_MAX_TIMER_TICKS)
    {
        time_in_microseconds = TIM1_MAX_TIMER_TICKS;
    }
    // reset timer for counting
    *TCNT1 = 0;  
    // turn on timer
    *TCCR1B |= CLK_PRESCALE_BY_8 << CS1_2_0;
    // wait for timer to expire
    while(*TCNT1 < (time_in_microseconds * 2))
        asm volatile("nop");
    // turn off timer
    *TCCR1B = 0;
}