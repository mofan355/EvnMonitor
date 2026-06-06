#include "Servo.h"

extern TIM_HandleTypeDef htim2;

void Servo_Start_CW(void)
{
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,500);
}

void Servo_Start_CounterCW(void)
{
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,2500);
}

void Servo_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,1500);
}
