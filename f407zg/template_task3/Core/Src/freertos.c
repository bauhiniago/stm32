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
#include "tim.h"
#include "lvgl.h"
#include "lvgl_app.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "AD9854.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define pi 3.141592
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId BlinkTaskHandle;
osThreadId lvglTaskHandle;
osThreadId ADCTaskHandle;
osThreadId TestTaskHandle;
osThreadId WaveModeTaskHandle;
osThreadId GifTaskHandle;
extern uint8_t adc1CpltFlg;
extern uint8_t adc2CpltFlg;
extern uint8_t adc3CpltFlg;
extern lv_obj_t * wave_chart;
extern uint8_t waveStopFlg;
extern lv_chart_series_t * wave_ser;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DAC_HandleTypeDef hdac;
extern uint16_t wave[wave_num];
extern uint16_t wave_auto_points[wave_num];
extern uint16_t AD1[AD_num];
extern uint16_t AD2[AD_num];
extern uint16_t AD3[AD_num];
extern uint8_t waveAutoFlg;
extern uint16_t wave_auto_num;
extern uint8_t distortion_mode;
extern float fft_outputbuf[FFT_LENGTH];
extern  uint16_t wave_point[4096] ;
extern  uint16_t point_num;
extern  float khz;
float Vadc1,Vadc2,Vadc3,Vin,Vin1,Vin2,Vout1,Vout2,A,Rin,Rout;
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
  /* 
  wave_btn();
  distortion_list();
  */
  Task3_label();

  // Wireless_Welcome();
  // SCH_draw();

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
void wave_init(uint16_t *wave_target,uint16_t  point,float volt){
  HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
  for(uint16_t i=0;i<point;i++){
    wave_target[i]=(uint16_t)(2048+round(sin(1.0*i/point*2*pi)*volt/3.3*2047));
    // printf("%d\r\n",wave_target[i]);
    // osDelay(10);
  }
  // printf("\r\n");
  // osDelay(1000);
  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)wave_point, point_num, DAC_ALIGN_12B_R);
}

void Wave_Scan(void){
  for (uint32_t i = 1; i <= 1000; i++)
  {
    AD9854_SetSine_double(i*1000,200.0/2000*4095);   
    osDelay(200);
  }
}


void VppGet(uint16_t * AD_Target,float * VppTarget){
  uint16_t AD_MAX_MID=0;
  uint16_t AD_MIN_MID=100000;
  for (size_t i = 0; i < AD_num; i++)
  {
    if(AD_MAX_MID<AD_Target[i])AD_MAX_MID=AD_Target[i];
    if(AD_MIN_MID>AD_Target[i])AD_MIN_MID=AD_Target[i];
  }
  *VppTarget=(AD_MAX_MID-AD_MIN_MID)/4095.0*3.3;
}

