/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stdio.h"
#include "can.h"
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

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId my6623TaskHandle;
osThreadId my2006TaskHandle;
osThreadId myLEDTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void my6623TaskFunction(void const * argument);
void my2006TaskFunction(void const * argument);
void myLEDTaskFunction(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of my6623Task */
  osThreadDef(my6623Task, my6623TaskFunction, osPriorityIdle, 0, 128);
  my6623TaskHandle = osThreadCreate(osThread(my6623Task), NULL);

  /* definition and creation of my2006Task */
  osThreadDef(my2006Task, my2006TaskFunction, osPriorityIdle, 0, 128);
  my2006TaskHandle = osThreadCreate(osThread(my2006Task), NULL);

  /* definition and creation of myLEDTask */
  osThreadDef(myLEDTask, myLEDTaskFunction, osPriorityIdle, 0, 128);
  myLEDTaskHandle = osThreadCreate(osThread(myLEDTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    //printf("this is in default task\r\n");
    vTaskDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_my6623TaskFunction */
/**
* @brief Function implementing the my6623Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_my6623TaskFunction */
void my6623TaskFunction(void const * argument)
{
  /* USER CODE BEGIN my6623TaskFunction */
  /* Infinite loop */
  for(;;)
  {
	//printf("this is in 6223 task\r\n");
	set_motor_voltage(0x1ff,0x03E8,0x1f,0x1f,0x1f);
    vTaskDelay(1);
  }
  /* USER CODE END my6623TaskFunction */
}

/* USER CODE BEGIN Header_my2006TaskFunction */
/**
* @brief Function implementing the my2006Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_my2006TaskFunction */
void my2006TaskFunction(void const * argument)
{
  /* USER CODE BEGIN my2006TaskFunction */
  /* Infinite loop */
  for(;;)
  {
	set_motor_voltage(0x200,0x03E8,0x03E8,0x03E8,0x03E8);
	//printf("this is in 2006 task\r\n");
    vTaskDelay(1);
  }
  /* USER CODE END my2006TaskFunction */
}

/* USER CODE BEGIN Header_myLEDTaskFunction */
/**
* @brief Function implementing the myLEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_myLEDTaskFunction */
void myLEDTaskFunction(void const * argument)
{
  /* USER CODE BEGIN myLEDTaskFunction */
  /* Infinite loop */
  for(;;)
  {
	  vTaskDelay(1);
  }
  /* USER CODE END myLEDTaskFunction */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
