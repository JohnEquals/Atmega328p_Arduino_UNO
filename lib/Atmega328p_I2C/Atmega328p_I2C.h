#ifndef ATMEGA328P_I2C_H
#define ATMEGA328P_I2C_H

#include <stdint.h>

// on Atmega328p,  PC5 - SCL, PC4 - SDA



// User(application) specific macros 

// object holds user-specified settings for i2c
typedef struct 
{
    uint8_t speed;  // speed of the bus.  

}i2c_config_t;


#define I2C_SPEED_100KHZ    // 
#define I2C_SPEED_400KHZ





// MCU specific macros.  USER SHOULD NOT MODIFY.


/*
 *      I2C (TWI = 2-wire interface) Register Definition 
 */

typedef struct 
{
    volatile uint8_t twbr;
    volatile uint8_t twsr;
    volatile uint8_t twar;
    volatile uint8_t twdr;
    volatile uint8_t twcr;    
    volatile uint8_t twamr;
}i2c_reg_def_t;

typedef enum
{
    SUCCESS = 0,
    ERROR_1,
    ERROR_2,
    ERROR_3,
    ERROR_I2C_NOT_IN_MASTER_MODE,
    ERROR_I2C_NOT_IN_SLAVE_MODE,
    NC
}i2c_error_t;

// I2C Object Handler Macros

#define I2C_BASE_ADDR   (0xB8)  // 
#define I2C     ((i2c_reg_def_t *)I2C_BASE_ADDR)

/* 
 *     I2C Register Bitfield Definitions
 *
 *     Use these bitmasks to check the bitfield configurations 
 *     ex:  I2C-> twbr & TWINT returns the setting for TWINT bitfield
 */

//TWSR
#define TWS     (0xF8U)
#define TWPS    (0x03U)

//

/* 
 *     I2C Register Bitfield configurations
 */

//TWCR
#define TWINT_CLEAR_INT_FLAG    (0x80U)
#define TWEA_ENABLE_ACK_BIT     (0x40U)
#define TWEA_DISABLE_ACK_BIT    (~(0x40U))
#define TWSTA_MASTER_MODE       (0x20U) 
#define TWSTA_MASTER_GENERATE_START_BIT (TWSTA_MASTER_MODE)  
#define TWSTA_SLAVE_MODE        (~(0x20U))
#define TWSTO_MASTER_GENERATE_STOP_BIT (0x10U)
#define TWSTO_SLAVE_RELEASE_BUS        (~(0x10U))
#define TWEN_ENABLE_I2C         (0x04U)
#define TWEN_DISABLE_I2C        (~(0x04U))
#define TWIE_ENABLE_INT         (0x01U)
#define TWIE_DISABLE_INT        (~(0x01U))

#define MASTER_MODE     (1U)
#define SLAVE_MODE      (0U)

//TWSR
#define TWPS_PRESCALER_1 (0x00U)  // Note:  please clear bitfield before setting
#define TWPS_PRESCALER_4 (0x01U) 
#define TWPS_PRESCALER_16 (0x02U) 
#define TWPS_PRESCALER_64 (0x03U) 

#define TWSR_PRESCALER_BITMASK  (0x03U)
/*
 *  I2C Communication Status Codes  
 */

// Mode: Master Tx 
#define MASTER_TX_STATUS_START_TX             (0x08U)  // A START condition has been transmitted
#define MASTER_TX_STATUS_REPEAT_START_TX      (0x10U)  // A repeated START condition has been mtransmitted
#define MASTER_TX_STATUS_SLA_W_ACK_RCV        (0x18U)  // SLA+W has been transmitted; ACK has been received
#define MASTER_TX_STATUS_SLA_W_NACK_RCV       (0x20U)  // SLA+W has been transmitted; NOT ACK has been received
#define MASTER_TX_STATUS_DATA_BYTE_ACK_RCV    (0x28U)  // Data byte has been transmitted; ACK has been received
#define MASTER_TX_STATUS_DATA_BYTE_NACK_RCV    (0x30U)  // Data byte has been transmitted; NACK has been received
#define MASTER_TX_STATUS_ARBITRATION_LOST     (0x38U)  // Arbitration lost in SLA+W or data bytes            

// Mode: Master Rcv
// START_TX, REPEAT_START_TX, and ARBITRATION_LOST for Master Rcv are taken from Master Tx status codes
#define MASTER_RCV_STATUS_START_TX             (0x08U)
#define MASTER_RCV_STATUS_REPEAT_START_TX      (0x10U)
#define MASTER_RCV_STATUS_ARBITRATION_LOST     (0x38U)
#define MASTER_RCV_STATUS_SLA_R_ACK_RCV        (0x40U)
#define MASTER_RCV_STATUS_SLA_R_NACK_RCV       (0x48U)
#define MASTER_RCV_STATUS_DATA_BYTE_ACK_RCV    (0x50U)
#define MASTER_RCV_STATUS_DATA_BYTE_NACK_RCV   (0x58U)


/*
 *  SCL (serial clock pin) frequency = (CPU clock frequency) / (16 + 2 * (value of TWI bit rate register) * (TWI bit rate prescaler) )       
 *  Use the above formula to set TWBR register to get desired SCL frequency.  
 *  Note:  SCL frequency (and capacitive bus line load) impacts the pull-up resistor value.  
 *  
 *  Pullup resistor for I2C:   Per datasheet, 
 *  For f_SCL <=  100 kHz, (VCC - 0.4C)/ 3mA < R_pullup < (1000ns)/(Capacitance of bus line)
 *  For f_SCL  >  100 kHz, (VCC - 0.4C)/ 3mA < R_pullup < (300ns)/(Capacitance of bus line)
 *  Max capacitance of each I/O pin = 10 pF 
 *  SCL Frequency Max = 400 kHz (per I2C backpack datasheet).  This max frequency is true for this I2C driver
 */

#define GET_TWI_STATUS()    (I2C->twsr & TWS)
#define I2C_READY_FOR_DATA  (I2C->twcr & TWINT)


i2c_error_t i2c_master_transmit_data(uint8_t *data, uint8_t len, uint8_t slaveAddr);
void i2c_init(void);

#endif