void ADC_Task(void){                   

  HAL_ADC_Start_DMA(&hadc1,(uint16_t *)AD1,AD_num);
  HAL_ADC_Start_DMA(&hadc2,(uint16_t *)AD2,AD_num);
  HAL_ADC_Start_DMA(&hadc3,(uint16_t *)AD3,AD_num);
  // osDelay(500);

  uint16_t adc1_times=0;
  uint16_t adc2_times=0;
  uint16_t adc3_times=0;
  #define adc_temp_num 20
  float adc1_temp[adc_temp_num+1];
  float adc2_temp[adc_temp_num+1];
  float adc3_temp[adc_temp_num+1];
  for (;;)
  {
    if (adc1CpltFlg==1)
    {
      osDelay(2);
      adc1_times++;
      VppGet(AD1,&adc1_temp[adc1_times]);
      if (adc1_times==adc_temp_num)
      {
        float temp;
        for (int i = 1,k=1; i <= adc_temp_num; i++)
        {
          k=i;
          for (int j = i+1; j <= adc_temp_num; j++)
          {
            if (adc1_temp[j]<adc1_temp[k])k=j;
          }
          if (k!=i)
          {
            temp=adc1_temp[i];
            adc1_temp[i]=adc1_temp[k];
            adc1_temp[k]=temp;
          }
        }
        Vadc1=adc1_temp[adc_temp_num/2-1];
       
        adc1_times=0;
      }
      adc1CpltFlg=0;
      HAL_ADC_Start_DMA(&hadc1,(uint16_t *)AD1,AD_num);
    }
    if (adc2CpltFlg==1)
    {
      osDelay(2);
      adc2_times++;
      VppGet(AD2,&adc2_temp[adc2_times]);
      if (adc2_times==adc_temp_num)
      {
        float temp;
        for (int i = 1,k=1; i <= adc_temp_num; i++)
        {
          k=i;
          for (int j = i+1; j <= adc_temp_num; j++)
          {
            if (adc2_temp[j]<adc2_temp[k])k=j;
          }
          if (k!=i)
          {
            temp=adc2_temp[i];
            adc2_temp[i]=adc2_temp[k];
            adc2_temp[k]=temp;
          }
        }
        Vadc2=adc2_temp[adc_temp_num/2-1];
        // extern lv_obj_t * text_Rout;
        // char char_temp[10];
        // sprintf(char_temp,"Vpp:%.3f",Vadc2);
        // lv_label_set_text(text_Rout, char_temp);
        adc2_times=0;
      }
      adc2CpltFlg=0;
      HAL_ADC_Start_DMA(&hadc2,(uint16_t *)AD2,AD_num);
    }
    if (adc3CpltFlg==1)
    {
      osDelay(2);
      adc3_times++;
      VppGet(AD3,&adc3_temp[adc3_times]);
      if (adc3_times==adc_temp_num)
      {
        float temp;
        for (int i = 1,k=1; i <= adc_temp_num; i++)
        {
          k=i;
          for (int j = i+1; j <= adc_temp_num; j++)
          {
            if (adc3_temp[j]<adc3_temp[k])k=j;
          }
          if (k!=i)
          {
            temp=adc3_temp[i];
            adc3_temp[i]=adc3_temp[k];
            adc3_temp[k]=temp;
          }
        }
        Vadc3=adc3_temp[adc_temp_num/2-1];
        // extern lv_obj_t * text_A;
        // char char_temp[10];
        // sprintf(char_temp,"Vpp:%.3f",Vadc3);
        // lv_label_set_text(text_A, char_temp);
        // printf("%f\r\n",Vadc3);
        adc3_times=0;
      }
      adc3CpltFlg=0;
      HAL_ADC_Start_DMA(&hadc3,(uint16_t *)AD3,AD_num);
    }
  }
}

