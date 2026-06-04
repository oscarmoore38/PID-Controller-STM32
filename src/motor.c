#include "motor.h"
#include "stm32f446xx.h"
#include "hardware_setup.h"

void SetPWM(Motor *motorObj){
    // calculate ARR value 
    uint16_t PWMTimerDutyCycle = (uint16_t)(((float)(motorObj->PWM)/255) * PWM_TIMER_ARR_VALUE);

    if (motorObj->PWMTimer == 1){
        TIM1->CCR1 = PWMTimerDutyCycle;
    } else if(motorObj->PWMTimer == 8){
        TIM8->CCR1 = PWMTimerDutyCycle; 
    }
}

