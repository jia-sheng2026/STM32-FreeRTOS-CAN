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
#include "semphr.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "can_frame.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* 外部声明：在 main.c 中定义的 CAN 和 UART 句柄 */
extern CAN_HandleTypeDef hcan;
extern UART_HandleTypeDef huart1;

typedef struct {
    uint32_t id;
    uint8_t data[8];
} CAN_Frame_t;

/* 队列句柄（在中断中发送，在任务中接收） */
osMessageQueueId_t CanFrameQueueHandle;
/* 打印队列 */
osMessageQueueId_t PrintStringQueueHandle;

/* USER CODE END Variables */

/* Definitions for Data_Proc_Task */
osThreadId_t Data_Proc_TaskHandle;
const osThreadAttr_t Data_Proc_Task_attributes = {
  .name = "Data_Proc_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UART_Print_Task */
osThreadId_t UART_Print_TaskHandle;
const osThreadAttr_t UART_Print_Task_attributes = {
  .name = "UART_Print_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* 队列定义属性 */
const osMessageQueueAttr_t CanFrameQueue_attributes = {
  .name = "CanFrameQueue"
};
const osMessageQueueAttr_t PrintStringQueue_attributes = {
  .name = "PrintStringQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartData_Proc_Task(void *argument);
void StartUART_Print_Task(void *argument);

void MX_FREERTOS_Init(void);

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_QUEUES */
  CanFrameQueueHandle = osMessageQueueNew(10, sizeof(CAN_Frame_t), &CanFrameQueue_attributes);
  PrintStringQueueHandle = osMessageQueueNew(20, 64, &PrintStringQueue_attributes);
  /* USER CODE END RTOS_QUEUES */

  /* 创建任务（不再需要 CAN_Rx_Task） */
  Data_Proc_TaskHandle = osThreadNew(StartData_Proc_Task, NULL, &Data_Proc_Task_attributes);
  UART_Print_TaskHandle = osThreadNew(StartUART_Print_Task, NULL, &UART_Print_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartData_Proc_Task */
/**
* @brief Function implementing the Data_Proc_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartData_Proc_Task */
void StartData_Proc_Task(void *argument)
{
  /* USER CODE BEGIN StartData_Proc_Task */
  HAL_UART_Transmit(&huart1, (uint8_t*)"Data Proc Task Started\r\n", 24, 100);

  CAN_Frame_t frame;
  char buf[64];
  osStatus_t status;

  for(;;)
  {
    status = osMessageQueueGet(CanFrameQueueHandle, &frame, NULL, osWaitForever);
    if (status == osOK) {
        HAL_UART_Transmit(&huart1, (uint8_t*)"Data Proc\r\n", 11, 100);
        sprintf(buf, "ID:0x%03lX Data:%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                (unsigned long)frame.id,
                frame.data[0], frame.data[1], frame.data[2], frame.data[3],
                frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
        status = osMessageQueuePut(PrintStringQueueHandle, buf, 0, 0);
        if (status != osOK) {
            HAL_UART_Transmit(&huart1, (uint8_t*)"Print Queue Fail\r\n", 18, 100);
        }
    }
  }
  /* USER CODE END StartData_Proc_Task */
}

/* USER CODE BEGIN Header_StartUART_Print_Task */
/**
* @brief Function implementing the UART_Print_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUART_Print_Task */
void StartUART_Print_Task(void *argument)
{
  /* USER CODE BEGIN StartUART_Print_Task */
  HAL_UART_Transmit(&huart1, (uint8_t*)"UART Print Task Started\r\n", 25, 100);

  char buf[64];
  osStatus_t status;

  for(;;)
  {
    status = osMessageQueueGet(PrintStringQueueHandle, buf, NULL, osWaitForever);
    if (status == osOK) {
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 100);
    }
  }
  /* USER CODE END StartUART_Print_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
