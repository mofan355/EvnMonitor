#ifndef __BH1750_H
#define __BH1750_H

#include "main.h"

extern uint8_t BH1750_Buf[2];
extern float BH1750_LightIntensity;

#define BH1750_ADDR_H 0X5C
#define BH1750_ADDR_L 0X23
#define BH1750_PWER_DOWN 0X00
#define BH1750_PWER_ON 0X01
#define BH1750_RESET 0x07
#define BH1750_CONT_H_RES_MODE 0X10
#define BH1750_CONT_H_RES_MODE2 0X11
#define BH1750_CONT_L_RES_MODE 0X13
#define BH1750_ONE_H_RES_MODE 0X20
#define BH1750_ONE_H_RES_MODE2 0X21
#define BH1750_ONE_L_RES_MODE 0X23
//实际测量时间 = 典型时间 × (MTreg / 69),MTreg的默认值为69
#define BH1750_MESURE_TIME_H(data) (0x40|(data&0x07))
#define BH1750_MESURE_TIME_L(data) (0x60|(data&0x1F))


#define BH1750_Port GPIOB
#define BH1750_SCL_Pin GPIO_PIN_6
#define BH1750_SDA_Pin GPIO_PIN_7

void BH1750_Init_Cmd(void);
void BH1750_W_SCL(uint8_t state);
void BH1750_W_SDA(uint8_t state);
void BH1750_Init(void);
void BH1750_Start(void);
void BH1750_Stop(void);
void BH1750_SendACK(uint8_t ack);
int BH1750_ReceiveACK(void);
void BH1750_SendByte(uint8_t Byte);
void BH1750_ReceiveByte(uint8_t *Byte);
void BH1750_SendCmd(uint8_t addr,uint8_t cmd);
void BH1750_ReceiveData(uint8_t addr,uint8_t *data,uint16_t len);
void Show_BH1750UI(void);
void BH1750Task(void *argument);

#endif
