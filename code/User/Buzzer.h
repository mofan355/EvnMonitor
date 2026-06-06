#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f1xx_hal.h"

#define Buzzer_PORT GPIOA
#define Buzzer_PIN GPIO_PIN_1

void Buzzer_Init(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);

#endif
