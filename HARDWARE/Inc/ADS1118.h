#ifndef __ADS1118_H
#define __ADS1118_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

/* ADS1118 软件 SPI 引脚映射 / ADS1118 software SPI pin mapping. */
#define ADS1118_SCK_PORT      SCK_ADS1118_GPIO_Port
#define ADS1118_SCK_PIN       SCK_ADS1118_Pin
#define ADS1118_CS_PORT       CS_AD1118_GPIO_Port
#define ADS1118_CS_PIN        CS_AD1118_Pin
#define ADS1118_MISO_PORT     MISO_ADS1118_GPIO_Port
#define ADS1118_MISO_PIN      MISO_ADS1118_Pin
#define ADS1118_MOSI_PORT     MOSI_ADS1118_GPIO_Port
#define ADS1118_MOSI_PIN      MOSI_ADS1118_Pin

#define ADS1118_SCK_HIGH()    HAL_GPIO_WritePin(ADS1118_SCK_PORT, ADS1118_SCK_PIN, GPIO_PIN_SET)
#define ADS1118_SCK_LOW()     HAL_GPIO_WritePin(ADS1118_SCK_PORT, ADS1118_SCK_PIN, GPIO_PIN_RESET)
#define ADS1118_CS_HIGH()     HAL_GPIO_WritePin(ADS1118_CS_PORT, ADS1118_CS_PIN, GPIO_PIN_SET)
#define ADS1118_CS_LOW()      HAL_GPIO_WritePin(ADS1118_CS_PORT, ADS1118_CS_PIN, GPIO_PIN_RESET)
#define ADS1118_MOSI_HIGH()   HAL_GPIO_WritePin(ADS1118_MOSI_PORT, ADS1118_MOSI_PIN, GPIO_PIN_SET)
#define ADS1118_MOSI_LOW()    HAL_GPIO_WritePin(ADS1118_MOSI_PORT, ADS1118_MOSI_PIN, GPIO_PIN_RESET)
#define ADS1118_MISO_READ()   HAL_GPIO_ReadPin(ADS1118_MISO_PORT, ADS1118_MISO_PIN)

/* 满量程范围选择 / Full-scale range selection. */
typedef enum
{
    ADS1118_RANGE_6P144V = 0,
    ADS1118_RANGE_4P096V,
    ADS1118_RANGE_2P048V,
    ADS1118_RANGE_1P024V,
    ADS1118_RANGE_0P512V,
    ADS1118_RANGE_0P256V,
} ADS1118_Range_t;

/* 转换速率选择 / Conversion rate selection. */
typedef enum
{
    ADS1118_RATE_8SPS = 0,
    ADS1118_RATE_16SPS,
    ADS1118_RATE_32SPS,
    ADS1118_RATE_64SPS,
    ADS1118_RATE_128SPS,
    ADS1118_RATE_250SPS,
    ADS1118_RATE_475SPS,
    ADS1118_RATE_860SPS,
} ADS1118_Rate_t;

/* 输入复用选择 / Input mux selection. */
typedef enum
{
    ADS1118_CHANNEL_AIN0_AIN1 = 0,
    ADS1118_CHANNEL_AIN0_AIN3,
    ADS1118_CHANNEL_AIN1_AIN3,
    ADS1118_CHANNEL_AIN2_AIN3,
    ADS1118_CHANNEL_AIN0_GND,
    ADS1118_CHANNEL_AIN1_GND,
    ADS1118_CHANNEL_AIN2_GND,
    ADS1118_CHANNEL_AIN3_GND,
} ADS1118_Channel_t;

/* 功能模式选择 / Functional mode selection. */
typedef enum
{
    ADS1118_MODE_ADC = 0,
    ADS1118_MODE_TEMPERATURE = 1,
} ADS1118_Mode_t;

/*
 * ADS1118 运行配置 / ADS1118 runtime configuration.
 *
 * range:
 *   中文: ADC 输入满量程范围。
 *   English: ADC full-scale input range.
 *
 * rate:
 *   中文: 单次转换速率。
 *   English: Single conversion rate.
 *
 * pull_up_en:
 *   中文: DOUT 内部上拉使能。
 *   English: Enable internal DOUT pull-up.
 */
typedef struct
{
    ADS1118_Range_t range;
    ADS1118_Rate_t rate;
    bool pull_up_en;
} ADS1118_Config_t;

/*
 * 初始化总线空闲状态 / Initialize idle SPI bus state.
 *
 * 中文: 在 MX_GPIO_Init() 和 delay_init() 之后调用。
 * English: Call after MX_GPIO_Init() and delay_init().
 */
void ADS1118_Init(void);

/*
 * 应用后续采样配置 / Apply configuration for subsequent reads.
 *
 * cfg:
 *   中文: 配置结构体指针。
 *   English: Pointer to the configuration structure.
 */
void ADS1118_SetConfig(const ADS1118_Config_t *cfg);

/*
 * 读取一次原始转换结果 / Read one raw conversion result.
 *
 * channel: 输入通道 / input channel
 * mode   : 模式 / mode
 * raw    : 原始结果输出指针 / raw result output pointer
 */
bool ADS1118_ReadRawSingle(ADS1118_Channel_t channel, ADS1118_Mode_t mode, int16_t *raw);

/* 读取电压值 / Read one voltage sample in volts. */
bool ADS1118_ReadVoltageSingle(ADS1118_Channel_t channel, float *voltage_v);

/* 读取内部温度 / Read internal die temperature in Celsius. */
bool ADS1118_ReadInternalTempC(float *temp_c);

/* 读取热电偶温度 / Read thermocouple temperature in Celsius. */
bool ADS1118_ReadThermocoupleTempC(ADS1118_Channel_t channel, float *temp_c);

/* 调试读取接口 / Debug helper for raw/config/MISO state. */
bool ADS1118_ReadRawSingleDebug(ADS1118_Channel_t channel, ADS1118_Mode_t mode, int16_t *raw, uint16_t *cfg_word, uint8_t *miso_idle_high);

/* 扩展调试接口 / Extended debug helper kept for compatibility. */
bool ADS1118_ReadRawSingleDebugEx(ADS1118_Channel_t channel,
                                  ADS1118_Mode_t mode,
                                  int16_t *raw_mode1,
                                  int16_t *raw_mode0,
                                  uint16_t *cfg_word,
                                  uint8_t *miso_idle_high);

/* 原始码转电压 / Convert raw ADC code to voltage. */
float ADS1118_RawToVoltage(int16_t raw, ADS1118_Range_t range);

/* 冷端补偿辅助函数 / Cold-junction compensation helper. */
int ADS1118_LocalCompensationCode(int local_code);

/* 热电偶码值转 0.1 摄氏度 / Convert thermocouple code to 0.1C units. */
int ADS1118_CodeToThermocoupleTempX10(int code);

#endif /* __ADS1118_H */
