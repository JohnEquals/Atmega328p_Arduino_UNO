// Blink LED Program
#include <stdio.h>
#include <stdint.h>
#include <Atmega328p_ADC.h>
#include <Atmega328p_GPIO.h>
#include <Atmega328p_TIM.h>
#include <UART.h>
#include <atmega328p_utility.h>
#include "atmega328p_I2C.h"
#include "lcd_1602a.h"
#include "dht11.h"
#include <avr\interrupt.h>
//#include "avr8-stub.h"
//#include "app_api.h" // only needed with flash breakpoints

void test_timer1(void);
void bsp_init(void);
void dht11_test(void);
void button_init(void);
// Register definitions for system clock registers (TODO: Port these over to appropriate library files to configure clock settings)
//#define CLKPR	(*((volatile uint8_t *)0x61))

// PCn pin can be used as ADC
#define PIN7                7
#define PIN6                6
#define PIN5                5
#define PIN2				2
#define PIN1				1
#define PIN0				0

#define VREF_5V				5
#define VREF_3V3			3	

#define OSCCAL_M	((volatile uint8_t *) 0x66U)
#define CLKPR_M		((volatile uint8_t *) 0x61U)
#define UBRR0_M 	((volatile uint16_t *) 0xc4U)

#define LCD_ADDR	((uint8_t)(0x27U))

int main(void)
{ 

	//debug_init();
	// Default clock speed for Arduino UNO is 16 MHz.  We use this API from avrlib to set system 
	// prescale value to 16 which makes our system clock to all peripherals as 1 MHz
	//clock_prescale_set(clock_div_16);
	//unsigned char buffer[40];
	bsp_init();
	//breakpoint();
	//lcd_test1();
	//dht11_test();
	//lcd_write((uint8_t *)"", 17U);
	/*
	pinConfig(PIN2, PORT_B, OUTPUT_PIN);
	OCR1A = 999u;
	TCCR1A = 16U;
	TCCR1B = 9U;
	*/
	//test_timer1();
	//breakpoint();

	uint16_t adcVal, voltage_val;
	//uint8_t len;

	uint32_t blah = 16000000U;
	//len = sprintf((char*)buffer,"OSCCAL: %u\nCLKPR %u\nUBRR0 %u\nblah: %lu\n",*OSCCAL_M,*CLKPR_M, *UBRR0_M, blah);
	//uart_sendData(buffer, len);
	//DDRB |= 1 << PINB1; // Set pin 9 on arduino to output

	/* 1. Set Fast PWM mode 14: set WGM11, WGM12, WGM13 to 1 */
	/* 3. Set pre-scaler of 8 */
	/* 4. Set Fast PWM non-inverting mode */
	//TCCR1A |= (1 << WGM11) | (1 << COM1A1);
	//TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);

	// tim1 freq: 2 MHz
	/* 2. Set ICR1 register: PWM period */
	//ICR1 = 39999;

	/* Offset for correction */
	//int offset = 800;

	/* 5. Set duty cycle */
	//OCR1A = 3000;
	while(1){ 
		//adcVal = adcConvert();
		//len = sprintf((char*)buffer,"The ADC value is: %d\n",adcVal);
		//uart_sendData(buffer, len);
		//voltage_val = adcValueToVoltage(adcVal);  // convert ADC value to voltage
		//len = sprintf((char*)buffer,"Vref: %d\n",voltage_val);
		//uart_sendData(buffer, len);
		//pwm_set_duty_cycle((uint8_t)PWM_DUTY_CYCLE_7_5);
		//my_delay();
		//my_delay();
		//pwm_set_duty_cycle((uint8_t)PWM_DUTY_CYCLE_5);
		//my_delay();
		//my_delay();
		//pwm_set_duty_cycle((uint8_t)PWM_DUTY_CYCLE_10);
		//OCR1A = 3999 + offset;
		if(gpio_getPinState(PIN7, GPIO_PORT_D) == HIGH)
		{
			gpio_setPinState(PIN0, GPIO_PORT_B, HIGH);
		}
		else 
			gpio_setPinState(PIN0, GPIO_PORT_B, LOW);
		//.else
		//	gpio_setPinState(PIN0, PORT_B, LOW);
		my_delay();
		//OCR1A = 1999 - offset;
	}

	return 0;
}

