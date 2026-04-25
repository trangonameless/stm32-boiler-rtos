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
#include "system_state.h"
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
QueueHandle_t stateQueue;
QueueHandle_t tempQueue;
QueueHandle_t irQueue;

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
/* Definitions for ControlTask */
osThreadId_t ControlTaskHandle;
const osThreadAttr_t ControlTask_attributes = {
  .name = "ControlTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
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
void Start_Control_Task(void *argument);

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
  irSemaphoreHandle = osSemaphoreNew(1, 1, &irSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  uartQueue = xQueueCreate(8, sizeof(char[64]));
  stateQueue = xQueueCreate(1, sizeof(SystemState_t));
  tempQueue = xQueueCreate(5, sizeof(float));
  irQueue = xQueueCreate(5, sizeof(int));
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

  /* creation of ControlTask */
  ControlTaskHandle = osThreadNew(Start_Control_Task, NULL, &ControlTask_attributes);

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
//	    char msg[64];

	    // Read DS18B20 ROM code
	    if (ds18b20_read_address(ds1) != HAL_OK) {
	        Error_Handler();
	    }
	    float temp;

	    for(;;)
	    {
	        // Start measurement
	        ds18b20_start_measure(NULL);

	        // Wait for conversion
	        osDelay(750);

	        // Get temperature
	        temp = ds18b20_get_temp(NULL);
//	        snprintf(msg, sizeof(msg), "{\"temperature\":%.1f}\n\r", temp);
	        xQueueSend(tempQueue, &temp, 0);
	        osDelay(1000);

	        }
}
  /* USER CODE END Start_temp_Task */


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
  SystemState_t state;

  /* Infinite loop */


  for (;;) {
      if (xQueuePeek(stateQueue, &state, portMAX_DELAY) == pdPASS) {

          snprintf(msg, sizeof(msg),
              "{\"set\":%d,\"temp\":%.1f}\r\n",
              state.set_temp,
              state.temp);

          HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
      }
      osDelay(200);
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
  /* USER CODE BEGIN Start_ir_Task */
    ir_init();
  /* Infinite loop */
	   for (;;) {
	        if (osSemaphoreAcquire(irSemaphoreHandle, osWaitForever) == osOK) {

	            int value = ir_read();
	            int delta = 0;

	            if (value >= 0) {
	                switch (value) {
	                    case IR_CODE_PLUS:
	                        delta = 1;
	                        break;
	                    case IR_CODE_MINUS:
	                        delta = -1;
	                        break;
	                }

	                if (delta != 0) {
	                    xQueueSend(irQueue, &delta, 0);
	                }
	            }
	        }
	    }
	}
  /* USER CODE END Start_ir_Task */


/* USER CODE BEGIN Header_Start_Control_Task */
/**
* @brief Function implementing the ControlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Control_Task */
void Start_Control_Task(void *argument)
{
  /* USER CODE BEGIN Start_Control_Task */
	#define TEMP_MIN 20
	#define TEMP_MAX 70
	#define TEMP_HYST 1
    SystemState_t state = {
        .set_temp = TEMP_MIN,
        .temp = 0
    };

    int delta;
    float temp;

  /* Infinite loop */

    for (;;)
    {
        // 1. Recive from IR
        if (xQueueReceive(irQueue, &delta, 0) == pdPASS)
        {
            state.set_temp += delta;

            if (state.set_temp < TEMP_MIN) state.set_temp = TEMP_MIN;
            if (state.set_temp > TEMP_MAX) state.set_temp = TEMP_MAX;
        }

        // 2. Recive from temperature sensor
        if (xQueueReceive(tempQueue, &temp, 0) == pdPASS)
        {
            state.temp = temp;
        }

        // 3. Control relay
        if (state.temp <= (state.set_temp - TEMP_HYST))
        {
  	      HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
  	      HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
        }
        else if (state.temp >= (state.set_temp + TEMP_HYST))
        {
  	      HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
  	      HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
        }

        // 4. Public state
        xQueueOverwrite(stateQueue, &state);
        seg7_show(state.set_temp);
        osDelay(100);
    }
}
  /* USER CODE END Start_Control_Task */


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

