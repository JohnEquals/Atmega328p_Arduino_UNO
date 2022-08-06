#ifndef LCD_1602A_H
#define LCD_1602A_H

#include<stdint.h>
#include "uart.h"
#include <stdio.h>

#define LCD_ADDR	((uint8_t)(0x27U))

#define TOP_NIBBLE_MSK  (0xF0U)
#define BOT_NIBBLE_MSK  (0x0FU)
/*
 *  
 * Important Note: this library should only be used for 1602a (16x2) LCD with the HiLetGo I2C backpack connected.  
 *                 It only supports 4-bit mode.  It has the following data bit to pin mapping:
 *                        _______________________________________
 *   I2C data byte ->    | b7 | b6 | b5 | b4 | b3| b2  | b1 | b0 |
 *   LCD pin       ->    |     Date Nibble   | K | CS/E| RW | RS |
 * 
 *  
 * 
 *   Glossary:
 *   Data Nibble - corresponds to either the data written to pins D7-D4 or D3-D0 on LCD.  
 *   K - controls the emitter of NPN transistor which controls state of K pin (cathode).  Keep this bit value to 1 to keep backlight ON. 
 *   CS/E - Chip Select/Enable pin
 *   RW - Read/Write bit
 *   RS - Register select          
 * 
 * 
 * 
 *  Important Usage notes:
 *  - The I2C backpack (AnalogTek AT8574+2018 chip <- chinese IC which is controlled similarly to TI PCF8574 IC) 
 *  - When initially sending any data or command to the LCD, while the enable bit maintains the same value (either set or reset), the first
 *    data byte sent over i2c writes the data in the top nibble to pins D7-D4.  A subsequent data byte transmitted will write the top nibble to
 *    pins D3-D0.  Make sure to write the same byte of data twice with the enable bit set initially and then reset on the later byte.  
 *  - To get the LCD to register an instruction or data, you must write the same I2C data twice, with the Enable bit set on the 1st byte,
 *    and the enable bit reset on the 2nd byte.  Once you've essentially "toggled" the enable bit in this manner, this resets the next 
 *    I2C data write to write to D7-D4.  
 *  - Based on the above procedure, in 4-bit mode, you must send 4 bytes of data over I2C per instruction/data
 *  - Generate a sufficient delay between each write to the LCD pins as this process takes quite some time due to the time needed to tx the data over
 *    I2C, the time for the I/O expander to put the data on the 8 LCD I/O pins, and the time for the LCD to register the commands after each enable pin toggle. 
 */



// use this to configure the bottom nibble of i2c data byte for the following lcd commands
#define INSTRUCTION_MODE    (~(0x01U))    // use to bitwise OR with the i2c data byte
#define DATA_MODE           (0x01U)

#define READ_MODE           (0x02U)
#define WRITE_MODE          (~(0x02U))

#define ENABLE_BIT_SET      (0x04U)
#define ENABLE_BIT_RESET     (~(0x04U))

// for all commands, must send top nibble of data in first i2c byte followed by bottom nibble
#define CLEAR_DISPLAY_DATA  (0x01U) // clear display
#define RETURN_HOME_DATA    (0x02U) // return cursor to home position


// entry mode set.  Configures how cursor/display shifts as data comes in
#define CONFIG_SHIFT_CURSOR_LEFT_DATA  (0x04) // shifts cursor to left
#define CONFIG_SHIFT_CURSOR_RIGHT_DATA  (0x06U) // shifts cursor to right
#define CONFIG_SHIFT_DISPLAY_LEFT_DATA  (0x07U) // shifts display to left
#define CONFIG_SHIFT_DISPLAY_RIGHT_DATA (0x05U) // shifts display to left



/*
 *  LCD Commands 
 *  Description:  Send these directly to the LCD over I2C to config/control LCD
 */
 
 #define CLEAR_DISPLAY_TOP  ( (uint8_t)((CLEAR_DISPLAY_DATA & TOP_NIBBLE_MSK) << 0)\
                              & WRITE_MODE & INSTRUCTION_MODE & ENABLE_BIT_SET )
 #define CLEAR_DISPLAY_BOT  ( (uint8_t)((CLEAR_DISPLAY_DATA & BOT_NIBBLE_MSK) << 4)\
                              & WRITE_MODE & INSTRUCTION_MODE & ENABLE_BIT_SET )

#define CLEAR_DISPLAY_NIBBLE_TOP    (0x0CU)
#define CLEAR_DISPLAY_NIBBLE_BOT    (0x1CU)
 

#define RETURN_HOME_TOP  ( (uint8_t)((RETURN_HOME_DATA  & TOP_NIBBLE_MSK) << 0)\
                              & WRITE_MODE & INSTRUCTION_MODE & ENABLE_BIT_SET )
#define RETURN_HOME_BOT  ( (uint8_t)((RETURN_HOME_DATA & BOT_NIBBLE_MSK) << 4)\
                              & WRITE_MODE & INSTRUCTION_MODE & ENABLE_BIT_SET )

#define RETURN_HOME_NIBBLE_TOP    (0x0CU)
#define RETURN_HOME_NIBBLE_BOT    (0x24U)

#define ENTRY_MODE_SET_NIBBLE_TOP    (0x0CU)
#define ENTRY_MODE_SET_NIBBLE_BOT    (0x7CU)//((uint8_t)((CONFIG_SHIFT_CURSOR_RIGHT_DATA) << 4 ))
// TODO

#define FUNCTION_SET_NIBBLE_TOP (0x2CU)
#define FUNCTION_SET_NIBBLE_BOT (0x8CU)

// NB: Nibble Bottom, NT: Nibble Top

#define TURN_DISPLAY_ON_NT    (0x0CU)  // turn on display
#define TURN_DISPLAY_ON_NB    (0xFCU)  // turn on display


#define WRITE_DATA  (0x0DU) 

// Set cursor position top nibble and bottom nibbles
#define SET_CURSOR_POSITION_NT  (0x8CU)
#define SET_CURSOR_POSITION_NB  (0x0CU)

// Display Configuration Instruction

#define DISPLAY_CONST_BITS_NT   (0x0CU) // first byte to send
#define DISPLAY_CONST_BITS_NB   (0x8CU) // 2nd byte to send
// Display settings.  Use on DISPLAY_CONST_BITS_NB
#define TURN_ON_DISPLAY    (0x40U)
#define TURN_OFF_DISPLAY     (~(0x40U)

#define TURN_ON_CURSOR    (0x20U)
#define TURN_OFF_CURSOR    (~(0x20U))
#define TURN_ON_BLINK     (0x10U)
#define TURN_OFF_BLINK     (~(0x10U))




/*
 *  init i2c data
 */  



void lcd_init(void);
void lcd_write(uint8_t * data, uint8_t dat_len);
void lcd_clear_display(void);
void lcd_returnCursorHome(void);
void lcd_setCursorPos(uint8_t cursorPos);
#endif