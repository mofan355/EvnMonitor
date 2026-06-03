#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f1xx_hal.h"

#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_1

void DHT11_data(GPIO_PinState PinState);
void DHT11_Init(void);
void DHT11Receive(uint8_t *buf);
void Show_DHT11UI(void);
int DHT11_Check(uint8_t *buf);
void Show_DHT11_AllData(void);

void DHT11Task(void *argument);

#endif
