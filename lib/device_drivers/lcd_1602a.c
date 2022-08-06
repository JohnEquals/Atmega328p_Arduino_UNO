#include "lcd_1602a.h"

#include "Atmega328p_I2C.h"
#include <stdint.h>

static uint8_t current_cursor_pos = 0x0U;

static void delay_lcd(void)
{
    for(volatile uint16_t i = 0; i < 10; i++)
    {
        for(volatile uint16_t i = 0; i < 65535; i++)
        {
            asm ("nop");
            asm ("nop");
            asm ("nop");
            asm ("nop");
            asm ("nop");
            asm ("nop");
            asm ("nop");
            asm ("nop");
            asm ("nop");
            asm ("nop");
        }
    }
}

/*
    0x34U, (0x34U & ENABLE_BIT_RESET), \
    0x34U, (0x34U & ENABLE_BIT_RESET), \
    0x34U, (0x34U & ENABLE_BIT_RESET), \
    0x24U, (0x24U & ENABLE_BIT_RESET), \
                                      FUNCTION_SET_NIBBLE_TOP, (FUNCTION_SET_NIBBLE_TOP & ENABLE_BIT_RESET),
                                      FUNCTION_SET_NIBBLE_BOT, (FUNCTION_SET_NIBBLE_BOT & ENABLE_BIT_RESET),\
                                      TURN_DISPLAY_ON_NT, (TURN_DISPLAY_ON_NT & ENABLE_BIT_RESET),\
                                      TURN_DISPLAY_ON_NB, (TURN_DISPLAY_ON_NB & ENABLE_BIT_RESET),\
                                      CLEAR_DISPLAY_NIBBLE_TOP, (CLEAR_DISPLAY_NIBBLE_TOP & ENABLE_BIT_RESET), \
                                      CLEAR_DISPLAY_NIBBLE_BOT, (CLEAR_DISPLAY_NIBBLE_BOT & ENABLE_BIT_RESET),\
                                      ENTRY_MODE_SET_NIBBLE_TOP, (ENTRY_MODE_SET_NIBBLE_TOP & ENABLE_BIT_RESET),\
                                      ENTRY_MODE_SET_NIBBLE_BOT, (ENTRY_MODE_SET_NIBBLE_BOT & ENABLE_BIT_RESET)
                                   
*/

void lcd_init(void)   
{

    uint8_t init_4_bit_mode_seq[] = { \
                                      0x3CU, (0x3CU & ENABLE_BIT_RESET), \  
                                      0x2CU, (0x2CU & ENABLE_BIT_RESET), \
                                      FUNCTION_SET_NIBBLE_TOP, (FUNCTION_SET_NIBBLE_TOP & ENABLE_BIT_RESET),
                                      FUNCTION_SET_NIBBLE_BOT, (FUNCTION_SET_NIBBLE_BOT & ENABLE_BIT_RESET),\
                                      TURN_DISPLAY_ON_NT, (TURN_DISPLAY_ON_NT & ENABLE_BIT_RESET),\
                                      TURN_DISPLAY_ON_NB, (TURN_DISPLAY_ON_NB & ENABLE_BIT_RESET),\
                                      CLEAR_DISPLAY_NIBBLE_TOP, (CLEAR_DISPLAY_NIBBLE_TOP & ENABLE_BIT_RESET), \
                                      CLEAR_DISPLAY_NIBBLE_BOT, (CLEAR_DISPLAY_NIBBLE_BOT & ENABLE_BIT_RESET),\
                                      ENTRY_MODE_SET_NIBBLE_TOP, (ENTRY_MODE_SET_NIBBLE_TOP & ENABLE_BIT_RESET),\
                                      ENTRY_MODE_SET_NIBBLE_BOT, (ENTRY_MODE_SET_NIBBLE_BOT & ENABLE_BIT_RESET)
                                    };
    uint8_t arr[] = {0x3CU, (0x3CU & ENABLE_BIT_RESET)};
    uint8_t seq_len = sizeof(init_4_bit_mode_seq);
    i2c_error_t status = NC;
    status = i2c_master_transmit_data(arr, 2, LCD_ADDR);
    delay_lcd();
    status = i2c_master_transmit_data(arr, 2, LCD_ADDR);
    delay_lcd();
    //for(uint8_t cnt = 0; cnt <  seq_len; cnt = cnt + 2)
    //{
    status = i2c_master_transmit_data(init_4_bit_mode_seq, seq_len, LCD_ADDR);
    //delay_lcd();
    //}
    //delay_lcd();
    //status = i2c_master_transmit_data(((uint8_t *){DISPLAY_CONST_BITS_NT, (DISPLAY_CONST_BITS_NT & ENABLE_BIT_RESET), DISPLAY_CONST_BITS_NB, (DISPLAY_CONST_BITS_NB & ENABLE_BIT_RESET)}), 4, LCD_ADDR);
    //delay_lcd();
    // on reset, wait min 15 ms @ 5V, 40 ms @ 3V
    // write high data nibble 0x3_ 
    // wait min 4.1 ms
    // write high data nibble 0x3_
    // wait min 100 us
    // write data 0x32 to set 4 bit mode 
    // write data 0x2_ to funct set
    // lower nibble either ^ 1_LINE_SMALL_FONT (0x_0)
    // 1_LINE_LARGE_FONT (0x_4)
    // 2_LINE_SMALL_FONT (0x_8)

    // turn on display

    // clear display
    
    // entry mode

}

