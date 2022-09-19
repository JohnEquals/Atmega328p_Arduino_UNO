#include "Atmega328p_TIM.h"
//#include "UART.h"

#include <stdint.h>
#include <stdio.h>
//#include <formulas.h>
#include <avr\interrupt.h>
#include <string.h>

static uint8_t tim0_clock_prescale_setting = 0;
static uint8_t tim1_clock_prescale_setting = 0;
static uint8_t tim2_clock_prescale_setting = 0;

static void tim0_set_clock_prescaler(uint8_t prescale_val)
{
    tim0_clock_prescale_setting = prescale_val;
}

static uint8_t tim0_get_clock_prescaler(void)
{
    return tim0_clock_prescale_setting;
}

static void tim1_set_clock_prescaler(uint8_t prescale_val)
{
    tim1_clock_prescale_setting = prescale_val;
}

static uint8_t tim1_get_clock_prescaler(void)
{
    return tim1_clock_prescale_setting;
}

static void tim2_set_clock_prescaler(uint8_t prescale_val)
{
    tim2_clock_prescale_setting = prescale_val;
}

static uint8_t tim2_get_clock_prescaler(void)
{
    return tim2_clock_prescale_setting;
}

// default macros for Timer clock frequency
#define TIM1_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ)
#define TIM1_DEFAULT_MAX_TICKS_IN_MS    (4U) 
#define TIM1_DEFAULT_MAX_TICKS_IN_US    (4095U)
#define TIM1_MAX_ACCURATE_COUNTS_IN_US  (TIM1_DEFAULT_MAX_TICKS_IN_US)
#define TIM1_MAX_ACCURATE_COUNTS_IN_MS  (TIM1_DEFAULT_MAX_TICKS_IN_MS)   

#define TIM0_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ)
#define TIM0_MAX_ACCURATE_COUNTS_IN_US  (UINT8_MAX/TIM0_CLK_FREQ) // 16MHz -> 15 us max count

#define TIM2_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ)
#define TIM2_MAX_ACCURATE_COUNTS_IN_US  (UINT8_MAX/TIM2_CLK_FREQ)  // 16MHz -> 15 us max count
/**
 *   TIM1 is ideal for usec/msec delay (blocking and interrupt versions) since it has the resolution (16-bit)
 * 
 */

#define GET_PRESCALE_VALUE(cs_bits)     ( \
                                        (cs_bits==CLK_NO_PRESCALE)?1U:\
                                        (cs_bits==CLK_PRESCALE_BY_8)?8U:\
                                        (cs_bits==CLK_PRESCALE_BY_64)?64U:\
                                        (cs_bits==CLK_PRESCALE_BY_256)?256U:\
                                        (cs_bits==CLK_PRESCALE_BY_1024)?1024U:0\
                                        )

// assertions for PWM modes

#define IS_NORMAL_MODE(waveform_gen_mode)           (waveform_gen_mode == TIM_WGM_MODE_0)

#define IS_CTC_MODE(waveform_gen_mode)              (\
                                                    (waveform_gen_mode == TIM_WGM_MODE_4)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_12)\
                                                    )         

#define IS_PHASE_CORRECT_MODE(waveform_gen_mode)    (\
                                                    (waveform_gen_mode == TIM_WGM_MODE_1)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_2)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_3)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_10)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_11)\
                                                    )

#define IS_PHASE_AND_FREQ_CORRECT_MODE(waveform_gen_mode)   (\
                                                            (waveform_gen_mode == TIM_WGM_MODE_8)||\
                                                            (waveform_gen_mode == TIM_WGM_MODE_9)\
                                                            )                                                 
                                    
#define IS_FAST_MODE(waveform_gen_mode)             (\
                                                    (waveform_gen_mode == TIM_WGM_MODE_5)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_6)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_7)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_14)||\
                                                    (waveform_gen_mode == TIM_WGM_MODE_15)\
                                                    )

#define IS_PWM_MODE(waveform_gen_mode)              (\
                                                    (IS_PHASE_CORRECT_MODE(waveform_gen_mode))||\
                                                    (IS_FAST_MODE(waveform_gen_mode)) ||\
                                                    (IS_PHASE_AND_FREQ_CORRECT_MODE(waveform_gen_mode))\
                                                    )

#define ASSERT_TIMER_MODE(waveform_gen_mode)       (\
                                                    (IS_NORMAL_MODE(waveform_gen_mode))||\
                                                    (IS_CTC_MODE(waveform_gen_mode))||\
                                                    (IS_PHASE_CORRECT_MODE(waveform_gen_mode))||\
                                                    (IS_PHASE_AND_FREQ_CORRECT_MODE(waveform_gen_mode))||\
                                                    (IS_FAST_MODE(waveform_gen_mode))\
                                                    )

                                            

