#include "../include/hardware_setup.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdio.h>

// Globals
volatile uint8_t ISRFlag = 0; 
volatile int32_t PulseCountDTMotor1 = 0; 
volatile int32_t PulseCountDTMotor2 = 0;
volatile int32_t PulseCountAllTim2 = 0; 
volatile int32_t PulseCountAllTim5 = 0; 
volatile uint8_t DirectionFlagMotor1 = 0; 
volatile uint8_t DirectionFlagMotor2 = 0; 
int32_t LastPulseCountTim2 = 0; 
int32_t LastPulseCountTim5 = 0; 

// ISR
void TIM1_BRK_TIM9_IRQHandler(void){
    DirectionFlagMotor1 = (TIM2->CR1 & TIM_CR1_DIR_Msk) >> TIM_CR1_DIR_Pos;
    DirectionFlagMotor2 = (TIM5->CR1 & TIM_CR1_DIR_Msk) >> TIM_CR1_DIR_Pos;
    PulseCountAllTim2 = TIM2->CNT; 
    PulseCountAllTim5 = TIM5->CNT; 
    PulseCountDTMotor1 = PulseCountAllTim2 - LastPulseCountTim2; 
    PulseCountDTMotor2 = PulseCountAllTim5 - LastPulseCountTim5; 
    LastPulseCountTim2 = PulseCountAllTim2; 
    LastPulseCountTim5 = PulseCountAllTim5; 
   
    ISRFlag = 1;
    TIM9->SR &= ~(TIM_SR_UIF_Msk);
}

