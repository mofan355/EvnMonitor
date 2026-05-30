#include "DHT11.h"
#include "cmsis_os.h"
#include "OLED.h"
#include "Delay_us.h"
#include <stdio.h>

uint8_t count= 0;
uint8_t DHT11_data_buf[5] = {1};

void DHT11_data(GPIO_PinState PinState)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, PinState);
}

void DHT11_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    DHT11_data(GPIO_PIN_SET);
}

void DHT11Start(void)
{
    DHT11_data(GPIO_PIN_RESET);
    osDelay(20);
    DHT11_data(GPIO_PIN_SET);
    delay_us(40);

    uint16_t timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        timeout++;
        delay_us(1);
        if(timeout > 1000) 
        {
            OLED_ShowString(55, 0, "DHT11 wait Error1", OLED_8X16);
            return;
        }
    }
    timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        timeout++;
        delay_us(1);
        if(timeout > 1000) 
        {
            OLED_ShowString(55, 0, "DHT11 wait Error2", OLED_8X16);
            return;
        }
    }
}

void DHT11Receive(uint8_t *buf)
{
    uint8_t t=0;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<8;j++)
        {
            while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET);
            while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET) 
            {
                t++;
                delay_us(1);
                if(t > 1000) 
                {
                    OLED_ShowString(55, 0, "DHT11 Receive Error", OLED_8X16);
                    return;
                }
            }
            if(t>60) buf[i] |= (1 << (7-j));
            else if(t>20) buf[i] &= ~(1 << (7-j));
        }
    }
}

void Show_DHT11UI(void)
{
    // OLED_ShowNum(55,0,count,4,OLED_8X16);
    OLED_Printf(55,0,OLED_8X16,"%d,%d",DHT11_data_buf[0],DHT11_data_buf[2]);
}

/*
��ʪ�ȴ�����DHT11��������
*/
void DHT11Task(void *argument)
{
    while(1)
    {
        DHT11Start();
        DHT11Receive(DHT11_data_buf);
        osDelay(1000);
    }
}
