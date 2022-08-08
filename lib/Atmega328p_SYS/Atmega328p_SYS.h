#ifndef ATMEGA328P_SYS_H
#define ATMEGA328P_SYS_H

#include <avr\io.h>

#define SYS_CLK_FREQ_IN_MHZ    (16U)

#define SYS_CLK_FREQ    SYS_CLK_FREQ_IN_MHZ

#define DISABLE_INTERRUPTS      (do{uint8_t sreg_data = SREG; \
                                cli(); \
                                }while(0))

#define ENABLE_INTERRUPTS       (do{uint8_t sreg_data = SREG; \
                                cli(); \
                                }while(0))


// DHT11 data pin - PB0 - D8
#define DHT11_DATA_PIN  (0)         // Port D4-D9
#define DHT11_DATA_PORT (PORT_B)




#endif