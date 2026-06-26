#include "display.h"
#include "stm32f446xx.h"
#include <string.h>
#include <stdio.h>


void BufferRPMForOutput(float Motor1Rpm, float Motor2Rpm, char* Buf){
    sprintf(Buf, "M1: %d | M2: %d RPM\r\n", (int)Motor1Rpm, (int)Motor2Rpm);

}