#define ASSERT_TIMER_PRESCALER(prescaler)          (\
                                                    (prescaler == TIM_NO_PRESCALE)||\
                                                    (prescaler == TIM_DIV_BY_8)||\
                                                    (prescaler == TIM_DIV_BY_64)||\
                                                    (prescaler == TIM_DIV_BY_256)||\
                                                    (prescaler == TIM_DIV_BY_1024)\
                                                    )

#define ASSERT_TIMER_INTERRUPT_FLAG(flag)           (\
                                                    (TIM_INTERRUPT_INPUT_CAPTURE_FLAG == flag)||\
                                                    (TIM_INTERRUPT_OUTPUT_CMP_B_FLAG == flag)||\
                                                    (TIM_INTERRUPT_OUTPUT_CMP_A_FLAG == flag)||\
                                                    (TIM_INTERRUPT_OVERFLOW_FLAG == flag)\
                                                    )

#define ASSERT_8BIT_TIMER_COUNT(counts)             (\
                                                    (TIMER_8BIT_MAX_VALUE >= counts)&&\
                                                    (TIMER_8BIT_MIN_VALUE <= counts)\
                                                    )

#define ASSERT_16BIT_TIMER_COUNT(counts)            (\
                                                    (TIMER_16BIT_MAX_VALUE >= counts)&&\
                                                    (TIMER_16BIT_MIN_VALUE <= counts)\
                                                    )

#define ASSERT_TIMER1_INPUT_CAPTURE_PIN_EDGE(edge)  (\
                                                    (TIM_INPUT_CAPTURE_EDGE_SELECT_RISING_EDGE == edge)||\
                                                    (TIM_INPUT_CAPTURE_EDGE_SELECT_FALLING_EDGE == edge)\
                                                    )

#ifndef F_CPU
#define F_CPU   (16000000L) 
#endif

#define ASSERT_PWM_FREQUENCY(frequency)             ((frequency >= 0) && (frequency <= F_CPU))
    

#define ENABLE_TIM1    (PRR &= ~(1<< PRTIM1))
// choose prescale value such that TIM1 CLK frequency is 2 MHz
#define DISABLE_TIM1     (PRR |= (1<< PRTIM1))  

#define RESET_TCNT1     (TCNT1 = 0)
#define RESET_OCR1B     (OCR1B = 0)
#define RESET_OCR1A    (OCR1A = 0)

#define TOP_IS_OCR_VALUE(timer_mode)    ((timer_mode == MODE_4)||\
                                        (timer_mode == MODE_9)||\
                                        (timer_mode == MODE_11))

#define TOP_IS_ICR_VALUE(timer_mode)    ((timer_mode == MODE_8)||\
                                        (timer_mode == MODE_10)||\
                                        (timer_mode == MODE_14))

