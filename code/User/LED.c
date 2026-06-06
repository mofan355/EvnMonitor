#include "LED.h"

void LED_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin=LED_PIN;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_PORT,&GPIO_InitStruct);

    HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
}

void LED_ON(void)
{
    HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);
}

void LED_OFF(void)
{
    HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
}
