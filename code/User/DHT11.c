#include "DHT11.h"
#include "cmsis_os.h"
#include "OLED.h"
#include "Delay_us.h"
#include <stdio.h>
#include <string.h>
#include "Key.h"
#include "usart.h"

extern osEventFlagsId_t KeyFinishedEventGroup;

uint8_t DHT11_data_buf[5] = {1};
uint8_t DHT11_AlertLine[4]={70,0,35,0};
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

    uint16_t timeout = 0;
    while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET)
    {
        timeout++;
        delay_us(1);
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
        if(timeout > 1000) 
        {
            OLED_ClearArea(41,22,4,16);
            OLED_ShowString(1,22, "Erro2", OLED_8X16);
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
            if(t>35) buf[i] |= (1 << (7-j));
            else if(t>15) buf[i] &= ~(1 << (7-j));
        }
    }
    DHT11_data(GPIO_PIN_SET);
}

void DHT11_Show_AllData(void)
{
    OLED_Printf(1,16,OLED_8X16,"%d.%d",DHT11_data_buf[0],DHT11_data_buf[1]);
    OLED_Printf(1,32,OLED_8X16,"%d.%d",DHT11_data_buf[2],DHT11_data_buf[3]);
    OLED_Printf(1,48,OLED_8X16,"%d",DHT11_data_buf[4]);
    OLED_Update();
}

int DHT11_Check(uint8_t *buf)
{
    uint16_t sum = buf[0] + buf[1] + buf[2] + buf[3];
    if((sum&0xFF)==buf[4]) return 0;
    else return -1;
}

void Show_DHT11UI(void)
{   
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
            OLED_ShowString(1,23, "DErro", OLED_8X16);
            OLED_ShowString(1,46, "DErro", OLED_8X16);
        }
    }
}

void Show_DHT11UI2(void)
{
    int option=5;
    Key_Num=0;
    OLED_Clear();
    OLED_Update();
    while(1)
    {
        if(Key_GetState())
        {
            Key_GetNum();
        }

        if(Key_Num==1)
        {
            option--;
            if(option<1) option=5;
        }else if(Key_Num==2)
        {
            option++;
            if(option>5) option=1;
        }else if(Key_Num==3)
        {
            Key_Num=0;
            if(option==5) break;
            Show_DHT11_AlterValueUI(option);
        }
        
        OLED_ShowString(7,0,"温度报警线设置",OLED_8X16);
        OLED_Printf(23,16,OLED_8X16,"湿度:%02d.%02d",DHT11_AlertLine[0],DHT11_AlertLine[1]);
        OLED_Printf(23,32,OLED_8X16,"温度:%02d.%02d",DHT11_AlertLine[2],DHT11_AlertLine[3]);
        OLED_ShowImage(101,47,16,16,Return);

        if(option==1)
        {
            OLED_ReverseArea(63,16,16,16);
        }
        else if(option==2)
        {
            OLED_ReverseArea(87,16,16,16);
        }
        else if(option==3)
        {
            OLED_ReverseArea(63,32,16,16);
        }
        else if(option==4)
        {
            OLED_ReverseArea(87,32,16,16);
        }
        else if (option==5)
        {
            OLED_ReverseArea(101,47,16,16);
        }
        OLED_Update();
        Key_Num=0;
        osDelay(100);
    }
}

void Show_DHT11_AlterValueUI(uint8_t option)
{
    OLED_Clear();
    OLED_Update();
    while(1)
    {
        Key_Num=0;
        if(Key_GetState())
        {
            Key_GetNum();
        }

        if(Key_Num==1) DHT11_AlertLine[option-1]++;
        else if(Key_Num==2) 
        {
            DHT11_AlertLine[option-1]--;
            if(DHT11_AlertLine[option-1]==255||DHT11_AlertLine[option-1]>99)
            {
                DHT11_AlertLine[option-1]=0;
            }
        }
        else if(Key_Num==3) 
        {
            Key_Num=0;
            break;
        }

        OLED_ShowString(7,0,"温度报警线设置",OLED_8X16);
        OLED_Printf(23,16,OLED_8X16,"湿度:%02d.%02d",DHT11_AlertLine[0],DHT11_AlertLine[1]);
        OLED_Printf(23,32,OLED_8X16,"温度:%02d.%02d",DHT11_AlertLine[2],DHT11_AlertLine[3]);
        OLED_ShowImage(101,47,16,16,Return);

        if(option==1)
        {
            OLED_ReverseArea(63,16,16,16);
        }
        else if(option==2)
        {
            OLED_ReverseArea(87,16,16,16);
        }
        else if(option==3)
        {
            OLED_ReverseArea(63,32,16,16);
        }
        else if(option==4)
        {
            OLED_ReverseArea(87,32,16,16);
        }
        OLED_Update();

        osDelay(100);
    }
}

void Show_DHT11_AllData(void)
{
    OLED_Printf(50,0,OLED_8X16,"%d,%d",DHT11_data_buf[0],DHT11_data_buf[1]);
    OLED_Printf(50,16,OLED_8X16,"%d,%d",DHT11_data_buf[2],DHT11_data_buf[3]);
    OLED_Printf(50,32,OLED_8X16,"%d",DHT11_data_buf[4]);
    OLED_Update();
}

uint8_t DHT11_GetCondition(void)
{
    if(DHT11_data_buf[0]>DHT11_AlertLine[0]||
        DHT11_data_buf[2]>DHT11_AlertLine[2])
    {
        return 1;
    }
    return 0;
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
        uart_printf_rtos("湿度：%d.%d%%RH,温度：%d.%d℃,校验和：%d\r\n",DHT11_data_buf[0],DHT11_data_buf[1],DHT11_data_buf[2],DHT11_data_buf[3],DHT11_data_buf[4]);
        osMutexRelease(Mutex1Handle);
        osDelay(1000);
    }
}
