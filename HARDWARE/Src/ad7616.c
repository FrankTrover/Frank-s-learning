#include "ad7616.h"
#include "delay.h"
#include <stddef.h>

/* 当前驱动默认缓存量程与工作模式 / Cache the last configured range and working mode. */
static uint8_t g_range = Range_10_V;
static uint8_t g_mode = HARDWARE_MODE;

/*
 * 中文: 板级 DB0~DB15 分散在多个 GPIO 端口，不能一次性整口读取。
 * English: DB0~DB15 are distributed across multiple GPIO ports, so the bus is reconstructed bit by bit.
 */
static GPIO_TypeDef *const g_port_list[16] = {
    AD7616_D0_GPIO_Port, AD7616_D1_GPIO_Port, AD7616_D2_GPIO_Port, AD7616_D3_GPIO_Port,
    AD7616_D4_GPIO_Port, AD7616_D5_GPIO_Port, AD7616_D6_GPIO_Port, AD7616_D7_GPIO_Port,
    AD7616_D8_GPIO_Port, AD7616_D9_GPIO_Port, AD7616_D10_GPIO_Port, AD7616_D11_GPIO_Port,
    AD7616_D12_GPIO_Port, AD7616_D13_GPIO_Port, AD7616_D14_GPIO_Port, AD7616_D15_GPIO_Port
};

static const uint16_t g_pin_list[16] = {
    AD7616_D0_Pin, AD7616_D1_Pin, AD7616_D2_Pin, AD7616_D3_Pin,
    AD7616_D4_Pin, AD7616_D5_Pin, AD7616_D6_Pin, AD7616_D7_Pin,
    AD7616_D8_Pin, AD7616_D9_Pin, AD7616_D10_Pin, AD7616_D11_Pin,
    AD7616_D12_Pin, AD7616_D13_Pin, AD7616_D14_Pin, AD7616_D15_Pin
};

static uint8_t ad7616_normalize_range(uint8_t range)
{
    switch (range)
    {
        case Range_2_5_V:
        case Range_5_V:
        case Range_10_V:
            return range;
        default:
            return Range_10_V;
    }
}

static GPIO_PinState ad7616_read_bit(GPIO_TypeDef *port, uint16_t pin)
{
    return HAL_GPIO_ReadPin(port, pin);
}

static void ad7616_write_bit(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState level)
{
    HAL_GPIO_WritePin(port, pin, level);
}

static void AD7616_Set_Control_Idle(void)
{
    /* 让控制线回到空闲高电平，便于统一进入下一次命令或采样时序。 */
    ad7616_write_bit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, GPIO_PIN_SET);
    ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_SET);
    ad7616_write_bit(AD7616_WR_GPIO_Port, AD7616_WR_Pin, GPIO_PIN_SET);
    ad7616_write_bit(AD7616_CONV_GPIO_Port, AD7616_CONV_Pin, GPIO_PIN_SET);
    ad7616_write_bit(AD7616_RST_GPIO_Port, AD7616_RST_Pin, GPIO_PIN_SET);
}

static HAL_StatusTypeDef AD7616_Wait_Busy_Ready(uint32_t timeout_us)
{
    uint32_t start_tick = DWT->CYCCNT;
    uint32_t timeout_ticks = timeout_us * (HAL_RCC_GetSysClockFreq() / 1000000U);

    /* BUSY 高电平表示转换尚未完成 / BUSY stays high while conversion is still running. */
    while (ad7616_read_bit(AD7616_BUSY_GPIO_Port, AD7616_BUSY_Pin) == GPIO_PIN_SET)
    {
        if ((DWT->CYCCNT - start_tick) >= timeout_ticks)
        {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

static void AD7616_Working_Mode(uint8_t mode)
{
    /* SER/PAR 引脚低为并口，高为串口 / SER low selects parallel mode, high selects serial mode. */
    ad7616_write_bit(AD7616_SER_GPIO_Port,
                     AD7616_SER_Pin,
                     (mode == HARDWARE_MODE) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void AD7616_Set_Pin_Output(void)
{
    GPIO_InitTypeDef init = {0};
    uint8_t i;

    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_HIGH;

    for (i = 0U; i < 16U; ++i)
    {
        init.Pin = g_pin_list[i];
        HAL_GPIO_Init(g_port_list[i], &init);
    }
}

static void AD7616_Set_Pin_Input(void)
{
    GPIO_InitTypeDef init = {0};
    uint8_t i;

    init.Mode = GPIO_MODE_INPUT;
    init.Pull = GPIO_NOPULL;

    for (i = 0U; i < 16U; ++i)
    {
        init.Pin = g_pin_list[i];
        HAL_GPIO_Init(g_port_list[i], &init);
    }
}

static uint16_t AD7616_Read_Data_16Bit(void)
{
    uint16_t value = 0U;
    uint8_t i;

    /* 板级 DBx 跨端口分布，所以逐位拼接 / Rebuild the 16-bit word bit by bit across split GPIO ports. */
    for (i = 0U; i < 16U; ++i)
    {
        if (ad7616_read_bit(g_port_list[i], g_pin_list[i]) == GPIO_PIN_SET)
        {
            value |= (uint16_t)(1U << i);
        }
    }

    return value;
}

static HAL_StatusTypeDef AD7616_Read_Current_Frame(int16_t *data_A, int16_t *data_B)
{
    if ((data_A == NULL) || (data_B == NULL))
    {
        return HAL_ERROR;
    }

    /* CS 拉低后执行一次完整读周期 / Perform one complete read transaction while CS is asserted. */
    ad7616_write_bit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, GPIO_PIN_RESET);
    delay_us(1U);

    if (g_mode == HARDWARE_MODE)
    {
        /* 并口模式下两次 RD 分别取 A、B 组 / Two RD strobes fetch group A then group B in parallel mode. */
        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_RESET);
        delay_us(1U);
        *data_A = (int16_t)AD7616_Read_Data_16Bit();
        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_SET);
        delay_us(1U);

        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_RESET);
        delay_us(1U);
        *data_B = (int16_t)AD7616_Read_Data_16Bit();
        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_SET);
    }
    else
    {
        /* 串口模式下通过 SDOA/SDOB 串行移出 / Serial mode shifts data out through SDOA/SDOB. */
        AD7616_Read_Serial(data_A, data_B);
    }

    ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_SET);
    ad7616_write_bit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, GPIO_PIN_SET);
    return HAL_OK;
}

