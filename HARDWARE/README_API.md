# HARDWARE 函数使用说明

本文档面向业务开发，说明 `HARDWARE` 目录中的常用接口如何调用。

## 1. 通用初始化顺序

推荐顺序如下：

```c
HAL_Init();
SystemClock_Config();
delay_init(HAL_RCC_GetSysClockFreq());
MX_GPIO_Init();
```

之后再按需初始化具体模块。

## 2. ADS1118

### 2.1 初始化

```c
#include "ADS1118.h"

ADS1118_Init();
```

用途：

- 设置软件 SPI 空闲电平
- 给芯片提供短暂稳定时间

### 2.2 设置配置

```c
ADS1118_Config_t cfg;

cfg.range = ADS1118_RANGE_4P096V;
cfg.rate = ADS1118_RATE_128SPS;
cfg.pull_up_en = true;
ADS1118_SetConfig(&cfg);
```

参数说明：

- `range`
  输入量程。测 0~3.3V 推荐 `ADS1118_RANGE_4P096V`
- `rate`
  采样率。常用 `ADS1118_RATE_128SPS`
- `pull_up_en`
  DOUT 内部上拉使能

### 2.3 读取电压

```c
float voltage_v;

if (ADS1118_ReadVoltageSingle(ADS1118_CHANNEL_AIN0_GND, &voltage_v))
{
    /* voltage_v 单位为伏特 */
}
```

常用通道：

- `ADS1118_CHANNEL_AIN0_GND`
- `ADS1118_CHANNEL_AIN1_GND`
- `ADS1118_CHANNEL_AIN0_AIN1`

### 2.4 读取原始码值

```c
int16_t raw;

if (ADS1118_ReadRawSingle(ADS1118_CHANNEL_AIN0_GND, ADS1118_MODE_ADC, &raw))
{
    float voltage_v = ADS1118_RawToVoltage(raw, ADS1118_RANGE_4P096V);
}
```

### 2.5 读取内部温度

```c
float temp_c;

if (ADS1118_ReadInternalTempC(&temp_c))
{
    /* temp_c 为内部温度 */
}
```

### 2.6 读取热电偶温度

```c
float thermo_c;

if (ADS1118_ReadThermocoupleTempC(ADS1118_CHANNEL_AIN0_AIN1, &thermo_c))
{
    /* thermo_c 为补偿后的热电偶温度 */
}
```

### 2.7 调试读取

```c
int16_t raw;
uint16_t cfg_word;
uint8_t miso_idle;

ADS1118_ReadRawSingleDebug(ADS1118_CHANNEL_AIN0_GND,
                           ADS1118_MODE_ADC,
                           &raw,
                           &cfg_word,
                           &miso_idle);
```

适用场景：

- 查通信
- 看配置字
- 看 MISO 空闲电平

## 3. AD7616

### 3.1 初始化

```c
#include "ad7616.h"

delay_init(HAL_RCC_GetSysClockFreq());
MX_GPIO_Init();

AD7616_Init(HARDWARE_MODE);
AD7616_Reset();
AD7616_Parallel_Set_voltage(Range_10_V);
AD7616_Parallel_Channel_Select(0U);
```

说明：

- `HARDWARE_MODE` 表示并口读取
- `SOFTWARE_MODE` 表示串口寄存器配置与串口读取
- `AD7616_Reset()` 建议在初始化模式后调用一次

### 3.2 启动一次转换并读取

```c
int16_t data_a;
int16_t data_b;

AD7616_Conversion();
AD7616_Read_Data(&data_a, &data_b);
```

说明：

- `data_a`、`data_b` 分别对应当前通道对的 A/B 组输出
- `AD7616_Read_Data()` 内部会等待 `BUSY` 结束

### 3.3 带超时读取

```c
int16_t data_a;
int16_t data_b;

if (AD7616_Read_Data_Timeout(&data_a, &data_b, 2000U) == HAL_OK)
{
    /* 读取成功 */
}
```

