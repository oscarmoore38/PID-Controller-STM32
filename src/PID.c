#include "PID.h"
extern float TimeDTSec; 

void PIDControl(Motor *motorObj, PID *PIDObj){

    float error = motorObj->RPMSetPoint - motorObj->MotorRPM; 

    if((motorObj->PWM <= 0 && error < 0) || (motorObj->PWM >= 255 && error > 0)){
        motorObj->PWM += (PIDObj->Kp * error); 
    } else {
        PIDObj->Integral += error * TimeDTSec; 
        motorObj->PWM += (PIDObj->Kp * error) + (PIDObj->Ki * PIDObj->Integral);
    }

    if (motorObj->PWM < 0){
        motorObj->PWM = 0;
    } else if (motorObj->PWM > 255){
        motorObj->PWM = 255; 
    }

}
