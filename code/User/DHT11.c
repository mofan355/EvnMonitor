#include "DHT11.h"
#include "cmsis_os.h"
#include "OLED.h"
#include "Delay_us.h"
#include <stdio.h>
#include <string.h>
#include "event_groups.h"

extern osEventFlagsId_t KeyFinishedEventGroup;

uint8_t DHT11_count= 0;
uint8_t DHT11_data_buf[5] = {1};
GPIO_InitTypeDef GPIO_InitStructure;

void DHT11_data(GPIO_PinState PinState)
{
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, PinState);
}

static void DHT11_OUTMode(void)
{
    GPIO_InitStructure.Pin = DHT11_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

static void DHT11_INMode(void)
{
    GPIO_InitStructure.Pin = DHT11_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

void DHT11_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    DHT11_OUTMode();
    DHT11_data(GPIO_PIN_SET);
}

void DHT11Start(void)
{
    DHT11_data(GPIO_PIN_RESET);
    osDelay(20);
    DHT11_data(GPIO_PIN_SET);
    delay_us(40);

    DHT11_INMode();

    uint16_t timeout = 0;
    while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET)
    {
        timeout++;
        delay_us(1);
        OLED_ShowNum(1,23, timeout, 4, OLED_8X16);
        OLED_Update();
        if(timeout > 1000) 
        {
            OLED_ShowString(1,7, "Err1", OLED_8X16);
            OLED_Update();
            return;
        }
    }
    timeout = 0;
    while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
    {
        timeout++;
        delay_us(1);
        if(timeout > 200) 
        {
            OLED_ClearArea(41,22,4,16);
            OLED_ShowString(1,22, "Erro2", OLED_8X16);
            OLED_Update();
            return;
        }
        // OLED_ShowNum(1,23, timeout, 4, OLED_8X16);
        // OLED_Update();
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
            while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET);
            while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) 
            {
                t++;
                delay_us(1);
                if(t > 200) 
                {
                    OLED_ShowString(1,43, "Erro3", OLED_8X16);
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
    // OLED_ShowNum(1,22,DHT11_count,5,OLED_8X16);
    // return;
    if(DHT11_data_buf[0]+DHT11_data_buf[1]>0)
    {
        if(DHT11_Check(DHT11_data_buf) == 0)
        {
            OLED_Printf(1,22,OLED_8X16,"%d.%d",DHT11_data_buf[2],DHT11_data_buf[3]);
            OLED_Printf(1,43,OLED_8X16,"%d.%d%%",DHT11_data_buf[0],DHT11_data_buf[1]);
            OLED_ShowImage(33, 22, 12, 16, Celsius);
        }
        else 
        {
            OLED_ShowString(1,23, "Error", OLED_8X16);
            OLED_ShowString(1,46, "Error", OLED_8X16);
        }
    }
}

void Show_DHT11_AllData(void)
{
    OLED_Printf(50,0,OLED_8X16,"%d,%d",DHT11_data_buf[0],DHT11_data_buf[1]);
    OLED_Printf(50,16,OLED_8X16,"%d,%d",DHT11_data_buf[2],DHT11_data_buf[3]);
    OLED_Printf(50,32,OLED_8X16,"%d",DHT11_data_buf[4]);
    OLED_Update();
}

extern osMutexId_t Mutex1Handle;
void DHT11Task(void *argument)
{
    while(1)
    {
        osEventFlagsWait(KeyFinishedEventGroup,0x01,osFlagsWaitAny|osFlagsNoClear,osWaitForever);
        osMutexAcquire(Mutex1Handle,osWaitForever);
        DHT11Start();
        DHT11Receive(DHT11_data_buf);
        // DHT11_count++;
        osMutexRelease(Mutex1Handle);
        osDelay(4000);
    }
}
