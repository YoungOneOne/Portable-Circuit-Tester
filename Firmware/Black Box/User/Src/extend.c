#include "font96.h"
#include "oled.h"
#include "adc_fft_rms.h"
#include "lcd_driver.h"
#include <stdio.h>
#include "gui.h"
#include "math.h"
#include <float.h>  // for FLT_MAX
#include "ad9833.h"
#include "gpio.h"
#include "black_box.h"
#include "extend.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void Get_DC(float* v1,float* v2)
{
	HAL_Delay(10);
	*v1 = Get_DC_Value_1();
    *v2 = Get_DC_Value_2();
//	OLED_NewFrame();
//	char ad1[32];
//	sprintf(ad1, "v1: %.3f", *v1);
//    OLED_PrintASCIIString(30, 0, (char*)ad1, &afont12x6, OLED_COLOR_NORMAL);
//	char ad2[32];
//	sprintf(ad2, "v2: %.3f", *v2);
//    OLED_PrintASCIIString(30, 10, (char*)ad2, &afont12x6, OLED_COLOR_NORMAL);
//	OLED_ShowFrame();
}
/**
 * @brief 元件类型识别
 * @return ComponentType
 */
//ComponentType IdentifyComponent(void)
//{
//    GPIO_Mode_Init(); // 所有引脚先设置为模拟输入，避免影响

//    const float HIGH_V_TH = 3.1f; // 高电平阈值
//    const float LOW_V_TH  = 0.25f; // 低电平阈值
//    float v1, v2, v3, v4, vf, vs;

//    // 第一步：1高2低，1000Ω接地，判断是否断路
//    Measure_F1T2_1000(); 
//    HAL_Delay(100);
//    Get_DC(&v1,&v2);
//    if (v1 > HIGH_V_TH && v2 < LOW_V_TH) // 若1高2低为断路
//		{
//		GPIO_Mode_Init();
//        // 检查反向（2高1低）
//        Measure_F2T1_1000();
//        HAL_Delay(600);
//		Get_DC(&v1,&v2);

