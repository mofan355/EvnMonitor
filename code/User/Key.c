#include "Key.h"
#include "Delay_us.h"

uint8_t Key_Num = 0;

void Key_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

extern EventGroupHandle_t KeyFinishedEventGroup;
void Key_GetNum()
{
    xEventGroupClearBits(KeyFinishedEventGroup, 1<<0);
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
        delay_us(20000);
        while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET);
        delay_us(20000);
        Key_Num = 1;
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET)
    {
        delay_us(20000);
        while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET);
        delay_us(20000);
        Key_Num = 2;
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        delay_us(20000);
        while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET);
        delay_us(20000);
        Key_Num = 3;
    }
    xEventGroupSetBits(KeyFinishedEventGroup, 1<<0);
}

uint8_t Key_GetState()
{
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET||
       HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET||
       HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        return 1;
    }
    else return 0;
}
