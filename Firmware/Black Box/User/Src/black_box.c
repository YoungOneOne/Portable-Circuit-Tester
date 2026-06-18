#include "adc_fft_rms.h"
#include "lcd_driver.h"
#include <stdio.h>
#include "gui.h"
#include "math.h"
#include <float.h>  // for FLT_MAX
#include "ad9833.h"
#include "gpio.h"
#include "font96.h"
#include "oled.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
extern const Font font12x12;
//电阻接地连接组态

// 电路类型
typedef enum {
    CIRCUIT_UNKNOWN,
    RC_SERIES,      // RC串联
    RC_PARALLEL,    // RC并联
    RL_SERIES,      // RL串联
    RL_PARALLEL,    // RL并联
	RESISTIVE_ONLY  // 纯电阻
} CircuitType;

/**
 * @brief 根据一般公式计算电容C（单位：法拉F）
 * @param fx   截止频率（Hz）
 * @param Au   截止点的增益比（Au = A_u / A0）
 * @param R1   电路中的前级电阻（Ω）
 * @param R    电路中的反馈/后级电阻（Ω）
 * @return     电容值（有效：>0；无效：-1）
 */
float Calculate_C_RC_Series(uint32_t fx, float n, float R1, float R) {
    if (fx == 0) {
        return -1.0f;
    }
    float term = R1 * R1 - n * n * (R1 + R) * (R1 + R);  // R1^2 - n^2*(R1+R)^2
    if (term <= 0.0f) {
        return -1.0f;
    }
    float omega = 2.0f * M_PI * (float)fx;  // 计算角频率
    float sqrtTerm = sqrtf(term);  // 计算根号内的值
    float denominator = omega * sqrtTerm;  // 计算分母
    if (denominator <= 0.0f) {
        return -1.0f;
    }
    float C = n / denominator;  // 根据公式计算电容值
    return C;
}

float Calculate_C_RC_Parallel(uint32_t fx, float n, float R1, float R) {
    // 输入合法性检查（频率、n、电阻需为正，且 0 < n < 1）
    if (fx == 0 || n <= 0.0f || n >= 1.0f || R1 <= 0.0f || R <= 0.0f) 
        return -1.0f;  

    // 计算分子：√[n2(R?+R)2 - R?2]
    float R_sum = R1 + R;
    float num_inside = n*n * R_sum*R_sum - R1*R1;  
    if (num_inside < 0.0f) return -1.0f;  // 根号内非负性检查
    float numerator = sqrtf(num_inside);  

    // 计算分母：2πf · R · R? · √(1 - n2)
    float den_inside = 1.0f - n*n;  
    if (den_inside <= 0.0f) return -1.0f;  // 根号内正性检查（n<1 理论成立，防浮点误差）
    float denominator = 2*M_PI*fx * R * R1 * sqrtf(den_inside);  

    // 最终计算电容 C（单位：F）
    return denominator == 0.0f ? -1.0f : numerator / denominator;  
}

float Calculate_L_RL_Series(uint32_t fx, float Au, float R1, float R) {
    if (fx == 0 || Au <= 0.0f || R1 <= 0.0f || R <= 0.0f) {
        return -1.0f;
    }
    float numer = (R1 * R1) / (Au * Au) - (R1 + R) * (R1 + R);
    if (numer < 0.0f) {
        return -1.0f;
    }
    float inside = numer;
    if (inside < 0.0f) {
        return -1.0f;
    }
    float omega = 2.0f * M_PI * (float)fx;
    return (1.0f / omega) * sqrtf(inside);
}

float Calculate_L_RL_Parallel(uint32_t fx, float Au, float Rg, float R, float RL) {
    // 参数检查
    if (fx == 0 || Au <= 0.0f || Au >= 1.0f || Rg <= 0.0f || R <= 0.0f || RL <= 0.0f) {
        return -1.0f;
    }
    
    const float omega = 2.0f * M_PI * (float)fx;  // 角频率
    
    // 计算传递函数分子项：Rg * (R + RL)
    float numerator = Rg * (R + RL);
    
    // 计算传递函数分母项：(Rg + RL) * R + Rg * RL
    float denominator_static = (Rg + RL) * R + Rg * RL;
    
    // 计算动态项系数：ω * (R + Rg)
    float omega_coeff = omega * (R + Rg);
    
    float term1 = Au * Au * denominator_static * denominator_static - numerator * numerator;
    float term2 = omega * omega * Rg * Rg - Au * Au * omega_coeff * omega_coeff;
    
    if (term2 == 0.0f || term1 / term2 <= 0.0f) {
        return -1.0f;  // 无实数解
    }
    
    return sqrtf(term1 / term2);
}