GPIO_PinState AD7616_IsBusy(void)
{
    return ad7616_read_bit(AD7616_BUSY_GPIO_Port, AD7616_BUSY_Pin);
}

void AD7616_Serial_GPIO_Init(void)
{
    /*
     * 中文: 保持 CubeMX 生成的基础配置，只在运行期改写数据线方向。
     * English: Keep CubeMX base GPIO setup and only override data-line directions at runtime.
     */
    AD7616_Set_Pin_Output();

    /* SDOB/SDOA 在串行输出模式下是输入 / SDOB and SDOA become inputs in serial output mode. */
    {
        GPIO_InitTypeDef init = {0};

        init.Mode = GPIO_MODE_INPUT;
        init.Pull = GPIO_NOPULL;

        init.Pin = AD7616_SDOB_Pin;
        HAL_GPIO_Init(AD7616_SDOB_GPIO_Port, &init);

        init.Pin = AD7616_SDOA_Pin;
        HAL_GPIO_Init(AD7616_SDOA_GPIO_Port, &init);
    }
}

void AD7616_Parallel_GPIO_Init(void)
{
    /* 并口采样时 DB0~DB15 需要全部切回输入 / In parallel mode DB0~DB15 must be inputs. */
    AD7616_Set_Pin_Input();
}

void AD7616_Reset(void)
{
    /* 按数据手册给复位脉冲与稳定时间 / Apply reset pulse and settling time per datasheet intent. */
    ad7616_write_bit(AD7616_RST_GPIO_Port, AD7616_RST_Pin, GPIO_PIN_RESET);
    delay_ms(1U);
    ad7616_write_bit(AD7616_RST_GPIO_Port, AD7616_RST_Pin, GPIO_PIN_SET);
    delay_ms(20U);
}

void AD7616_Conversion(void)
{
    /* CONV 下降沿启动一次同步采样 / Toggle CONV to start one simultaneous conversion. */
    ad7616_write_bit(AD7616_CONV_GPIO_Port, AD7616_CONV_Pin, GPIO_PIN_RESET);
    delay_us(1U);
    ad7616_write_bit(AD7616_CONV_GPIO_Port, AD7616_CONV_Pin, GPIO_PIN_SET);
}

void AD7616_Parallel_Set_voltage(uint8_t range)
{
    g_range = ad7616_normalize_range(range);

    /* RNG1/RNG0 直接对应硬件量程脚 / RNG1 and RNG0 directly control the hardware range pins. */
    switch (g_range)
    {
        case Range_2_5_V:
            ad7616_write_bit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, GPIO_PIN_SET);
            ad7616_write_bit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, GPIO_PIN_RESET);
            break;
        case Range_5_V:
            ad7616_write_bit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, GPIO_PIN_RESET);
            ad7616_write_bit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, GPIO_PIN_SET);
            break;
        case Range_10_V:
        default:
            ad7616_write_bit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, GPIO_PIN_SET);
            ad7616_write_bit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, GPIO_PIN_SET);
            break;
    }
}

void AD7616_Read_Serial(int16_t *data_A, int16_t *data_B)
{
    uint16_t shift = 0x8000U;
    uint16_t i;
    int16_t input_a = 0;
    int16_t input_b = 0;

    if ((data_A == NULL) || (data_B == NULL))
    {
        return;
    }

    /* 每个 RD 周期输出 A/B 各 1 bit，双线模式下同步读取 / Each RD pulse shifts one bit from A and B outputs simultaneously. */
    for (i = 0U; i < 16U; ++i)
    {
        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_RESET);
        delay_us(1U);

        if (ad7616_read_bit(AD7616_SDOA_GPIO_Port, AD7616_SDOA_Pin) == GPIO_PIN_SET)
        {
            input_a |= (int16_t)shift;
        }

        if (ad7616_read_bit(AD7616_SDOB_GPIO_Port, AD7616_SDOB_Pin) == GPIO_PIN_SET)
        {
            input_b |= (int16_t)shift;
        }

        shift >>= 1U;
        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_SET);
        delay_us(1U);
    }

    *data_A = input_a;
    *data_B = input_b;
}

