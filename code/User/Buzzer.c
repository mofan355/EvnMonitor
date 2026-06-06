#include "Buzzer.h"

void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin =Buzzer_PIN;
    GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(Buzzer_PORT,&GPIO_InitStruct);

    HAL_GPIO_WritePin(Buzzer_PORT,Buzzer_PIN,GPIO_PIN_SET);
}

void Buzzer_ON(void)
{
    HAL_GPIO_WritePin(Buzzer_PORT,Buzzer_PIN,GPIO_PIN_RESET);
}

void Buzzer_OFF(void)
{
    HAL_GPIO_WritePin(Buzzer_PORT,Buzzer_PIN,GPIO_PIN_SET);
}
