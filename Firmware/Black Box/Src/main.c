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
  //电路黑箱测试仪   YoungOneOne   2025.7.16
  
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_driver.h"
#include "gui.h"
#include "ad9833.h"
#include <stdio.h>
#include "adc_fft_rms.h"
#include "black_box.h"
#include "font96.h"
#include "oled.h"
#include "extend.h" 
#include "stm32f4xx_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint16_t adc_buffer1[ADC_BUFFER_SIZE]; // ADC1通道数据 
uint16_t adc_buffer2[ADC_BUFFER_SIZE]; // ADC2通道数据 
uint16_t adc_buffer3[ADC_BUFFER_SIZE]; // ADC3通道数据 
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static volatile uint8_t measure_flag = 0;
static GPIO_PinState last_dip_state = GPIO_PIN_RESET; // 记录上一次的拨码开关状态

volatile uint32_t idle_ms = 0;      // 无操作计时（毫秒）
volatile uint8_t user_active = 1;   // 1=有操作，0=无操作
volatile uint8_t trigger_pulse = 0; // 触发IO拉高标志

volatile uint8_t key_pending = 0;
volatile uint32_t key_time = 0;
#define DEBOUNCE_TIME 10  // 10ms消抖时间

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float voltage1 = 0.0f, voltage2 = 0.0f, voltage3 = 0.0f;
float rms1 = 0.0f, rms2 = 0.0f, rms3 = 0.0f;  // 添加 RMS 变量

