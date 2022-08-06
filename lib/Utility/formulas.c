#include "formulas.h"
#include <stdint.h>





uint16_t map(uint16_t value, uint16_t y_min, uint16_t y_max, uint16_t x_min, uint16_t x_max)
{

    uint16_t output = 0;
    if((y_max > y_min)&&(x_max > x_min))
    {
        if((x_max >= value) && (x_min <= value))
        {
            uint16_t slope = (y_max - y_min)/(x_max - x_min);
            output = slope*( value - x_min) + y_min;
            // point slope formula 
        }
    }  
    return output;  
}