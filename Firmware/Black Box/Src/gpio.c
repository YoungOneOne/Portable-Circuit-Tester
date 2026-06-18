/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
void GPIO_SwitchMode(GPIO_TypeDef  *GPIOx, uint32_t GPIO_PIN, uint32_t Mode)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(Mode == 1)
	{
		GPIO_InitStruct.Pin = GPIO_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOx,GPIO_PIN,GPIO_PIN_SET);
	}
	else if(Mode == 2)
	{
		GPIO_InitStruct.Pin = GPIO_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOx,GPIO_PIN,GPIO_PIN_RESET);
	}
	else if(Mode == 3)
	{
		 GPIO_InitStruct.Pin = GPIO_PIN;
		 GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		 GPIO_InitStruct.Pull = GPIO_NOPULL;
		 HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	}
	else if(Mode == 4)
	{
		 GPIO_InitStruct.Pin = GPIO_PIN;
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		 GPIO_InitStruct.Pull = GPIO_NOPULL;
		 HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	}
}

void GPIO_Mode_Init(void)//����ȫ����ʼ��Ϊģ��״̬
{
	//1端口
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,Floating);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_10,Floating);

	//2端口
	GPIO_SwitchMode(GPIOC,GPIO_PIN_8,Floating);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_8,Floating);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_9,Floating);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_11,Floating);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_12,Floating);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_13,Floating);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_14,Floating);
}

void Measure_F1T2_1000()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_12,Low);
}


void Measure_F2T1_1000()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOC,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_10,Low);
}

void Measure_F2T1_Discharge()
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOD,GPIO_PIN_9,Low);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_10,Low);
}
void Measure_F1T2_51()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_9,Low);
}

void Measure_F1T2_68()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_8,Low);
}
void Measure_F1T2_100()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_11,Low);
}

void Measure_F1T2_100k()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_13,Low);
}

void Measure_F1T2_510k()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD,GPIO_PIN_14,Low);
}

void Measure_F1T2_31()//FΪfront TΪto
{
	GPIO_Mode_Init();	//初始化所有引脚为Floating
	GPIO_SwitchMode(GPIOA,GPIO_PIN_8,High);
	GPIO_SwitchMode(GPIOD, GPIO_PIN_11, Low); 
    GPIO_SwitchMode(GPIOD, GPIO_PIN_9, Low);    
	GPIO_SwitchMode(GPIOD, GPIO_PIN_8, Low);    	
}
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Relay_Pin|Auto_Shutdown_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DDS_FSY_Pin|DDS_CLK_Pin|DDS_DAT_Pin|DDS_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GND_EX2_Pin|GND_EX1_Pin|LCD_SCL_Pin|LCD_SDA_Pin
                          |LCD_DC_Pin|LCD_RES_Pin|LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(Relay_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY_MODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_MODE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Auto_Shutdown_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Auto_Shutdown_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = WAKEUP_KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(WAKEUP_KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = DDS_FSY_Pin|DDS_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = DDS_DAT_Pin|DDS_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin */
  GPIO_InitStruct.Pin = GND_EX2_Pin|GND_EX1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin
                           PDPin */
  GPIO_InitStruct.Pin = GND_1K_Pin|GND_100_Pin|GND_1KD12_Pin|GND_100K_Pin
                          |GND_470K_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PIN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PIN2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PIN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PIN1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin
                           PDPin */
  GPIO_InitStruct.Pin = LCD_SCL_Pin|LCD_SDA_Pin|LCD_DC_Pin|LCD_RES_Pin
                          |LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