void UpdateModeDisplay(GPIO_PinState mode)
{
    OLED_NewFrame();
    OLED_DrawLine(64, 1, 64, 42, OLED_COLOR_NORMAL);
    OLED_DrawLine(0, 42, 127, 42, OLED_COLOR_NORMAL);
//  模式菜单显示逻辑：选中的模式使用反色显示，模拟光标选中
    if (mode == GPIO_PIN_RESET) {
        OLED_PrintString(82, 5, "元件", &font16x16, OLED_COLOR_REVERSED);
        OLED_PrintString(82, 21, "模式", &font16x16, OLED_COLOR_REVERSED);
        OLED_PrintString(15, 5, "黑箱", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(15, 21, "模式", &font16x16, OLED_COLOR_NORMAL);
    } else {
        OLED_PrintString(15, 5, "黑箱", &font16x16, OLED_COLOR_REVERSED);
        OLED_PrintString(15, 21, "模式", &font16x16, OLED_COLOR_REVERSED);
        OLED_PrintString(82, 5, "元件", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(82, 21, "模式", &font16x16, OLED_COLOR_NORMAL);
    }

    OLED_PrintString(8, 47, "按下按键进行测量", &font14x14, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	GPIO_SwitchMode(GPIOC, GPIO_PIN_2, Floating);
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(20);         //上电延时20ms
	Lcd_Init();            //LCD初始化
	Lcd_Clear(BLACK);      // 清屏：白色
    	
	HAL_Delay(20);
	OLED_Init();			// OLED初始化
	
    AD9833_GPIO_Init();    //AD9833初始化
    AD9833_WaveSeting(000, 0, SIN_WAVE, 0);  // 设置频率为000Hz，波形为正弦波
    AD9833_AmpSet(200);              // 设置AD9833输出幅度
	
//  AD9833_Enable();	// 设置幅度，1~255 
//	AD9833_Disable();   // 暂停输出
	
	HAL_TIM_Base_Start(&htim2);      //启动定时器TIM2，用于触发ADC采样
	
	//启动三个ADC
	HAL_ADC_Start(&hadc1);           
	HAL_ADC_Start(&hadc2);
	HAL_ADC_Start(&hadc3);
	
	// 启动三个ADC的DMA转换
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer1, ADC_BUFFER_SIZE);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_buffer2, ADC_BUFFER_SIZE);
	HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_buffer3, ADC_BUFFER_SIZE);
	
	//开机显示信息
	OLED_NewFrame();
	OLED_PrintString( 20 , 8  , "电路黑箱" , &font20x20 , OLED_COLOR_NORMAL);
	OLED_PrintString( 30 , 35 , "测试仪" , &font20x20 , OLED_COLOR_NORMAL);
	OLED_ShowFrame();
	HAL_Delay(1250);
	OLED_NewFrame();
	OLED_PrintString( 6 , 4 , "YoungOneOne" , &font16x16 , OLED_COLOR_NORMAL);
	OLED_PrintString( 6 , 22 , "Member B:YYYY" , &font16x16 , OLED_COLOR_NORMAL);
	OLED_PrintString( 6 , 40 , "Member C:ZZZZ" , &font16x16 , OLED_COLOR_NORMAL);
	OLED_ShowFrame();
    HAL_Delay(750);

	uint8_t need_refresh = 1;  // 是否需要刷新显示
	GPIO_PinState last_dip_state = GPIO_PIN_RESET;

	idle_ms = 0;
	
	 UpdateModeDisplay(GPIO_PIN_RESET);  // 开机初始化显示

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
 
 /* 关机逻辑：30秒无操作自动关机 */
    if (trigger_pulse) {
		GPIO_Mode_Init();
        trigger_pulse = 0;
		OLED_NewFrame();
		OLED_PrintString( 3 , 26 , "三秒后将自动关机..." , &font13x13 , OLED_COLOR_NORMAL);
		OLED_ShowFrame();
        GPIO_SwitchMode(GPIOA, GPIO_PIN_0, Low);
        HAL_Delay(1000);
		HAL_Delay(1000);
		OLED_NewFrame();
		OLED_PrintASCIIString(24, 23,"Bye Bye !!!", &afont16x8, OLED_COLOR_NORMAL);
		OLED_ShowFrame();
		HAL_Delay(2000);
        GPIO_SwitchMode(GPIOA, GPIO_PIN_0, Floating);  // 拉低PA0三秒，模拟手动按下按键，触发开关机芯片关机
		
    }
	// 中断回调函数中不能使用延时消抖，造成阻塞，在主循环中进行消抖
if (key_pending) {
    if (HAL_GetTick() - key_time >= DEBOUNCE_TIME) {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
            measure_flag = 1;  // 真正确认按下有效
        }
        key_pending = 0;
    }
}

    // 检测拨码开关变化
        GPIO_PinState current_dip_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
        if (current_dip_state != last_dip_state) {
            trigger_pulse = 0;
            idle_ms = 0;
            last_dip_state = current_dip_state;
            need_refresh = 1;  // 触发刷新
        }

        // 检测按键按下触发测量
        if (measure_flag) {
            measure_flag = 0;
            trigger_pulse = 0;
            idle_ms = 0;

            OLED_NewFrame();
            OLED_PrintString(10, 28, "测量中,请稍候...", &font14x14, OLED_COLOR_NORMAL);
            OLED_ShowFrame();

            if (current_dip_state == GPIO_PIN_RESET) {
                HAL_Delay(20);
                TwoTerminalComponent();  // 元件模式
                HAL_Delay(500);
            } else {
                OLED_NewFrame();
                GPIO_Mode_Init();
                HAL_Delay(50);
//                Lcd_Init();
//                Lcd_Clear(WHITE);
                GPIO_SwitchMode(GPIOC, GPIO_PIN_0, High);  //打开继电器，接入AD9833
                MeasureFrequencyResponse(18);  // 黑箱模式，括号内为扫频点数
                GPIO_SwitchMode(GPIOC, GPIO_PIN_0, Low);    ////关闭继电器，断开AD9833
                HAL_Delay(500);
            }
			idle_ms = 0;
            need_refresh = 0;  // 测量完成后保持结果，不刷新界面
        }

        // 界面刷新逻辑：测量完成后保持显示，切换模式或按下按键再刷新屏幕
        if (need_refresh) {
            UpdateModeDisplay(current_dip_state);
            need_refresh = 0;
			idle_ms = 0;        
        }

        HAL_Delay(10);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
            key_pending = 1;
            key_time = HAL_GetTick();  // 记录时间戳
        }
    }
}


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
