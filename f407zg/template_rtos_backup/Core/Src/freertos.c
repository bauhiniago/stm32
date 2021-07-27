/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "adc.h"
#include "dac.h"
#include "lvgl.h"
#include "lvgl_app.h"
#include <stdio.h>
#include <string.h>
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
osThreadId BlinkTaskHandle;
osThreadId lvglTaskHandle;
osThreadId ADCTaskHandle;
osThreadId WaveModeTaskHandle;
extern uint8_t adc1CpltFlg;
extern lv_obj_t * wave_chart;
extern uint8_t waveStopFlg;
extern lv_chart_series_t * wave_ser;
extern ADC_HandleTypeDef hadc1;
extern DAC_HandleTypeDef hdac;
extern uint16_t wave[wave_num];
extern uint16_t wave_auto_points[wave_num];
extern uint16_t AD1[AD_num];
extern uint8_t waveAutoFlg;
extern uint16_t wave_auto_num;
extern uint8_t distortion_mode;
extern float fft_outputbuf[FFT_LENGTH];
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void lvgl_init(void){
  /* lvgl init */
  lv_init();
  lv_port_disp_init();        // 显示器初始化
  lv_port_indev_init();       // 输入设备初始化（如果没有实现就注释掉）
  // lv_port_fs_init();          // 文件系统设备初始化（如果没有实现就注释掉）

  wave_chart_init();
  wave_btn();
  distortion_list();
  THD_label();

  for (;;)
  {
    osDelay(LVGL_TICK);
    lv_task_handler();
  }
}
void LED_Blinks(void){
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    //printf("hello world\r\n");
    osDelay(500);
  }
}
void ADC_Task(void){
  HAL_ADC_Start_DMA(&hadc1,(uint16_t *)AD1,AD_num);
  for (;;)
  {
    
    if (adc1CpltFlg==1)
    {
      osDelay(20);
      if(waveStopFlg==0){
        FFT();
        lv_chart_set_point_count(wave_chart, FFT_LENGTH/2);
        lv_chart_set_ext_y_array(wave_chart, wave_ser, (lv_coord_t *)wave);
        // Wave_Data_Init();
        // if(waveAutoFlg){
        //   Wave_Auto();
        //   lv_chart_set_point_count(wave_chart, wave_auto_num);
        //   lv_chart_set_ext_y_array(wave_chart, wave_ser, (lv_coord_t *)wave_auto_points); 
        // }else{
        //   lv_chart_set_point_count(wave_chart, wave_num);
        //   lv_chart_set_ext_y_array(wave_chart, wave_ser, (lv_coord_t *)wave); 
        // }
      }
      adc1CpltFlg=0;
      HAL_ADC_Start_DMA(&hadc1,(uint16_t *)AD1,AD_num);
    }
  }
}
void Wave_Mode(void){
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
  HAL_GPIO_WritePin(TwoWay_GPIO_Port,TwoWay_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Cross1_GPIO_Port,Cross1_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Cross2_GPIO_Port,Cross2_Pin,GPIO_PIN_RESET);
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(2.4/3.4*4095));
  for (;;)
  {
    if(distortion_mode==0){
      HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(2.4/3.4*4095));
      HAL_GPIO_WritePin(TwoWay_GPIO_Port,TwoWay_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross1_GPIO_Port,Cross1_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross2_GPIO_Port,Cross2_Pin,GPIO_PIN_RESET);
    }else if(distortion_mode==1){
      HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(2.0/3.4*4095));
      HAL_GPIO_WritePin(TwoWay_GPIO_Port,TwoWay_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross1_GPIO_Port,Cross1_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross2_GPIO_Port,Cross2_Pin,GPIO_PIN_RESET);
    }else if(distortion_mode==2){
      HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(2.7/3.4*4095));
      HAL_GPIO_WritePin(TwoWay_GPIO_Port,TwoWay_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross1_GPIO_Port,Cross1_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross2_GPIO_Port,Cross2_Pin,GPIO_PIN_RESET);
    }else if(distortion_mode==3){
      HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(1.0/3.4*4095));
      HAL_GPIO_WritePin(TwoWay_GPIO_Port,TwoWay_Pin,GPIO_PIN_SET);
      HAL_GPIO_WritePin(Cross1_GPIO_Port,Cross1_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross2_GPIO_Port,Cross2_Pin,GPIO_PIN_RESET);
    }else if(distortion_mode==4){
      HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(2.4/3.4*4095));
      HAL_GPIO_WritePin(TwoWay_GPIO_Port,TwoWay_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Cross1_GPIO_Port,Cross1_Pin,GPIO_PIN_SET);
      HAL_GPIO_WritePin(Cross2_GPIO_Port,Cross2_Pin,GPIO_PIN_SET);
    }
    osDelay(250);
  }
  

}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
   lv_tick_inc(1);
}
/* USER CODE END 3 */

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

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(lvgl, lvgl_init, osPriorityRealtime , 0, 512);
  lvglTaskHandle = osThreadCreate(osThread(lvgl), NULL);
  osThreadDef(adctask, ADC_Task, osPriorityNormal, 0, 1024);
  ADCTaskHandle = osThreadCreate(osThread(adctask), NULL);
  osThreadDef(wavemode, Wave_Mode, osPriorityNormal , 0, 128);
  WaveModeTaskHandle = osThreadCreate(osThread(wavemode), NULL);
  // osThreadDef(blink, LED_Blinks, osPriorityNormal, 0, 128);
  // BlinkTaskHandle = osThreadCreate(osThread(blink), NULL);
/*   osThreadDef(hello, Helloworld, osPriorityNormal, 0, 128);
  HelloTaskHandle = osThreadCreate(osThread(hello), NULL); */


  
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
    //printf("1\r\n");
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
