/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "DHT11.h"
#include "MQ2.h"
#include "BH1750.h"
#include "stdio.h"
#include "LED.h"
#include "Buzzer.h"
#include "Servo.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t OLED_FlashTaskHandle;
const osThreadAttr_t OLED_FlashTask_attributes = {
  .name = "OLED_FlashTask",
  .stack_size = 512 * 2,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t DHT11TaskHandle;
const osThreadAttr_t DHT11Task_attributes = {
  .name = "DHT11Task",
  .stack_size = 256 * 2,
  .priority = (osPriority_t) osPriorityNormal2,
};

osThreadId_t MQ2TaskHandle;
const osThreadAttr_t MQ2Task_attributes = {
  .name = "MQ2Task",
  .stack_size = 512 * 2,
  .priority = (osPriority_t) osPriorityNormal2,
};

osThreadId_t BH1750TaskHandle;
const osThreadAttr_t BH1750Task_attributes = {
  .name = "BH1750Task",
  .stack_size = 512 * 2,
  .priority = (osPriority_t) osPriorityNormal2,
};

osThreadId AlertTaskHandle;
const osThreadAttr_t AlertTask_attributes={
  .name="AlertTask",
  .stack_size =256*2,
  .priority =(osPriority_t)osPriorityNormal,
};

osEventFlagsId_t KeyFinishedEventGroup;
const osEventFlagsAttr_t KeyFinishedEventGroup_attributes={
  .name="KeyFinishedEventGroup",
  .attr_bits=0,
};


osMutexId_t Mutex1Handle;
const osMutexAttr_t Mutex1_attributes ={
  .name="Mutex1",
  .attr_bits=osMutexPrioInherit,
};

UBaseType_t oled_watermark;
UBaseType_t dht11_watermark;
UBaseType_t mq2_watermark;
UBaseType_t bh1750_watermark;
UBaseType_t alert_watermark;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 2,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void AlertTask(void *argument)
{
  while(1)
  {
    if(DHT11_GetCondition()+MQ2_GetCondition()+BH1750_GetCondition()>1)
      {
        LED_ON();
        Buzzer_ON();
        Servo_Start_CW();
        osDelay(100);

        Buzzer_OFF();
        LED_OFF();
      }
      else Servo_Stop();

      osDelay(100);
  }
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  Mutex1Handle=osMutexNew(&Mutex1_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  OLED_FlashTaskHandle = osThreadNew(OLED_FlashTask, NULL, &OLED_FlashTask_attributes);
  DHT11TaskHandle = osThreadNew(DHT11Task, NULL, &DHT11Task_attributes);
  MQ2TaskHandle = osThreadNew(MQ2Task, NULL, &MQ2Task_attributes);
  BH1750TaskHandle = osThreadNew(BH1750Task,NULL, &BH1750Task_attributes);
  AlertTaskHandle=osThreadNew(AlertTask,NULL,&AlertTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  KeyFinishedEventGroup = osEventFlagsNew(&KeyFinishedEventGroup_attributes);
  osEventFlagsSet(KeyFinishedEventGroup,0x01);
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    oled_watermark = uxTaskGetStackHighWaterMark(OLED_FlashTaskHandle);
    dht11_watermark = uxTaskGetStackHighWaterMark(DHT11TaskHandle);
    mq2_watermark = uxTaskGetStackHighWaterMark(MQ2TaskHandle);
    bh1750_watermark = uxTaskGetStackHighWaterMark(BH1750TaskHandle);
    alert_watermark =uxTaskGetStackHighWaterMark(AlertTaskHandle);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

