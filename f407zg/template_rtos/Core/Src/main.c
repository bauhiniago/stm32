/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lvgl.h"
#include "lvgl_app.h"
#include <stdio.h>
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

/* USER CODE BEGIN PV */
uint8_t adc1CpltFlg=0;
uint16_t wave[wave_num];
uint16_t wave_auto_points[wave_num];
uint16_t AD1[AD_num];
uint16_t wave_auto_num=0;
uint16_t wave_max;
uint16_t wave_min;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
  if(hadc->Instance == ADC1){
    adc1CpltFlg =1;
    HAL_ADC_Stop_DMA(&hadc1);
  }
}
extern uint8_t waveAutoFlg;
void Wave_Data_Init(void){
  //AD1_Max=0;
  //AD1_Min=4096;
  uint16_t Point_num=AD_num;
  wave_max=0;
  wave_min=3300;
  for(int i = 0,k=0; i < Point_num;) {
    wave[k]=0;
    for (int j = 0; j < Point_num/wave_num; ++j) {
      wave[k]+=AD1[i++];
    }
    //printf("%ld\r\n",AD1[i]);
    wave[k]/=(Point_num/wave_num);
    if (!waveAutoFlg){
      wave[k]=(wave[k]/4096.0)*3300;
    }
    {
      /* code */
    }
    
    if(wave_max<wave[k]){
      wave_max=wave[k];
    }
    if(wave_min>wave[k]){
      wave_min=wave[k];
    }
    // if (wave[k]>3300)
    // {
    //   wave[k]=wave[k-1];
    // }
    //printf("%d\r\n",wave[k]);
    k++;
  }
}
void Wave_Auto(void){
  uint16_t wave_trigger;
  uint8_t triggerFlg=0;
  uint8_t endFlg=0;
  uint8_t k=0;
  wave_auto_num=0;
  memset(wave_auto_points,0,wave_num);
  wave_trigger=(wave_max-wave_min)/5*1+wave_min;
  int i=0;

  /* 
  A<B<C
  A<=B<C
  A<B<=C
  
  
  
   */
  while (!(wave[i]<=wave_trigger&&wave_trigger<wave[i+1])&&i<wave_num)
  {
    i++;
  }    
  i+=1;
  for (uint8_t j = 0; j < 3; j++)
  {
    wave_auto_points[wave_auto_num]=(wave[i]/4096.0)*3300;
    wave_auto_num++;
    i++;
    while (!(wave[i]<=wave_trigger&&wave_trigger<wave[i+1])&&i<wave_num)
    {
      wave_auto_points[wave_auto_num]=(wave[i]/4096.0)*3300;
      wave_auto_num++;
      i++;
    }
  }
  
  
  // i++;
  // while (!(wave[i]<=wave_trigger&&wave_trigger<=wave[i+1]))
  // {
  //   wave_auto_points[wave_auto_num]=wave[i];
  //   wave_auto_num++;
  //   i++;
  // }
  printf("triggerFlg: %d\r\n",triggerFlg);
}
void TIM2_Callback(){
  //lv_task_handler();
}
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

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // lv_init();
  // lv_port_disp_init();        // 显示器初始化
  // lv_port_indev_init();       // 输入设备初始化（如果没有实现就注释掉）
  // // lv_port_fs_init();          // 文件系统设备初始化（如果没有实现就注释掉）

  // wave_chart_init();
  // wave_btn();
  // HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if(htim->Instance == htim2.Instance)
    TIM2_Callback();
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
