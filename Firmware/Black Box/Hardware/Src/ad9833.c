#include "ad9833.h"

// 静态变量保存当前波形配置
static uint16_t currentWaveCtrl = 0x2000; // 默认正弦波控制字

// ====== 延迟函数（简单占空循环） ======
static void AD9833_Delay(void)
{
    for (volatile int i = 0; i < 10; i++);
}

// ====== GPIO 初始化 ======
void AD9833_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = AD9833_FSYNC_PIN | AD9833_SCLK_PIN | AD9833_SDATA_PIN | AMP_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    FSYNC_HIGH();
    SCLK_HIGH();
    SDATA_LOW();
    AMP_CS_HIGH();
    
    // 初始化后设置默认状态
    currentWaveCtrl = 0x2000; // 正弦波
}

// ====== 写入 16 位数据到 AD9833 ======
void AD9833_Write(uint16_t TxData)
{
    SCLK_HIGH();
    FSYNC_HIGH();
    AD9833_Delay();
    FSYNC_LOW();
    AD9833_Delay();

    for (uint8_t i = 0; i < 16; i++)
    {
        if (TxData & 0x8000)
            SDATA_HIGH();
        else
            SDATA_LOW();

        AD9833_Delay();
        SCLK_LOW();
        AD9833_Delay();
        SCLK_HIGH();

        TxData <<= 1;
    }

    FSYNC_HIGH();
}

// ====== 幅度控制，适用于 MCP41010 数字电位器 ======
void AD9833_AmpSet(uint8_t amp)
{
    uint16_t temp = 0x1100 | amp;

    AMP_CS_LOW();
    for (uint8_t i = 0; i < 16; i++)
    {
        SCLK_LOW();
        if (temp & 0x8000)
            SDATA_HIGH();
        else
            SDATA_LOW();
        temp <<= 1;
        SCLK_HIGH();
        AD9833_Delay();
    }
    AMP_CS_HIGH();
}

// ====== 设置波形输出 ======
void AD9833_WaveSeting(double Freq, uint16_t Freq_SFR, uint16_t WaveMode, uint16_t Phase)
{
    uint16_t freq_LSB, freq_MSB, phase_data;
    double freq_word = Freq * (268435456.0 / 25000000.0); // Assuming 25MHz MCLK
    uint32_t freq_hex = (uint32_t)freq_word;

    freq_LSB = (uint16_t)(freq_hex & 0x3FFF);
    freq_MSB = (uint16_t)((freq_hex >> 14) & 0x3FFF);

    phase_data = Phase | 0xC000;

    AD9833_Write(0x0100);  // RESET
    AD9833_Write(0x2100);  // B28 = 1, RESET = 1

    if (Freq_SFR == 0)
    {
        AD9833_Write(freq_LSB | 0x4000);  // FREQ0 LSB
        AD9833_Write(freq_MSB | 0x4000);  // FREQ0 MSB
        AD9833_Write(phase_data);
    }
    else
    {
        AD9833_Write(freq_LSB | 0x8000);  // FREQ1 LSB
        AD9833_Write(freq_MSB | 0x8000);  // FREQ1 MSB
        AD9833_Write(phase_data);
    }

    // 保存并设置波形控制字
    if (WaveMode == TRI_WAVE)
        currentWaveCtrl = 0x2002;  // 三角波
    else if (WaveMode == SQU_WAVE)
        currentWaveCtrl = 0x2028;  // 方波
    else // 默认正弦波
        currentWaveCtrl = 0x2000;  // 正弦波
    
    // 发送波形控制命令
    AD9833_Write(currentWaveCtrl);
}

// ====== 关闭AD9833输出（进入睡眠模式） ======
void AD9833_Disable(void) {
    /* 
    设置控制寄存器：
    - SLEEP1 (D11) = 1 : 关闭内部时钟
    - SLEEP12 (D10) = 1 : 关闭DAC
    - 控制字: 0x2C00 (0010 1100 0000 0000)
    */
    AD9833_Write(0x2C00);
}

// ====== 开启AD9833输出（退出睡眠模式） ======
void AD9833_Enable(void) {
    /* 
    恢复之前保存的波形控制寄存器
    - 自动清除睡眠位（SLEEP1/SLEEP12=0）
    */
    AD9833_Write(currentWaveCtrl);
}