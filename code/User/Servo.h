#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f1xx_hal.h"

void Servo_Start_CW(void);
void Servo_Start_CounterCW(void);
void Servo_Stop(void);

#endif // !__SERVO_H