//        if (v2 > HIGH_V_TH && v1 < LOW_V_TH)   //若反向也断路，可能为电容或大电阻
//			{
//			// 电容或电阻，尝试放电后再次测量
////		Measure_F2T1_Discharge();
////		HAL_Delay(100);
//		GPIO_Mode_Init();
//	    GPIO_SwitchMode(GPIOC,GPIO_PIN_0,High);   //打开继电器，接入AD9833
//	    GPIO_SwitchMode(GPIOD,GPIO_PIN_11,Low);    //1000欧电阻接地
//	    AD9833_WaveSeting(10, 0, SIN_WAVE, 0);  // 设置频率为600kHz，波形为正弦波
//	    HAL_Delay(160);              //延时
//        v1 = Get_RMS_Value_3();  	
//		AD9833_WaveSeting(1000000, 0, SIN_WAVE, 0);  // 设置频率为600kHz，波形为正弦波
//		HAL_Delay(160);  
//        v2 = Get_RMS_Value_3();        //通过1和3通道获得正弦电压有效值，进行比较
//				
//		GPIO_SwitchMode(GPIOC,GPIO_PIN_0,Low);   //打开继电器，接入AD9833
//				
//        Measure_F1T2_510k();
//	    HAL_Delay(230);              //延时
//        v3 = Get_DC_Value_1();  	
//        v4 = Get_DC_Value_2();        //通过1和3通道获得正弦电压有效值，进行比较
//				
//		OLED_NewFrame();
//	  char ad1[32];
//		sprintf(ad1, "vf: %.3f", v1);
//      OLED_PrintASCIIString(30, 0, (char*)ad1, &afont12x6, OLED_COLOR_NORMAL);
//	  
//	  char ad2[32];
//		sprintf(ad2, "vs: %.3f", v2);
//      OLED_PrintASCIIString(30, 10, (char*)ad2, &afont12x6, OLED_COLOR_NORMAL);
//	  OLED_ShowFrame();
//			HAL_Delay(500);
//		if((v2-v1)>0.1)       //低频高频激励差值大于阈值，为电容
//		{
//			GPIO_SwitchMode(GPIOC,GPIO_PIN_0,Low);  //退出前关闭继电器，断开AD9833
//			return CAPACITOR;
//		}
//		if(v4>0.5)
//		{                      //否则为电阻
//			GPIO_SwitchMode(GPIOC,GPIO_PIN_0,Low);  //关闭继电器，断开AD9833
//			return RESISTOR;
//		}
//		if(v4<0.5)       //低频高频激励差值大于阈值，为电容
//		{
//			GPIO_SwitchMode(GPIOC,GPIO_PIN_0,Low);  //退出前关闭继电器，断开AD9833
//			return UNKNOWN;
//		}
//		
//        } else {
//            return DIODE_2_1;  // 单向导通：二极管 2→1
//        }
//    }
//	
//	// 若1高2低非断路,2高1低，1000欧姆接地判断是否断路
//        Measure_F2T1_1000();
//        HAL_Delay(170);
//        Get_DC(&v1,&v2);
//        if (v2 > HIGH_V_TH && v1 < LOW_V_TH) //断路，二极管1-2
//			{
//        return DIODE_1_2;
//    }
//		else      //若非断路，可能是电感和小电阻，用高频正弦信号判断
//			{
//		GPIO_Mode_Init();
//	    GPIO_SwitchMode(GPIOC,GPIO_PIN_0,High);   //打开继电器，接入AD9833
//	    GPIO_SwitchMode(GPIOD,GPIO_PIN_11,Low);    //1000欧电阻接地
//	    AD9833_WaveSeting(100, 0, SIN_WAVE, 0);  // 设置频率为600kHz，波形为正弦波
//	    HAL_Delay(160);              
//        v1 = Get_RMS_Value_3()/Get_RMS_Value_1();  
//		AD9833_WaveSeting(1000000, 0, SIN_WAVE, 0);  // 设置频率为600kHz，波形为正弦波
//		HAL_Delay(160);  
//        v2 = Get_RMS_Value_3()/Get_RMS_Value_1();        //通过1和3通道获得正弦电压有效值，进行比较
//////		OLED_NewFrame();
//////	  char ad1[32];
//////		sprintf(ad1, "vf: %.2f", v1);
//////      OLED_PrintASCIIString(30, 0, (char*)ad1, &afont12x6, OLED_COLOR_NORMAL);
//////	  char ad2[32];
//////		sprintf(ad2, "vd: %.2f", v2);
//////      OLED_PrintASCIIString(30, 10, (char*)ad2, &afont12x6, OLED_COLOR_NORMAL);
//////	  OLED_ShowFrame();
//////	HAL_Delay(500);

//	    if(fabs(v1-v2)>0.18 )       //高频正弦信号激励下断路，说明是电感
//		{
//			return INDUCTOR;
//		}
//		else{
//			return RESISTOR;
//		}
//        }
//}

