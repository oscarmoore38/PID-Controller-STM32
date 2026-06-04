#ifndef PID_H
#define PID_H

#include "motor.h"
#include "PID.h"

typedef struct {
    float Kp; 
    float Ki; 
    float Integral; 
} PID;

void PIDControl(Motor *motorObj, PID *PIDObj);

#endif 