// Allows dynamic re-initialization of peripheral
sys_err_e_t tim0_init(tim_config_handle_t * htim0)
{
    //sys_err_e_t status = SYS_STATUS_OKAY;
    if(!ASSERT_TIMER_MODE(htim0->waveform_gen_mode))
    {
        return SYS_ERROR_TIM0_INIT_INPUT_INVALID_WGM;
    }
    if(!ASSERT_TIMER_PRESCALER(htim0->clock_prescale))
    {
        return SYS_ERROR_TIM0_INIT_INPUT_INVALID_PRESCALER;
    }

    uint8_t sreg_data = SREG;
    cli();

    // save settings and disable timer
    TCCR0B &= ~TIM_TCCR0B_CS_MSK;   
    uint8_t temp_reg1 = 0;
    uint8_t temp_reg2 = 0;

    // turn on TIM0 
    PRR &= ~(1u<<PRTIM0);

    // configure WGM
    switch(htim0->waveform_gen_mode)
    {
        case TIM_WGM_MODE_0:
        {
            // already configured by default values on reset
        }
        case TIM_WGM_MODE_1:
        {
            temp_reg1 |= (1U << WGM00); 
        }
        case TIM_WGM_MODE_4:
        {
            temp_reg1 |= (1U << WGM01);
        }
        case TIM_WGM_MODE_5:
        {
            temp_reg1 |= (1U << WGM00) | (1U << WGM00);
        }
        case TIM_WGM_MODE_11:
        {
            temp_reg1 |= (1U << WGM00);
            temp_reg2 |= (1U << WGM02);
        }
        case TIM_WGM_MODE_15:
        {
            temp_reg1 |= (1U << WGM00) | (1U << WGM00);
            temp_reg2 |= (1U << WGM02);
        }
        default:
        {
            return SYS_ERROR_TIM0_INIT_INPUT_INVALID_WGM;
        }
    }

    // configure Compare Output Mode for Channel B
    if(TIM_COM_NORMAL == htim0->compare_output_mode_b)
    {
        // default
    }
    else if(TIM_COM_NONPWM_TOGGLE_OCXN_MATCH == htim0->compare_output_mode_b)
    {
        temp_reg1 |= (1 << COM0B0);
    }
    else if((TIM_COM_NONPWM_CLEAR_OCXN_MATCH == htim0->compare_output_mode_b) || \
       (TIM_COM_FAST_PWM_SET_OCXN_BOTTOM_CLEAR_OCXN_MATCH == htim0->compare_output_mode_b) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_DOWN_CLEAR_OCXN_UP == htim0->compare_output_mode_b))
    {
        temp_reg1 |= (1 << COM0B1);
    }
    else if((TIM_COM_NONPWM_SET_OCXN_MATCH == htim0->compare_output_mode_b) || \
       (TIM_COM_FAST_PWM_CLEAR_OCXN_BOTTOM_SET_OCXN_MATCH == htim0->compare_output_mode_b) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN == htim0->compare_output_mode_b))
    {
        temp_reg1 |= (1 << COM0B1) | (1 << COM0B0);
    }

    // configure Compare Output Mode for Channel A
    if(TIM_COM_NORMAL == htim0->compare_output_mode_a)
    {
        // default
    }
    else if((TIM_COM_NONPWM_TOGGLE_OCXN_MATCH == htim0->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_OCXA_TOP_TOGGLE_OCXA_MATCH == htim0->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_TOGGLE_OCXA_MATCH == htim0->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM0A0);
    }
    else if((TIM_COM_NONPWM_CLEAR_OCXN_MATCH == htim0->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_SET_OCXN_BOTTOM_CLEAR_OCXN_MATCH == htim0->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_DOWN_CLEAR_OCXN_UP == htim0->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM0A1);
    }
    else if((TIM_COM_NONPWM_SET_OCXN_MATCH == htim0->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_CLEAR_OCXN_BOTTOM_SET_OCXN_MATCH == htim0->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN == htim0->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM0A1) | (1 << COM0A0);
    }

    // configure internal clock settings.  Leave timer disable.  Only enable on interrupt init or with blocking delay 
    // temp_reg2 |= (htim0->clock_prescale <<CS00);   
    tim0_set_clock_prescaler(htim0->clock_prescale);

    // configure settings
    TCCR0A = temp_reg1;
    TCCR0B = temp_reg2; 

        // set macros for time generation
    switch(htim0->clock_prescale)
    {
        case TIM_NO_PRESCALE:
        {
#undef TIM0_CLK_FREQ
#define TIM0_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ)
            break;
        }
        case TIM_DIV_BY_8:
        {
#undef TIM0_CLK_FREQ
#define TIM0_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ/8U)
            break;
        }
        default:
        {
            // invalid clock frequency for microsecond timer if TIM2 clock freq < 1 MHz
            // since clock speed is too slow
            break;
        }
    }

/**
 * @brief TIM0_MAX_ACCURATE_COUNTS_IN_US/_MS is the total number of microseconds/millseconds that can be counted
 *        accurately.  
 * 
 *        Calculations:
 *        1/(Timer0 clock frequency) = period of TIM0 = how many seconds it takes for TIM0 to count 1 tick
 *        ticks of TIM0 to count 1 ms = 1 ms/(period of TIM0)
 *        ticks of TIM0 to count 1 us = 1 us/(period of TIM0)
 *        (resolution of TIM0)/(ticks of TIM0 to count 1 ms) = highest time tim0 can count with accuracy in ms
 *        (resolution of TIM0)/(ticks of TIM0 to count 1 us) = highest time tim0 can count with accuracy in us
 * 
 * @note  The above formulas can be used generally for figuring out these values.  Note that the results should be
 *        integers to be valid values.     
 */