ComponentType IdentifyComponent(void)
{
    GPIO_Mode_Init(); // 所有引脚先设置为模拟输入，避免影响
	GPIO_SwitchMode(GPIOC,GPIO_PIN_0,High);   //打开继电器，接入AD9833
	GPIO_SwitchMode(GPIOD,GPIO_PIN_11,Low);    //100欧电阻接地
	float v1, v2, v3, v4, vf, vs;
	const float HIGH_V_TH = 3.1f; // 高电平阈值
    const float LOW_V_TH  = 0.15f; // 低电平阈值
	// 在100Ω电阻接地条件下，优先通过高低频增益判断是否为电容或电感，加快整体判断测量所需时间
	AD9833_WaveSeting(10, 0, SIN_WAVE, 0);  // 设置频率为10Hz，波形为正弦波
	HAL_Delay(180);              //延时
    v1 = Get_RMS_Value_1();
	v2 = Get_RMS_Value_3();  	
	vf = v2/v1;
	AD9833_WaveSeting(1300000, 0, SIN_WAVE, 0);  // 设置频率为1300kHz，波形为正弦波
	HAL_Delay(180);  
    v1 = Get_RMS_Value_1();        //通过1和3通道获得正弦电压有效值，进行比较
	v2 = Get_RMS_Value_3();
	vs = v2/v1;
	
	GPIO_SwitchMode(GPIOC,GPIO_PIN_0,Low);        //关闭继电器，断开AD9833
	
    if((vf - vs) > 0.3)      //低频增益大于高频，为电感
	{
		return INDUCTOR;
			
	}
	else if((vs - vf) > 0.3)    //低频增益小于高频，为电容
	{
		return CAPACITOR;
	}
		//  1高2低，1000Ω接地，判断剩余组态
	Measure_F1T2_1000(); 
    HAL_Delay(150);
    Get_DC(&v1,&v2);
    if (v2 > LOW_V_TH) // 若非断路
		{
        // 检查反向（2高1低）
        Measure_F2T1_1000();
        HAL_Delay(170);
		Get_DC(&v1,&v2);
			if (v1 < LOW_V_TH)   //正向不断路，反向断路，为二极管正向
			{
				return DIODE_1_2;
				
			}
			else               // 大小两个档位判断是否断路， 都断路，判断为NULL，否则为电阻
			{
				Measure_F1T2_1000();
				HAL_Delay(100);
				v3 = Get_DC_Value_2();
				Measure_F1T2_100k();
				HAL_Delay(100);
				v4= Get_DC_Value_2();
				if(v3<LOW_V_TH && v4<LOW_V_TH)
				{
				  return UNKNOWN;
				}
				else
				{
					return RESISTOR;
				}
				
			}
    }
	// 若1高2低非断路,2高1低，1000欧姆接地判断是否断路
        Measure_F2T1_1000();
        HAL_Delay(180);
        Get_DC(&v1,&v2);
        if (v1 > LOW_V_TH) //正向断路，反向不断路，为二极管反向
			{
        return DIODE_2_1;
    }
		else           // 双向断路，可能是NULL或者大电阻，需要换用高档位，重测2端电压，若仍断路，判断为NULL，否则是大电阻
			{
				Measure_F1T2_1000();
				HAL_Delay(100);
				v3 = Get_DC_Value_2();
				Measure_F1T2_100k();
				HAL_Delay(100);
				v4=  Get_DC_Value_2();
				if(v3<LOW_V_TH && v4<LOW_V_TH)
				{
				  return UNKNOWN;
				}
				else
				{
					return RESISTOR;
				}
			}
}
//-------------------- 识别电路类型 -------------------------
// 根据幅频曲线的单调性判断电容或电感
CircuitTypes IdentifyCircuitType__(float au_low, float au_high, const float* gain, uint16_t total_points) {
    const float threshold = 0.2f;

    if (au_low > au_high) {
        return RL_SERIES_;
    } else if (au_low < au_high) {
        return RC_SERIES_;
    } else {
        return RESISTIVE_;
    }
}

// ------------------------- RC 单点法 -------------------------
// 能取到高频稳定增益的RC组合，用单点法
float Calculate_RC_Series1_(uint32_t fx, float n, float R1, float R) {
    if (fx == 0) return -1.0f;

    float term = R1 * R1 - n * n * (R1 + R) * (R1 + R);
    if (term <= 0.0f) return -1.0f;

    float omega = 2.0f * M_PI * fx;
    float sqrtTerm = sqrtf(term);
    float denominator = omega * sqrtTerm;
    if (denominator <= 0.0f) return -1.0f;

    return n / denominator;
}

