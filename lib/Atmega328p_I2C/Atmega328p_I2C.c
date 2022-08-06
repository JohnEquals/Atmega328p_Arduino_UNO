#include "Atmega328p_I2C.h"


#define READ_MODE   ((0x01U))
#define WRITE_MODE    (~(0x01U))

void i2c_init(void) //i2c_config_t * hi2c
{
    /* SCL (serial clock pin) frequency = (CPU clock frequency) / (16 + 2 * (value of TWI bit rate register) * (TWI bit rate prescaler) )       
     *Use the above formula to set TWBR register to get desired SCL frequency.  
     */
    // configure SCL freq to 100 KHz (set prescale = 1, TWI bit rate = 32)
    I2C->twsr &= ~TWPS; 
    I2C->twsr |= TWPS_PRESCALER_1;   // set prescale

    I2C->twbr = 72U;
}


//  TODO: add a timeout feature to enable i2c to break out of these infinite loops.  The timeout should be based on 
//         internal timer and last longer than the max time for the TWINT to be set.
//  Note:  Use this when ATMEGA328P mcu is master device. slaveAddr should be the 7-bit slave address occupying b6-b0.  
i2c_error_t i2c_master_transmit_data(uint8_t *data, uint8_t len, uint8_t slaveAddr)
{
    // assert
    //i2c_error_t status = SUCCESS;
    
    // prepare address message for transmission.  b0 is read/write bit.  b1-b7 are slave (destination) address.
    uint8_t addrByte = (slaveAddr << 1) & WRITE_MODE;

        
    // 1. generate START condition to start I2C tx    
    I2C->twsr &= TWSTO_SLAVE_RELEASE_BUS;
    I2C->twcr |= TWSTA_MASTER_MODE | TWEN_ENABLE_I2C | TWINT_CLEAR_INT_FLAG;

    // wait for START condition to tx 
    while(!(I2C->twcr & TWINT))
    {
        asm volatile("NOP");
    }
    // check TWSR for START error code
    if(MASTER_TX_STATUS_START_TX != GET_TWI_STATUS())
    {
        return ERROR_1;
    }





    // 2. send address byte
    I2C->twdr = addrByte;   // place address byte onto the shift register to prepare for tx
    I2C->twcr &= TWSTA_SLAVE_MODE & TWSTO_SLAVE_RELEASE_BUS;
    I2C->twcr |= TWEN_ENABLE_I2C | TWINT_CLEAR_INT_FLAG;

    // wait for ACK from slave
    while(!(I2C->twcr & TWINT))
    {
        asm volatile("NOP");
    }
    // CHECK TWSR for address byte sent + ACK received
    if(MASTER_TX_STATUS_SLA_W_ACK_RCV != GET_TWI_STATUS())
    {
        return ERROR_2;
    }




    // send data 
    for(uint8_t dataIdx = 0; dataIdx < len; dataIdx++)
    {
        while(!I2C_READY_FOR_DATA)
        {
            asm volatile("NOP");
        }   
        
        //if(I2C_READY_FOR_DATA)
        //{
        I2C->twdr = *data++;
        I2C->twcr |= TWINT_CLEAR_INT_FLAG | TWEN_ENABLE_I2C; 

        // wait for ACK from slave
        while(!(I2C->twcr & TWINT))
        {
            asm volatile("NOP");
        }

        // check TWSR for if data was sent + ACK received
        if(MASTER_TX_STATUS_DATA_BYTE_ACK_RCV != GET_TWI_STATUS())
        {
            return ERROR_3;
        }



        //}
    }

    // send STOP condition
    I2C->twcr |= TWINT_CLEAR_INT_FLAG | TWEN_ENABLE_I2C | TWSTO_MASTER_GENERATE_STOP_BIT; 
    return SUCCESS;
}


i2c_error_t i2c_master_receive_data(uint8_t *dataBuffer, uint8_t len)
{
    // generate 
}


// TODO: make interrupt-based I2C