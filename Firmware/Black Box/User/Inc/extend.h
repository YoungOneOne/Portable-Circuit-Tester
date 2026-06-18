#ifndef EXTEND_H
#define EXTEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

// 引脚定义
#define PIN1_PORT GPIOA
#define PIN1_PIN  GPIO_PIN_0
#define PIN2_PORT GPIOA
#define PIN2_PIN  GPIO_PIN_1

// ----------- 类型定义 -----------

// 元件类型枚举
typedef enum {
    UNKNOWN,
    RESISTOR,
    CAPACITOR,
    INDUCTOR,
    DIODE_1_2,    // 正向 1→2
    DIODE_2_1     // 反向 2→1
} ComponentType;

// 电路类型（频率响应识别中用到）
typedef enum {
    RC_SERIES_,
    RL_SERIES_,
    RESISTIVE_
} CircuitTypes;

// 频率响应测量结果
typedef struct {
    float R;  // 欧姆
    float C;  // 法拉
    float L;  // 亨利
} MeasurementResults;

// 元件识别测量结果
typedef struct {
    ComponentType type;
    float value;    // R:Ω, C:F, L:H, D:V
} MeasurementResult;

// ----------- 函数声明 -----------

// GPIO & 测试切换控制
void GPIO_Mode_Init(void);
void Measure_F1T2_1000(void);
void Measure_F2T1_1000(void);
void Measure_F1T2_100k(void);
void Measure_F1T2_510k(void);
void Measure_F1T2_100(void);
void Measure_F1T2_Discharge(void);
void Measure_F2T1_Discharge(void);

// ADC 测量接口
float Get_DC_Value_1(void);
float Get_DC_Value_2(void);
float Get_RMS_Value_1(void);
float Get_RMS_Value_3(void);

// 核心功能
ComponentType IdentifyComponent(void);
float MeasureDiode(ComponentType diodeType);
float MeasureRESISTOR(void);
void MeasureCapacitance(float* R_out, float* C_out);
void MeasureINDUCTOR(float* R_out, float* L_out);
MeasurementResults MeasureFrequencyResponse__(uint16_t total_points);
void TwoTerminalComponent(void);

#ifdef __cplusplus
}
#endif

#endif // EXTEND_H
