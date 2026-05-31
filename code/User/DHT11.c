#include "DHT11.h"
#include "cmsis_os.h"
#include "OLED.h"
#include "Delay_us.h"
#include <stdio.h>
#include <string.h>

uint8_t count= 0;
uint8_t DHT11_data_buf[5] = {1};
GPIO_InitTypeDef GPIO_InitStructure;

void DHT11_data(GPIO_PinState PinState)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, PinState);
}

void DHT11_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
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
            OLED_ShowString(55,23, "Error1", OLED_8X16);
            OLED_Update();
            return;
        }
    }
    timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        timeout++;
        delay_us(1);
        if(timeout > 200) 
        {
            OLED_ShowString(55,23, "Error2", OLED_8X16);
            OLED_Update();
            return;
        }
    }
}

void DHT11Receive(uint8_t *buf)
{
    memset(DHT11_data_buf, 0, sizeof DHT11_data_buf);
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<8;j++)
        {
            uint8_t t=0;
            while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET);
            while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET) 
            {
                t++;
                delay_us(1);
                if(t > 200) 
                {
                    OLED_ShowString(65,23, "Error3", OLED_8X16);
                    OLED_Update();
                    return;
                }
            }
            if(t>40) buf[i] |= (1 << (7-j));
            else if(t>26) buf[i] &= ~(1 << (7-j));
        }
    }
    DHT11_data(GPIO_PIN_SET);
}

int DHT11_Check(uint8_t *buf)
{
    uint16_t sum = buf[0] + buf[1] + buf[2] + buf[3];
    if(sum < buf[4]+3 && sum > buf[4]-3) return 0;
    else return -1;
}

void Show_DHT11UI(void)
{
    OLED_ShowNum(100,50,count,2,OLED_8X16);
    if(DHT11_Check(DHT11_data_buf) == 0)
    {
        OLED_Printf(1,46,OLED_8X16,"%d.%d%%",DHT11_data_buf[0],DHT11_data_buf[1]);
        OLED_Printf(1,23,OLED_8X16,"%d.%d",DHT11_data_buf[2],DHT11_data_buf[3]);
        OLED_ShowImage(33, 23, 12, 16, Celsius);
    }
    else 
    {
        OLED_ShowString(1,23, "Error", OLED_8X16);
        OLED_ShowString(1,46, "Error", OLED_8X16);
    }
}

void Show_DHT11_AllData(void)
{
    OLED_Printf(0,0,OLED_8X16,"%d,%d",DHT11_data_buf[0],DHT11_data_buf[1]);
    OLED_Printf(0,16,OLED_8X16,"%d,%d",DHT11_data_buf[2],DHT11_data_buf[3]);
    OLED_Printf(0,32,OLED_8X16,"%d",DHT11_data_buf[4]);
    OLED_Update();
}

void DHT11Task(void *argument)
{
    while(1)
    {
        DHT11Start();
        DHT11Receive(DHT11_data_buf);
        count++;
        osDelay(3000);
    }
}
