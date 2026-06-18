                                                                                                                                                                                                               #ifndef __AD9833_H
#define __AD9833_H

#include "main.h"

// ==== 管脚定义 ====

//   PA4----AD_FSY
//   PA5----AD_SCLK
//   PA6----AD_SDA
//   PA7----AD_CS

#define AD9833_FSYNC_GPIO_PORT   GPIOA
#define AD9833_FSYNC_PIN         GPIO_PIN_4

#define AD9833_SCLK_GPIO_PORT    GPIOA
#define AD9833_SCLK_PIN          GPIO_PIN_5

#define AD9833_SDATA_GPIO_PORT   GPIOA
#define AD9833_SDATA_PIN         GPIO_PIN_6

#define AMP_CS_GPIO_PORT         GPIOA
#define AMP_CS_PIN               GPIO_PIN_7

// ==== 控制宏 ====
#define FSYNC_LOW()     HAL_GPIO_WritePin(AD9833_FSYNC_GPIO_PORT, AD9833_FSYNC_PIN, GPIO_PIN_RESET)
#define FSYNC_HIGH()    HAL_GPIO_WritePin(AD9833_FSYNC_GPIO_PORT, AD9833_FSYNC_PIN, GPIO_PIN_SET)

#define SCLK_LOW()      HAL_GPIO_WritePin(AD9833_SCLK_GPIO_PORT, AD9833_SCLK_PIN, GPIO_PIN_RESET)
#define SCLK_HIGH()     HAL_GPIO_WritePin(AD9833_SCLK_GPIO_PORT, AD9833_SCLK_PIN, GPIO_PIN_SET)

#define SDATA_LOW()     HAL_GPIO_WritePin(AD9833_SDATA_GPIO_PORT, AD9833_SDATA_PIN, GPIO_PIN_RESET)
#define SDATA_HIGH()    HAL_GPIO_WritePin(AD9833_SDATA_GPIO_PORT, AD9833_SDATA_PIN, GPIO_PIN_SET)

#define AMP_CS_LOW()    HAL_GPIO_WritePin(AMP_CS_GPIO_PORT, AMP_CS_PIN, GPIO_PIN_RESET)
#define AMP_CS_HIGH()   HAL_GPIO_WritePin(AMP_CS_GPIO_PORT, AMP_CS_PIN, GPIO_PIN_SET)

// ==== 波形宏 ====
#define TRI_WAVE   0
#define SIN_WAVE   1
#define SQU_WAVE   2

// ==== 函数声明 ====
void AD9833_GPIO_Init(void);
void AD9833_Write(uint16_t TxData);
void AD9833_WaveSeting(double Freq, uint16_t Freq_SFR, uint16_t WaveMode, uint16_t Phase);
void AD9833_AmpSet(uint8_t amp);

// ====== 开启/关闭功能 ======
void AD9833_Enable(void);    // 开启AD9833输出（退出睡眠模式）
void AD9833_Disable(void);   // 关闭AD9833输出（进入睡眠模式）

#endif