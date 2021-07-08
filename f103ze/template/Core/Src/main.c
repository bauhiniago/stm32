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
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "LCD.h"
#include "GT9147.h"
#include "lvgl.h"
#include "SHT30.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LVGL_TICK 	5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t recv_dat[6] = {0};
float temperature = 0.0;
float humidity = 0.0;
char temp_str[10];
char humi_str[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void touchdev_test(void){
	uint8_t t=0;
	uint8_t i=0;	  	    
 	uint16_t lastpos[5][2];		//最后一次的数据 
	while(1)
	{
		touchdev.scan(0);
		for(t=0;t<5;t++)
		{
			if((touchdev.sta)&(1<<t))
			{
                //printf("X坐标:%d,Y坐标:%d\r\n",touchdev.x[0],touchdev.y[0]);
				if(touchdev.x[t]<lcddev.width&&touchdev.y[t]<lcddev.height)
				{
					printf("x:%d y:%d\r\n",touchdev.x[0],touchdev.y[0]);
				}
			}else lastpos[t][0]=0XFFFF;
		}
		HAL_Delay(5);
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
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */



  /* 初始化 LVGL */
  HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_SET);
  lv_init();
  lv_port_disp_init();        // 显示器初始化
  lv_port_indev_init();       // 输入设备初始化（如果没有实现就注释掉）
  // lv_port_fs_init();          // 文件系统设备初始化（如果没有实现就注释掉）
  
  
  lv_example_btn_1();
  //lv_example_label_1();
  SHT30_bar();
  //lv_example_arc_2();
  //lv_example_keyboard_1();

    SHT30_Reset();
  if(SHT30_Init() == HAL_OK)
    printf("sht30 init ok.\r\n");
  else
    printf("sht30 init fail.\r\n");

  uint32_t tick[5];
  tick[0]=HAL_GetTick()+1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    lv_tick_inc(LVGL_TICK);
		lv_task_handler();
    HAL_Delay(LVGL_TICK);
    if((HAL_GetTick()-tick[0])>1000){
      tick[0]=HAL_GetTick();
      //printf("alive\r\n");

    if(SHT30_Read_Dat(recv_dat) == HAL_OK){
        if(SHT30_Dat_To_Float(recv_dat, &temperature, &humidity)==0){
          extern lv_obj_t * temp_bar;
          extern lv_obj_t * temp_label;
          extern lv_obj_t * humi_bar;
          extern lv_obj_t * humi_label;
          lv_bar_set_value(temp_bar, temperature, LV_ANIM_ON);  
          lv_bar_set_value(humi_bar, humidity, LV_ANIM_ON);  
          sprintf(temp_str,"%.2fC",temperature);
          sprintf(humi_str,"%.2f%%",humidity);
          lv_label_set_text(temp_label,temp_str);
          lv_label_set_text(humi_label,humi_str);
          //printf("Temp: %.2fC Humi: %.2f%%\r\n",temperature,humidity);
        }else{
          printf("crc check fail.\r\n");
        }
      }else{
        printf("read data from sht30 fail.\r\n");
      }
      HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
    }

    //
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
