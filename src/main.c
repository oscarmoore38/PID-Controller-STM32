#include <stdint.h> 
#include "stm32f446xx.h"
#include "hardware_setup.h"
#include "motor.h"
#include "PID.h"
#include "display.h"
#include <stdlib.h>

extern volatile uint8_t ISRFlag; 
extern volatile int32_t PulseCountDTMotor1; 
extern volatile int32_t PulseCountDTMotor2; 
extern volatile uint8_t DirectionFlagMotor1; 
extern volatile uint8_t DirectionFlagMotor2;
extern volatile uint8_t ISRFlagDisplay; 
uint16_t MotorPPR12vDC = 1632;

float TimeDTSec = 0.08; 
Motor Motor1; 
Motor Motor2;
PID PIDMotor1; 
PID PIDMotor2; 
UartXfer UartXferObj;  

// ISRs

void USART2_IRQHandler (void){
    if (USART2->SR & USART_SR_TXE) {
        if (UartXferObj.buf[UartXferObj.idx] != '\0'){
            USART2->DR = UartXferObj.buf[UartXferObj.idx++]; 
        } else {
            USART2->CR1 &= ~(USART_CR1_TXEIE);
        }
    }
    
}

int main(void) {
    int32_t PulseCountDTMotor1Local = 0; 
    int32_t PulseCountDTMotor2Local = 0; 
    Motor1.MotorPPR = MotorPPR12vDC;
    Motor2.MotorPPR = MotorPPR12vDC; 
    Motor1.RPMSetPoint = 150; 
    Motor2.RPMSetPoint = 75; 
    Motor1.PWM = 127; 
    Motor2.PWM = 127; 
    PIDMotor1.Kp = 0.5;
    PIDMotor1.Ki = 0.05; 
    PIDMotor2.Kp = 0.5;
    PIDMotor2.Ki = 0.05;
    Motor1.PWMTimer = 1; 
    Motor2.PWMTimer = 8;
    UartXferObj.Msg = UartXferObj.buf; 
   
    hardware_setup();

    while(1){
        // PID Interval 
        if (ISRFlag == 1){
            NVIC_DisableIRQ(TIM1_BRK_TIM9_IRQn);
            PulseCountDTMotor1Local = PulseCountDTMotor1;
            PulseCountDTMotor2Local = PulseCountDTMotor2;
            ISRFlag = 0; 
            NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

            Motor1.MotorRPM = ((float)abs(PulseCountDTMotor1Local) / Motor1.MotorPPR) * (60.0f / TimeDTSec);
            Motor2.MotorRPM = ((float)abs(PulseCountDTMotor2Local) / Motor2.MotorPPR) * (60.0f / TimeDTSec);

            PIDControl(&Motor1, &PIDMotor1); 
            PIDControl(&Motor2, &PIDMotor2); 

            SetPWM(&Motor1);
            SetPWM(&Motor2);

            }
        
        if (ISRFlagDisplay == 1){
            NVIC_DisableIRQ(TIM8_BRK_TIM12_IRQn);
            ISRFlagDisplay = 0; 
            NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);

            // copy current RPM val to output buffer, set iterator to 0, and enable TXE interrupt in USART2
            BufferRPMForOutput(Motor1.MotorRPM, Motor2.MotorRPM, UartXferObj.buf);
            UartXferObj.idx = 0; 
            USART2->CR1 |= USART_CR1_TXEIE;

        }
    }


    return 0;
}