#undef  TIM0_MAX_ACCURATE_COUNTS_IN_US
#define TIM0_MAX_ACCURATE_COUNTS_IN_US  (UINT8_MAX/(TIM0_CLK_FREQ*SYS_1_MHZ_IN_HZ))

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim0_config_interrupt(uint8_t tim0_interrupt_mask_position, _Bool interrupt_state)
{

    if(!ASSERT_TIMER_INTERRUPT_FLAG(tim0_interrupt_mask_position))
    {
        return SYS_ERROR_TIM0_INIT_INPUT_INVALID_INTERRUPT_FLAG;
    }

    uint8_t sreg_data = SREG;
    cli();

    if((TIM_INTERRUPT_OVERFLOW_FLAG == tim0_interrupt_mask_position)||\
       (TIM_INTERRUPT_OUTPUT_CMP_A_FLAG == tim0_interrupt_mask_position)||\
       (TIM_INTERRUPT_OUTPUT_CMP_B_FLAG == tim0_interrupt_mask_position))
    {
        if(interrupt_state)
        {
            TIMSK0 |= (1<<tim0_interrupt_mask_position);
        }
        else
        {
            TIMSK0 &= ~(1<<tim0_interrupt_mask_position);
        }
    }

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim0_set_count(uint8_t tick_count)
{
    uint8_t sreg_data = SREG;
    cli();

    // save clock settings and temporarily turn off timer 
    uint8_t clock_prescaler_setting = TCCR0B & TIM_TCCR0B_CS_MSK;
    TCCR0B &= ~TIM_TCCR0B_CS_MSK;

    TCNT0 =  tick_count;
    
    TCCR0B |= clock_prescaler_setting;

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

uint8_t tim0_get_count(void)
{
    return TCNT0;
}


void tim0_usec_delay(uint16_t time_in_microseconds)
{   
    #ifdef TIM0_MAX_ACCURATE_COUNTS_IN_US

        uint8_t clk_prescale = tim0_get_clock_prescaler(); 
        TCCR0B &= clk_prescale;  // turn on timer

        if(TIM0_MAX_ACCURATE_COUNTS_IN_US < time_in_microseconds)
        {
            time_in_microseconds = TIM0_MAX_ACCURATE_COUNTS_IN_US;
        }
        // reset timer for counting
        TCNT0 = 0;  

        // wait for timer to expire
        while(TCNT0 < (time_in_microseconds * clk_prescale))
            asm volatile("nop");
        // turn off timer
        TCCR0B = 0;
    #endif
}

sys_err_e_t tim0_set_ocr0(uint8_t top, uint8_t channel)
{
    //uint8_t sreg_data = SREG;
    //cli();

    if(TIM_CHANNEL_A == channel)
    {
        OCR0A = top;
    }
    else if(TIM_CHANNEL_B == channel)
    {
        OCR0B = top;
    }
    else
    {
        return SYS_ERROR_TIM0_INPUT_INVALID_CHANNEL;
    }
    //SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim0_clear_interrupt_flag(uint8_t flag)
{
    if(TIM_INTERRUPT_OVERFLOW_FLAG == flag)
    {
        TIFR0 |= 1 << TIM_INTERRUPT_OVERFLOW_FLAG;
    }
    else if(TIM_INTERRUPT_OUTPUT_CMP_A_FLAG == flag)
    {
        TIFR0 |= 1 << TIM_INTERRUPT_OUTPUT_CMP_A_FLAG;
    }
    else if(TIM_INTERRUPT_OUTPUT_CMP_B_FLAG == flag)
    {
        TIFR0 |= 1 << TIM_INTERRUPT_OUTPUT_CMP_B_FLAG;
    }
    else
    {
        return SYS_ERROR_TIM0_INIT_INPUT_INVALID_INTERRUPT_FLAG;
    }
    return SYS_STATUS_OK;
}

void tim1_usec_delay(uint16_t time_in_microseconds)
{   
    #ifdef TIM1_MAX_ACCURATE_COUNTS_IN_US

        uint8_t tim1_clk_prescale = tim1_get_clock_prescaler(); 
        TCCR1B &= tim1_clk_prescale;  // turn on timer

        if(TIM1_MAX_ACCURATE_COUNTS_IN_US < time_in_microseconds)
        {
            time_in_microseconds = TIM1_MAX_ACCURATE_COUNTS_IN_US;
        }
        // reset timer for counting
        TCNT1 = 0;  

        // wait for timer to expire
        while(TCNT1 < (time_in_microseconds * tim1_clk_prescale))
            asm volatile("nop");
        // turn off timer
        TCCR1B = 0;
    #endif
}

// TODO:  implement input capture feature

sys_err_e_t tim1_init(tim_config_handle_t * htim1)
{
    
    uint8_t sreg_data = SREG;
    cli();

    // save settings and disable timer
    TCCR1B &= ~TIM_TCCR1B_CS_MSK;   
    uint8_t temp_reg1 = 0;
    uint8_t temp_reg2 = 0;

    // turn on timer module
    PRR &= ~(1<<PRTIM1);

    //sys_err_e_t status = SYS_STATUS_OKAY;
    if(!ASSERT_TIMER_MODE(htim1->waveform_gen_mode))
    {
        return SYS_ERROR_TIM1_INIT_INPUT_INVALID_WGM;
    }
    if(!ASSERT_TIMER_PRESCALER(htim1->clock_prescale))
    {
        return SYS_ERROR_TIM1_INIT_INPUT_INVALID_PRESCALER;
    }
    // configure WGM
    switch(htim1->waveform_gen_mode)
    {
        case TIM_WGM_MODE_0:
        {
            // already configured by default values on reset
        }
        case TIM_WGM_MODE_1:
        {
            temp_reg1 |= (1 << WGM10); 
        }
        case TIM_WGM_MODE_2:
        {
            temp_reg1 |= (1 << WGM11); 
        }
        case TIM_WGM_MODE_3:
        {
            temp_reg1 |= (1 << WGM11) | (1 << WGM10); 
        }
        case TIM_WGM_MODE_4:
        {
            temp_reg2 |= (1 << WGM12);
        }
        case TIM_WGM_MODE_5:
        {
            temp_reg1 |= (1 << WGM10);
            temp_reg2 |= (1 << WGM12);
        }
        case TIM_WGM_MODE_6:
        {
            temp_reg1 |= (1 << WGM11);
            temp_reg2 |= (1 << WGM12);
        }
        case TIM_WGM_MODE_7:
        {
            temp_reg1 |= (1 << WGM11) | (1 << WGM10);
            temp_reg2 |= (1 << WGM12);
        }
        case TIM_WGM_MODE_8:
        {
            temp_reg2 |= (1 << WGM13);
        }
        case TIM_WGM_MODE_9:
        {
            temp_reg1 |= (1 << WGM10);
            temp_reg2 |= (1 << WGM13);
        }
        case TIM_WGM_MODE_10:
        {
            temp_reg1 |= (1 << WGM11);
            temp_reg2 |= (1 << WGM13);
        }
        case TIM_WGM_MODE_11:
        {
            temp_reg1 |= (1 << WGM11) | (1 << WGM10);
            temp_reg2 |= (1 << WGM13);
        }
        case TIM_WGM_MODE_12:
        {
            temp_reg2 |= (1 << WGM13) | (1 << WGM12);
        }
        case TIM_WGM_MODE_14:
        {
            temp_reg1 |= (1 << WGM11);
            temp_reg2 |= (1 << WGM13) | (1 << WGM12);
        }
        case TIM_WGM_MODE_15:
        {
            temp_reg1 |= (1U << WGM20) | (1U << WGM20);
            temp_reg2 |= (1U << WGM22);
        }
        default:
        {
            return SYS_ERROR_TIM1_INIT_INPUT_INVALID_WGM;
        }
    }

    // configure Compare Output Mode for Channel B
    if(TIM_COM_NORMAL == htim1->compare_output_mode_b)
    {
        // default
    }
    else if(TIM_COM_NONPWM_TOGGLE_OCXN_MATCH == htim1->compare_output_mode_b)
    {
        temp_reg1 |= (1 << COM1B0);
    }
    else if((TIM_COM_NONPWM_CLEAR_OCXN_MATCH == htim1->compare_output_mode_b) || \
       (TIM_COM_FAST_PWM_SET_OCXN_BOTTOM_CLEAR_OCXN_MATCH == htim1->compare_output_mode_b) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_DOWN_CLEAR_OCXN_UP == htim1->compare_output_mode_b))
    {
        temp_reg1 |= (1 << COM1B1);
    }
    else if((TIM_COM_NONPWM_SET_OCXN_MATCH == htim1->compare_output_mode_b) || \
       (TIM_COM_FAST_PWM_CLEAR_OCXN_BOTTOM_SET_OCXN_MATCH == htim1->compare_output_mode_b) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN == htim1->compare_output_mode_b))
    {
        temp_reg1 |= (1 << COM1B1) | (1 << COM1B0);
    }

    // configure Compare Output Mode for Channel A
    if(TIM_COM_NORMAL == htim1->compare_output_mode_a)
    {
        // default
    }
    else if((TIM_COM_NONPWM_TOGGLE_OCXN_MATCH == htim1->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_OCXA_TOP_TOGGLE_OCXA_MATCH == htim1->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_TOGGLE_OCXA_MATCH == htim1->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM1A0);
    }
    else if((TIM_COM_NONPWM_CLEAR_OCXN_MATCH == htim1->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_SET_OCXN_BOTTOM_CLEAR_OCXN_MATCH == htim1->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_DOWN_CLEAR_OCXN_UP == htim1->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM1A1);
    }
    else if((TIM_COM_NONPWM_SET_OCXN_MATCH == htim1->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_CLEAR_OCXN_BOTTOM_SET_OCXN_MATCH == htim1->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN == htim1->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM1A1) | (1 << COM1A0);
    }

    // configure internal clock settings.  Leave timer disable.  Only enable on interrupt init or with blocking delay 
    // temp_reg2 |= (htim1->clock_prescale <<CS10);   
    tim1_set_clock_prescaler(htim1->clock_prescale);

        // set macros for time generation
    switch(htim1->clock_prescale)
    {
        case TIM_NO_PRESCALE:
        {
#undef TIM1_CLK_FREQ
#define TIM1_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ)
            break;
        }
        case TIM_DIV_BY_8:
        {
#undef TIM1_CLK_FREQ
#define TIM1_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ/8U)
            break;
        }
        default:
        {
            // invalid clock frequency for microsecond timer if TIM1 clock freq < 1 MHz
            // since clock speed is too slow
            break;
        }
    }

/**
 * @brief TIM1_MAX_ACCURATE_COUNTS_IN_US/_MS is the total number of microseconds/millseconds that can be counted
 *        accurately.  
 * 
 *        Calculations:
 *        1/(Timer1 clock frequency) = period of TIM1 = how many seconds it takes for TIM1 to count 1 tick
 *        ticks of TIM1 to count 1 ms = 1 ms/(period of TIM1)
 *        ticks of TIM1 to count 1 us = 1 us/(period of TIM1)
 *        (resolution of TIM1)/(ticks of TIM1 to count 1 ms) = highest time tim1 can count with accuracy in ms
 *        (resolution of TIM1)/(ticks of TIM1 to count 1 us) = highest time tim1 can count with accuracy in us
 * 
 * @note  The above formulas can be used generally for figuring out these values.  Note that the results should be
 *        integers to be valid values.     
 */

#undef  TIM1_MAX_ACCURATE_COUNTS_IN_US
#undef  TIM1_MAX_ACCURATE_COUNTS_IN_MS
#define TIM1_MAX_ACCURATE_COUNTS_IN_US  (UINT16_MAX/(TIM1_CLK_FREQ))
#define TIM1_MAX_ACCURATE_COUNTS_IN_MS  (UINT16_MAX/(TIM1_CLK_FREQ*SYS_1_MHZ_IN_HZ))

    
    temp_reg1 |= 1 << ICNC1; //  

    // configure settings
    TCCR1A = temp_reg1;
    TCCR1B = temp_reg2; 

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim1_config_interrupt(uint8_t tim1_interrupt_mask_position, _Bool interrupt_state)
{
    if(!ASSERT_TIMER_INTERRUPT_FLAG(tim1_interrupt_mask_position))
    {
        return SYS_ERROR_TIM1_INIT_INPUT_INVALID_INTERRUPT_FLAG;
    }

    uint8_t sreg_data = SREG;
    cli();

    if((TIM_INTERRUPT_OVERFLOW_FLAG == tim1_interrupt_mask_position)||\
       (TIM_INTERRUPT_OUTPUT_CMP_A_FLAG == tim1_interrupt_mask_position)||\
       (TIM_INTERRUPT_OUTPUT_CMP_B_FLAG == tim1_interrupt_mask_position)||\
       (TIM_INTERRUPT_INPUT_CAPTURE_FLAG == tim1_interrupt_mask_position))
    {
        if(interrupt_state)
        {
            TIMSK1 |= (1<<tim1_interrupt_mask_position);
        }
        else
        {
            TIMSK1 &= ~(1<<tim1_interrupt_mask_position);
        }
    }

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim1_set_count(uint16_t tick_count)
{
    uint8_t sreg_data = SREG;
    cli();

    // save clock settings and temporarily turn off timer 
    uint8_t clock_prescaler_setting = TCCR1B & TIM_TCCR1B_CS_MSK;
    TCCR1B &= ~TIM_TCCR1B_CS_MSK;

    TCNT1 =  tick_count;
    
    TCCR1B |= clock_prescaler_setting;

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

inline uint8_t tim1_get_count(void)
{
    return TCNT1;
}

sys_err_e_t tim1_set_ocr1(uint16_t top, uint8_t channel)
{
    //uint8_t sreg_data = SREG;
    //cli();

    if(TIM_CHANNEL_A == channel)
    {
        OCR1A = top;
    }
    else if(TIM_CHANNEL_B == channel)
    {
        OCR1B = top;
    }
    else
    {
        return SYS_ERROR_TIM1_INPUT_INVALID_CHANNEL;
    }
    //SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim1_clear_interrupt_flag(uint8_t flag)
{
    if(TIM_INTERRUPT_OVERFLOW_FLAG == flag)
    {
        TIFR1 |= 1 << TIM_INTERRUPT_OVERFLOW_FLAG;
    }
    else if(TIM_INTERRUPT_OUTPUT_CMP_A_FLAG == flag)
    {
        TIFR1 |= 1 << TIM_INTERRUPT_OUTPUT_CMP_A_FLAG;
    }
    else if(TIM_INTERRUPT_OUTPUT_CMP_B_FLAG == flag)
    {
        TIFR1 |= 1 << TIM_INTERRUPT_OUTPUT_CMP_B_FLAG;
    }
    else if(TIM_INTERRUPT_INPUT_CAPTURE_FLAG == flag)
    {
        TIFR1 |= 1 << TIM_INTERRUPT_INPUT_CAPTURE_FLAG;
    }
    else
    {
        return SYS_ERROR_TIM1_INIT_INPUT_INVALID_INTERRUPT_FLAG;
    }
    return SYS_STATUS_OK;
}

uint16_t tim1_read_icr(void)
{
    return ICR1;
}

void tim1_set_icr(uint16_t top)
{
    uint8_t sreg_data = SREG;
    cli();
    
    ICR1 = top;

    SREG = sreg_data;
}

sys_err_e_t tim1_input_pin_filter_config(_Bool enable)
{   
    uint8_t sreg_data = SREG;
    cli();

    if(enable)
    {
        TCCR1B |= 1<< ICNC1;
    }
    else
    {
        TCCR1B &= ~(1<< ICNC1);
    }

    SREG = sreg_data;
    return SYS_STATUS_OK;
}

sys_err_e_t tim1_input_pin_capture_edge_config(uint8_t edge)
{   
    if(!ASSERT_TIMER1_INPUT_CAPTURE_PIN_EDGE(edge))
    {
        return SYS_ERROR_TIM1_INPUT_INVALID_EDGE;
    }
    uint8_t sreg_data = SREG;
    cli();

    if(TIM_INPUT_CAPTURE_EDGE_SELECT_RISING_EDGE == edge)
    {
        TCCR1B |= 1<< ICNC1;
    }
    else if(TIM_INPUT_CAPTURE_EDGE_SELECT_FALLING_EDGE == edge)
    {
        TCCR1B &= ~(1<< ICNC1);
    }

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim2_init(tim_config_handle_t * htim2)
{
    
    uint8_t sreg_data = SREG;
    cli();

    // save settings and disable timer
    TCCR2B &= ~TIM_TCCR2B_CS_MSK;   
    uint8_t temp_reg1 = 0;
    uint8_t temp_reg2 = 0;

    // turn on timer module
    PRR &= ~(1<<PRTIM2);

    //sys_err_e_t status = SYS_STATUS_OKAY;
    if(!ASSERT_TIMER_MODE(htim2->waveform_gen_mode))
    {
        return SYS_ERROR_TIM2_INIT_INPUT_INVALID_WGM;
    }
    if(!ASSERT_TIMER_PRESCALER(htim2->clock_prescale))
    {
        return SYS_ERROR_TIM2_INIT_INPUT_INVALID_PRESCALER;
    }
    // configure WGM
    switch(htim2->waveform_gen_mode)
    {
        case TIM_WGM_MODE_0:
        {
            // already configured by default values on reset
        }
        case TIM_WGM_MODE_1:
        {
            temp_reg1 |= (1U << WGM20); 
        }
        case TIM_WGM_MODE_4:
        {
            temp_reg1 |= (1U << WGM21);
        }
        case TIM_WGM_MODE_5:
        {
            temp_reg1 |= (1U << WGM20) | (1U << WGM20);
        }
        case TIM_WGM_MODE_11:
        {
            temp_reg1 |= (1U << WGM20);
            temp_reg2 |= (1U << WGM22);
        }
        case TIM_WGM_MODE_15:
        {
            temp_reg1 |= (1U << WGM20) | (1U << WGM20);
            temp_reg2 |= (1U << WGM22);
        }
        default:
        {
            return SYS_ERROR_TIM2_INIT_INPUT_INVALID_WGM;
        }
    }

    // configure Compare Output Mode for Channel B
    if(TIM_COM_NORMAL == htim2->compare_output_mode_b)
    {
        // default
    }
    else if(TIM_COM_NONPWM_TOGGLE_OCXN_MATCH == htim2->compare_output_mode_b)
    {
        temp_reg1 |= (1 << COM2B0);
    }
    else if((TIM_COM_NONPWM_CLEAR_OCXN_MATCH == htim2->compare_output_mode_b) || \
       (TIM_COM_FAST_PWM_SET_OCXN_BOTTOM_CLEAR_OCXN_MATCH == htim2->compare_output_mode_b) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_DOWN_CLEAR_OCXN_UP == htim2->compare_output_mode_b))
    {
        temp_reg1 |= (1 << COM2B1);
    }
    else if((TIM_COM_NONPWM_SET_OCXN_MATCH == htim2->compare_output_mode_b) || \
       (TIM_COM_FAST_PWM_CLEAR_OCXN_BOTTOM_SET_OCXN_MATCH == htim2->compare_output_mode_b) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN == htim2->compare_output_mode_b))
    {
        temp_reg1 |= (1 << COM2B1) | (1 << COM2B0);
    }

    // configure Compare Output Mode for Channel A
    if(TIM_COM_NORMAL == htim2->compare_output_mode_a)
    {
        // default
    }
    else if((TIM_COM_NONPWM_TOGGLE_OCXN_MATCH == htim2->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_OCXA_TOP_TOGGLE_OCXA_MATCH == htim2->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_TOGGLE_OCXA_MATCH == htim2->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM2A0);
    }
    else if((TIM_COM_NONPWM_CLEAR_OCXN_MATCH == htim2->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_SET_OCXN_BOTTOM_CLEAR_OCXN_MATCH == htim2->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_DOWN_CLEAR_OCXN_UP == htim2->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM2A1);
    }
    else if((TIM_COM_NONPWM_SET_OCXN_MATCH == htim2->compare_output_mode_a) || \
       (TIM_COM_FAST_PWM_CLEAR_OCXN_BOTTOM_SET_OCXN_MATCH == htim2->compare_output_mode_a) || \
       (TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN == htim2->compare_output_mode_a))
    {
        temp_reg1 |= (1 << COM2A1) | (1 << COM2A0);
    }

    // configure internal clock settings.  Leave timer disable.  Only enable on interrupt init or with blocking delay 
    // temp_reg2 |= (htim2->clock_prescale <<CS20);   
    tim2_set_clock_prescaler(htim2->clock_prescale);

        // set macros for time generation
    switch(htim2->clock_prescale)
    {
        case TIM_NO_PRESCALE:
        {
#undef TIM2_CLK_FREQ
#define TIM2_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ)
            break;
        }
        case TIM_DIV_BY_8:
        {
#undef TIM2_CLK_FREQ
#define TIM2_CLK_FREQ                   (SYS_CLK_FREQ_IN_MHZ/8U)
            break;
        }
        default:
        {
            // invalid clock frequency for microsecond timer if TIM2 clock freq < 1 MHz
            // since clock speed is too slow
            break;
        }
    }

/**
 * @brief TIM2_MAX_ACCURATE_COUNTS_IN_US/_MS is the total number of microseconds/millseconds that can be counted
 *        accurately.  
 * 
 *        Calculations:
 *        1/(Timer2 clock frequency) = period of TIM2 = how many seconds it takes for TIM2 to count 1 tick
 *        ticks of TIM2 to count 1 ms = 1 ms/(period of TIM2)
 *        ticks of TIM2 to count 1 us = 1 us/(period of TIM2)
 *        (resolution of TIM2)/(ticks of TIM2 to count 1 ms) = highest time tim2 can count with accuracy in ms
 *        (resolution of TIM2)/(ticks of TIM2 to count 1 us) = highest time tim2 can count with accuracy in us
 * 
 * @note  The above formulas can be used generally for figuring out these values.  Note that the results should be
 *        integers to be valid values.     
 */

#undef  TIM2_MAX_ACCURATE_COUNTS_IN_US
#define TIM2_MAX_ACCURATE_COUNTS_IN_US  (UINT8_MAX/(TIM2_CLK_FREQ*SYS_1_MHZ_IN_HZ))

    // configure settings
    TCCR2A = temp_reg1;
    TCCR2B = temp_reg2; 

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

inline sys_err_e_t tim2_config_interrupt(uint8_t tim2_interrupt_mask_position, _Bool interrupt_state)
{
    if(!ASSERT_TIMER_INTERRUPT_FLAG(tim2_interrupt_mask_position))
    {
        return SYS_ERROR_TIM2_INIT_INPUT_INVALID_INTERRUPT_FLAG;
    }

    uint8_t sreg_data = SREG;
    cli();

    if(interrupt_state)
    {
        TIMSK1 |= (1<<tim2_interrupt_mask_position);
    }
    else
    {
        TIMSK1 &= ~(1<<tim2_interrupt_mask_position);
    }

    SREG = sreg_data;
    
    return SYS_STATUS_OK;
}

sys_err_e_t tim2_set_count(uint8_t tick_count)
{
    uint8_t sreg_data = SREG;
    cli();

    // save clock settings and temporarily turn off timer 
    uint8_t clock_prescaler_setting = TCCR2B & TIM_TCCR2B_CS_MSK;
    TCCR2B &= ~TIM_TCCR2B_CS_MSK;

    TCNT2 =  tick_count;
    
    TCCR2B |= clock_prescaler_setting;

    SREG = sreg_data;

    return SYS_STATUS_OK;
}

inline uint8_t tim2_get_count(void)
{
    return TCNT2;
}

sys_err_e_t tim2_set_ocr2(uint8_t top, uint8_t channel)
{
    //uint8_t sreg_data = SREG;
    //cli();

    if(TIM_CHANNEL_A == channel)
    {
        OCR2A = top;
    }
    else if(TIM_CHANNEL_B == channel)
    {
        OCR2B = top;
    }
    else
    {
        return SYS_ERROR_TIM2_INPUT_INVALID_CHANNEL;
    }
    //SREG = sreg_data;

    return SYS_STATUS_OK;
}

sys_err_e_t tim2_clear_interrupt_flag(uint8_t flag)
{
    if(TIM_INTERRUPT_OVERFLOW_FLAG == flag)
    {
        TIFR2 |= 1 << TIM_INTERRUPT_OVERFLOW_FLAG;
    }
    else if(TIM_INTERRUPT_OUTPUT_CMP_A_FLAG == flag)
    {
        TIFR2 |= 1 << TIM_INTERRUPT_OUTPUT_CMP_A_FLAG;
    }
    else if(TIM_INTERRUPT_OUTPUT_CMP_B_FLAG == flag)
    {
        TIFR2 |= 1 << TIM_INTERRUPT_OUTPUT_CMP_B_FLAG;
    }
    else
    {
        return SYS_ERROR_TIM2_INIT_INPUT_INVALID_INTERRUPT_FLAG;
    }
    return SYS_STATUS_OK;
}

 void tim2_usec_delay(uint16_t time_in_microseconds)
{   
    #ifdef TIM2_MAX_ACCURATE_COUNTS_IN_US

        uint8_t clk_prescale = tim2_get_clock_prescaler(); 
        TCCR2B &= clk_prescale;  // turn on timer

        if(TIM2_MAX_ACCURATE_COUNTS_IN_US < time_in_microseconds)
        {
            time_in_microseconds = TIM2_MAX_ACCURATE_COUNTS_IN_US;
        }
        // reset timer for counting
        TCNT2 = 0;  

        // wait for timer to expire
        while(TCNT2 < (time_in_microseconds * clk_prescale))
            asm volatile("nop");
        // turn off timer
        TCCR2B = 0;
    #endif
}