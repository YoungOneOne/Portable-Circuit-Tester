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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Relay_Pin GPIO_PIN_0
#define Relay_GPIO_Port GPIOC
#define KEY_MODE_Pin GPIO_PIN_1
#define KEY_MODE_GPIO_Port GPIOC
#define Auto_Shutdown_Pin GPIO_PIN_2
#define Auto_Shutdown_GPIO_Port GPIOC
#define WAKEUP_KEY_Pin GPIO_PIN_0
#define WAKEUP_KEY_GPIO_Port GPIOA
#define WAKEUP_KEY_EXTI_IRQn EXTI0_IRQn
#define DDS_FSY_Pin GPIO_PIN_4
#define DDS_FSY_GPIO_Port GPIOA
#define DDS_CLK_Pin GPIO_PIN_5
#define DDS_CLK_GPIO_Port GPIOA
#define DDS_DAT_Pin GPIO_PIN_6
#define DDS_DAT_GPIO_Port GPIOA
#define DDS_CS_Pin GPIO_PIN_7
#define DDS_CS_GPIO_Port GPIOA
#define GND_EX2_Pin GPIO_PIN_8
#define GND_EX2_GPIO_Port GPIOD
#define GND_EX1_Pin GPIO_PIN_9
#define GND_EX1_GPIO_Port GPIOD
#define GND_1K_Pin GPIO_PIN_10
#define GND_1K_GPIO_Port GPIOD
#define GND_100_Pin GPIO_PIN_11
#define GND_100_GPIO_Port GPIOD
#define GND_1KD12_Pin GPIO_PIN_12
#define GND_1KD12_GPIO_Port GPIOD
#define GND_100K_Pin GPIO_PIN_13
#define GND_100K_GPIO_Port GPIOD
#define GND_470K_Pin GPIO_PIN_14
#define GND_470K_GPIO_Port GPIOD
#define PIN2_Pin GPIO_PIN_8
#define PIN2_GPIO_Port GPIOC
#define PIN1_Pin GPIO_PIN_8
#define PIN1_GPIO_Port GPIOA
#define LCD_SCL_Pin GPIO_PIN_0
#define LCD_SCL_GPIO_Port GPIOD
#define LCD_SDA_Pin GPIO_PIN_1
#define LCD_SDA_GPIO_Port GPIOD
#define LCD_DC_Pin GPIO_PIN_2
#define LCD_DC_GPIO_Port GPIOD
#define LCD_RES_Pin GPIO_PIN_3
#define LCD_RES_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */
#define AD_FSY_Pin GPIO_PIN_4
#define AD_FSY_GPIO_Port GPIOA
#define AD_SCLK_Pin GPIO_PIN_5
#define AD_SCLK_GPIO_Port GPIOA
#define AD_SDA_Pin GPIO_PIN_6
#define AD_SDA_GPIO_Port GPIOA
#define AD_CS_Pin GPIO_PIN_7
#define AD_CS_GPIO_Port GPIOA
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
