#ifndef __LED_H
#define __LED_H

#include "stm32f1xx_hal.h"

#define LED_PORT GPIOA
#define LED_PIN GPIO_PIN_0

void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);

#endif // !__LED_H