适用场景：

- 调试 BUSY 线是否正常
- 避免异常连线时主循环长时间阻塞

### 3.4 并口模式切换通道对

```c
AD7616_Parallel_Channel_Select(3U);
```

说明：

- `channel` 对应 `CHS[2:0]`
- 典型取值为 `0~7`

### 3.5 并口模式设置量程

```c
AD7616_Parallel_Set_voltage(Range_5_V);
```

可选量程：

- `Range_10_V`
- `Range_5_V`
- `Range_2_5_V`

### 3.6 串口模式初始化示例

```c
AD7616_Init(SOFTWARE_MODE);
AD7616_Reset();
AD7616_Serial_Output_Format(Serial_Line_2_Output);
AD7616_Serial_Set_voltage(Range_10_V);
AD7616_Seria_Channel(0U);
```

说明：

- `Serial_Line_2_Output` 使用 `SDOA + SDOB` 双线同时输出
- 如果走串口模式，推荐优先使用双线输出，读取逻辑更直接

### 3.7 串口模式读取

```c
int16_t data_a;
int16_t data_b;

AD7616_Conversion();

if (AD7616_Read_Data_Timeout(&data_a, &data_b, AD7616_BUSY_TIMEOUT_US) == HAL_OK)
{
    /* software mode 下内部会调用 AD7616_Read_Serial() */
}
```

### 3.8 使用注意事项

- `delay_init()` 必须先完成，否则 `delay_us()` 与 BUSY 超时等待都不可靠
- 当前板级 `DB0~DB15` 分散在多个 GPIO 端口，驱动已逐位拼接，不需要用户手工处理
- `AD7616_Read_Data()` 只负责读当前选中的通道对，不会自动轮询 8 路

## 4. delay

### 3.1 初始化

```c
delay_init(HAL_RCC_GetSysClockFreq());
```

### 3.2 微秒延时

```c
delay_us(10);
```

### 3.3 毫秒延时

```c
delay_ms(100);
```

### 3.4 纳秒延时

```c
delay_ns(200);
```

说明：

- `delay_ns()` 只适合粗略短延时。

## 5. LCD

### 4.1 初始化

```c
#include "lcd.h"

LCD_Init();
```

### 4.2 清屏

```c
LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
```

### 4.3 显示字符串

```c
LCD_ShowString(0, 0, (const u8 *)"HELLO", WHITE, BLACK, 16, 0);
```

参数：

- `x, y`：起始坐标
- `p`：字符串指针
- `fc`：前景色
- `bc`：背景色
- `sizey`：字高
- `mode`：0 带背景，1 透明模式

### 4.4 显示整数

```c
LCD_ShowIntNum(0, 20, 1234, 4, YELLOW, BLACK, 16);
```

### 4.5 画点/线/框

```c
LCD_DrawPoint(10, 10, RED);
LCD_DrawLine(0, 0, 50, 50, GREEN);
LCD_DrawRectangle(5, 5, 60, 40, CYAN);
```

## 6. AD9834

### 5.1 初始化

```c
#include "AD9834.h"

AD9834_Init();
```

### 5.2 设置频率

```c
AD9834_Set_Freq(AD9834_FREQ0, 1000U);
```

### 5.3 选择波形

```c
AD9834_Select_Wave(AD9834_WAVE_SINE);
```

### 5.4 推荐高层接口

```c
if (!AD9834_ConfigureOutput(AD9834_WAVEFORM_SINE, 10000U))
{
    /* 参数非法 */
}
```

## 7. 最小业务模板

```c
int main(void)
{
    float voltage_v;

    HAL_Init();
    SystemClock_Config();
    delay_init(HAL_RCC_GetSysClockFreq());
    MX_GPIO_Init();

    ADS1118_Init();

    while (1)
    {
        ADS1118_ReadVoltageSingle(ADS1118_CHANNEL_AIN0_GND, &voltage_v);
        HAL_Delay(100);
    }
}
```