void AD7616_Read_Data(int16_t *data_A, int16_t *data_B)
{
    (void)AD7616_Read_Data_Timeout(data_A, data_B, AD7616_BUSY_TIMEOUT_US);
}

HAL_StatusTypeDef AD7616_Read_Data_NonBlocking(int16_t *data_A, int16_t *data_B)
{
    if ((data_A == NULL) || (data_B == NULL))
    {
        return HAL_ERROR;
    }

    if (AD7616_IsBusy() == GPIO_PIN_SET)
    {
        return HAL_BUSY;
    }

    return AD7616_Read_Current_Frame(data_A, data_B);
}

HAL_StatusTypeDef AD7616_Read_Data_Timeout(int16_t *data_A, int16_t *data_B, uint32_t timeout_us)
{
    if ((data_A == NULL) || (data_B == NULL))
    {
        return HAL_ERROR;
    }

    if (AD7616_Wait_Busy_Ready(timeout_us) != HAL_OK)
    {
        return HAL_TIMEOUT;
    }

    return AD7616_Read_Current_Frame(data_A, data_B);
}

void AD7616_Parallel_Channel_Select(uint8_t channel)
{
    channel &= 0x07U;

    /* CHS[2:0] 选择当前输出的通道对 / CHS[2:0] selects the active channel pair. */
    ad7616_write_bit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, ((channel & 0x01U) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    ad7616_write_bit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, ((channel & 0x02U) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    ad7616_write_bit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, ((channel & 0x04U) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void AD7616_Init(uint8_t mode)
{
    if (mode != SOFTWARE_MODE)
    {
        mode = HARDWARE_MODE;
    }

    g_mode = mode;

    /* 先配置数据线方向，再恢复控制线空闲态 / Prepare pin directions first, then restore control lines to idle state. */
    if (mode == HARDWARE_MODE)
    {
        AD7616_Parallel_GPIO_Init();
    }
    else
    {
        AD7616_Serial_GPIO_Init();
    }

    AD7616_Set_Control_Idle();
    AD7616_Working_Mode(mode);
}

void AD7616_Write_Serial(uint16_t data)
{
    uint16_t shift = 0x8000U;
    uint16_t i;

    /* 软件时序发送 16 位控制字 / Bit-bang a 16-bit serial control word. */
    for (i = 0U; i < 16U; ++i)
    {
        ad7616_write_bit(AD7616_SDI_GPIO_Port, AD7616_SDI_Pin, ((data & shift) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_RESET);
        delay_us(1U);
        ad7616_write_bit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, GPIO_PIN_SET);
        delay_us(1U);
        shift >>= 1U;
    }
}

void AD7616_Serial_Write_Rang(uint8_t address, uint8_t data)
{
    uint16_t write_data = (uint16_t)(((uint16_t)address << 9) | 0x8000U | data);

    /* 最高位为写命令位，其后跟寄存器地址与 8 位数据。 */
    ad7616_write_bit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, GPIO_PIN_RESET);
    AD7616_Write_Serial(write_data);
    ad7616_write_bit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, GPIO_PIN_SET);
}

void AD7616_Serial_Set_voltage(uint8_t range)
{
    uint8_t normalized_range = ad7616_normalize_range(range);
    uint8_t range_data = (uint8_t)((normalized_range << 6) | (normalized_range << 4) | (normalized_range << 2) | normalized_range);

    g_range = normalized_range;
    /* A1/A2/B1/B2 四个寄存器分别覆盖 8 个模拟通道 / Program all four range registers to cover eight analog channels. */
    AD7616_Serial_Write_Rang(Input_Range_Register_A1, range_data);
    AD7616_Serial_Write_Rang(Input_Range_Register_A2, range_data);
    AD7616_Serial_Write_Rang(Input_Range_Register_B1, range_data);
    AD7616_Serial_Write_Rang(Input_Range_Register_B2, range_data);
}

void AD7616_Seria_Channel(uint8_t channel)
{
    channel &= 0x07U;
    uint8_t channel_data = (uint8_t)((channel << 4) | channel);

    /* 高 4 位对应 A 组，低 4 位对应 B 组，这里保持两组同步选择。 */
    AD7616_Serial_Write_Rang(Channel_Register, channel_data);
}

void AD7616_Serial_Output_Format(uint8_t format)
{
    /* SER1W=0 仅输出到 SDOA，SER1W=1 同时输出到 SDOA/SDOB / SER1W=0 routes data to SDOA only, SER1W=1 drives both SDOA and SDOB. */
    ad7616_write_bit(AD7616_SER1W_GPIO_Port,
                     AD7616_SER1W_Pin,
                     (format == Serial_Line_2_Output) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
