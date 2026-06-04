#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

typedef struct {
    float MotorRPM; 
    uint8_t Direction; // 0 for reverse, 1 for forward. 
    int16_t PWM; 
    uint8_t RPMSetPoint; 
    float MotorPPR;
    uint8_t PWMTimer; 
} Motor; 

void SetPWM(Motor *MotorObj);

#endif