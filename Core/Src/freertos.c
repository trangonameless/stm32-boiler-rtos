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
#include "queue.h"
#include "ds18b20.h"
#include "wire.h"
#include "ir.h"
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
QueueHandle_t uartQueue;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for temp_Task */
osThreadId_t temp_TaskHandle;
const osThreadAttr_t temp_Task_attributes = {
  .name = "temp_Task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for uartTask */
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for irTask */
osThreadId_t irTaskHandle;
const osThreadAttr_t irTask_attributes = {
  .name = "irTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for irSemaphore */
osSemaphoreId_t irSemaphoreHandle;
const osSemaphoreAttr_t irSemaphore_attributes = {
  .name = "irSemaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Start_temp_Task(void *argument);
void Start_uart_Task(void *argument);
void Start_ir_Task(void *argument);

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
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of irSemaphore */
  irSemaphoreHandle = osSemaphoreNew(1, 0, &irSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  uartQueue = xQueueCreate(8, sizeof(char[64]));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of temp_Task */
  temp_TaskHandle = osThreadNew(Start_temp_Task, NULL, &temp_Task_attributes);

  /* creation of uartTask */
  uartTaskHandle = osThreadNew(Start_uart_Task, NULL, &uartTask_attributes);

  /* creation of irTask */
  irTaskHandle = osThreadNew(Start_ir_Task, NULL, &irTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
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
//	extern IWDG_HandleTypeDef hiwdg;
  /* Infinite loop */
  for(;;)
  {

//	  HAL_IWDG_Refresh(&hiwdg);
	  osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Start_temp_Task */
/**
* @brief Function implementing the temp_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_temp_Task */
void Start_temp_Task(void *argument)
{
  /* USER CODE BEGIN Start_temp_Task */
	    uint8_t ds1[DS18B20_ROM_CODE_SIZE];
	    char msg[64];

	    // Read DS18B20 ROM code
	    if (ds18b20_read_address(ds1) != HAL_OK) {
	        Error_Handler();
	    }

	    for(;;)
	    {
	        // Start measurement
	        ds18b20_start_measure(NULL);

	        // Wait for conversion
	        osDelay(750);

	        // Get temperature
	        float temp = ds18b20_get_temp(NULL);
	        snprintf(msg, sizeof(msg), "{\"temperature\":%.1f}\n\r", temp);
	        xQueueSend(uartQueue, msg, 0);
	        osDelay(1000);

	        }

  /* USER CODE END Start_temp_Task */
}

/* USER CODE BEGIN Header_Start_uart_Task */
/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_uart_Task */
void Start_uart_Task(void *argument)
{
  /* USER CODE BEGIN Start_uart_Task */
  char msg[64];
  /* Infinite loop */
  for(;;)
  {
    if (xQueueReceive(uartQueue, msg, portMAX_DELAY) == pdPASS)
    {
      HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
  }
}
  /* USER CODE END Start_uart_Task */


/* USER CODE BEGIN Header_Start_ir_Task */
/**
* @brief Function implementing the irTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_ir_Task */
void Start_ir_Task(void *argument)
{
	int set_temp = 0;
	char msg[64];
	ir_init();
	for (;;) {
		if (osSemaphoreAcquire(irSemaphoreHandle, osWaitForever) == osOK){
	int value = ir_read();
	if (value >= 0){
		switch (value) {
		case IR_CODE_PLUS: set_temp++;
		break;
		case IR_CODE_MINUS: set_temp--;
		break;
		}
	snprintf(msg, sizeof(msg), "{\"set temperature\":%d}\r\n", set_temp);
	xQueueSend(uartQueue, msg, 0); }
	}
	}
  /* USER CODE END Start_ir_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

