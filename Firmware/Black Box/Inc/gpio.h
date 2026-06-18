/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
void GPIO_SwitchMode(GPIO_TypeDef  *GPIOx, uint32_t GPIO_PIN, uint32_t Mode);
void GPIO_Mode_Init();
void Measure_F1T2_1000();
void Measure_F2T1_1000();
void Measure_F2T1_Discharge();
void Measure_F1T2_51();
void Measure_F1T2_68();
void Measure_F1T2_100();
void Measure_F1T2_100k();
void Measure_F1T2_510k();
void Measure_F1T2_31();
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define High     1
#define Low      2
#define Analog   3
#define Floating 4
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
