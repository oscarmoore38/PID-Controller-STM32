#include "../include/hardware_setup.h"
#include "stm32f446xx.h"
#include <stdint.h>

#define TIMER_DISPLAY_INTERVAL 499
#define TIMER_PID_LOOP_INTERVAL 79
#define TIMER_PRESCALER_VALUE 15999
#define PWM_TIMER_PRESCALER_VALUE 15
#define PWM_TIMER_ARR_VALUE 999
#define PWM_TIMER_DUTY_CYCLE 499 // 50% duty cycle by default.  

void hardware_setup(void){
    // ENABLE CLOCKS 

    // Timers 
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN_Msk;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN_Msk;
    RCC->APB2ENR |= RCC_APB2ENR_TIM8EN_Msk;
    RCC->APB2ENR |= RCC_APB2ENR_TIM11EN_Msk; // For loop interval timing
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

    // Set state high for PC1, PC2
    GPIOC->ODR |= GPIO_ODR_OD1_Msk;
    GPIOC->ODR |= GPIO_ODR_OD2_Msk;

    // Set state low for PC0, PC3
    GPIOC->ODR &= ~GPIO_ODR_OD0_Msk;
    GPIOC->ODR &= ~GPIO_ODR_OD3_Msk;

    // GPIO pins for PWM motor driver PA8 PC6 - alternate function mode
    GPIOC->MODER &= ~(GPIO_MODER_MODER6_Msk);
    GPIOA->MODER &= ~(GPIO_MODER_MODER8_Msk);

    GPIOC->MODER |= GPIO_MODER_MODER6_1; 
    GPIOA->MODER |= GPIO_MODER_MODER8_1; 

    // Set AF
    GPIOC->AFR[0] &= ~(GPIO_AFRL_AFSEL6_Msk);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk);

    GPIOC->AFR[0] |= GPIO_AFRL_AFRL6_0; // AF3
    GPIOC->AFR[0] |= GPIO_AFRL_AFRL6_1; 
    GPIOA->AFR[1] |= GPIO_AFRH_AFSEL8_0; // AF1

    //PWM Timers 1/8 CH1
    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M_Msk);
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2; // Enable PWM mode - '110'
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1;
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE_Msk; // Enable Auto preload register
    TIM1->ARR = PWM_TIMER_ARR_VALUE;
    TIM1->CCR1 = PWM_TIMER_DUTY_CYCLE; 
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
    TIM8->CCR1 = PWM_TIMER_DUTY_CYCLE; 
    TIM8->PSC = PWM_TIMER_PRESCALER_VALUE;
    TIM8->CCER |= TIM_CCER_CC1E_Msk; 
    TIM8->BDTR |= TIM_BDTR_MOE_Msk;
    TIM8->EGR |= TIM_EGR_UG_Msk; 
    TIM8->CNT = 0; 
    TIM8->CR1 |= TIM_CR1_CEN_Msk;

}