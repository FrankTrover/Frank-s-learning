#include "ADS1118.h"
#include "delay.h"

static ADS1118_Config_t g_ads1118_cfg =
{
    .range = ADS1118_RANGE_4P096V,
    .rate = ADS1118_RATE_128SPS,
    .pull_up_en = true,
};

/* 组装 16 位配置字 / Build the 16-bit configuration word. */
static uint16_t ads1118_build_config(ADS1118_Channel_t channel, ADS1118_Mode_t mode, bool start_single)
{
    uint16_t cfg = 0U;

    if (start_single)
    {
        cfg |= (1U << 15); /* OS: 启动单次转换 / start one-shot conversion */
    }

    cfg |= ((uint16_t)(channel & 0x07U) << 12);             /* MUX: 输入选择 / input select */
    cfg |= ((uint16_t)(g_ads1118_cfg.range & 0x07U) << 9);  /* PGA: 量程 / gain-range bits */
    cfg |= (1U << 8);                                        /* 单次模式 / single-shot mode */
    cfg |= ((uint16_t)(mode & 0x01U) << 4);                 /* ADC/温度模式 / ADC or temperature */
    cfg |= ((uint16_t)(g_ads1118_cfg.rate & 0x07U) << 5);   /* DR: 数据速率 / data rate */

    if (g_ads1118_cfg.pull_up_en)
    {
        cfg |= (1U << 3); /* DOUT 上拉 / DOUT pull-up enable */
    }

    cfg |= (1U << 1); /* 有效命令位 / valid command bits */

    return cfg;
}

/* 软件 SPI Mode1 单字节传输 / Software SPI mode-1 single-byte transfer. */
static uint8_t ads1118_spi_xfer_u8_mode1(uint8_t tx)
{
    uint8_t i;
    uint8_t rx = 0U;

    for (i = 0U; i < 8U; i++)
    {
        /* CPOL=0, CPHA=1: 从设备在上升沿更新数据，主机在下降后采样 / slave updates on rising edge, master samples after falling edge. */
        if ((tx & 0x80U) != 0U)
        {
            ADS1118_MOSI_HIGH();
        }
        else
        {
            ADS1118_MOSI_LOW();
        }

        delay_us(5);
        ADS1118_SCK_HIGH();
        delay_us(5);

        ADS1118_SCK_LOW();
        delay_us(5);

        /* mode 1 在下降沿之后采样。 */
        rx <<= 1;
        if (ADS1118_MISO_READ() == GPIO_PIN_SET)
        {
            rx |= 0x01U;
        }

        delay_us(5);
        tx <<= 1;
    }

    return rx;
}

/* 软件 SPI Mode1 双字节传输 / Software SPI mode-1 16-bit transfer. */
static uint16_t ads1118_spi_xfer_u16_mode1(uint16_t tx)
{
    uint16_t rx;

    rx = (uint16_t)ads1118_spi_xfer_u8_mode1((uint8_t)(tx >> 8));
    rx <<= 8;
    rx |= ads1118_spi_xfer_u8_mode1((uint8_t)(tx & 0xFFU));

    return rx;
}

/* 默认 16 位事务入口 / Default 16-bit transaction entry. */
static uint16_t ads1118_spi_xfer_u16(uint16_t tx)
{
    return ads1118_spi_xfer_u16_mode1(tx);
}

/* 用 dummy 时钟读取配置字 / Read configuration word using dummy clocks. */
static uint16_t ads1118_spi_read_config_word(void)
{
    uint16_t discard;
    uint16_t config;

    ADS1118_CS_LOW();
    delay_us(2);
    discard = ads1118_spi_xfer_u16(0xFFFFU);
    config = ads1118_spi_xfer_u16(0xFFFFU);
    delay_us(2);
    ADS1118_CS_HIGH();

    (void)discard;

    return config;
}

/* 发送 16 位配置字 / Write one 16-bit configuration word. */
static void ads1118_spi_write_config_word(uint16_t cfg)
{
    ADS1118_CS_LOW();
    delay_us(2);
    (void)ads1118_spi_xfer_u16(cfg);
    delay_us(2);
    ADS1118_CS_HIGH();
}

/* 用 dummy 时钟读取转换结果 / Read conversion result using dummy clocks. */
static uint16_t ads1118_spi_read_data_word(void)
{
    uint16_t data;

    ADS1118_CS_LOW();
    delay_us(2);
    data = ads1118_spi_xfer_u16(0xFFFFU);
    delay_us(2);
    ADS1118_CS_HIGH();

    return data;
}

