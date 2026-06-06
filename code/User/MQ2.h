#ifndef __MQ2_H
#define __MQ2_H

#include "stm32f1xx_hal.h"

extern uint32_t MQ2_ADC_Data;
extern float MQ2_Data;

void MQ2_GetData(void);
void Show_MQ2UI(void);
void Show_MQ2UI2(void);
void Show_MQ2_AlterValueUI(uint8_t option);
void MQ2Task(void *argument);

#endif
