/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : CAN 接收端 - FreeRTOS 版本
  ******************************************************************************
  * @attention
  *
  * 功能：STM32F103C8 作为 CAN 接收端，通过 FreeRTOS 任务接收 CAN 数据并打印
  * CAN配置：500kbps，Normal模式，过滤器仅接收ID 0x100
  * 串口：115200，用于调试输出
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include "semphr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* 定义 CAN 帧结构体（用于队列传递） */
typedef struct {
    uint32_t id;
    uint8_t data[8];
} CAN_Frame_t;

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* 外部声明：freertos.c 中定义的队列句柄 */
extern osMessageQueueId_t CanFrameQueueHandle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_CAN_Init();   // 硬件初始化

  /* USER CODE BEGIN 2 */

  // ====== 串口启动提示 ======
  HAL_UART_Transmit(&huart1, (uint8_t*)"Start\r\n", 7, 100);

  // ====== 配置 CAN 过滤器（IDLIST 模式，只接收 ID 0x100） ======
  CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
  sFilterConfig.FilterIdHigh = (0x100 << 5) & 0xFFFF;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0xFFFF;
  sFilterConfig.FilterMaskIdLow = 0xFFFF;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
      HAL_UART_Transmit(&huart1, (uint8_t*)"Filter Error\r\n", 14, 100);
      Error_Handler();
  }

  // ====== 启动 CAN（带重试机制） ======
  HAL_UART_Transmit(&huart1, (uint8_t*)"Starting CAN...\r\n", 18, 100);

  uint8_t retry = 0;
  HAL_StatusTypeDef can_status = HAL_ERROR;

  while (retry < 5 && can_status != HAL_OK) {
      can_status = HAL_CAN_Start(&hcan);
      if (can_status != HAL_OK) {
          retry++;
          HAL_UART_Transmit(&huart1, (uint8_t*)"Retry...\r\n", 9, 100);
          HAL_Delay(100);
          MX_CAN_Init();
          HAL_Delay(50);
      }
  }

  if (can_status != HAL_OK) {
      HAL_UART_Transmit(&huart1, (uint8_t*)"Start Error\r\n", 13, 100);
      Error_Handler();
  } else {
      HAL_UART_Transmit(&huart1, (uint8_t*)"Start OK\r\n", 10, 100);
  }

  // ====== 强制配置 PA11 为输入模式 ======
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // ====== 使能 CAN 接收中断 ======
  if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
      HAL_UART_Transmit(&huart1, (uint8_t*)"Notif Error\r\n", 13, 100);
      Error_Handler();
  }

  HAL_UART_Transmit(&huart1, (uint8_t*)"Init Complete\r\n", 15, 100);
  char msg[] = "FreeRTOS + CAN 接收端启动\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg)-1, 100);

  /* USER CODE END 2 */

  osKernelInitialize();
  MX_FREERTOS_Init();
  osKernelStart();

  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief CAN 接收中断回调
  * @param hcan CAN 句柄指针
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    // 读取 CAN 数据
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);

    // 将数据放入队列（中断中发送）
    CAN_Frame_t frame;
    frame.id = RxHeader.StdId;
    for(int i = 0; i < 8; i++) {
        frame.data[i] = RxData[i];
    }

    // 使用 CMSIS-RTOS V2 的队列发送函数（可以在中断中调用）
    osMessageQueuePut(CanFrameQueueHandle, &frame, 0, 0);

    static uint8_t first_time = 1;
    if (first_time) {
        first_time = 0;
        HAL_UART_Transmit(&huart1, (uint8_t*)"CAN IRQ Triggered!\r\n", 21, 100);
    }
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(200);
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