// ------------------------- RC 双点法 -------------------------
// 不能取到高频稳定增益的RC组合，用双点法
int Calculate_RC_Series2_(uint32_t f1, uint32_t f2,
                         float n1, float n2,
                         float R1,
                         float* R, float* C) {
    if (!R || !C || f1 == 0 || f2 == 0 || f1 == f2 || n1 <= 0 || n2 <= 0 || R1 <= 0) return -1;

    float w1 = 2.0f * M_PI * f1;
    float w2 = 2.0f * M_PI * f2;

    float y1 = (R1 * R1) / (n1 * n1);
    float y2 = (R1 * R1) / (n2 * n2);

    float k = (w2 / w1) * (w2 / w1);
    if (k == 1.0f) return -1;

    float num = k * y2 - y1;
    float den = k - 1.0f;
    if (den == 0.0f || num <= 0.0f) return -1;

    float A2 = num / den;
    float A = sqrtf(A2);
    if (A <= R1) return -1;

    float inside = y1 - A2;
    if (inside <= 0.0f) return -1;

    float C_val = 1.0f / (w1 * sqrtf(inside));
    float R_val = A - R1;
    if (C_val <= 0.0f || R_val <= 0.0f) return -1;

    *R = R_val;
    *C = C_val;
    return 0;
}

// ------------------------- RL 计算 -------------------------

float Calculate_L_RL(uint32_t fx, float Au, float R1, float R) {
    if (fx == 0 || Au <= 0 || R1 <= 0 || R <= 0) return -1.0f;

    float numer = (R1 * R1) / (Au * Au) - (R1 + R) * (R1 + R);
    if (numer < 0.0f) return -1.0f;

    float omega = 2.0f * M_PI * fx;
    return (1.0f / omega) * sqrtf(numer);
}

// ------------------------- 对数频率生成 -------------------------

void GenerateLogFrequencies__(uint32_t* freq_list, uint16_t points) {
    double start_decade = log10(10);   // 扫频起始频率
    double end_decade = log10(1200000);  // 扫频终止频率
    double step = (end_decade - start_decade) / (points - 2);

    freq_list[0] = 0;    // 第一个点设置为直流
    for (int i = 1; i < points - 1; i++) {
        double log_freq = start_decade + (i - 1) * step;
        freq_list[i] = (uint32_t)(pow(10, log_freq) + 0.5);
    }
    freq_list[points - 1] =1200000;     // 最后一个点设置为高频
}

// ------------------------- 查找中间点附近的两点 -------------------------
//  两点法找计算点
int GetTransitionBandPoints_(const uint32_t* frequencies, const float* gain, uint16_t total_points,
                             uint32_t* f1, float* n1, uint32_t* f2, float* n2) {
    if (!frequencies || !gain || !f1 || !n1 || !f2 || !n2 || total_points < 5) return -1;

    int valid_start = 1;
    int valid_end = total_points - 2;

    float min_gain = FLT_MAX, max_gain = -FLT_MAX;
    for (int i = valid_start; i <= valid_end; i++) {
        if (gain[i] < min_gain) min_gain = gain[i];
        if (gain[i] > max_gain) max_gain = gain[i];
    }
    float au_mid = 0.5f * (min_gain + max_gain);

    int best_idx = -1;
    float min_diff = FLT_MAX;
    for (int i = valid_start; i <= valid_end; i++) {
        float diff = fabsf(gain[i] - au_mid);
        if (diff < min_diff) {
            min_diff = diff;
            best_idx = i;
        }
    }

    if (best_idx <= valid_start || best_idx + 2 >= total_points) return -1;

    *f1 = frequencies[best_idx - 1];
    *n1 = gain[best_idx - 1];
    *f2 = frequencies[best_idx + 1];
    *n2 = gain[best_idx + 1];
    return (*f1 < *f2) ? 0 : -1;
}

// ------------------------- 查找最接近目标增益的索引 -------------------------
// 通过高低频增益，取中间值，找距离中间值增益最近的频点，为过渡带上一点，用于计算
int FindClosestGainIndex_(const float* gain, uint16_t total_points, float target_gain) {
    int best = -1;
    float min_diff = FLT_MAX;
    for (int i = 1; i < total_points; i++) {
        float diff = fabsf(gain[i] - target_gain);
        if (diff < min_diff) {
            min_diff = diff;
            best = i;
        }
    }
    return best;
}

