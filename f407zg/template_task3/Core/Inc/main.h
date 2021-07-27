/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void LED_Blinks(void);
void LCD_COUNTER(void);
void TIM2_Callback(void);
void ADC_Task(void);
void Wave_Data_Init(void);
void FFT(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_9
#define LED_GPIO_Port GPIOF
#define T1_Pin GPIO_PIN_13
#define T1_GPIO_Port GPIOF
#define T2_Pin GPIO_PIN_14
#define T2_GPIO_Port GPIOF
#define LCD_BACK_Pin GPIO_PIN_15
#define LCD_BACK_GPIO_Port GPIOB
#define FSK_Pin GPIO_PIN_7
#define FSK_GPIO_Port GPIOG
#define D0_Pin GPIO_PIN_8
#define D0_GPIO_Port GPIOG
#define OSK_Pin GPIO_PIN_6
#define OSK_GPIO_Port GPIOC
#define D1_Pin GPIO_PIN_7
#define D1_GPIO_Port GPIOC
#define RD_Pin GPIO_PIN_8
#define RD_GPIO_Port GPIOA
#define WD_Pin GPIO_PIN_6
#define WD_GPIO_Port GPIOD
#define UCLK_Pin GPIO_PIN_9
#define UCLK_GPIO_Port GPIOG
#define D2_Pin GPIO_PIN_11
#define D2_GPIO_Port GPIOG
#define A0_Pin GPIO_PIN_13
#define A0_GPIO_Port GPIOG
#define D3_Pin GPIO_PIN_14
#define D3_GPIO_Port GPIOG
#define A1_Pin GPIO_PIN_15
#define A1_GPIO_Port GPIOG
#define D4_Pin GPIO_PIN_3
#define D4_GPIO_Port GPIOB
#define A2_Pin GPIO_PIN_4
#define A2_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOB
#define A3_Pin GPIO_PIN_6
#define A3_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_7
#define D6_GPIO_Port GPIOB
#define A4_Pin GPIO_PIN_8
#define A4_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_9
#define D7_GPIO_Port GPIOB
#define A5_Pin GPIO_PIN_0
#define A5_GPIO_Port GPIOE
#define RST_Pin GPIO_PIN_1
#define RST_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define wave_num 1024
#define AD_num 2048
#define LVGL_TICK 5
#define FFT_LENGTH  1024			// FFT长度，默认是1024点FFT
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
