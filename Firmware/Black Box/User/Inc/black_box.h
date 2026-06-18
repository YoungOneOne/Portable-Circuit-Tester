#ifndef __BLACK_H
#define __BLACK_H


#include "adc_fft_rms.h"


// 电路类型枚举
typedef enum {
    CIRCUIT_UNKNOWN,
    RC_SERIES,      // RC串联
    RC_PARALLEL,    // RC并联
    RL_SERIES,      // RL串联
    RL_PARALLEL     // RL并联
} CircuitType;

// 函数声明
float Calculate_C_RC_Series(uint32_t fx, float Au, float R1, float R);
float Calculate_C_RC_Parallel(uint32_t fx, float Au, float R1, float R);
float Calculate_L_RL_Series(uint32_t fx, float Au, float R1, float R);
float Calculate_L_RL_Parallel(uint32_t fx, float Au, float R1, float R);
int FindClosestGainIndex(const float* gain, uint16_t total_points, float target_gain);
CircuitType IdentifyCircuitType(float au_low, float au_high);
void GenerateLogFrequencies(uint32_t* freq_list, uint16_t points);
void MeasureFrequencyResponse(uint16_t total_points);

#endif /* __BLACK_H */
