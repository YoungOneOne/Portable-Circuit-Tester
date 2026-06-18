#include "adc_fft_rms.h"
#include <stdint.h>
#include <math.h>

#define V_REF 3.3f
#define ADC_MAX 4095.0f

extern uint16_t adc_buffer1[ADC_BUFFER_SIZE];
extern uint16_t adc_buffer2[ADC_BUFFER_SIZE];
extern uint16_t adc_buffer3[ADC_BUFFER_SIZE];

static float rms_value_1 = 0.0f;
static float rms_value_2 = 0.0f;
static float rms_value_3 = 0.0f;
static float DC_value_1 = 0.0f;
static float DC_value_2 = 0.0f;
static float DC_value_3 = 0.0f;

/**
 * 将 ADC 数值转换为电压值（单位 V）
 */
static inline float ADC_To_Voltage(uint16_t adc_value)
{
    return ((float)adc_value / ADC_MAX) * V_REF;
}

/**
 * 快速平均（计算直流分量）
 */
float Calculate_Mean(uint16_t *buffer)
{
    double sum = 0.0;
    for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
        sum += ADC_To_Voltage(buffer[i]);
    }
    return (float)(sum / ADC_BUFFER_SIZE);
}

/**
 * RMS 计算：去偏置后平方求均值再开根号
 */
float Calculate_RMS_Internal(uint16_t *buffer)
{
    float mean = Calculate_Mean(buffer);
    double square_sum = 0.0;

    for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
        float v = ADC_To_Voltage(buffer[i]) - mean;
        square_sum += v * v;
    }

    return sqrtf(square_sum / ADC_BUFFER_SIZE);
}

// 快速获取正弦电压有效值及直流电压值调用函数

float Get_RMS_Value_1(void)
{
    rms_value_1 = Calculate_RMS_Internal(adc_buffer1);
    return rms_value_1;
}

float Get_RMS_Value_2(void)
{
    rms_value_2 = Calculate_RMS_Internal(adc_buffer2);
    return rms_value_2;
}

float Get_RMS_Value_3(void)
{
    rms_value_3 = Calculate_RMS_Internal(adc_buffer3);
    return rms_value_3;
}


float Get_DC_Value_1(void)
{
    DC_value_1 = Calculate_Mean(adc_buffer1);
    return DC_value_1;
}

float Get_DC_Value_2(void)
{
    DC_value_2 = Calculate_Mean(adc_buffer2);
    return DC_value_2;
}
float Get_DC_Value_3(void)
{
    DC_value_3 = Calculate_Mean(adc_buffer3);
    return DC_value_3;
}