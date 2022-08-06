#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <Arduino_UNO_sys.h>

// registers
#define UDR0  	(*((volatile uint8_t *) 0xc6U))  // data register
#define UBRR0H  (*((volatile uint8_t *) 0xc5U))
#define UBRR0L	(*((volatile uint8_t *) 0xc4U))
#define UBRR0	(*((volatile uint16_t *) 0xc4U))
#define UCSR0C	(*((volatile uint8_t *) 0xc2U))
#define UCSR0B	(*((volatile uint8_t *) 0xc1U))
#define UCSR0A	(*((volatile uint8_t *) 0xc0U))
// bitfield definitions
#define U2XN			1
#define TXENN			3
#define UMSELN			6
#define UPMN			4
#define USBSN			3
#define UCSZN1_0		1
#define UCSZN2			2
#define CHAR_SIZE_8BIT	3
#define UCPOLN			0
#define RX_FLAGS		2
#define TXCN			6
#define UDREN			5
// Mode
#define USART_ASYNC_MODE	(0)
// Baud Rates when U2Xn = 1 (async mode)
#define SYS_CLK_1MHZ_BAUD_RATE_9600  (12U) // this value is only valid when system clock is 1 MHz (after clock division) 

#if(SYSTEM_CLK_FREQ_MHZ == 16U)
#define SYS_CLK_16MHZ_BAUD_RATE_9600 (207U)
#endif 

// general
#define ENABLE  (1)
#define DISABLE (0)

#define TX_UDREN_FLAG	(1<< UDREN)
#define TX_READY		(1<<TXCN)

// API Prototypes
void uart_txConfig(void);
void uart_setTxMode(uint8_t EnableOrDisable);
void uart_sendData( uint8_t * data, uint8_t len);


#endif