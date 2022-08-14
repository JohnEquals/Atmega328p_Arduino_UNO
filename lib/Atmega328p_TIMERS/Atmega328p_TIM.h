#ifndef ATMEGA328P_TIM_H
#define ATMEGA328P_TIM_H

#include "Atmega328p_ADC.h"
#include <avr\io.h>
#include "Atmega328p_SYS.h"
#include "error_handling.h"

// Generic Compare Output Modes (COM)
#define COM_MODE_0  (0)
#define COM_MODE_1  (1U)
#define COM_MODE_2  (2U)
#define COM_MODE_3  (3U)

// COMxA/COMxB modes where x = 0,1 and n = A,B
#define TIM_COM_NORMAL                       COM_MODE_0  

#define TIM_COM_NONPWM_TOGGLE_OCXN_MATCH          COM_MODE_1
#define TIM_COM_NONPWM_CLEAR_OCXN_MATCH           COM_MODE_2
#define TIM_COM_NONPWM_SET_OCXN_MATCH             COM_MODE_3

#define TIM_COM_FAST_PWM_OCXA_TOP_TOGGLE_OCXA_MATCH         COM_MODE_1 // normal port operation if WGM02 = 0
#define TIM_COM_FAST_PWM_SET_OCXN_BOTTOM_CLEAR_OCXN_MATCH   COM_MODE_2
#define TIM_COM_FAST_PWM_CLEAR_OCXN_BOTTOM_SET_OCXN_MATCH   COM_MODE_3

#define TIM_COM_PHASE_CORRECT_PWM_TOGGLE_OCXA_MATCH            COM_MODE_1 // normal port operation if WGM02 = 0
#define TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_DOWN_CLEAR_OCXN_UP  COM_MODE_2  // clear/set on match
#define TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN  COM_MODE_3  // clear/set on match


// Timerx Waveform Generation Modes (bitfield values for WGMx1_0)
#define TIM_WGM_MODE_0  (0U)  // Normal; TOP = 0xFFFF
#define TIM_WGM_MODE_1  (1U)  // PWM, phase correct, 8-bit; TOP = 0x00FF
#define TIM_WGM_MODE_2  (2U)  // PWM, phase correct, 9-bit; TOP = 0x01FF
#define TIM_WGM_MODE_3  (3U)  // PWM, phase correct, 10-bit; TOP = 0x03FF
#define TIM_WGM_MODE_4  (4U)  // CTC; TOP = OCRxA
#define TIM_WGM_MODE_5  (5U)  // Fast PWM, 8-bit; TOP = 0x00FF
#define TIM_WGM_MODE_6  (6U)  // Fast PWM, 9-bit; TOP = 0x01FF
#define TIM_WGM_MODE_7  (7U)  // Fast PWM, 10-bit; TOP = 0x03FF
#define TIM_WGM_MODE_8  (8U)  // PWM, phase and frequency correct; TOP = ICRx
#define TIM_WGM_MODE_9  (9U)  // PWM, phase and frequency correct; TOP = OCRxA
#define TIM_WGM_MODE_10 (10U) // PWM, phase correct; TOP = ICRx
#define TIM_WGM_MODE_11 (11U) // PWM, phase correct; TOP = OCRxA
#define TIM_WGM_MODE_12 (12U) // CTC; TOP = ICRx

#define TIM_WGM_MODE_14 (14U) // Fast PWM; TOP = ICRx
#define TIM_WGM_MODE_15 (15U) // Fast PWM; TOP = OCRxA

#define WGM_BITS_1_0_MSK (0x3U) // use this mask to extract bits 1-0 from above WGM bitfield values
#define WGM_BITS_3_2_MSK (0xCU) // use this mask to extract bits 3-2 from above WGM bitfield values

// bitfield values for WGMx3_2
#define MODES_3_0   (0U)
#define MODES_7_4   (1U)
#define MODES_11_8  (2U)
#define MODES_15_12 (3U)
// Timer Channel
#define TIM_CHANNEL_A   (0)
#define TIM_CHANNEL_B   (1U)

