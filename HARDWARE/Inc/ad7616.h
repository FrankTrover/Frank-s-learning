#ifndef __AD7616_H__
#define __AD7616_H__

#include "main.h"
#include <stdint.h>

/*
 * AD7616 同步采样 ADC 驱动（STM32F407 HAL）/ AD7616 simultaneous-sampling ADC driver for STM32F407 HAL.
 * 引脚映射来自 CubeMX 生成的 main.h / Pin mapping follows Core/Inc/main.h.
 */

/* 串行模式复用引脚 / Reused pins in serial mode. */
#define AD7616_SDI_Pin       AD7616_D10_Pin
#define AD7616_SDI_GPIO_Port AD7616_D10_GPIO_Port
#define AD7616_SDOB_Pin      AD7616_D11_Pin
#define AD7616_SDOB_GPIO_Port AD7616_D11_GPIO_Port
#define AD7616_SDOA_Pin      AD7616_D12_Pin
#define AD7616_SDOA_GPIO_Port AD7616_D12_GPIO_Port

/* 软件模式下 SER1W 复用在 DB4 / SER1W is multiplexed on DB4 in software mode. */
#define AD7616_SER1W_Pin       AD7616_D4_Pin
#define AD7616_SER1W_GPIO_Port AD7616_D4_GPIO_Port

/* 硬件模式下的串并模式选择脚 / SER/PAR mode select signal in hardware mode. */
#define AD7616_SER_Pin       AD7616_S_P_Pin
#define AD7616_SER_GPIO_Port AD7616_S_P_GPIO_Port

/* 工作模式选择 / Working mode selection. */
#define HARDWARE_MODE 0U
#define SOFTWARE_MODE 1U

/* 串行寄存器地址 / Serial register addresses. */
enum AD7616_Register_Address
{
    Config_Register = 0x02,
    Channel_Register = 0x03,
    Input_Range_Register_A1 = 0x04,
    Input_Range_Register_A2 = 0x05,
    Input_Range_Register_B1 = 0x06,
    Input_Range_Register_B2 = 0x07,
};

/* 输入量程编码 / Input range codes. */
enum AD7616_Range
{
    Range_10_V = 0x00,
    Range_2_5_V = 0x01,
    Range_5_V = 0x02,
};

/* 串行输出格式 / Serial output format. */
enum AD7616_Serial_Output_Format
{
    Serial_Line_1_Output = 0x00,
    Serial_Line_2_Output = 0x01,
};

/* BUSY 等待超时，避免采样异常时永久阻塞 / BUSY wait timeout to avoid permanent blocking. */
#define AD7616_BUSY_TIMEOUT_US 5000U

/* 读取 BUSY 当前电平 / Read the current BUSY pin state. */
GPIO_PinState AD7616_IsBusy(void);

/* 配置并口数据线方向 / Configure data bus as parallel-input mode. */
void AD7616_Parallel_GPIO_Init(void);

/* 配置串口模式相关引脚方向 / Configure pin directions for serial mode. */
void AD7616_Serial_GPIO_Init(void);

/*
 * 初始化 AD7616 工作模式 / Initialize AD7616 working mode.
 *
 * mode:
 *   中文: HARDWARE_MODE 为并口读取，SOFTWARE_MODE 为串口寄存器/串口数据读取。
 *   English: HARDWARE_MODE selects parallel reads, SOFTWARE_MODE selects serial register/data access.
 */
void AD7616_Init(uint8_t mode);

/* 复位芯片 / Reset the converter. */
void AD7616_Reset(void);

/* 启动一次同步转换 / Trigger one simultaneous conversion. */
void AD7616_Conversion(void);

/* 并口模式设置全通道量程 / Set all channel ranges in parallel hardware mode. */
void AD7616_Parallel_Set_voltage(uint8_t range);

/*
 * 读取 A/B 两组转换结果 / Read one pair of A/B conversion results.
 *
 * 中文: 默认使用 AD7616_BUSY_TIMEOUT_US 等待 BUSY 拉低。
 * English: Uses AD7616_BUSY_TIMEOUT_US as the default BUSY timeout.
 */
void AD7616_Read_Data(int16_t *data_A, int16_t *data_B);

/* 非阻塞读取接口，BUSY 未就绪时返回 HAL_BUSY / Non-blocking read helper that returns HAL_BUSY while BUSY is still asserted. */
HAL_StatusTypeDef AD7616_Read_Data_NonBlocking(int16_t *data_A, int16_t *data_B);

/* 带超时的读取接口 / Timeout-aware data read helper. */
HAL_StatusTypeDef AD7616_Read_Data_Timeout(int16_t *data_A, int16_t *data_B, uint32_t timeout_us);

/* 并口模式通道选择 / Select channel pair in parallel mode. */
void AD7616_Parallel_Channel_Select(uint8_t channel);

/* 发送 16 位串行命令 / Shift out one 16-bit serial command. */
void AD7616_Write_Serial(uint16_t data);

/* 写串行寄存器 / Write one serial register. */
void AD7616_Serial_Write_Rang(uint8_t address, uint8_t data);

/* 串行模式设置全通道量程 / Set all channel ranges in serial mode. */
void AD7616_Serial_Set_voltage(uint8_t range);

/* 串行模式设置通道选择寄存器 / Program serial channel register. */
void AD7616_Seria_Channel(uint8_t channel);

/* 选择单线或双线串行输出，SER1W=0 为 SDOA 单线，SER1W=1 为 SDOA/SDOB 双线 / Select one-line or two-line serial output, where SER1W=0 routes data to SDOA only and SER1W=1 enables SDOA plus SDOB. */
void AD7616_Serial_Output_Format(uint8_t format);

/* 串行模式读取 A/B 两路数据 / Read one A/B sample pair through serial outputs. */
void AD7616_Read_Serial(int16_t *data_A, int16_t *data_B);

#endif /* __AD7616_H__ */
