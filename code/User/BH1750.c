#include "BH1750.h"
#include "OLED.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "Delay_us.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t BH1750_count=0;
uint8_t BH1750_Buf[2];
float BH1750_LightIntensity = 0;

void BH1750_Init_Cmd(void)
{
    BH1750_SendCmd(BH1750_ADDR_L,BH1750_PWER_ON);
    BH1750_SendCmd(BH1750_ADDR_L,BH1750_ONE_H_RES_MODE);

}

/*begin*/
//硬件i2c
/*

// 主机发送数据给BH1750
void BH1750_SendCmd(uint8_t addr,uint8_t cmd)
{
    if (HAL_I2C_Master_Transmit(&hi2c1, addr<<1, &cmd, 1, 100) != HAL_OK)
    {
        Error_Handler();
    }
}

// 主机从BH1750接收数据
void BH1750_ReceiveData(uint8_t addr,uint8_t *data,uint16_t len)
{
    if (HAL_I2C_Master_Receive(&hi2c1, (addr<<1)|0x01, data, len, 100) != HAL_OK)
    {
        Error_Handler();
    }
}
*/
/*end*/

//软件i2c
void BH1750_W_SCL(uint8_t state)
{
    HAL_GPIO_WritePin(BH1750_Port, BH1750_SCL_Pin, (GPIO_PinState)!!state);
    delay_us(10);
}

void BH1750_W_SDA(uint8_t state)
{
    HAL_GPIO_WritePin(BH1750_Port, BH1750_SDA_Pin, (GPIO_PinState)!!state);
    delay_us(10);
}

void BH1750_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct={0};
    GPIO_InitStruct.Pin = BH1750_SCL_Pin|BH1750_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BH1750_Port, &GPIO_InitStruct);
}

void BH1750_Start(void)
{
    BH1750_W_SDA(1);
    BH1750_W_SCL(1);
    BH1750_W_SDA(0);
    BH1750_W_SCL(0);
}

void BH1750_Stop(void)
{
    BH1750_W_SCL(0);
    BH1750_W_SDA(0);
    BH1750_W_SCL(1);
    BH1750_W_SDA(1);
}

void BH1750_SendACK(uint8_t ack)
{
    BH1750_W_SCL(0);
    BH1750_W_SDA(ack);
    BH1750_W_SCL(1);
    BH1750_W_SCL(0);
}

int BH1750_ReceiveACK(void)
{
    int ack;
    BH1750_W_SCL(0);
    BH1750_W_SDA(1);
    BH1750_W_SCL(1);
    ack=HAL_GPIO_ReadPin(BH1750_Port, BH1750_SDA_Pin);
    BH1750_W_SCL(0);
    return ack;
}

void BH1750_SendByte(uint8_t Byte)
{
    BH1750_W_SCL(0);
    for(int i=0;i<8;i++)
    {
        BH1750_W_SDA(Byte&(0x80>>i));
        BH1750_W_SCL(1);
        BH1750_W_SCL(0);
    }
}

void BH1750_ReceiveByte(uint8_t *Byte)
{
    *Byte=0;
    BH1750_W_SCL(0);
    for(int i=0;i<8;i++)
    {
        BH1750_W_SCL(1);
        *Byte<<=1;
        if(HAL_GPIO_ReadPin(BH1750_Port, BH1750_SDA_Pin))
        {
            *Byte|=0x01;
        }
        BH1750_W_SCL(0);
    }

}

void BH1750_SendCmd(uint8_t addr,uint8_t cmd)
{
    BH1750_Start();
    BH1750_SendByte(addr<<1);
    BH1750_ReceiveACK();
    BH1750_SendByte(cmd);
    BH1750_ReceiveACK();
    BH1750_Stop();
}

void BH1750_ReceiveData(uint8_t addr,uint8_t *data,uint16_t len)
{
    BH1750_Start();
    BH1750_SendByte((addr<<1)|0x01);
    BH1750_ReceiveACK();
    for(int i=0;i<len;i++)
    {
        BH1750_ReceiveByte(&data[i]);
        if(i<len-1) BH1750_SendACK(0);
        else BH1750_SendACK(1);
    }
}

/*
极暗（黑夜 / 月夜）
BH1750_LightIntensity < 10
暗（室内弱光 / 傍晚）
10 ≤ BH1750_LightIntensity < 50
中等（室内正常照明）
50 ≤ BH1750_LightIntensity < 500
亮（窗边 / 室外阴天）
500 ≤ BH1750_LightIntensity < 5000
极亮（晴天直射）
BH1750_LightIntensity ≥ 5000
*/
void Show_BH1750UI(void)
{
    // BH1750_LightIntensity=5000;
    // OLED_ShowNum(56,16,BH1750_count,5,OLED_8X16);
    // return;
    // if(BH1750_LightIntensity==0) return;
    if(BH1750_LightIntensity < 10)
    {
        OLED_ShowString(56, 16, "极  暗",OLED_8X16);
    }
    else if(BH1750_LightIntensity >= 10 && BH1750_LightIntensity < 50)
    {
        OLED_ShowString(56, 16, "较  暗",OLED_8X16);
    }
    else if(BH1750_LightIntensity >= 50 && BH1750_LightIntensity < 500)
    {
        OLED_ShowString(56, 16, "中  等",OLED_8X16);
    }
    else if(BH1750_LightIntensity >= 500 && BH1750_LightIntensity < 5000)
    {
        OLED_ShowString(56, 16, "较  亮",OLED_8X16);
    }
    else
    {
        OLED_ShowString(56, 16, "极  亮",OLED_8X16);
    }
}

extern osEventFlagsId_t KeyFinishedEventGroup;
extern osMutexId_t Mutex1Handle;
void BH1750Task(void *argument)
{
    while(1)
    {
        osEventFlagsWait(KeyFinishedEventGroup,0x01,osFlagsWaitAny|osFlagsNoClear,osWaitForever);
        osMutexAcquire(Mutex1Handle,osWaitForever);
        BH1750_Init_Cmd();
        osDelay(200);
        BH1750_ReceiveData(BH1750_ADDR_L,BH1750_Buf, 2);
        BH1750_LightIntensity = (float)((BH1750_Buf[0]<<8)|BH1750_Buf[1]) / 1.2;
        // BH1750_count++;
        osMutexRelease(Mutex1Handle);
        
        osDelay(4000);
    }
}
