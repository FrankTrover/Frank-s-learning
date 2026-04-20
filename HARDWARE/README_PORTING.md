# HARDWARE 移植说明

本文档面向换板、改引脚、重新生成 CubeMX 工程等场景。

## 1. 总原则

移植时优先修改：

1. `Core/Inc/main.h`
2. `Core/Src/gpio.c`
3. `HARDWARE/Inc/*.h`
4. `HARDWARE/Src/*.c`

不要反过来只改 `HARDWARE` 宏而忽略 `main.h`。

## 2. ADS1118 移植

### 2.1 关键引脚

- `SCK_ADS1118_Pin`
- `CS_AD1118_Pin`
- `MISO_ADS1118_Pin`
- `MOSI_ADS1118_Pin`

### 2.2 检查顺序

1. `main.h` 中四个引脚是否正确。
2. `gpio.c` 中方向是否正确：
   - `SCK/CS/MOSI` 输出
   - `MISO` 输入
3. 模块供电与地是否可靠。
4. 模拟输入是否接在 `AIN0~AIN3`，不是模块 `VCC/VIN`。

### 2.3 常见故障

- `RAW=-1`
  常见于 `MISO/DOUT` 无返回数据。
- 电压始终 0
  常见于通道接错、量程不对、AIN 未接到真实输入。

## 3. LCD 移植

### 3.1 关键引脚

- `SCL_Pin`
- `SDA_Pin`
- `RES_Pin`
- `DC_Pin`
- `CS_Pin`
- `BLK_Pin`

### 3.2 常改宏

- `LCD_BLK_ACTIVE_HIGH`
  背光极性
- `USE_HORIZONTAL`
  屏幕方向

### 3.3 端口时钟

`lcd.c` 中 `LCD_Enable_GPIO_Clock()` 必须覆盖你使用到的 GPIO 端口。

## 4. AD7616 移植

### 4.1 关键引脚

- `AD7616_CONV_Pin`
- `AD7616_BUSY_Pin`
- `AD7616_CS_Pin`
- `AD7616_RD_Pin`
- `AD7616_WR_Pin`
- `AD7616_RST_Pin`
- `AD7616_S_P_Pin`
- `AD7616_CHS0_Pin`
- `AD7616_CHS1_Pin`
- `AD7616_CHS2_Pin`
- `AD7616_RNG0_Pin`
- `AD7616_RNG1_Pin`
- `AD7616_D0_Pin ~ AD7616_D15_Pin`

### 4.2 关键检查点

1. `main.h` 中 AD7616 相关引脚名称和端口是否完整。
2. `gpio.c` 中 `BUSY` 是否为输入，`CONV/CS/RD/WR/RST/CHS/RNG` 是否为输出。
3. 并口模式下，`DB0~DB15` 是否都配置为输入。
4. 串口模式下，`DB10/DB11/DB12` 是否允许复用成 `SDI/SDOB/SDOA`。

### 4.3 当前工程的特殊点

- 板级 `DB0~DB15` 分布在多个 GPIO 端口，不能照搬 F1 示例里“整口读 GPIO”的做法。
- 驱动当前通过逐位读取再拼成 16 位数据，适合本板映射。
- `AD7616_Read_Data_Timeout()` 依赖 DWT 计数器做微秒超时，先确保 `delay_init()` 可用。

### 4.4 模式选择建议

- 追求读取简单、GPIO 足够时，优先用并口模式。
- GPIO 紧张或需要写内部寄存器时，使用串口模式。
- 串口模式下推荐 `Serial_Line_2_Output`，这样 A/B 两组可同步读取。

### 4.5 常见故障

- `AD7616_Read_Data_Timeout()` 返回 `HAL_TIMEOUT`
  常见于 `BUSY` 线未接通、转换未启动、DWT 未初始化。
- 读取值恒为 0 或固定值
  常见于 `DB0~DB15` 方向错误，或 `CS/RD` 时序没有真正拉低。
- 串口模式只有一组数据变化
  常见于输出格式和实际接线不一致，需要检查 `SER1W` 设置与 `SDOA/SDOB` 接线。

### 4.6 推荐初始化顺序

```c
HAL_Init();
SystemClock_Config();
delay_init(HAL_RCC_GetSysClockFreq());
MX_GPIO_Init();

AD7616_Init(HARDWARE_MODE);
AD7616_Reset();
AD7616_Parallel_Set_voltage(Range_10_V);
AD7616_Parallel_Channel_Select(0U);
```

## 5. AD9834 移植

### 4.1 关键引脚

- `FSY_Pin`
- `SCK_Pin`
- `SDA_ad_Pin`
- `RST_Pin`
- `FS_Pin`
- `PS_Pin`

### 4.2 最重要宏

- `AD9834_SYSTEM_CLOCK`

如果模块参考时钟变了，这个值必须同步修改。

### 4.3 端口时钟

`AD9834_Init()` 当前显式打开 `GPIOB` 和 `GPIOD` 时钟。

迁移到其他端口时，这里要一起改。

## 6. delay 模块移植

当前 `delay.c` 使用 DWT 计数器。

移植前确认：

1. 内核支持 DWT
2. `CoreDebug->DEMCR` 可访问
3. `delay_init()` 传入的时钟频率真实正确

## 7. 推荐初始化顺序

```c
HAL_Init();
SystemClock_Config();
delay_init(HAL_RCC_GetSysClockFreq());
MX_GPIO_Init();

ADS1118_Init();
AD7616_Init(HARDWARE_MODE);
LCD_Init();
AD9834_Init();
```

## 8. 排障顺序

推荐按这个顺序排：

1. 看 `main.h`
2. 看 `gpio.c`
3. 看 `HARDWARE/Inc/*.h` 映射
4. 看 `HARDWARE/Src/*.c` 时钟和初始化
5. 最后再看业务逻辑

## 9. 建议

后续如果还会重新生成 CubeMX 代码，建议把业务代码尽量限制在：

- `USER CODE BEGIN 2`
- `USER CODE BEGIN 3`

这样最不容易被覆盖。