void Test_Task(void){
  extern lv_obj_t * text_Rin;
  extern lv_obj_t * text_Rout;
  extern lv_obj_t * text_A;
  extern lv_obj_t * text_Problem;
  extern uint8_t Flag_Rin;
  extern uint8_t Flag_Rout;
  extern uint8_t Flag_A;
  extern uint8_t Flag_Scan;
  extern uint8_t Flag_Problem;

  lv_label_set_text(text_Rin, "Rin: 41020o");
  lv_label_set_text(text_Rout, "Rout: 41020o");
  lv_label_set_text(text_A, "A: 100.12");
  lv_label_set_text(text_Problem, "Problem:    ");

  // 继电器断电，采样电阻断开
  HAL_GPIO_WritePin(T1_GPIO_Port,T1_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(T2_GPIO_Port,T2_Pin,GPIO_PIN_RESET);

  // 输出1KHz 幅值200mV 正弦波
  AD9854_Init();
  osDelay(1000);//
  AD9854_SetSine(1000,200.0/2000*4095);
HAL_GPIO_WritePin(T1_GPIO_Port,T1_Pin,GPIO_PIN_SET);
  uint8_t test_loop=1;
  for (;;)
  {
    osDelay(50);
    if (Flag_Rin==1||test_loop)
    {
      
      osDelay(100);
      Vin1=Vadc1;
      Vin2=Vadc3;
      printf("%f %f \r\n",Vin1,Vin2);
      Rin=10000.0*Vin2/(Vin1-Vin2);
      //HAL_GPIO_WritePin(T1_GPIO_Port,T1_Pin,GPIO_PIN_RESET);

      char char_temp[10];
      sprintf(char_temp,"Rin:%.3fk",Rin/1000.0);
      lv_label_set_text(text_Rin, char_temp);

      // sprintf(char_temp,"Vpp:%.3f",Vin1);
      // lv_label_set_text(text_Rout, char_temp);

      // sprintf(char_temp,"Vpp:%.3f",Vin2);
      // lv_label_set_text(text_A, char_temp);

      Flag_Rin=0;
    }
    if (Flag_Rout==1)
    {
      HAL_GPIO_WritePin(T1_GPIO_Port,T1_Pin,GPIO_PIN_RESET);
      osDelay(50);
      Vout1=Vadc2;
      HAL_GPIO_WritePin(T1_GPIO_Port,T1_Pin,GPIO_PIN_SET);
      osDelay(50);
      Vout2=Vadc2;
      Rout=10000.0*Vin2/(Vin1-Vin2);
      HAL_GPIO_WritePin(T1_GPIO_Port,T1_Pin,GPIO_PIN_SET);

      char char_temp[10];
      sprintf(char_temp,"Rout:%.3fk",Rout/1000.0);
      lv_label_set_text(text_Rout, char_temp);

      // sprintf(char_temp,"Vpp:%.3f",Vin1);
      // lv_label_set_text(text_Rout, char_temp);

      // sprintf(char_temp,"Vpp:%.3f",Vin2);
      // lv_label_set_text(text_A, char_temp);

      Flag_Rout=0;
    }
    
  }
}

void Wave_Mode(void){
  for (;;)
  {
    osDelay(250);
  }
}

void gif_loop(void){
  extern lv_obj_t * fox_img;
  for(;;){
    LV_IMG_DECLARE(f0);
    lv_img_set_src(fox_img, &f0);
    osDelay(70);
    LV_IMG_DECLARE(f1);
    lv_img_set_src(fox_img, &f1);
    osDelay(70);
    LV_IMG_DECLARE(f2);
    lv_img_set_src(fox_img, &f2);
    osDelay(70);
    LV_IMG_DECLARE(f3);
    lv_img_set_src(fox_img, &f3);
    osDelay(70);
    // LV_IMG_DECLARE(f4);
    // lv_img_set_src(fox_img, &f4);
    // osDelay(33);
    // LV_IMG_DECLARE(f5);
    // lv_img_set_src(fox_img, &f5);
    // osDelay(33);
    // LV_IMG_DECLARE(f6);
    // lv_img_set_src(fox_img, &f6);
    // osDelay(33);
    // LV_IMG_DECLARE(f7);
    // lv_img_set_src(fox_img, &f7);
    // osDelay(33);
    // LV_IMG_DECLARE(f8);
    // lv_img_set_src(fox_img, &f8);
    // osDelay(33);
    // LV_IMG_DECLARE(f9);
    // lv_img_set_src(fox_img, &f9);
    // osDelay(33);
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

  
  // lvgl任务调度及初始化
  osThreadDef(lvgl, lvgl_init, osPriorityRealtime , 0, 512);
  lvglTaskHandle = osThreadCreate(osThread(lvgl), NULL);

  // osThreadDef(gif, gif_loop, osPriorityNormal , 0, 512);
  // GifTaskHandle = osThreadCreate(osThread(gif), NULL);

  
  // ADC任务
  osThreadDef(adctask, ADC_Task, osPriorityNormal, 0, 1024);
  ADCTaskHandle = osThreadCreate(osThread(adctask), NULL);
  // 逻辑判断线程
  osThreadDef(testtask, Test_Task, osPriorityNormal, 0, 512);
  TestTaskHandle = osThreadCreate(osThread(testtask), NULL);
  

  /*   
  // 控制线程
  osThreadDef(wavemode, Wave_Mode, osPriorityNormal , 0, 128);
  WaveModeTaskHandle = osThreadCreate(osThread(wavemode), NULL); 
  */

  // osThreadDef(blink, LED_Blinks, osPriorityNormal, 0, 128);
  // BlinkTaskHandle = osThreadCreate(osThread(blink), NULL);
  // osThreadDef(hello, Helloworld, osPriorityNormal, 0, 128);
  // HelloTaskHandle = osThreadCreate(osThread(hello), NULL);


  
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
