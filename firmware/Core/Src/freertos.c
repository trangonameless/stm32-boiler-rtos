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
#include "iwdg.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"
#include "ds18b20.h"
#include "wire.h"
#include "ir.h"
#include "usart.h"
#include "seg7.h"
#include <stdio.h>
#include "system_state.h"
#include "ir_messages.h"
#include "system_monitor.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMP_MIN 20
#define TEMP_MAX 70
#define TEMP_HYST 0.5
#define ONOFF_DEBOUNCE_MS 400

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
/* Definitions for WatchdogTask */
osThreadId_t WatchdogTaskHandle;
const osThreadAttr_t WatchdogTask_attributes = {
  .name = "WatchdogTask",
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

void StartWatchdogTask(void *argument);
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
  tempQueue = xQueueCreate(1, sizeof(float));
  irQueue = xQueueCreate(8, sizeof(IrMessage));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of WatchdogTask */
  WatchdogTaskHandle = osThreadNew(StartWatchdogTask, NULL, &WatchdogTask_attributes);

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

/* USER CODE BEGIN Header_StartWatchdogTask */
/**
  * @brief  Function implementing the WatchdogTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartWatchdogTask */
void StartWatchdogTask(void *argument)
{
  /* USER CODE BEGIN StartWatchdogTask */

  /* Infinite loop */

    //watch dog
	    for (;;)
	    {

	        HAL_IWDG_Refresh(&hiwdg);

	        osDelay(500);
  }
  /* USER CODE END StartWatchdogTask */
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

	    // Read DS18B20 ROM code
	    if (ds18b20_read_address(ds1) != HAL_OK) {
	        Error_Handler();
	    }
	    float temp;
	    // Reads temperature from DS18B20 sensor and updates system queue
	    for(;;)
	    {
	        // Start measurement
	        ds18b20_start_measure(NULL);

	        // Wait for conversion
	        osDelay(750);

	        // Get temperature
	        temp = ds18b20_get_temp(NULL);
	        xQueueOverwrite(tempQueue, &temp);
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
  char msg[128];
  SystemState_t state;

  /* Infinite loop */


  for (;;) {
      //sending data to RPI for MQTT publishing
      if (xQueuePeek(stateQueue, &state, 25) == pdPASS) {
    	  int len = snprintf(msg, sizeof(msg),
    	      "{\"temperature\":%.1f,\"heating\":%d,\"set_temperature\":%d,\"boiler_off\":%d,\"boiler_error\":%d}\n",
    	      state.temp,
			  (state.mode == BOILER_HEATING) ? 1 : 0,
    	      state.set_temp,
			  (state.mode == BOILER_OFF) ? 1 : 0,
			  (state.mode == BOILER_ERROR) ? 1: 0

    	  );

    	  if (len > 0 && len < sizeof(msg)) {
    	      HAL_UART_Transmit(&huart1, (uint8_t*)msg, len, 50);
    	  }

/*
  //debugging
          snprintf(msg, sizeof(msg),
        		  "{\"temperature\":%.1f,\"heating\":%d,\"set_temperature\":%d,\"boiler_off\":%d,\"boiler_error\":%d}\n",
        		      	      state.temp,
        		  			  (state.mode == BOILER_HEATING) ? 1 : 0,
        		      	      state.set_temp,
        		  			  (state.mode == BOILER_OFF) ? 1 : 0,
        		  			  (state.mode == BOILER_ERROR) ? 1: 0

        		      	  );
          HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
      }
*/

      osDelay(200);
  }
  }

  /* USER CODE END Start_uart_Task */
}

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

    static uint32_t last_onoff_time = 0;
    ir_init();
  /* Infinite loop */

	   // Task responsible for processing IR remote control commands
	   // and forwarding decoded events to the application queue.
	   for (;;) {

		    // Wait for IR reception event from ISR
	        if (osSemaphoreAcquire(irSemaphoreHandle, osWaitForever) == osOK) {

	            int value = ir_read();
	            IrMessage msg;


	            if (value >= 0) {
	                switch (value) {
                    // Increase  temperature by 1°C
	                    case IR_CODE_PLUS:
	                        msg.type = IR_EVENT_TEMP_CHANGE;
	                        msg.delta = 1;
	                        xQueueSend(irQueue, &msg, 0);
	                        break;

	                    case IR_CODE_MINUS:
	                 // Decrease temperature by 1°C
	                        msg.type = IR_EVENT_TEMP_CHANGE;
	                        msg.delta = -1;
	                        xQueueSend(irQueue, &msg, 0);
	                        break;

	                    case IR_CODE_ONOFF:
	                    {
	                 // ON/OFF control with debouncing
	                        uint32_t now = osKernelGetTickCount();


	                        if ((now - last_onoff_time) > ONOFF_DEBOUNCE_MS)
	                        {

	                            msg.type = IR_EVENT_TOGGLE_POWER;
	                            msg.delta = 0;
	                            xQueueSend(irQueue, &msg, 0);

	                            last_onoff_time = now;
	                        }
	                    }
	                    break;
	                }
	              }
	            }
	        }

  /* USER CODE END Start_ir_Task */
}

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

    SystemState_t state = {
        .set_temp = TEMP_MIN,
        .temp = 0,
	    .mode = BOILER_OFF
    };

    IrMessage msg;
    float temp;
    static uint32_t last_toggle = 0;

  /* Infinite loop */

    // Main boiler control task:
    // processes user commands, updates system state,
    // controls outputs and publishes current status.
    for (;;)
    {
        //  Recive from IR
        if (xQueueReceive(irQueue, &msg, 0) == pdPASS)
        {

            switch (msg.type) {

                case IR_EVENT_TEMP_CHANGE:
                    state.set_temp += msg.delta;
                    break;

                case IR_EVENT_TOGGLE_POWER:
                    if (state.mode == BOILER_OFF)
                    {
                        state.mode = BOILER_IDLE;
                    }
                    else
                    {
                        state.mode = BOILER_OFF;
                    }
                    break;
            }
        }

        //  Recive from temperature sensor
        if (xQueueReceive(tempQueue, &temp, 0) == pdPASS)
        {
            state.temp = temp;
        }

        //  Control relay and temperature sensor fault handler
        // Sensor fault detection
        if (state.temp == 85)
        {
            state.mode = BOILER_ERROR;
        }
        //set temperature clamps

        if (state.set_temp < TEMP_MIN) state.set_temp = TEMP_MIN;
        if (state.set_temp > TEMP_MAX) state.set_temp = TEMP_MAX;
        //state
        switch (state.mode)
        {
            case BOILER_OFF:

                break;

            case BOILER_IDLE:

                if (state.temp <= (state.set_temp - TEMP_HYST))
                {
                    state.mode = BOILER_HEATING;
                }

                break;

            case BOILER_HEATING:

                if (state.temp >= (state.set_temp + TEMP_HYST))
                {
                    state.mode = BOILER_IDLE;
                }

                break;

            case BOILER_ERROR:

                break;
        }


        // Outputs
        switch (state.mode)
        {
            case BOILER_OFF:

                HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);

                break;

            case BOILER_IDLE:

                HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);

                break;

            case BOILER_HEATING:

                HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);

                break;

            case BOILER_ERROR:

                HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
                //led blinking to show alarm state
                if (osKernelGetTickCount() - last_toggle >= 200)
                {
                    HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
                    last_toggle = osKernelGetTickCount();
                }

                HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);

                break;
        }

        // Public state
        xQueueOverwrite(stateQueue, &state);
        seg7_show(state.set_temp);
        osDelay(100);
    }

  /* USER CODE END Start_Control_Task */
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