/* 按数据速率等待单次转换完成 / Wait one-shot conversion to complete. */
static void ads1118_wait_single_conversion_done(ADS1118_Rate_t rate)
{
    switch (rate)
    {
        case ADS1118_RATE_8SPS:
            delay_ms(138);
            break;
        case ADS1118_RATE_16SPS:
            delay_ms(69);
            break;
        case ADS1118_RATE_32SPS:
            delay_ms(35);
            break;
        case ADS1118_RATE_64SPS:
            delay_ms(18);
            break;
        case ADS1118_RATE_128SPS:
            delay_ms(9);
            break;
        case ADS1118_RATE_250SPS:
            delay_ms(5);
            break;
        case ADS1118_RATE_475SPS:
            delay_ms(3);
            break;
        case ADS1118_RATE_860SPS:
        default:
            delay_ms(2);
            break;
    }
}

/* 初始化软件 SPI 空闲状态 / Initialize software SPI idle states. */
void ADS1118_Init(void)
{
    ADS1118_CS_HIGH();
    ADS1118_SCK_LOW();
    ADS1118_MOSI_LOW();
    delay_ms(10);
}

/* 应用新的运行配置 / Apply new runtime configuration. */
void ADS1118_SetConfig(const ADS1118_Config_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    g_ads1118_cfg = *cfg;
}

/* 读取一次原始结果 / Read one raw conversion result. */
bool ADS1118_ReadRawSingle(ADS1118_Channel_t channel, ADS1118_Mode_t mode, int16_t *raw)
{
    uint16_t cfg;

    if (raw == NULL)
    {
        return false;
    }

    (void)ads1118_spi_read_config_word();
    cfg = ads1118_build_config(channel, mode, true);

    ads1118_spi_write_config_word(cfg);
    ads1118_wait_single_conversion_done(g_ads1118_cfg.rate);

    *raw = (int16_t)ads1118_spi_read_data_word();

    return true;
}

/* 调试接口：返回原始值、配置字和 MISO 空闲状态 / Debug helper returning raw/config/MISO idle state. */
bool ADS1118_ReadRawSingleDebug(ADS1118_Channel_t channel, ADS1118_Mode_t mode, int16_t *raw, uint16_t *cfg_word, uint8_t *miso_idle_high)
{
    uint16_t cfg;
    int16_t raw_local;

    cfg = ads1118_build_config(channel, mode, true);

    if (cfg_word != NULL)
    {
        *cfg_word = cfg;
    }

    if (miso_idle_high != NULL)
    {
        *miso_idle_high = (ADS1118_MISO_READ() == GPIO_PIN_SET) ? 1U : 0U;
    }

    if (!ADS1118_ReadRawSingle(channel, mode, &raw_local))
    {
        return false;
    }

    if (raw != NULL)
    {
        *raw = raw_local;
    }

    return true;
}

/* 扩展调试接口 / Extended debug helper kept for compatibility. */
bool ADS1118_ReadRawSingleDebugEx(ADS1118_Channel_t channel,
                                  ADS1118_Mode_t mode,
                                  int16_t *raw_mode1,
                                  int16_t *raw_mode0,
                                  uint16_t *cfg_word,
                                  uint8_t *miso_idle_high)
{
    uint16_t cfg;
    int16_t raw_local;

    cfg = ads1118_build_config(channel, mode, true);

    if (cfg_word != NULL)
    {
        *cfg_word = cfg;
    }

    if (miso_idle_high != NULL)
    {
        *miso_idle_high = (ADS1118_MISO_READ() == GPIO_PIN_SET) ? 1U : 0U;
    }

    if (!ADS1118_ReadRawSingle(channel, mode, &raw_local))
    {
        return false;
    }

    if (raw_mode1 != NULL)
    {
        *raw_mode1 = raw_local;
    }

    if (raw_mode0 != NULL)
    {
        *raw_mode0 = raw_local;
    }

    return true;
}

/* 按量程换算电压 / Convert raw code to voltage with selected range. */
float ADS1118_RawToVoltage(int16_t raw, ADS1118_Range_t range)
{
    float fs_v;

    switch (range)
    {
        case ADS1118_RANGE_6P144V:
            fs_v = 6.144f;
            break;
        case ADS1118_RANGE_4P096V:
            fs_v = 4.096f;
            break;
        case ADS1118_RANGE_2P048V:
            fs_v = 2.048f;
            break;
        case ADS1118_RANGE_1P024V:
            fs_v = 1.024f;
            break;
        case ADS1118_RANGE_0P512V:
            fs_v = 0.512f;
            break;
        case ADS1118_RANGE_0P256V:
        default:
            fs_v = 0.256f;
            break;
    }

    return ((float)raw * fs_v) / 32768.0f;
}