// Clock Select bit (CSx0/CSx1) values
#define TIM_NO_PRESCALE	    1U
#define TIM_DIV_BY_8		2U
#define TIM_DIV_BY_64		3U
#define TIM_DIV_BY_256		4U
#define TIM_DIV_BY_1024	    5U
		
// Timer range
#define TIMER_8BIT_MAX_VALUE (255U)
#define TIMER_8BIT_MIN_VALUE (0U)
#define TIMER_16BIT_MAX_VALUE (65535U)
#define TIMER_16BIT_MIN_VALUE (0U)


// timer interrupt flags
#define TIM_INTERRUPT_INPUT_CAPTURE_FLAG    (5)
#define TIM_INTERRUPT_OUTPUT_CMP_B_FLAG     (2)
#define TIM_INTERRUPT_OUTPUT_CMP_A_FLAG     (1)
#define TIM_INTERRUPT_OVERFLOW_FLAG         (0)

// register bitfield bitmasks
#define TIM_TCCR0B_CS_MSK    ((1 << CS02) | (1 << CS01) | (1 << CS00))
#define TIM_TCCR1B_CS_MSK    ((1 << CS12) | (1 << CS11) | (1 << CS10))
#define TIM_TCCR2B_CS_MSK    ((1 << CS22) | (1 << CS21) | (1 << CS20))

#define TIM_INPUT_CAPTURE_EDGE_SELECT_FALLING_EDGE  (0)
#define TIM_INPUT_CAPTURE_EDGE_SELECT_RISING_EDGE   (1)


typedef struct 
{
    uint16_t waveform_gen_mode      :4;
    //uint16_t channel                :1;  
    uint16_t compare_output_mode_a  :2;
    uint16_t compare_output_mode_b  :2;
    uint16_t clock_prescale         :3;   
    uint16_t reserved               :5;
    uint16_t pwm_freq               ;
    uint8_t duty_cycle              ;
}tim_config_handle_t;


// Timer1 Macros
#define TIM1_MAX_TIMER_TICKS (32767U) // Valid for TIM1 CLK Freq = 2 MHz.  Equal to UINT16_MAX/2 

// API Prototypes

//TODO: update APIs
sys_err_e_t tim0_init(tim_config_handle_t * htim0);
sys_err_e_t tim0_config_interrupt(uint8_t tim0_interrupt_mask_position, _Bool interrupt_state);
sys_err_e_t tim0_set_count(uint8_t tick_count);
uint8_t tim0_get_count(void);
sys_err_e_t tim0_set_ocr0(uint8_t top, uint8_t channel);
sys_err_e_t tim0_clear_interrupt_flag(uint8_t flag);


sys_err_e_t tim1_init(tim_config_handle_t * htim1);
sys_err_e_t tim1_config_interrupt(uint8_t tim1_interrupt_mask_position, _Bool interrupt_state);
sys_err_e_t tim1_set_count(uint16_t tick_count);
inline uint8_t tim1_get_count(void);
sys_err_e_t tim1_set_ocr1(uint16_t top, uint8_t channel);
sys_err_e_t tim1_clear_interrupt_flag(uint8_t flag);
uint16_t tim1_read_icr(void);
void tim1_set_icr(uint16_t top);
sys_err_e_t tim1_input_pin_filter_config(_Bool enable);
sys_err_e_t tim1_input_pin_capture_edge_config(uint8_t edge);

/**
 * @brief Initializes Timer2 
 * 
 * @param htim2 - handler for user-defined timer2 configuration settings
 * @return sys_err_e_t 
 */
sys_err_e_t tim2_init(tim_config_handle_t * htim2);
inline sys_err_e_t tim2_config_interrupt(uint8_t tim2_interrupt_mask_position, _Bool interrupt_state)
sys_err_e_t tim2_set_count(uint8_t tick_count);
inline uint8_t tim2_get_count(void);
sys_err_e_t tim2_set_ocr2(uint8_t top, uint8_t channel);
sys_err_e_t tim2_clear_interrupt_flag(uint8_t flag);
#endif