void test_timer1(void)
{
	// initialize OC1A or OC1B pins
	// D9 <- PB1 <- OC1A
	pinConfig(PIN1, PORT_B, OUTPUT_PIN, PULLUP_DISABLED);

	// D10 <- PB2 <- OC1B
	pinConfig(PIN2, PORT_B, OUTPUT_PIN, PULLUP_DISABLED);

	// pwm frequency handle
	tim_config_handle_t htim1 = {0}; 
	htim1.waveform_gen_mode = TIM_WGM_MODE_11;
	//htim1.channel = TIM_CHANNEL_A;
	htim1.compare_output_mode_a = TIM_COM_PHASE_CORRECT_PWM_SET_OCXN_UP_CLEAR_OCXN_DOWN;
	htim1.compare_output_mode_b = TIM_COM_NORMAL;
	htim1.clock_prescale = TIM_DIV_BY_256;
	//htim1.pwm_freq = 50U;

	tim1_init(&htim1);
	//pwm_set_duty_cycle((uint8_t)PWM_DUTY_CYCLE_7_5);
	// pwm duty cycle handle
	//tim1_config_handle_t htim1_duty_cycle = {0}; 
	//htim1_duty_cycle.waveform_gen_mode = MODE_11;
	//htim1.channel = TIM_CHANNEL_B;
	// for f_cpu = 16MHz and Timer1 prescale = 8, f_timer1 = 2 MHz so a TOP value of 40000 produces 50 Hz pwm
	//tim1_set_counter_max_value((uint16_t)10000U - 1,htim1);
}


//void lcd_init(void)
//{

//}

void i2c_test1(void)
{
	//unsigned char buffer[40];
	//uint8_t len;
	i2c_error_t status = NC;
	uint8_t data[] = {};
	uint8_t dataLen = sizeof(data)/sizeof(uint8_t);
	status = i2c_master_transmit_data(data, dataLen, LCD_ADDR);
	//len = sprintf((char*)buffer,"%u\n", (uint8_t)status);
	//uart_sendData(buffer, len);
}


void lcd_test1(void)
{
	uint8_t arr[50];
	uint8_t str_len = sprintf(arr, "LCD O/P Test 1");
	lcd_write((uint8_t *)"DEADBEEF", 8);
	lcd_clear_display();
	lcd_write(arr, str_len);
	my_delay();  
	lcd_clear_display();
	float temperature = 14.4f;
	str_len = sprintf(arr, "Temp data: %f", temperature);
	lcd_write(arr, str_len);
	//lcd_returnCursorHome();
	lcd_clear_display();
	//lcd_write((uint8_t *)"", 17U);
}

// Initialize board support package library
// Info: initialize base peripherlas on mcu and external device drivers
// All GPIO initialization should be handled separately by each peripheral using the device
void bsp_init(void)
{
	//adcConfig();
	uart_txConfig();
	uart_setTxMode(ENABLE);
	//i2c_init();
	//microsec_timer_init();   // init tim1 as a counter

	// device drivers
	//lcd_init();
	//dht11_init();
	button_init();

}


void pins_init(void)
{
	// initialize PWM pins
	// D9 <- PB1 <- OC1A
	//pinConfig(PIN1, PORT_B, OUTPUT_PIN);

	// D10 <- PB2 <- OC1B
	//pinConfig(PIN2, PORT_B, OUTPUT_PIN);


	// init ADC pin
	//PC0 for ADC0
	//pinConfig(PIN0, PORT_C, INPUT_PIN);

}

void dht11_test(void)
{
	dht_handle_t h_dht11 = {0};
	dht11_readData(&h_dht11);
	lcd_clear_display();
	uint8_t arr[50];
	uint8_t str_len = sprintf(arr, "%u   %u.%u", h_dht11.data.dht11_error, h_dht11.data.humidity_decimal, h_dht11.data.humidity_integral);
	lcd_write(arr, str_len);
	
}

void button_init(void)
{
	pinConfig(PIN7, GPIO_PORT_D, INPUT_PIN, PULLUP_DISABLED);
	pinConfig(PIN0, GPIO_PORT_B, OUTPUT_PIN, PULLUP_DISABLED);
}


//ISR(TIMER)