/* 直接读取电压 / Read voltage directly. */
bool ADS1118_ReadVoltageSingle(ADS1118_Channel_t channel, float *voltage_v)
{
    int16_t raw;

    if (voltage_v == NULL)
    {
        return false;
    }

    if (!ADS1118_ReadRawSingle(channel, ADS1118_MODE_ADC, &raw))
    {
        return false;
    }

    *voltage_v = ADS1118_RawToVoltage(raw, g_ads1118_cfg.range);

    return true;
}

bool ADS1118_ReadInternalTempC(float *temp_c)
{
    int16_t raw;

    if (temp_c == NULL)
    {
        return false;
    }

    if (!ADS1118_ReadRawSingle(ADS1118_CHANNEL_AIN0_GND, ADS1118_MODE_TEMPERATURE, &raw))
    {
        return false;
    }

    /* 温度模式为 14 位左对齐，分辨率 0.03125C/LSB / temperature mode output is 14-bit left-justified, 0.03125C per LSB. */
    *temp_c = (float)raw / 128.0f;

    return true;
}

/* 热电偶冷端补偿辅助 / Cold-junction compensation helper for thermocouple. */
int ADS1118_LocalCompensationCode(int local_code)
{
    float tmp;
    float local_temp;
    int comp;

    local_code = local_code / 4;
    local_temp = (float)local_code / 32.0f;

    if ((local_temp >= 0.0f) && (local_temp <= 5.0f))
    {
        tmp = (0x0019U * local_temp) / 5.0f;
        comp = (int)tmp;
    }
    else if ((local_temp > 5.0f) && (local_temp <= 10.0f))
    {
        tmp = (0x001AU * (local_temp - 5.0f)) / 5.0f + 0x0019U;
        comp = (int)tmp;
    }
    else if ((local_temp > 10.0f) && (local_temp <= 20.0f))
    {
        tmp = (0x0033U * (local_temp - 10.0f)) / 10.0f + 0x0033U;
        comp = (int)tmp;
    }
    else if ((local_temp > 20.0f) && (local_temp <= 30.0f))
    {
        tmp = (0x0034U * (local_temp - 20.0f)) / 10.0f + 0x0066U;
        comp = (int)tmp;
    }
    else if ((local_temp > 30.0f) && (local_temp <= 40.0f))
    {
        tmp = (0x0034U * (local_temp - 30.0f)) / 10.0f + 0x009AU;
        comp = (int)tmp;
    }
    else if ((local_temp > 40.0f) && (local_temp <= 50.0f))
    {
        tmp = (0x0035U * (local_temp - 40.0f)) / 10.0f + 0x00CEU;
        comp = (int)tmp;
    }
    else if ((local_temp > 50.0f) && (local_temp <= 60.0f))
    {
        tmp = (0x0035U * (local_temp - 50.0f)) / 10.0f + 0x0103U;
        comp = (int)tmp;
    }
    else if ((local_temp > 60.0f) && (local_temp <= 80.0f))
    {
        tmp = (0x006AU * (local_temp - 60.0f)) / 20.0f + 0x0138U;
        comp = (int)tmp;
    }
    else if ((local_temp > 80.0f) && (local_temp <= 125.0f))
    {
        tmp = (0x00EEU * (local_temp - 80.0f)) / 45.0f + 0x01A2U;
        comp = (int)tmp;
    }
    else
    {
        comp = 0;
    }

    return comp;
}

