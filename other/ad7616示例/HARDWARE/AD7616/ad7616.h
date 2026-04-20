#ifndef __AD7616_H__
#define __AD7616_H__
#include "stm32f10x.h"

/*
	需更改成自己控制板的引脚时。例如AD7616_D0_Pin脚改成PC2控制，则定义"#define AD7616_D0_Pin GPIO_Pin_2"
	注意！！！
	更改完成后需在“void AD7616_Serial_GPIO_Init(void)”初始化对应的引脚
*/

//并行输出/输入数据位 0 至数据位 3详情参见“并行接口”部分
//在串行模式下，这些引脚必须与 DGND 相连。
#define AD7616_D0_Pin 					GPIO_Pin_0
#define AD7616_D0_GPIO_Port 		GPIOC

#define AD7616_D1_Pin 					GPIO_Pin_1
#define AD7616_D1_GPIO_Port  		GPIOC

#define AD7616_D2_Pin						 GPIO_Pin_2
#define AD7616_D2_GPIO_Port 		 GPIOC

#define AD7616_D3_Pin					   GPIO_Pin_3
#define AD7616_D3_GPIO_Port      GPIOC

//并行模式下，此引脚充当三态并行数字输出/输入引脚。
#define AD7616_D4_Pin						 GPIO_Pin_4
#define AD7616_D4_GPIO_Port      GPIOC


#define AD7616_D5_Pin 						GPIO_Pin_5
#define AD7616_D5_GPIO_Port			  GPIOC

//并行输出/输入数据位 6 和数据位 7。
#define AD7616_D6_Pin 						GPIO_Pin_6
#define AD7616_D6_GPIO_Port			  GPIOC

#define AD7616_D7_Pin 						GPIO_Pin_7
#define AD7616_D7_GPIO_Port			  GPIOC

//并行输出/输入数据位 9 和数据位 8。
#define AD7616_D8_Pin							GPIO_Pin_8
#define AD7616_D8_GPIO_Port			  GPIOC
	
#define AD7616_D9_Pin						  GPIO_Pin_9
#define AD7616_D9_GPIO_Port			  GPIOC

//并行输出/输入数据位
//在串行模式下，当SER/PAR = 1 时，此引脚用作SPI接口的数据输入。
//当SER/PAR = 0 时，此引脚充当三态并行数字输入/输出。

#define AD7616_D10_Pin						GPIO_Pin_10
#define AD7616_D10_GPIO_Port			GPIOC
//并行输出/输入数据位 11/串行数据输出B。
#define AD7616_D11_Pin						 GPIO_Pin_11
#define AD7616_D11_GPIO_Port			 GPIOC
//并行输出/输入数据位 12/串行数据输出A。 
#define AD7616_D12_Pin 							GPIO_Pin_12
#define AD7616_D12_GPIO_Port			  GPIOC


//并行输出/输入数据位 13、数据位 14 和数据位 15/过采样率选择。 
#define AD7616_D13_Pin						  GPIO_Pin_13
#define AD7616_D13_GPIO_Port			  GPIOC

#define AD7616_D14_Pin              GPIO_Pin_14
#define AD7616_D14_GPIO_Port        GPIOC

#define AD7616_D15_Pin 							GPIO_Pin_15
#define AD7616_D15_GPIO_Port			  GPIOC




//片选
#define AD7616_CS_Pin							  GPIO_Pin_0
#define AD7616_CS_GPIO_Port				  GPIOA
//写入/突发模式使能。
#define AD7616_WR_Pin 							GPIO_Pin_1
#define AD7616_WR_GPIO_Port					GPIOA
//并行数据读取控制输入
#define AD7616_RD_Pin								GPIO_Pin_2
#define AD7616_RD_GPIO_Port					GPIOA
//输出繁忙。
#define AD7616_BUSY_Pin							GPIO_Pin_3
#define AD7616_BUSY_GPIO_Port				GPIOA
//通道组 A 和通道组 B 的转换开始输入。
#define AD7616_CONV_Pin 						GPIO_Pin_4
#define AD7616_CONV_GPIO_Port				GPIOA     


//复位输入。
#define AD7616_RST_Pin							GPIO_Pin_5
#define AD7616_RST_GPIO_Port				GPIOA

//选择被锁存。硬件模式下的范围选择不会被锁存。
#define AD7616_RNG0_Pin							GPIO_Pin_6
#define AD7616_RNG0_GPIO_Port				GPIOA

//通道选择输入 0 至输入 2。
#define AD7616_CHS2_Pin							GPIO_Pin_7 
#define AD7616_CHS2_GPIO_Port				GPIOA

#define AD7616_CHS1_Pin							GPIO_Pin_8
#define AD7616_CHS1_GPIO_Port				GPIOA

#define AD7616_CHS0_Pin 						GPIO_Pin_9
#define AD7616_CHS0_GPIO_Port			  GPIOA

//并行输出/输入数据位    在并行模式下
//此引脚充当三态并行数字输出/输入引脚
#define AD7616_SER_Pin							GPIO_Pin_10
#define AD7616_SER_GPIO_Port 				GPIOA

//硬件/软件模式选择，
#define AD7616_RNG1_Pin 						GPIO_Pin_11
#define AD7616_RNG1_GPIO_Port				GPIOA



#define HARDWARE_MODE										  0	//引脚控制模式
#define SOFTWARE_MODE											1//软件控制模式

enum AD7616_Register_Address
{
  Config_Register = 0x02,//配置寄存器
  Channel_Register = 0x03,//通道寄存器
  Input_Range_Register_A1,//选择输入范围寄存器A1。
  Input_Range_Register_A2,//选择输入范围寄存器A2。
  Input_Range_Register_B1,//选择输入范围寄存器B1。
  Input_Range_Register_B2,//选择输入范围寄存器B2。
};

/* 电压量程 */
//HW_RNGSEL0,HW_RNGSEL1决定输入电压范围
enum AD7616_Range
{
  Range_10_V = 0x00,//输入电压范围 ±10V
  Range_2_5_V,//输入电压范围 ±2.5V
  Range_5_V,//输入电压范围 ±5V
};


/* DoutX输出格式选择 */
//DB4控制数据输出
enum AD7606B_Serial_Output_Format
{
  Serial_Line_1_Output = 0x00,//当SER1W为低电平时，串行输出仅工作在SDOA上
  Serial_Line_2_Output,//当SER1W为高电平时，串行输出工作在SDOA和SDOB上。

};
void AD7616_Parallel_GPIO_Init(void);
void AD7616_Init(uint8_t mode);//AD7616 模块初始化
void AD7616_Reset(void);//AD7616 模块复位
void AD7616_Conversion(void);//AD7616 模块启动转换
void AD7616_Parallel_Set_voltage(uint8_t range);//配置AD7616量程
void AD7616_Read_Data(int16_t *data_A,int16_t *data_B);//获取AD7616单次采集数据
void AD7616_Parallel_Channel_Select(uint8_t channel);//AD7616通道组选择  共8组


//串行
void AD7616_Serial_GPIO_Init(void);//串行 AD7616 I/O口初始化
void AD7616_Write_Serial(uint16_t data);//向AD7616写入16位数据
void AD7616_Serial_Write_Rang(uint8_t address, uint8_t data);//写寄存器 
void AD7616_Serial_Set_voltage(uint8_t range);//配置AD7616量程
void AD7616_Seria_Channel(uint8_t channel);//AD7616通道组选择
void AD7616_Serial_Output_Format(uint8_t format);//设置AD7616串行输出模式
void AD7616_Read_Serial(int16_t *data_A,int16_t *data_B);
#endif
