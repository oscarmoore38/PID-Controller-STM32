#include <stdint.h> 
#include "stm32f446xx.h"
#include "hardware_setup.h"

volatile uint32_t delay; 

int main(void) {
   
    hardware_setup();

    while(1){

    }

    // while(1){
    //     // ODR - Setting state on pin - High or Low. 
    //     if ((TIM5->SR & TIM_SR_UIF_Msk) == 1){
    //         TIM5->SR &= ~(TIM_SR_UIF_Msk);
    //         GPIOA->ODR ^= GPIO_ODR_OD5_Msk;

    //     }

    // }
    return 0;
}