void hardware_setup(void){
    // ENABLE CLOCKS 

    // Timers 
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN_Msk; // For Encoder
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk; // For Encoder
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN_Msk; // For PWM
    RCC->APB2ENR |= RCC_APB2ENR_TIM8EN_Msk; // For PWM
    RCC->APB1ENR |= RCC_APB1ENR_TIM12EN_Msk; // For loop interval timing
    RCC->APB2ENR |= RCC_APB2ENR_TIM9EN_Msk;  // For loop interval timing

    // GPIO
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN_Msk;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN_Msk;

    // I2C
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_Msk;

    // SET MODE AND STATE  

    // GPIO pins for motor driver - general output mode
    GPIOC->MODER &= ~(GPIO_MODER_MODER0_Msk);
    GPIOC->MODER &= ~(GPIO_MODER_MODER1_Msk);
    GPIOC->MODER &= ~(GPIO_MODER_MODER2_Msk);
    GPIOC->MODER &= ~(GPIO_MODER_MODER3_Msk);

    GPIOC->MODER |= GPIO_MODER_MODER0_0; 
    GPIOC->MODER |= GPIO_MODER_MODER1_0; 
    GPIOC->MODER |= GPIO_MODER_MODER2_0; 
    GPIOC->MODER |= GPIO_MODER_MODER3_0; 

    // // Set state high for PC1, PC2
    GPIOC->ODR |= GPIO_ODR_OD1_Msk;
    GPIOC->ODR |= GPIO_ODR_OD2_Msk;

    // // Set state low for PC0, PC3
    GPIOC->ODR &= ~GPIO_ODR_OD0_Msk;
    GPIOC->ODR &= ~GPIO_ODR_OD3_Msk;

    // GPIO pins for Encoder - alternate function mode
    // Tim2 - PB8, PB9
    GPIOB->MODER &= ~(GPIO_MODER_MODER8_Msk); 
    GPIOB->MODER &= ~(GPIO_MODER_MODER9_Msk);

    GPIOB->MODER |= GPIO_MODER_MODER8_1; 
    GPIOB->MODER |= GPIO_MODER_MODER9_1; 

    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk);
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL9_Msk);

    GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_0 ; // AF1
    GPIOB->AFR[1] |= GPIO_AFRH_AFSEL9_0; // AF1 

    //Tim5 - PA0, PA1
    GPIOA->MODER &= ~(GPIO_MODER_MODER0_Msk);
    GPIOA->MODER &= ~(GPIO_MODER_MODER1_Msk);

    GPIOA->MODER |= GPIO_MODER_MODER0_1;
    GPIOA->MODER |= GPIO_MODER_MODER1_1;

    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0_Msk);
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL1_Msk);

    GPIOA->AFR[0] |= GPIO_AFRL_AFRL0_1; // AF2
    GPIOA->AFR[0] |= GPIO_AFRL_AFRL1_1; // AF2

    // GPIO pins for PWM motor driver PA8, PC6 - alternate function mode
    GPIOC->MODER &= ~(GPIO_MODER_MODER6_Msk);
    GPIOA->MODER &= ~(GPIO_MODER_MODER8_Msk);

    GPIOC->MODER |= GPIO_MODER_MODER6_1; 
    GPIOA->MODER |= GPIO_MODER_MODER8_1; 

    // Set AF for PWM 
    GPIOC->AFR[0] &= ~(GPIO_AFRL_AFSEL6_Msk);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk);

    GPIOC->AFR[0] |= GPIO_AFRL_AFRL6_0; // AF3
    GPIOC->AFR[0] |= GPIO_AFRL_AFRL6_1; 
    GPIOA->AFR[1] |= GPIO_AFRH_AFSEL8_0; // AF1


    // PWM Timers 1/8 CH1
    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M_Msk);
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2; // Enable PWM mode - '110'
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1;
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE_Msk; // Enable Auto preload register
    TIM1->ARR = PWM_TIMER_ARR_VALUE;
    TIM1->CCR1 = PWM_DEFAULT_DUTY_CYCLE; 
    TIM1->PSC = PWM_TIMER_PRESCALER_VALUE;
    TIM1->CCER |= TIM_CCER_CC1E_Msk;
    TIM1->BDTR |= TIM_BDTR_MOE_Msk;
    TIM1->EGR |= TIM_EGR_UG_Msk;
    TIM1->CNT = 0; 
    TIM1->CR1 |= TIM_CR1_CEN_Msk; 
    
    TIM8->CCMR1 &= ~(TIM_CCMR1_OC1M_Msk);
    TIM8->CCMR1 |= TIM_CCMR1_OC1M_2; // Enable PWM mode - '110'
    TIM8->CCMR1 |= TIM_CCMR1_OC1M_1;
    TIM8->CCMR1 |= TIM_CCMR1_OC1PE_Msk; // Enable Auto preload register
    TIM8->ARR = PWM_TIMER_ARR_VALUE;
    TIM8->CCR1 = PWM_DEFAULT_DUTY_CYCLE; 
    TIM8->PSC = PWM_TIMER_PRESCALER_VALUE;
    TIM8->CCER |= TIM_CCER_CC1E_Msk; 
    TIM8->BDTR |= TIM_BDTR_MOE_Msk;
    TIM8->EGR |= TIM_EGR_UG_Msk; 
    TIM8->CNT = 0; 
    TIM8->CR1 |= TIM_CR1_CEN_Msk;

    // Interval Timers 
    // PID Timer 
    TIM9->ARR = TIMER_PID_LOOP_INTERVAL;
    TIM9->PSC = TIMER_PRESCALER_VALUE;
    TIM9->CNT = 0; 
    TIM9->EGR |= TIM_EGR_UG_Msk; 
    TIM9->SR &= ~(TIM_SR_UIF_Msk);
    TIM9->DIER |= TIM_DIER_UIE_Msk;
    TIM9->CR1 |= TIM_CR1_CEN_Msk;

    // Set processor to listen for Tim9 overflow event
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

    // Encoder Timers 2/5
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk);
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk);
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0; // IC1 mapped to TI1
    TIM2->CCMR1 |= TIM_CCMR1_CC2S_0; // IC2 mapped to TI2
    TIM2->CCER &=  ~(TIM_CCER_CC1P_Msk);
    TIM2->CCER &= ~(TIM_CCER_CC1NP_Msk);
    TIM2->CCMR1 &= ~(TIM_CCMR1_IC1F_Msk); 
    TIM2->CCER &=  ~(TIM_CCER_CC2P_Msk);
    TIM2->CCER &= ~(TIM_CCER_CC2NP_Msk); 
    TIM2->CCMR1 &= ~(TIM_CCMR1_IC2F_Msk); 
    TIM2->SMCR &= ~(TIM_SMCR_SMS_Msk);
    TIM2->SMCR |= TIM_SMCR_SMS_0; // Count on TI1 & TI2, SMS=011
    TIM2->SMCR |= TIM_SMCR_SMS_1;
    TIM2->CR1 |= TIM_CR1_CEN_Msk;

    TIM5->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk);
    TIM5->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk);
    TIM5->CCMR1 |= TIM_CCMR1_CC1S_0; // IC1 mapped to TI1
    TIM5->CCMR1 |= TIM_CCMR1_CC2S_0; // IC2 mapped to TI2
    TIM5->CCER &=  ~(TIM_CCER_CC1P_Msk);
    TIM5->CCER &= ~(TIM_CCER_CC1NP_Msk);
    TIM5->CCMR1 &= ~(TIM_CCMR1_IC1F_Msk); 
    TIM5->CCER &=  ~(TIM_CCER_CC2P_Msk);
    TIM5->CCER &= ~(TIM_CCER_CC2NP_Msk); 
    TIM5->CCMR1 &= ~(TIM_CCMR1_IC2F_Msk); 
    TIM5->SMCR &= ~(TIM_SMCR_SMS_Msk);
    TIM5->SMCR |= TIM_SMCR_SMS_0; // Count on TI1 & TI2, SMS=011
    TIM5->SMCR |= TIM_SMCR_SMS_1;
    TIM5->CR1 |= TIM_CR1_CEN_Msk;

}

