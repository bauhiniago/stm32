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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

/* USER CODE BEGIN PV */
double vol[2];
uint32_t adc_value[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void MG996R(uint8_t mode,uint8_t angle){
  if (mode==0)  
  {
    if (angle<=180&&angle>=0)
    {//112 37
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,angle/180.0*75+37-1);
      //水平舵机
    }
  }else if(mode==1)
  {
    if (angle<=180&&angle>=0)
    {
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,angle/180.0*100+25-1);
      //垂直舵机
    }
  }
  
}

void GetADC(uint8_t channel){
  if (channel==0)
  {
    HAL_ADC_Start(&hadc1);	                //启动ADC单次转换
    HAL_ADC_PollForConversion(&hadc1, 50);	//等待ADC转换完成
    adc_value[0] = HAL_ADC_GetValue(&hadc1); 	//读取ADC转换数据
    vol[0] = ((double)adc_value[0]/4096)*3.3;
  }else{
    HAL_ADC_Start(&hadc2);	                //启动ADC单次转换
    HAL_ADC_PollForConversion(&hadc2, 50);	//等待ADC转换完成
    adc_value[1] = HAL_ADC_GetValue(&hadc2); 	//读取ADC转换数据
    vol[1] = ((double)adc_value[1]/4096)*3.3;
  }
  
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
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
  uint16_t time_div=2000;
  // MG996R(1,135);


  uint8_t pos_x=90;
  uint8_t pos_y=30;/* 30~130 */
  MG996R(0,pos_x);
  MG996R(1,pos_y);
  HAL_Delay(100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    GetADC(0);//x max = 1.79  adcx 变小，posx变大
    GetADC(1);//y max = 2.07  adcy 变小，posy变大
    if(vol[0]<1.71&&pos_x>0)
    {
      pos_x--;
    }else if(vol[0]>1.80&&pos_x<180){
      pos_x++;
    }
    if(vol[1]<1.95&&pos_y<130)
    {
      pos_y++;
    }else if(vol[1]>2.19&&pos_y>30){
      pos_y--;
    }
    printf("pos_x: %3d adc = %d vol 0 = %.2fV.\n",pos_x,adc_value[0],vol[0]);
    printf("pos_y: %3d adc = %d vol 1 = %.2fV.\n",pos_y,adc_value[1],vol[1]);
    
    MG996R(0,pos_x);
    MG996R(1,pos_y);
    HAL_Delay(10);




    //转向测试
    // for (size_t i = 0; i < 180; i++)
    // {
    //   MG996R(0,i);
    //   HAL_Delay(10);
    // }
    // for (size_t i = 180; i > 0; i--)
    // {
    //   MG996R(0,i);
    //   HAL_Delay(10);

    // }
    // for (size_t i = 30; i <130; i++)
    // {
    //   MG996R(1,i);
    //   HAL_Delay(10);
    // }
    // for (size_t i = 130; i >30; i--)
    // {
    //   MG996R(1,i);
    //   HAL_Delay(10);
    // }


    

    
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
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
  /** Initializes the CPU, AHB and APB buses clocks
  */
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