// ------------------------- 主测量函数 -------------------------
// 扫频测量
MeasurementResults MeasureFrequencyResponse__(uint16_t total_points) {
    uint32_t frequencies[total_points];
    float gain[total_points];
    float R_value = -1.0f, C_value = -1.0f, L_value = -1.0f;
    const float R1_known = 30.5f;
    GenerateLogFrequencies__(frequencies, total_points);
static int last_amp = -1;
	AD9833_WaveSeting(0 , 0, SIN_WAVE, 0);
//  清屏+坐标轴  ，调试观察波形用
    Lcd_Clear(BLACK);
    Gui_DrawLine(1, 130, 127, 130, WHITE);
    Gui_DrawLine(2, 2,   2, 130, WHITE);

    const float au_min = 0.0f, au_max = 1.1f;
    const int y_min = 2, y_max = 130;
    int last_x = -1, last_y = -1;

    // === 扫频 & 采样 & 绘图 ===
    for (int i = 0; i < total_points; i++) {
        AD9833_WaveSeting(frequencies[i], 0, SIN_WAVE, 0);
		int amp = 120;  // 默认值
if (frequencies[i] == 0)            amp = 190;   //使用AD9833，设置频率为0，近似直流，但需要设置较高的幅度，才能获得正确的直流增益
else if (frequencies[i] <= 10000)   amp = 120;
else if (frequencies[i] <= 100000)  amp = 120;
else if (frequencies[i] <= 600000)  amp = 125;   //对AD9833的输出幅度进行动态补偿（AD9833的输出幅度会随着频率升高而降低）
else if (frequencies[i] <= 1000000) amp = 130;    
else if (frequencies[i] > 1000000) amp = 150;    
if (amp != last_amp) {
    AD9833_AmpSet(amp);
    last_amp = amp;
}
        HAL_Delay(165);

        float rms_in = (i == 0) ? Get_DC_Value_1() : Get_RMS_Value_1();
        float rms_out = (i == 0) ? Get_DC_Value_2() : Get_RMS_Value_3();
         gain[i] = rms_out / rms_in;
        float au = gain[i];
        if (au > au_max) au = au_max;
        if (au < au_min) au = au_min;
        int y = y_max - (int)((au - au_min)*(y_max - y_min)/(au_max - au_min));
        int x = (i * 128) / total_points;
        if (last_x >= 0) Gui_DrawLine(last_x, last_y, x, y, WHITE);
        else          Gui_DrawPoint(x, y, WHITE);
        last_x = x; last_y = y;
    }

    float au_low = gain[0];
    float au_high = gain[total_points - 1];
    CircuitTypes type = IdentifyCircuitType__(au_low, au_high, gain, total_points);

    float min_gain = FLT_MAX, max_gain = -FLT_MAX;
    for (int i = 0; i < total_points; i++) {
        if (gain[i] < min_gain) min_gain = gain[i];
        if (gain[i] > max_gain) max_gain = gain[i];
    }
    float au_mid = 0.5f * (min_gain + max_gain);
    int cutoff_index = FindClosestGainIndex_(gain, total_points, au_mid);
    uint32_t cutoff_freq = (cutoff_index >= 0) ? frequencies[cutoff_index] : 0;
    float cutoff_au = (cutoff_index >= 0) ? gain[cutoff_index] : -1.0f;

    uint32_t f1 = 0, f2 = 0;
    float n1 = 0, n2 = 0;
    GetTransitionBandPoints_(frequencies, gain, total_points, &f1, &n1, &f2, &n2);

    switch (type) {
        case RC_SERIES_:
            if (gain[total_points - 1] - gain[total_points - 2] < 0.08f) {
                R_value = (R1_known * (1.0f - au_high )) / (au_high);
				C_value = Calculate_RC_Series1_(cutoff_freq, cutoff_au, R1_known, R_value);
//				if(R_value < 3)
//				{
//					R_value = 0;
//				}
//                C_value = Calculate_RC_Series1_(cutoff_freq, cutoff_au + 0.04f, R1_known, R_value);
				if(R_value < 10  || (C_value > 1.6e-6 && C_value < 4.6e-6))    // 线性补偿
				{
					
					if(C_value > 1.6e-6 && C_value < 4.6e-6 )
					{
					C_value = C_value * 1.2 ;
					}
					else 
					{
						C_value = C_value * 1.14 ;
					}
					
				}
            } else {
                Calculate_RC_Series2_(f1, f2, n1, n2, R1_known, &R_value, &C_value);
				R_value = 0;
            }
            break;

        case RL_SERIES_:
            R_value = ((R1_known * (1.0f - au_low)) / au_low);
            L_value = Calculate_L_RL(cutoff_freq, cutoff_au, R1_known, R_value);

		break;


        default:
            break;
    }
	char tbuf[32];
 // 显示截止频率
    if (cutoff_index >= 0) {
        sprintf(tbuf, "Cutoff: %dHz", (int)cutoff_freq);
        Gui_DrawFont_GBK16(8, 50, WHITE, BLACK, (unsigned char*)tbuf);
		sprintf(tbuf, "AU1: %.3f", gain[cutoff_index]);
        Gui_DrawFont_GBK16(10, 138, WHITE, BLACK, (unsigned char*)tbuf);
		
    }
    AD9833_WaveSeting(000, 0, SIN_WAVE, 0);
    MeasurementResults result = { R_value, C_value, L_value };
    return result;
}


