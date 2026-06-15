#include "MQ2.h"
#include "adc.h"
#include "cmsis_os.h"
#include "OLED.h"
#include "Key.h"
#include "usart.h"

extern osEventFlagsId_t KeyFinishedEventGroup;

uint32_t MQ2_ADC_Data=0;
float MQ2_Data=0;
float MQ2_AlertLine=2.5f;

void MQ2_GetData(void)
{
    MQ2_ADC_Data = ADC_GetValue();

    MQ2_Data = (float)MQ2_ADC_Data/4096.0f*3.3;
}

/*
0.5V ~ 1.0V	纯净空气、无燃气、无烟雾	气敏电阻 最大，导电最弱，环境非常干净
1.0V ~ 2.0V	正常室内空气	日常室内轻微杂味，无泄漏
2.0V ~ 3.0V	轻微烟雾 / 微量燃气	开始有气体
3.0V ~ 4.0V	明显燃气 / 烟雾	泄漏明显，报警
4.0V ~ 5.0V	高浓度燃气 / 浓烟	危险浓度，严重泄漏
*/
void Show_MQ2UI(void)
{
    // OLED_ShowFloatNum(56,50,MQ2_Data,2,2,OLED_8X16);
    // return;
    if(MQ2_Data < 0.15f)
    {
        OLED_ShowString(68,50,"无气体",OLED_8X16);
    }
    else if(MQ2_Data>=0.15f && MQ2_Data < 3.0f)
    {
        OLED_ShowString(68,50,"正  常",OLED_8X16);
    }
    else if(MQ2_Data>=3.0f && MQ2_Data < 4.0f)
    {
        OLED_ShowString(68,50,"异  常",OLED_8X16);
    }
    else if(MQ2_Data >= 4.0f)
    {
        OLED_ShowString(68,50,"危  险",OLED_8X16);
    }
}

void Show_MQ2UI2(void)
{
    int option=3;
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
            if(option<1) option=3;
        }else if(Key_Num==2)
        {
            option++;
            if(option>3) option=1;
        }else if(Key_Num==3)
        {
            Key_Num=0;
            if(option==3) break;
            Show_MQ2_AlterValueUI(option);
        }
        
        OLED_ShowString(7,0,"烟雾报警线设置",OLED_8X16);
        OLED_Printf(43,28,OLED_8X16,"%.1f",MQ2_AlertLine);
        OLED_ShowImage(101,47,16,16,Return);

        if(option==1)
        {
            OLED_ReverseArea(43,28,8,16);
        }
        else if(option==2)
        {
            OLED_ReverseArea(59,28,8,16);
        }
        else if (option==3)
        {
            OLED_ReverseArea(101,47,16,16);
        }
        OLED_Update();
        Key_Num=0;
        osDelay(100);
    }
}

void Show_MQ2_AlterValueUI(uint8_t option)
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

        if(Key_Num==1) 
        {
            if(option==1) 
            {
                if(MQ2_AlertLine>=9) MQ2_AlertLine-=9;
                else MQ2_AlertLine+=1;
            }
            else if(option==2) 
            {
                MQ2_AlertLine+=0.1f;
            }
        }
        else if(Key_Num==2) 
        {
            if(option==1) 
            {
                if(MQ2_AlertLine>=1) MQ2_AlertLine-=1;
            }
            else if(option==2) 
            {
                if(MQ2_AlertLine>0) MQ2_AlertLine-=0.1f;
            }
        }
        else if(Key_Num==3) 
        {
            Key_Num=0;
            break;
        }

        OLED_ShowString(7,0,"烟雾报警线设置",OLED_8X16);
        OLED_Printf(43,28,OLED_8X16,"%.1f",MQ2_AlertLine);
        OLED_ShowImage(101,47,16,16,Return);

         if(option==1)
        {
            OLED_ReverseArea(43,28,8,16);
        }
        else if(option==2)
        {
            OLED_ReverseArea(59,28,8,16);
        }
        OLED_Update();

        osDelay(100);
    }
}

uint8_t MQ2_GetCondition(void)
{
    if(MQ2_Data>MQ2_AlertLine) return 1;
    return 0;
}

extern osMutexId_t Mutex1Handle;
void MQ2Task(void *argument)
{
    while(1)
    {
        osEventFlagsWait(KeyFinishedEventGroup,0x01,osFlagsWaitAny|osFlagsNoClear,osWaitForever);
        osMutexAcquire(Mutex1Handle,osWaitForever);
        MQ2_GetData();
        uart_printf_rtos("烟雾浓度：%.2fv\r\n",MQ2_Data);
        osMutexRelease(Mutex1Handle);
        osDelay(1000);
    }
}