/**
 * @brief 在幅频曲线中找到与目标增益最接近的索引
 * @param gain         增益数组
 * @param total_points 数组长度
 * @param target_gain  目标增益比
 * @return 索引，失败返回 -1
 */
int FindClosestGainIndex(const float* gain, uint16_t total_points, float target_gain) {
    int best = -1;
    float min_diff = FLT_MAX;
    for (int i = 2; i < total_points; i++) {
        float diff = fabsf(gain[i] - target_gain);
        if (diff < min_diff) {
            min_diff = diff;
            best = i;
        }
    }
    return best;
}

/**
 * @brief 识别电路具体组态：RC 串/并，RL 串/并，纯电阻
 * @param au_low       低频增益（Aout/Ain）
 * @param au_high      高频增益（Aout/Ain）
 * @param gain         全频带增益数组
 * @param total_points 数组长度
 * @return CircuitType
 */
CircuitType IdentifyCircuitType(
    float au_low,
    float au_high,
    const float* gain,
    uint16_t total_points
) {
    const float resistive_variation = 0.05f;  // 纯电阻波动阈值
    const float eps_zero            = 0.09f;  // 近零判断阈值

    // —— 1. 纯电阻判定 —— 
    float max_gain = au_low, min_gain = au_low;
    for (int i = 0; i < total_points; i++) {
        if (gain[i] > max_gain) max_gain = gain[i];
        if (gain[i] < min_gain) min_gain = gain[i];
    }
    if ((max_gain - min_gain) < resistive_variation) {
        return RESISTIVE_ONLY;
    }

    // —— 2. RC vs RL —— 
    // 传入时已知道是 RC 或 RL（通过两点法），这里只根据增益方向和零点判断串并联
    float delta = au_high - au_low;
    if (delta > 0) {
        // RC 类（增益随频率上升）
        if (au_low < eps_zero)        return RC_SERIES;
        else                           return RC_PARALLEL;
    } else {
        // RL 类（增益随频率下降）
        if (au_high < eps_zero)       return RL_SERIES;
        else                           return RL_PARALLEL;
    }
}

void GenerateLogFrequencies(uint32_t* freq_list, uint16_t points, 
                           uint32_t start_freq, uint32_t end_freq)  {
     const double start_decade = log10(start_freq);
    const double end_decade = log10(end_freq);
    const double step = (end_decade - start_decade) / (points - 2); // 计算步长（points-2因为首尾特殊）

    // 第一个点固定为0（直流）
    freq_list[0] = 0;
	
    // 中间点按对数分布生成（10Hz~1MHz）
    for (int i = 1; i < points - 1; i++) {
        double log_freq = start_decade + (i-1) * step;
        freq_list[i] = (uint32_t)(pow(10, log_freq) + 0.5);
    }
	freq_list[points-1] = end_freq;
}

