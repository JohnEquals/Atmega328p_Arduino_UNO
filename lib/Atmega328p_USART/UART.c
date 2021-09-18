#include "UART.h"
#include <stdint.h>

void txConfig(void){
	// set async mode
	UCSR0C &= ~(3 << UMSELN);
	// disable parity mode
	UCSR0C &= ~(3 << UPMN);
	// set only 1 stop bit for Tx
	UCSR0C &= ~(1 << USBSN);
	// set number of data bits (character size) in a frame
	UCSR0C |= CHAR_SIZE_8BIT << UCSZN1_0;
	UCSR0B &= ~(1<< UCSZN2);
	// reset for async mode
	UCSR0C &= ~(1 << UCPOLN);
	// disable DOR, UPE, FE before writing to UCSR0A
	UCSR0A &= ~(7 << RX_FLAGS);
	// set speed mode
	// set double UART tx speed
	UCSR0A |= 1 << U2XN;
	// set baud rate to 9600 bps
	UBRR0 = (uint16_t) BAUD_RATE_9600;

}


// enable Tx mode only after finishing configuring USART
void setTxMode (uint8_t EnableOrDisable){
	if(EnableOrDisable == ENABLE)
		UCSR0B |= 1 << TXENN; 
	else if(EnableOrDisable == DISABLE)
		UCSR0B &= ~(1<< TXENN); 
} 

// send data over UART
void sendData( uint8_t * data, uint8_t len){
	// wait until transmit buffer ready to receive new data
	while(len > 0){
		while(!(UCSR0A & TX_UDREN_FLAG));
		UDR0 = *data;
		data++;
		len = len - 1;
		UCSR0A |= 1<< TXCN;
	}
}