float MeasureDiode(ComponentType diodeType)    //二极管测量压降模块，根据类型选择计算方向，串联1000Ω电阻，限流在10mA以下
{
	GPIO_Mode_Init();
    float v_forward = 0.0f;   // 二极管压降
    
    if (diodeType == DIODE_1_2) {
        // 测量正向（1→2方向）
        Measure_F1T2_1000();  // 1脚输出高电平，2脚接地
        HAL_Delay(250);        // 等待测量稳定
        
        v_forward = Get_DC_Value_1()-Get_DC_Value_2();
   
    } 
    else if (diodeType == DIODE_2_1) {
        // 测量正向（2→1方向）
        Measure_F2T1_1000();  // 2脚输出高电平，1脚接地
        HAL_Delay(250);
        
        v_forward = Get_DC_Value_2()-Get_DC_Value_1();
        
    }
    
    return v_forward;
}

// 电阻测量电阻档位定义区
#define R_REF_510K   475000.0f
#define R_REF_100K   103000.0f
#define R_REF_1K     1030.0f
#define R_REF_100    127.0f
#define R_REF_31     30.3f
/**
 * 每档有效测量上限（取参考阻值的 90%）
 */
#define MAX_510K     310000.0f      // 最高档不做上限判断
#define MAX_100K     (R_REF_100K * 0.15f)
#define MAX_1K       (R_REF_1K   * 0.2f)
#define MAX_100      (R_REF_100   * 0.3f)
#define MAX_31      0


/**
 * @brief   单次测量流程：切换档位→延时→读数→计算 Rx
 * @param   switchFunc  切换档位函数指针，如 Measure_F1T2_510k
 * @param   Rref        该档位参考电阻值（Ω）
 * @return  计算得到的被测电阻 Rx（Ω）
 */

static float MeasureOnce(float Rref)
{
    HAL_Delay(350);
    float v1 = Get_DC_Value_1();  
    float v2 = Get_DC_Value_2();  

    // Rx
    float Rx_measured = Rref * (v1 - v2) / v2;


    // OLED 显示调试电压
//    char v[50];
//    sprintf(v, "v1:%.3f  v2:%.3f", v1, v2);
//    OLED_PrintASCIIString(0, 0, (char*)v, &afont12x6, OLED_COLOR_NORMAL); 

    return Rx_measured;
}

/**
 * @brief   自上而下自动量程测阻，满足要求则选择该档位，否则切换到下一个低档位再次测量
 * @return  最终测得的被测电阻 Rx（Ω）
 */
