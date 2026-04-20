#include "AD9834.h"

/*
 * AD9834 DDS 驱动源文件 / AD9834 DDS driver source.
 *
 * 中文: 协议时序沿用已验证实现，只把 GPIO 操作迁移到 HAL。
 * English: Timing sequence follows a verified implementation, with GPIO access ported to HAL.
 */

void AD9834_Write_16Bits(uint16_t data)
{
    uint8_t i;

    AD9834_SCLK_SET();
    AD9834_FSYNC_CLR();

    for (i = 0; i < 16; i++)
    {
        if (data & 0x8000U)
            AD9834_SDATA_SET();
        else
            AD9834_SDATA_CLR();

        AD9834_SCLK_CLR();
        data <<= 1;
        AD9834_SCLK_SET();
    }

    AD9834_SDATA_SET();
    AD9834_FSYNC_SET();
}

void AD9834_Select_Wave(uint16_t wave_type)
{
    AD9834_FSYNC_SET();
    AD9834_SCLK_SET();

    AD9834_Write_16Bits(wave_type);
}

void AD9834_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    /*
     * 中文: 如果 AD9834 引脚迁移到其他端口，需要同步修改这里的端口时钟使能。
     * English: If AD9834 pins move to other ports, update the GPIO clock enable calls here.
     */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStructure.Pin = AD9834_FSYNC_PIN;
    HAL_GPIO_Init(AD9834_FSYNC_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = AD9834_SCLK_PIN;
    HAL_GPIO_Init(AD9834_SCLK_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = AD9834_SDATA_PIN;
    HAL_GPIO_Init(AD9834_SDATA_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = AD9834_RESET_PIN;
    HAL_GPIO_Init(AD9834_RESET_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = AD9834_FS_PIN;
    HAL_GPIO_Init(AD9834_FS_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = AD9834_PS_PIN;
    HAL_GPIO_Init(AD9834_PS_PORT, &GPIO_InitStructure);

    AD9834_FS_LOW();
    AD9834_PS_LOW();

    AD9834_Write_16Bits(0x2100U);
    AD9834_Write_16Bits(0x2038U);
    AD9834_Write_16Bits(0xC000U);
    AD9834_Write_16Bits(0x2100U);
}

void AD9834_Set_Freq(uint8_t freq_number, uint32_t freq)
{
    uint32_t FREQREG = (uint32_t)((((uint64_t)freq) << 28) / AD9834_SYSTEM_CLOCK);
    uint16_t FREQREG_LSB_14BIT = (uint16_t)(FREQREG & 0x3FFFU);
    uint16_t FREQREG_MSB_14BIT = (uint16_t)((FREQREG >> 14) & 0x3FFFU);

    if (freq_number == AD9834_FREQ0)
    {
        FREQREG_LSB_14BIT |= 0x4000U;
        FREQREG_MSB_14BIT |= 0x4000U;
    }
    else
    {
        FREQREG_LSB_14BIT |= 0x8000U;
        FREQREG_MSB_14BIT |= 0x8000U;
    }

    AD9834_Write_16Bits(FREQREG_LSB_14BIT);
    AD9834_Write_16Bits(FREQREG_MSB_14BIT);
}

bool AD9834_ConfigureOutput(AD9834_Waveform_t waveform, uint32_t freq_hz)
{
    uint16_t wave_word;

    if (freq_hz > AD9834_MAX_OUTPUT_FREQ)
    {
        return false;
    }

    switch (waveform)
    {
        case AD9834_WAVEFORM_SINE:
            wave_word = AD9834_WAVE_SINE;
            break;

        case AD9834_WAVEFORM_TRIANGLE:
            wave_word = AD9834_WAVE_TRIANGLE;
            break;

        case AD9834_WAVEFORM_SQUARE:
            wave_word = AD9834_WAVE_SQUARE;
            break;

        default:
            return false;
    }

    /*
     * 中文: 先进入 RESET 状态，避免写频率字时输出产生明显毛刺。
     * English: Enter RESET before updating frequency words to reduce output glitches.
     */
    AD9834_Write_16Bits(0x2100U);
    AD9834_Set_Freq(AD9834_FREQ0, freq_hz);

    /* 中文: 写入目标波形并退出 RESET。 / English: Write waveform control word and leave RESET. */
    AD9834_Select_Wave(wave_word);

    return true;
}
