#ifndef __AD9834_H
#define __AD9834_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * AD9834 DDS 驱动（STM32F407 HAL）/ AD9834 DDS driver for STM32F407 HAL.
 * 引脚映射来自 CubeMX 生成的 main.h / Pin mapping follows Core/Inc/main.h.
 */

#define AD9834_SYSTEM_CLOCK     75000000UL
#define AD9834_MAX_OUTPUT_FREQ  (AD9834_SYSTEM_CLOCK / 2UL)

/* 波形控制字 / Wave selection control words. */
#define AD9834_WAVE_TRIANGLE    0x2002U
#define AD9834_WAVE_SINE        0x2008U
#define AD9834_WAVE_SQUARE      0x2028U

typedef enum
{
	AD9834_WAVEFORM_SINE = 0,
	AD9834_WAVEFORM_TRIANGLE,
	AD9834_WAVEFORM_SQUARE
} AD9834_Waveform_t;

/* 兼容旧名称 / Backward compatible names. */
#define Sine_Wave       AD9834_WAVE_SINE
#define Triangle_Wave   AD9834_WAVE_TRIANGLE
#define Square_Wave     AD9834_WAVE_SQUARE

/* 寄存器索引 / Register index. */
#define AD9834_FREQ0    0U
#define AD9834_FREQ1    1U
#define AD9834_PHASE0   0U
#define AD9834_PHASE1   1U

/* 从 main.h 引用的 GPIO 映射 / GPIO mapping from main.h. */
#define AD9834_FSYNC_PORT   FSY_GPIO_Port
#define AD9834_FSYNC_PIN    FSY_Pin

#define AD9834_SCLK_PORT    SCK_GPIO_Port
#define AD9834_SCLK_PIN     SCK_Pin

#define AD9834_SDATA_PORT   SDA_ad_GPIO_Port
#define AD9834_SDATA_PIN    SDA_ad_Pin

#define AD9834_RESET_PORT   RST_GPIO_Port
#define AD9834_RESET_PIN    RST_Pin

#define AD9834_FS_PORT      FS_GPIO_Port
#define AD9834_FS_PIN       FS_Pin

#define AD9834_PS_PORT      PS_GPIO_Port
#define AD9834_PS_PIN       PS_Pin

/* 引脚控制宏 / Pin operation macros. */
#define AD9834_FSYNC_SET()  HAL_GPIO_WritePin(AD9834_FSYNC_PORT, AD9834_FSYNC_PIN, GPIO_PIN_SET)
#define AD9834_FSYNC_CLR()  HAL_GPIO_WritePin(AD9834_FSYNC_PORT, AD9834_FSYNC_PIN, GPIO_PIN_RESET)

#define AD9834_SCLK_SET()   HAL_GPIO_WritePin(AD9834_SCLK_PORT, AD9834_SCLK_PIN, GPIO_PIN_SET)
#define AD9834_SCLK_CLR()   HAL_GPIO_WritePin(AD9834_SCLK_PORT, AD9834_SCLK_PIN, GPIO_PIN_RESET)

#define AD9834_SDATA_SET()  HAL_GPIO_WritePin(AD9834_SDATA_PORT, AD9834_SDATA_PIN, GPIO_PIN_SET)
#define AD9834_SDATA_CLR()  HAL_GPIO_WritePin(AD9834_SDATA_PORT, AD9834_SDATA_PIN, GPIO_PIN_RESET)

#define AD9834_RESET_SET()  HAL_GPIO_WritePin(AD9834_RESET_PORT, AD9834_RESET_PIN, GPIO_PIN_SET)
#define AD9834_RESET_CLR()  HAL_GPIO_WritePin(AD9834_RESET_PORT, AD9834_RESET_PIN, GPIO_PIN_RESET)

#define AD9834_FS_HIGH()    HAL_GPIO_WritePin(AD9834_FS_PORT, AD9834_FS_PIN, GPIO_PIN_SET)
#define AD9834_FS_LOW()     HAL_GPIO_WritePin(AD9834_FS_PORT, AD9834_FS_PIN, GPIO_PIN_RESET)

#define AD9834_PS_HIGH()    HAL_GPIO_WritePin(AD9834_PS_PORT, AD9834_PS_PIN, GPIO_PIN_SET)
#define AD9834_PS_LOW()     HAL_GPIO_WritePin(AD9834_PS_PORT, AD9834_PS_PIN, GPIO_PIN_RESET)

void AD9834_Init(void);
void AD9834_Write_16Bits(uint16_t data);
void AD9834_Select_Wave(uint16_t wave_type);
void AD9834_Set_Freq(uint8_t freq_reg, uint32_t freq_hz);

/*
 * 配置 AD9834 输出波形与频率 / Configure AD9834 waveform and frequency.
 *
 * 参数 / Parameters:
 *   waveform: 波形类型 / waveform type
 *   freq_hz : 输出频率，单位 Hz / output frequency in Hz
 *
 * 返回值 / Return value:
 *   true  成功 / success
 *   false 失败 / invalid waveform or out-of-range frequency
 *
 * 示例 / Example:
 *   AD9834_ConfigureOutput(AD9834_WAVEFORM_SINE, 10000U);
 */
bool AD9834_ConfigureOutput(AD9834_Waveform_t waveform, uint32_t freq_hz);

#endif /* __AD9834_H */