float MeasureRESISTOR(void)
{
    float rx;
	GPIO_SwitchMode(GPIOC, GPIO_PIN_0, Low);    // 关闭继电器
	HAL_Delay(100);
    // 510 kΩ 档
	Measure_F1T2_510k();
    rx = MeasureOnce(R_REF_510K);
    if (rx >= MAX_510K) 
	{
//		OLED_PrintASCIIString(0, 10, "510k", &afont12x6, OLED_COLOR_NORMAL);  //调试用，显示选择的电阻档位
//		OLED_ShowFrame();
//		HAL_Delay(100);
		return rx;
	}

    // 100 kΩ 档
	Measure_F1T2_100k();
    rx = MeasureOnce(R_REF_100K);
    if (rx >= MAX_100K) {
//		OLED_PrintASCIIString(0, 10, "100k", &afont12x6, OLED_COLOR_NORMAL);  //调试用，显示选择的电阻档位
//		OLED_ShowFrame();
//		HAL_Delay(100);
		return rx;
	}

    // 1 kΩ 档
	Measure_F1T2_1000();
    rx = MeasureOnce(R_REF_1K);
    if (rx >= MAX_1K) {
//		OLED_PrintASCIIString(0, 10, "1k", &afont12x6, OLED_COLOR_NORMAL);  //调试用，显示选择的电阻档位
//		OLED_ShowFrame();
//		HAL_Delay(100);
		return rx;
	}

    // 100 Ω 档（最后一档，直接返回）
	Measure_F1T2_100();
	rx = MeasureOnce(R_REF_100);
	   if (rx >= MAX_100) {
//	OLED_PrintASCIIString(0, 10, "100", &afont12x6, OLED_COLOR_NORMAL);  //调试用，显示选择的电阻档位
//	OLED_ShowFrame();
//	HAL_Delay(100);
		return rx;
	}

	Measure_F1T2_31();
//	OLED_PrintASCIIString(0, 10, "31", &afont12x6, OLED_COLOR_NORMAL);  //调试用，显示选择的电阻档位
//	OLED_ShowFrame();
//	HAL_Delay(100);
    return MeasureOnce(R_REF_31);
}

/**
 * @brief 测量电容和其 ESR（等效串联电阻）
 * @param R_out ESR（Ω）
 * @param C_out 电容值（nF）
 */
void MeasureCapacitance(float* R_out, float* C_out)
{
    GPIO_Mode_Init();
    GPIO_SwitchMode(GPIOC, GPIO_PIN_0, High);    // 继电器打开，接入AD9833
	GPIO_SwitchMode(GPIOD, GPIO_PIN_9, Low);    // 使100欧电阻接地
	GPIO_SwitchMode(GPIOD, GPIO_PIN_8, Low);    // 100欧电阻接地
	GPIO_SwitchMode(GPIOD, GPIO_PIN_11, Low);
	AD9833_AmpSet(120); 
    HAL_Delay(200);

    MeasurementResults result = MeasureFrequencyResponse__(18);   // 获取测量结果

    GPIO_SwitchMode(GPIOC, GPIO_PIN_0, Low);     // 关闭继电器

    if (R_out) *R_out = result.R;
    if (C_out) *C_out = result.C * 1e9f;  // 转换为 nF
}

/**
 * @brief 测量电感值和其 ESR（等效串联电阻）
 * @param R_out ESR（Ω）
 * @param L_out 电感值（mH）
 */
void MeasureINDUCTOR(float* R_out, float* L_out)
{
    GPIO_Mode_Init();
    GPIO_SwitchMode(GPIOC, GPIO_PIN_0, High);    // 打开继电器，接入AD9833
    GPIO_SwitchMode(GPIOD, GPIO_PIN_9, Low);    // 使100欧电阻接地
	GPIO_SwitchMode(GPIOD, GPIO_PIN_8, Low);    // 100欧电阻接地
	GPIO_SwitchMode(GPIOD, GPIO_PIN_11, Low);
	AD9833_AmpSet(120); 
    HAL_Delay(200);

    MeasurementResults result = MeasureFrequencyResponse__(18);   // 获取测量结果

    GPIO_SwitchMode(GPIOC, GPIO_PIN_0, Low);     // 关闭继电器

    if (R_out) *R_out = result.R;
    if (L_out) *L_out = result.L * 1000.0f;  // 转换为 mH
}




