#include "dht11.h"
#include "Atmega328p_SYS.h"
#include "Atmega328p_GPIO.h"
#include <stdbool.h>
#include "lcd_1602a.h"

static void dht11_pin_output_mode(void);
static void dht11_pin_input_mode(void);
static uint8_t dht11_getPinState(void);
static void dht11_pullDataPinLow(void);

void dht11_init(void)
{
    // config GPIO pin to floating HIGH state
    // on atmega328p, input pin is floating by default.  Enabling a pullup or since the dht11 sensor has a pullup on it, we just config as input
    //dht11_pin_input_mode();
}

void dht11_readData(dht_handle_t * h_dht11)
{
    _Bool bResponseReceived = false;
    h_dht11->data.humidity_decimal = 0;
    h_dht11->data.humidity_integral = 0;
    h_dht11->data.temperature_decimal = 0;
    h_dht11->data.temperature_integral = 0;
    h_dht11->data.checksum = 0;
    h_dht11->data.dht11_error = 0;
    
    dht11_pin_output_mode();
    dht11_pullDataPinLow(); 
    usec_delay(18000U);
    dht11_pin_input_mode();
    usec_delay(18U);
    if(dht11_getPinState() == LOW)
    {
        usec_delay(84U);
        if(dht11_getPinState() == HIGH)
        {
            usec_delay(84U);
            bResponseReceived = true;
        }
        else 
            h_dht11->data.humidity_decimal = 254;
    }
    else 
        h_dht11->data.humidity_decimal = 241;
    if(bResponseReceived)
    {
        for(uint8_t i = 8; i > 0; i--)
        {
            if(dht11_getPinState() == LOW)
            {
            usec_delay(54U);
            if(dht11_getPinState() == HIGH)
            {
                    usec_delay(24);
                    h_dht11-> data.humidity_decimal |= 1 << (i - 1);    
                    while(dht11_getPinState() == HIGH)
                    {
                        asm volatile("nop");
                        h_dht11->data.dht11_error = 100;
                    }

            }
            else 
                    h_dht11->data.dht11_error = 199;
            }
            else
                h_dht11->data.dht11_error = 201;
        }
        for(uint8_t i = 8; i > 0; i--)
        {
            if(dht11_getPinState() == LOW)
            {
            usec_delay(54U);
            if(dht11_getPinState() == HIGH)
            {
                    usec_delay(24);
                    h_dht11-> data.humidity_integral |= 1 << (i - 1);    
                    while(dht11_getPinState() == HIGH)
                    {
                        asm volatile("nop");
                        h_dht11->data.dht11_error = 100;
                    }

            }
            else 
                    h_dht11->data.dht11_error = 199;
            }
            else
                h_dht11->data.dht11_error = 201;
        }
    }
}

void dht11_readDataaulh(dht_handle_t * h_dht11)
{   
    _Bool bResponseReceived = false;
    h_dht11->data.humidity_decimal = 0;
    h_dht11->data.humidity_integral = 0;
    h_dht11->data.temperature_decimal = 0;
    h_dht11->data.temperature_integral = 0;
    h_dht11->data.checksum = 0;
    
    dht11_pin_output_mode();
    dht11_pullDataPinLow();
    asm volatile("nop");
    if(dht11_getPinState() == LOW)
        h_dht11->data.humidity_decimal = 240;
    asm volatile("nop");
    //if(dht11_getPinState() == HIGH || dht11_getPinState() == LOW)
    //    h_dht11->data.humidity_decimal = 240;
    //if(dht11_getPinState() == LOW)
    //   h_dht11->data.humidity_decimal = 240;
    usec_delay(18000U);
    //dht11_pin_input_mode();
    usec_delay(10U);
    //if(dht11_getPinState() == LOW)
    if(0)
    {
        usec_delay(84U);
        if(dht11_getPinState() == HIGH)
        {
            usec_delay(84U);
            bResponseReceived = true;
        }
        else 
            h_dht11->data.humidity_decimal = 254;
    }
    //else
    //    h_dht11->data.humidity_decimal = 255;
    if(bResponseReceived)
    {
        for(uint8_t i = 7; i > 0; i--)
        {
            if(dht11_getPinState() == LOW)
            {
                usec_delay(50U);
                if(dht11_getPinState() == HIGH)  
                {
                    usec_delay(28U);
                    if(dht11_getPinState == HIGH)  
                        h_dht11->data.humidity_decimal |= (1 << i);
                    //else
                     //   h_dht11->data.humidity_decimal = 251;
                }
                //else 
                //    h_dht11->data.humidity_decimal = 252;
            }
            //else
           //     h_dht11->data.humidity_decimal = 253;
        }
    }
}

static void dht11_pin_output_mode(void)
{
    pinConfig(DHT11_DATA_PIN, DHT11_DATA_PORT, OUTPUT_PIN, PULLUP_DISABLED);
}

static void dht11_pin_input_mode(void)
{
    // Note: if on-board pullup on DHT11 is not sufficient, enable pull-up
    pinConfig(DHT11_DATA_PIN, DHT11_DATA_PORT, INPUT_PIN, PULLUP_DISABLED);
}

static uint8_t dht11_getPinState(void)
{
    return gpio_getPinState(DHT11_DATA_PIN, DHT11_DATA_PORT);
}

static void dht11_pullDataPinLow(void)
{
    gpio_setPinState(DHT11_DATA_PIN, DHT11_DATA_PORT, LOW);
}