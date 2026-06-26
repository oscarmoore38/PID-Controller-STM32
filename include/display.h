#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

typedef struct {
    char buf[32]; 
    char* Msg; 
    volatile uint8_t idx; 
} UartXfer; 


void BufferRPMForOutput(float Motor1Rpm, float Motor2Rpm, char* Buf);


#endif