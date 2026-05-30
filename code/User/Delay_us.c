#include "Delay_us.h"
#include "tim.h"

void delay_us(uint16_t x)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < x);
}
