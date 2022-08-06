./lib/interface

This folder defines the wrappers that interface the low-level on-chip peripherals to the device drivers.
Ideally the low-level driver design should enable flexibility to modify the mcu chip/develop board with little to no modifications
needed for the interface layer.  The upper layers (device drivers, application, RTOS, etc) should not require any modifications
as the point of the BSP is to produce HAL APIs (HW abstraction layers) to implement common HW features specific to the mcu/PCBA and the 



/*
    Toggle data pin LOW
    Wait at least 18 ms
    Reinitialize data pin as floating input pin (should pull HIGH due to pull-up) 
    Wait at least 40 us for dht11 response
    if data pin LOW, wait 80 us
        if data pin HIGH, wait 80 us
            set bResponse = true
    if bResponse
        while(not all data was read from data pin)
            if data pin LOW, wait 50 us
                if data pin HIGH
                    reset timer
                    getCurrentTimerTick and store into lastTime variable
                    bDataReceived = true
                    while(data pin is HIGH)
                        do nothing (has a NOP instruction)
                    getCurrentTimerTick and store into currentTime variable 
                if currentTime - lastTime > more than 40 us
                    store data bit = 1
                else 
                    store data bit = 0
            

  
 */