void lcd_write(uint8_t *data, uint8_t dat_len)  
{
    delay_lcd();
    i2c_error_t status = NC;
    uint8_t byte_data[4] = {0};
    uint8_t lcd_ram_addr[] = { ((current_cursor_pos & TOP_NIBBLE_MSK) | 0x8CU), (((current_cursor_pos & TOP_NIBBLE_MSK) | 0x8CU) & ENABLE_BIT_RESET), \
                               (((current_cursor_pos & BOT_NIBBLE_MSK) << 4) | 0x8CU), ((((current_cursor_pos & BOT_NIBBLE_MSK) << 4) | 0x8CU) & ENABLE_BIT_RESET), \  
                              };
    for(uint8_t i = 0; i < dat_len; i++)
    {
        byte_data[0] = (data[i] & TOP_NIBBLE_MSK) | WRITE_DATA;
        byte_data[1] = byte_data[0] & ENABLE_BIT_RESET;
        byte_data[2] = ((data[i] & BOT_NIBBLE_MSK) << 4) | WRITE_DATA;
        byte_data[3] = byte_data[2] & ENABLE_BIT_RESET;
        status = i2c_master_transmit_data(byte_data, 4, LCD_ADDR);
        //unsigned char buf[40] = {0};
        //uint8_t len_buf = 0;
        //len_buf = sprintf((char*)buf,"Real Data: %u\nI2C Top Nibble Data: %u\n%u\nI2C Bottom Nibble Data: %u\n%u\nLCD_Write Status: %u\n", data[i], byte_data[0] , byte_data[1] , byte_data[2], byte_data[3], (uint8_t)status);
        //uart_sendData(buf, len_buf);
        
        
        //if(0x0U =< current_cursor_pos &&current_cursor_pos < 0x27U)
        //{
        //    current_cursor_pos++;
        //}
        //else if(current_cursor_pos = 0x27U)
        //{
        //    current_cursor_pos = 0x40;
        //}
        //else if()
    }
    delay_lcd();
}

void lcd_clear_display(void)
{
    uint8_t clear_screen_seq[] = { CLEAR_DISPLAY_NIBBLE_TOP, (CLEAR_DISPLAY_NIBBLE_TOP & ENABLE_BIT_RESET), \
                                   CLEAR_DISPLAY_NIBBLE_BOT, (CLEAR_DISPLAY_NIBBLE_BOT & ENABLE_BIT_RESET)};
    uint8_t seq_len = sizeof(clear_screen_seq);
    i2c_error_t status = NC;
    status = i2c_master_transmit_data(clear_screen_seq, seq_len, LCD_ADDR);
    delay_lcd();
}

void lcd_returnCursorHome(void)
{
    uint8_t return_home_seq[] = { RETURN_HOME_NIBBLE_TOP, (RETURN_HOME_NIBBLE_TOP & ENABLE_BIT_RESET), \
                                   RETURN_HOME_NIBBLE_BOT, (RETURN_HOME_NIBBLE_BOT & ENABLE_BIT_RESET)};
    uint8_t seq_len = sizeof(return_home_seq);
    i2c_error_t status = NC;
    status = i2c_master_transmit_data(return_home_seq, seq_len, LCD_ADDR);
    delay_lcd(); 
}

// Set Cursor position.  
void lcd_setCursorPos(uint8_t cursorPos)
{
    cursorPos &= ~(1 << 7); // mask out 7th bit as its unused 
    uint8_t set_cursor_nt = SET_CURSOR_POSITION_NT | (cursorPos & 0x70U);
    uint8_t set_cursor_nb = SET_CURSOR_POSITION_NB | (cursorPos << 4);
    uint8_t set_cursor_pos_seq[] = { set_cursor_nt, (set_cursor_nt & ENABLE_BIT_RESET), \
                                   set_cursor_nb, (set_cursor_nb & ENABLE_BIT_RESET)};
    uint8_t seq_len = sizeof(set_cursor_pos_seq);
    i2c_error_t status = NC;
    status = i2c_master_transmit_data(set_cursor_pos_seq, seq_len, LCD_ADDR);
    delay_lcd(); 
}

