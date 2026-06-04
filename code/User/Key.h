#ifndef __KEY_H
#define __KEY_H

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

extern uint8_t Key_Num;
void Key_Init(void);
void Key_GetNum(void);
uint8_t Key_GetState(void);

#endif
