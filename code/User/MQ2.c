#include "MQ2.h"
#include "adc.h"
#include "cmsis_os.h"
#include "OLED.h"
#include "Key.h"

extern osEventFlagsId_t KeyFinishedEventGroup;

uint8_t MQ2_count=0;
uint32_t MQ2_ADC_Data=0;
float MQ2_Data=0;

void MQ2_GetData(void)
{
    MQ2_ADC_Data = ADC_GetValue();

    MQ2_Data = (float)MQ2_ADC_Data/4096.0f*3.3;
}

/*
0.5V ~ 1.0V	纯净空气、无燃气、无烟雾	气敏电阻 最大，导电最弱，环境非常干净
1.0V ~ 2.0V	正常室内空气	日常室内轻微杂味，无泄漏
2.0V ~ 3.0V	轻微烟雾 / 微量燃气	开始有气体，可设为预警线
3.0V ~ 4.0V	明显燃气 / 烟雾	泄漏明显，报警
4.0V ~ 5.0V	高浓度燃气 / 浓烟	危险浓度，严重泄漏
*/
void Show_MQ2UI(void)
{
    // OLED_ShowFloatNum(56,50,MQ2_Data,2,2,OLED_8X16);
    // OLED_ShowNum(56,50,MQ2_count,5,OLED_8X16);
    // return;
    if(MQ2_Data < 0.5f)
    {
        OLED_ShowString(56,50,"无气体",OLED_8X16);
    }
    else if(MQ2_Data>=0.5f && MQ2_Data < 3.0f)
    {
        OLED_ShowString(56,50,"正  常",OLED_8X16);
    }
    else if(MQ2_Data>=3.0f && MQ2_Data < 4.0f)
    {
        OLED_ShowString(56,50,"异  常",OLED_8X16);
    }
    else if(MQ2_Data >= 4.0f)
    {
        OLED_ShowString(56,50,"危  险",OLED_8X16);
    }
}

extern osMutexId_t Mutex1Handle;
void MQ2Task(void *argument)
{
    while(1)
    {
        osEventFlagsWait(KeyFinishedEventGroup,0x01,osFlagsWaitAny|osFlagsNoClear,osWaitForever);
        osMutexAcquire(Mutex1Handle,osWaitForever);
        MQ2_GetData();
        // MQ2_count++;
        osMutexRelease(Mutex1Handle);
        osDelay(4000);
    }
}
