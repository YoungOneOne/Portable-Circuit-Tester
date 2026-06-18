#define __ADC_FFT_RMS_H

#include <stdint.h>

// 缓冲区大小（必须与 DMA 配置一致）
//#define ADC_BUFFER_SIZE 8192
#define ADC_BUFFER_SIZE 4096

// 外部 ADC 缓冲区（main.c定义）
extern uint16_t adc_buffer1[ADC_BUFFER_SIZE];
extern uint16_t adc_buffer2[ADC_BUFFER_SIZE];
extern uint16_t adc_buffer3[ADC_BUFFER_SIZE];

// 主函数调用获取 RMS / DC
float Get_RMS_Value_1(void);
float Get_RMS_Value_2(void);
float Get_RMS_Value_3(void);
float Get_DC_Value_1(void);
float Get_DC_Value_2(void);
float Get_DC_Value_3(void);

// 内部函数（可以用于调试或拓展）
float Calculate_RMS_Internal(uint16_t *buffer);
float Calculate_Mean(uint16_t *buffer);