void MeasureFrequencyResponse(uint16_t total_points) {
	
    uint32_t frequencies[total_points];
    float gain[total_points];
    float au_low = 0.0f, au_high = 0.0f;
    float R_value = -1.0f;
    float C_value = -1.0f;
    float L_value = -1.0f;
	float RL_value = -1.0f;
    float cutoff_au = -1.0f;
    uint32_t cutoff_freq = 0;
    int cutoff_index = -1;
	
    float R1_known = 128.0f;
	const float R1_ref_RL   = 30.5f;   // RL 量程
    const float R1_ref_RC = 1025.0f;  // RC & 纯电阻 量程
	
	
	GPIO_SwitchMode(GPIOD, GPIO_PIN_11, Low);
	AD9833_AmpSet(110); 
	// —— A. 两点法快速测量 —— 
    // A.1 低频点：10 Hz
    AD9833_WaveSeting(0, 0, SIN_WAVE, 0);
	AD9833_AmpSet(200); 
    HAL_Delay(100);
    float vin_low  = Get_DC_Value_1();
    float vout_low = Get_DC_Value_2();
    au_low   = vout_low / vin_low;

    AD9833_WaveSeting(100000, 0, SIN_WAVE, 0);
	AD9833_AmpSet(150);
    HAL_Delay(100);
    float vin_midle  = Get_RMS_Value_1();
    float vout_midle = Get_RMS_Value_3();
    float au_midle   = vout_midle / vin_midle;
	
    // A.2 高频点：1.2 MHz
    AD9833_WaveSeting(1200000, 0, SIN_WAVE, 0);
	AD9833_AmpSet(150);
    HAL_Delay(100);
    float vin_high  = Get_RMS_Value_1();
    float vout_high = Get_RMS_Value_3();
    au_high   = vout_high / vin_high;
	
if (au_low < 0.03 && au_midle < 0.03 && au_high < 0.03) {
	GPIO_SwitchMode(GPIOD, GPIO_PIN_11, Floating);
	GPIO_SwitchMode(GPIOD, GPIO_PIN_12, Low);
	AD9833_AmpSet(130); 
	// —— A. 两点法快速测量 —— 
    // A.1 低频点：10 Hz
    AD9833_WaveSeting(0, 0, SIN_WAVE, 0);
	AD9833_AmpSet(210); 
    HAL_Delay(100);
    float vin_low  = Get_DC_Value_1();
    float vout_low = Get_DC_Value_2();
    au_low   = vout_low / vin_low;

    AD9833_WaveSeting(100000, 0, SIN_WAVE, 0);
	AD9833_AmpSet(150);
    HAL_Delay(100);
    float vin_midle  = Get_RMS_Value_1();
    float vout_midle = Get_RMS_Value_3();
    float au_midle   = vout_midle / vin_midle;
	
    // A.2 高频点：1.2 MHz
    AD9833_WaveSeting(1200000, 0, SIN_WAVE, 0);
	AD9833_AmpSet(140);
    HAL_Delay(100);
    float vin_high  = Get_RMS_Value_1();
    float vout_high = Get_RMS_Value_3();
    au_high   = vout_high / vin_high;
	if (au_low < 0.03 && au_midle < 0.03 && au_high < 0.03)
	{
    OLED_DrawImage(11, 6, &NULLImg, OLED_COLOR_NORMAL);  // 
    Gui_DrawFont_GBK16(8, 2, WHITE, BLACK, (unsigned char*)"NULL");
    OLED_ShowFrame();
	GPIO_Mode_Init();
    return;  // 提前返回，不再进行后续识别与计算
	}
}

	if(au_low - au_high > 0.01)
	{
//		R1_known = 41.0f;
		R1_known = 30.5f;
	GenerateLogFrequencies(&frequencies[0], total_points, 30, 700000);
	GPIO_Mode_Init();
	GPIO_SwitchMode(GPIOD, GPIO_PIN_11, Low);   // 使100Ω电阻接地
    GPIO_SwitchMode(GPIOD, GPIO_PIN_9, Low);    // 使51Ω电阻接地
	GPIO_SwitchMode(GPIOD, GPIO_PIN_8, Low);    // 使68Ω电阻接地	
	}
	else
	{
		R1_known = 1020.0f; 
		GPIO_Mode_Init();
		GPIO_SwitchMode(GPIOD,GPIO_PIN_12,Low);	// 使1000Ω电阻接地		
		GenerateLogFrequencies(&frequencies[0], total_points,10, 700000);
		HAL_Delay(100);
	}
	AD9833_WaveSeting(0, 0, SIN_WAVE, 0);

    // 清屏+坐标轴
    Lcd_Clear(BLACK);
    Gui_DrawLine(1, 130, 127, 130, WHITE);
    Gui_DrawLine(2, 2,   2, 130, WHITE);

    const float au_min = 0.0f, au_max = 1.1f;
    const int y_min = 2, y_max = 130;
    int last_x = -1, last_y = -1;
	static int last_amp = -1;  //静态记录上次设置的幅度
	
    // 2. 遍历测量并绘图
    for (int i = 0; i < total_points; i++) {
        AD9833_WaveSeting(frequencies[i], 0, SIN_WAVE, 0);

int amp = 120;  // 默认值
if (frequencies[i] == 0)            amp = 210;   //使用AD9833，设置频率为0，近似直流，但需要设置较高的幅度
else if (frequencies[i] <= 10000)   amp = 120;
else if (frequencies[i] <= 100000)  amp = 120;
else if (frequencies[i] <= 600000)  amp = 125;   //对AD9833的输出幅度进行动态补偿
else if (frequencies[i] <= 1000000) amp = 130;    
if (amp != last_amp) {
    AD9833_AmpSet(amp);
    last_amp = amp;
}
	
		HAL_Delay(145);
        float rms_in, rms_out;
       // 直流点使用直流测量函数
        if (i == 0) {
            rms_in  = Get_DC_Value_1();  // 通道2的直流电压（输入）
            rms_out = Get_DC_Value_2();  // 通道1的直流电压（输出）
        } 
        // 交流点使用RMS测量函数
        else {
            rms_in  = Get_RMS_Value_1(); // 通道2的交流RMS（输入）
            rms_out = Get_RMS_Value_3(); // 通道3的交流RMS（输出）
        }
        gain[i] = rms_out / rms_in;
        float au = gain[i];
        if (au > au_max) au = au_max;
        if (au < au_min) au = au_min;
        int y = y_max - (int)((au - au_min)*(y_max - y_min)/(au_max - au_min));
        int x = (i * 128) / total_points;
        if (last_x >= 0) Gui_DrawLine(last_x, last_y, x, y, WHITE);
        else          Gui_DrawPoint(x, y, WHITE);
        last_x = x; last_y = y;

        char buf[32];
        sprintf(buf, "AU: %.3f", gain[i]);
        Gui_DrawFont_GBK16(10, 138, WHITE, BLACK, (unsigned char*)buf);
    }

    // 3. 获取低频和高频增益
    au_low = gain[0];                  //取直流点为低频增益
    au_high = gain[total_points-1];    //取最后一个点为高频增益


	
    // 4. 识别电路类型
    CircuitType circuitType = IdentifyCircuitType(au_low, au_high, gain, total_points);
    
    // 5. 计算中点增益并找到截止频率点
    float min_gain = FLT_MAX;
float max_gain = -FLT_MAX;

// 在扫频过程中找到实际的最小和最大增益
for (int i = 0; i < total_points; i++) {
    if (gain[i] < min_gain) min_gain = gain[i];
    if (gain[i] > max_gain) max_gain = gain[i];
}

float au_mid = (min_gain + max_gain) * 0.5f;
cutoff_index = FindClosestGainIndex(gain, total_points, au_mid);
    if (cutoff_index >= 0) {
        cutoff_freq = frequencies[cutoff_index];
        cutoff_au = gain[cutoff_index];
    }
OLED_NewFrame(); 
    switch (circuitType) {
        case RC_SERIES:    // 低频增益≈1, 高频增益=k=R/(R1+R)
			OLED_DrawImage(11, 0, &RC_SERIESImg, OLED_COLOR_NORMAL);
            R_value = (R1_known * (1.0f - au_high)) / (au_high);
            if (cutoff_index >= 0 && R_value > 0) {
                C_value = Calculate_C_RC_Series(cutoff_freq, cutoff_au, R1_known, R_value);
            }
			R_value = (R1_known * (1.0f - au_high)) / (au_high);
            break;
            
        case RC_PARALLEL:  // 低频增益=k=R/(R1+R), 高频增益≈0
			OLED_DrawImage(10, 0, &RL_PARALLELImg, OLED_COLOR_NORMAL);
            R_value = (R1_known * (1.0f - au_low)) / (au_low);
            if (cutoff_index >= 0 && R_value > 0) {
                C_value = Calculate_C_RC_Parallel(cutoff_freq, cutoff_au, R1_known, R_value);
            }
            break;
            
        case RL_SERIES:    // 低频增益=k=R/(R1+R), 高频增益≈1
			OLED_DrawImage(12, 0, &RL_SERIESImg, OLED_COLOR_NORMAL);
            R_value = (R1_known * (1.0f - au_low)) / au_low;
            if (cutoff_index >= 0 && R_value > 0) {
                L_value = Calculate_L_RL_Series(cutoff_freq, cutoff_au, R1_known, R_value);
            }
            break;
            
case RL_PARALLEL:  // RL并联模型：直接计算R和RL，并调用L计算函数
    OLED_DrawImage(13, 0, &RL_PARALLELImg, OLED_COLOR_NORMAL);
    
    // 1. 通过高频增益计算并联电阻 R
    if (au_high > 0 && au_high < 1) {
        R_value = (R1_known * (1.0f - au_high)) / au_high;
    }

    // 2. 通过低频增益计算电感内阻 RL
    if (au_low > 0 && au_low < 1 && R_value > 0) {
        float numerator = R1_known * R_value * (1.0f - au_low);
        float denominator = au_low * (R1_known + R_value) - R1_known;
        RL_value = (denominator > 0) ? (numerator / denominator) : -1;
		char tbuf[32];
		sprintf(tbuf, "ESR%.1f", RL_value);
        Gui_DrawFont_GBK16(8, 82, WHITE, BLACK, (unsigned char*)tbuf);
//		OLED_PrintString( 30 , 52 , (char*)tbuf , &font12x12 , OLED_COLOR_NORMAL);
    }

    // 3. 调用函数计算电感 L（需确保参数有效）
    if (cutoff_index >= 0 && R_value > 0 && RL_value > 0) {
       L_value = Calculate_L_RL_Parallel(cutoff_freq, cutoff_au, R1_known, R_value, RL_value);
    }
    break;
			
		case RESISTIVE_ONLY:    // 
			OLED_DrawImage(11, 0, &RRRImg, OLED_COLOR_NORMAL);
            // 用低频增益计算一次
			float R_low = (R1_known * (1.0f - au_low)) / au_low;
    // 用高频增益计算一次
			float R_high = (R1_known * (1.0f - au_high)) / au_high;
    // 取平均，减小单侧误差
			R_value = (R_low  + R_high) / 2.0f;
            break;
    }

    // 7. 显示结果 - 添加电路类型标识
    const char* circuitName = "Unknown";
    switch (circuitType) {
        case RC_SERIES:    circuitName = "RC 串联"; break;
        case RC_PARALLEL:  circuitName = "RC 并联"; break;
        case RL_SERIES:    circuitName = "RL C"; break;
        case RL_PARALLEL:  circuitName = "RL b"; break;
		case RESISTIVE_ONLY: circuitName = "RRR"; break;
        default:           circuitName = "Unknown"; break;
    }
    
    char tbuf[32];
    sprintf(tbuf, "%s", circuitName);
    Gui_DrawFont_GBK16(8, 2, WHITE, BLACK, (unsigned char*)tbuf);
    
    // 显示增益信息
    sprintf(tbuf, "Low AU: %.3f", au_low);
    Gui_DrawFont_GBK16(8, 18, WHITE, BLACK, (unsigned char*)tbuf);
    sprintf(tbuf, "High AU: %.3f", au_high);
    Gui_DrawFont_GBK16(8, 34, WHITE, BLACK, (unsigned char*)tbuf);
    
    // 显示截止频率
    if (cutoff_index >= 0) {
        sprintf(tbuf, "Cutoff: %dHz", (int)cutoff_freq);
        Gui_DrawFont_GBK16(8, 50, WHITE, BLACK, (unsigned char*)tbuf);
		sprintf(tbuf, "AU1: %.3f", gain[cutoff_index]);
        Gui_DrawFont_GBK16(10, 138, WHITE, BLACK, (unsigned char*)tbuf);
		
    }

    // 显示计算结果
    if (R_value > 0) {
	char ohm[32];
		if (R_value>= 1000.0f)
                    sprintf(ohm, "R:%.2fkΩ", R_value / 1000.0f);
                else
                    sprintf(ohm, "R:%.2fΩ", R_value);
	if(C_value < 0 && L_value < 0)			
    OLED_PrintString( 35 , 52 , (char*)ohm , &font11x11 , OLED_COLOR_NORMAL);
	else
		OLED_PrintString( 2 , 52 , (char*)ohm , &font11x11 , OLED_COLOR_NORMAL);
		Gui_DrawFont_GBK16(13, 66, WHITE, BLACK, (unsigned char*)ohm);
    }
    
    if (C_value > 0) {
		if (C_value >= 1000.0f) {
    sprintf(tbuf, "C:%.2fuF", C_value * 1e9f / 1000.0f);  // 超过1μF显示为uF
} else {
    sprintf(tbuf, "C:%.2fnF", C_value * 1e9f);            // 否则显示为nF
}
//        sprintf(tbuf, "C=%.2fnF", C_value * 1e9f);
        Gui_DrawFont_GBK16(8, 82, WHITE, BLACK, (unsigned char*)tbuf);
		OLED_PrintASCIIString(67, 52,(char*)tbuf, &afont12x6, OLED_COLOR_NORMAL);
    }
    
    if (L_value > 0) {
        sprintf(tbuf, "L=%.2fmH", L_value * 1000.0f);
        Gui_DrawFont_GBK16(8, 98, WHITE, BLACK, (unsigned char*)tbuf);
		OLED_PrintASCIIString(67, 52,(char*)tbuf, &afont12x6, OLED_COLOR_NORMAL);
    }
	
	sprintf(tbuf, "R1_known:%.0f", R1_known);
    Gui_DrawFont_GBK16(8, 115, WHITE, BLACK, (unsigned char*)tbuf);
     OLED_ShowFrame();
    // 8. 标记截止频率线 & 恢复输出
    if (cutoff_index >= 0) {
        uint16_t x_cut = (cutoff_index * 128) / total_points;
        Gui_DrawDashedLine(x_cut, y_min, x_cut, y_max, WHITE, 2, 2);
    }
	
    AD9833_WaveSeting(00, 0, SIN_WAVE, 0);
	return;
}