void TwoTerminalComponent(void)
{
	OLED_NewFrame();
	GPIO_SwitchMode(GPIOC, GPIO_PIN_0, Low);    // 关闭继电器
    int comp = IdentifyComponent();
//	Measure_F2T1_Discharge();
    const char* compName = "Unknown";
    char displayStr[32] = {0};
    char esrStr[32] = {0};
    int showESR = 0;

    switch (comp) {
        case RESISTOR:
            compName = "Resistor";
            {
                float R = MeasureRESISTOR();
				OLED_DrawImage(13, 0, &RRRImg, OLED_COLOR_NORMAL);
                if (R >= 1000.0f)
                    sprintf(displayStr, "R:%.1fkΩ", R / 1000.0f);
                else
                    sprintf(displayStr, "R:%.2fΩ", R);
            }
            break;

        case CAPACITOR:
            compName = "Capacitor";
            {
                float R_esr = 0.0f;
                float C_val = 0.0f;
                MeasureCapacitance(&R_esr, &C_val);
                if (C_val >= 1000.0f) {
    sprintf(displayStr, "C:%.2fuF", C_val / 1000.0f);  // 超过1μF显示为uF
} else {
    sprintf(displayStr, "C:%.2fnF", C_val);            // 否则显示为nF
}
                sprintf(esrStr, "ESR:%.2fΩ", R_esr);
				OLED_DrawImage(13, 0, &RC_SERIESImg, OLED_COLOR_NORMAL);
                showESR = 1;
            }
            break;

        case INDUCTOR:
    compName = "INDUCTOR";
    {
        float R_esr = 0.0f;
        float L_val = 0.0f;
        MeasureINDUCTOR(&R_esr, &L_val);
        if (L_val < 1.0f)
            sprintf(displayStr, "L:%.2fuH", L_val * 1000.0f);
        else
            sprintf(displayStr, "L:%.2fmH", L_val);
        sprintf(esrStr, "ESR:%.2fΩ", R_esr);
        OLED_DrawImage(13, 0, &RL_SERIESImg, OLED_COLOR_NORMAL);
        showESR = 1;
    }
    break;


        case DIODE_1_2:
            compName = "Diode 1->2";
            {
                float Vd = MeasureDiode(comp);
                sprintf(displayStr, "Vf:%.2fV", Vd);
				OLED_DrawImage(13, 0, &Diode2Img, OLED_COLOR_NORMAL);
            }
            break;

        case DIODE_2_1:
            compName = "Diode 2->1";
            {
                float Vd = MeasureDiode(comp);
                sprintf(displayStr, "Vf:%.2fV", Vd);
				OLED_DrawImage(13, 0, &Diode1Img, OLED_COLOR_NORMAL);
            }
            break;

        default:
            compName = "Unknown";
//            sprintf(displayStr, "No Match");
		OLED_DrawImage(11, 6, &NULLImg, OLED_COLOR_NORMAL);
            break;
    }

//OLED_NewFrame();
//OLED_PrintASCIIString(40, 30, (char*)compName, &afont12x6, OLED_COLOR_NORMAL);

if (comp == CAPACITOR || comp == INDUCTOR) {
    // 左下显示 ESR，右下显示 C/L 值
    OLED_PrintString(2, 52, esrStr, &font11x11, OLED_COLOR_NORMAL);
    OLED_PrintString(68, 52, displayStr, &font12x12, OLED_COLOR_NORMAL);
} else {
    // 居中显示测量值
    OLED_PrintString(36, 52, displayStr, &font11x11, OLED_COLOR_NORMAL);
}
Measure_F2T1_Discharge();
    OLED_ShowFrame();
	HAL_Delay(1000);
}