int ADS1118_CodeToThermocoupleTempX10(int code)
{
    uint16_t code_u = (uint16_t)code;
    float temp = (float)code_u;

    if ((code_u > 0xFF6C) && (code_u <= 0xFFB5))
    {
        temp = (15.0f * (temp - 0xFF6C)) / 0x0049 - 30.0f;
    }
    else if ((code_u > 0xFFB5) && (code_u <= 0xFFFF))
    {
        temp = (15.0f * (temp - 0xFFB5)) / 0x004B - 15.0f;
    }
    else if (code_u <= 0x0019)
    {
        temp = (5.0f * temp) / 0x0019;
    }
    else if ((code_u > 0x0019) && (code_u <= 0x0033))
    {
        temp = (5.0f * (temp - 0x0019)) / 0x001A + 5.0f;
    }
    else if ((code_u > 0x0033) && (code_u <= 0x0066))
    {
        temp = (10.0f * (temp - 0x0033)) / 0x0033 + 10.0f;
    }
    else if ((code_u > 0x0066) && (code_u <= 0x009A))
    {
        temp = (10.0f * (temp - 0x0066)) / 0x0034 + 20.0f;
    }
    else if ((code_u > 0x009A) && (code_u <= 0x00CE))
    {
        temp = (10.0f * (temp - 0x009A)) / 0x0034 + 30.0f;
    }
    else if ((code_u > 0x00CE) && (code_u <= 0x0103))
    {
        temp = (10.0f * (temp - 0x00CE)) / 0x0035 + 40.0f;
    }
    else if ((code_u > 0x0103) && (code_u <= 0x0138))
    {
        temp = (10.0f * (temp - 0x0103)) / 0x0035 + 50.0f;
    }
    else if ((code_u > 0x0138) && (code_u <= 0x01A2))
    {
        temp = (20.0f * (temp - 0x0138)) / 0x006A + 60.0f;
    }
    else if ((code_u > 0x01A2) && (code_u <= 0x020C))
    {
        temp = ((temp - 0x01A2) * 20.0f) / 0x006A + 80.0f;
    }
    else if ((code_u > 0x020C) && (code_u <= 0x02DE))
    {
        temp = ((temp - 0x020C) * 40.0f) / 0x00D2 + 100.0f;
    }
    else if ((code_u > 0x02DE) && (code_u <= 0x03AC))
    {
        temp = ((temp - 0x02DE) * 40.0f) / 0x00CE + 140.0f;
    }
    else if ((code_u > 0x03AC) && (code_u <= 0x0478))
    {
        temp = ((temp - 0x03AC) * 40.0f) / 0x00CC + 180.0f;
    }
    else if ((code_u > 0x0478) && (code_u <= 0x0548))
    {
        temp = ((temp - 0x0478) * 40.0f) / 0x00D0 + 220.0f;
    }
    else if ((code_u > 0x0548) && (code_u <= 0x061B))
    {
        temp = ((temp - 0x0548) * 40.0f) / 0x00D3 + 260.0f;
    }
    else if ((code_u > 0x061B) && (code_u <= 0x06F2))
    {
        temp = ((temp - 0x061B) * 40.0f) / 0x00D7 + 300.0f;
    }
    else if ((code_u > 0x06F2) && (code_u <= 0x07C7))
    {
        temp = ((temp - 0x06F2) * 40.0f) / 0x00D5 + 340.0f;
    }
    else if ((code_u > 0x07C7) && (code_u <= 0x089F))
    {
        temp = ((temp - 0x07C7) * 40.0f) / 0x00D8 + 380.0f;
    }
    else if ((code_u > 0x089F) && (code_u <= 0x0978))
    {
        temp = ((temp - 0x089F) * 40.0f) / 0x00D9 + 420.0f;
    }
    else if ((code_u > 0x0978) && (code_u <= 0x0A52))
    {
        temp = ((temp - 0x0978) * 40.0f) / 0x00DA + 460.0f;
    }
    else
    {
        /* 超出查表范围时使用近似线性斜率 / fallback to near-linear Type-K slope when code is out of table range. */
        temp = (float)((int16_t)code_u) / 5.12f;
    }

    return (int)(temp * 10.0f);
}

/* 读取热电偶温度 / Read thermocouple temperature with internal cold-junction compensation. */
bool ADS1118_ReadThermocoupleTempC(ADS1118_Channel_t channel, float *temp_c)
{
    int16_t far_raw;
    int16_t local_raw;
    int comp_code;
    int temp_x10;

    if (temp_c == NULL)
    {
        return false;
    }

    if (!ADS1118_ReadRawSingle(channel, ADS1118_MODE_ADC, &far_raw))
    {
        return false;
    }

    if (!ADS1118_ReadRawSingle(channel, ADS1118_MODE_TEMPERATURE, &local_raw))
    {
        return false;
    }

    comp_code = ADS1118_LocalCompensationCode((int)local_raw);
    temp_x10 = ADS1118_CodeToThermocoupleTempX10((int)far_raw + comp_code);

    if (temp_x10 == -32768)
    {
        return false;
    }

    *temp_c = (float)temp_x10 / 10.0f;

    return true;
}
