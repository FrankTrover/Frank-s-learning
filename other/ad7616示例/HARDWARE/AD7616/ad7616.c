/**********************************************************
                       康威电子
本程序只供学习使用，未经作者许可，不得用于其它任何用途											 
功能：AD7616模块读取16路通道数据,串口发送数据
时间：2025/06/23
版本：1.8
作者：康威电子
其他：程序仅供功能验证，如需更精确测量或更复杂功能请自行编写程序。
			更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
			https://kvdz.taobao.com/
**********************************************************/
#include "ad7616.h"
#include "delay.h"
uint8_t ad7616_range,Mode;
/* Dx引脚端口 */
GPIO_TypeDef *port_list[] =
{
    AD7616_D0_GPIO_Port,  AD7616_D1_GPIO_Port,  AD7616_D2_GPIO_Port,
    AD7616_D3_GPIO_Port,  AD7616_D4_GPIO_Port,  AD7616_D5_GPIO_Port,
    AD7616_D6_GPIO_Port,  AD7616_D7_GPIO_Port,  AD7616_D8_GPIO_Port,
    AD7616_D9_GPIO_Port,  AD7616_D10_GPIO_Port, AD7616_D11_GPIO_Port,
    AD7616_D12_GPIO_Port, AD7616_D13_GPIO_Port, AD7616_D14_GPIO_Port,
    AD7616_D15_GPIO_Port
};

/* Dx引脚 */
uint16_t pin_list[] ={
    AD7616_D0_Pin,  AD7616_D1_Pin,  AD7616_D2_Pin,  AD7616_D3_Pin,
    AD7616_D4_Pin,  AD7616_D5_Pin,  AD7616_D6_Pin,  AD7616_D7_Pin,
    AD7616_D8_Pin,  AD7616_D9_Pin,  AD7616_D10_Pin, AD7616_D11_Pin,
    AD7616_D12_Pin, AD7616_D13_Pin, AD7616_D14_Pin, AD7616_D15_Pin
};

/**************************************************************** 
** 函数名称 AD7616_Serial_GPIO_Init
** 函数功能 ：串行通讯 AD7616 I/O口初始化
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Serial_GPIO_Init(void)
{

 GPIO_InitTypeDef GPIO_InitStruct = {0};


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);



	GPIO_ResetBits(GPIOA,AD7616_CS_Pin|AD7616_WR_Pin|AD7616_RD_Pin|AD7616_BUSY_Pin|AD7616_RST_Pin
												|AD7616_RNG0_Pin|AD7616_CHS2_Pin|AD7616_CHS1_Pin|AD7616_CHS0_Pin
												|AD7616_SER_Pin|AD7616_RNG1_Pin);
	

	
	GPIO_ResetBits(GPIOC,AD7616_D0_Pin|AD7616_D1_Pin|AD7616_D2_Pin|AD7616_D3_Pin|AD7616_D4_Pin|AD7616_D5_Pin|
											AD7616_D6_Pin|AD7616_D7_Pin|AD7616_D8_Pin|AD7616_D9_Pin|AD7616_D10_Pin
											|AD7616_D11_Pin | AD7616_D12_Pin|AD7616_D13_Pin| AD7616_D14_Pin|AD7616_D15_Pin);	



	GPIO_WriteBit(GPIOC,AD7616_D12_Pin| AD7616_D11_Pin | AD7616_D10_Pin |AD7616_D5_Pin |AD7616_D4_Pin, Bit_RESET);

	GPIO_WriteBit(GPIOA,AD7616_RNG1_Pin| AD7616_RNG0_Pin | AD7616_SER_Pin |AD7616_CONV_Pin |AD7616_RST_Pin
											|AD7616_RD_Pin|AD7616_CS_Pin|AD7616_BUSY_Pin, Bit_RESET);








  GPIO_InitStruct.GPIO_Pin =AD7616_D12_Pin| AD7616_D11_Pin | AD7616_D10_Pin |AD7616_D5_Pin |AD7616_D4_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = AD7616_RNG1_Pin| AD7616_RNG0_Pin | AD7616_SER_Pin |AD7616_CONV_Pin |AD7616_RST_Pin
														|AD7616_RD_Pin|AD7616_CS_Pin|AD7616_BUSY_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = AD7616_BUSY_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = AD7616_D11_Pin | AD7616_D12_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = AD7616_D5_Pin | AD7616_D4_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
	
}
/**************************************************************** 
** 函数名称 ：AD7616_Parallel_GPIO_Init
** 函数功能 ：并行GPIO口初始化
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Parallel_GPIO_Init(void)
{


  GPIO_InitTypeDef GPIO_InitStruct = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
   GPIO_ResetBits(GPIOA,AD7616_CS_Pin|AD7616_WR_Pin|AD7616_RD_Pin|AD7616_BUSY_Pin|AD7616_CONV_Pin
												|AD7616_RST_Pin|AD7616_RNG0_Pin|AD7616_CHS2_Pin|AD7616_CHS1_Pin|AD7616_CHS0_Pin
												|AD7616_SER_Pin|AD7616_RNG1_Pin);
	
	
	GPIO_ResetBits(GPIOC,AD7616_D0_Pin|AD7616_D1_Pin|AD7616_D2_Pin|AD7616_D3_Pin|AD7616_D4_Pin|AD7616_D5_Pin|
											AD7616_D6_Pin|AD7616_D7_Pin|AD7616_D8_Pin|AD7616_D9_Pin|AD7616_D10_Pin
											|AD7616_D11_Pin | AD7616_D12_Pin|AD7616_D13_Pin|AD7616_D14_Pin|AD7616_D15_Pin);
	
	
	
	
	GPIO_InitStruct.GPIO_Pin =AD7616_CS_Pin|AD7616_WR_Pin|AD7616_RD_Pin|AD7616_BUSY_Pin|AD7616_CONV_Pin
												|AD7616_RST_Pin|AD7616_RNG0_Pin|AD7616_CHS2_Pin|AD7616_CHS1_Pin|AD7616_CHS0_Pin
												|AD7616_SER_Pin|AD7616_RNG1_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	GPIO_InitStruct.GPIO_Pin = AD7616_BUSY_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
		GPIO_InitStruct.GPIO_Pin =AD7616_D0_Pin|AD7616_D1_Pin|AD7616_D2_Pin|AD7616_D3_Pin|AD7616_D4_Pin|AD7616_D5_Pin|
											AD7616_D6_Pin|AD7616_D7_Pin|AD7616_D8_Pin|AD7616_D9_Pin|AD7616_D10_Pin
											|AD7616_D11_Pin | AD7616_D12_Pin|AD7616_D13_Pin|AD7616_D14_Pin|AD7616_D15_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
	

	

}
/**************************************************************** 
** 函数名称 ：AD7616_Working_Mode
** 函数功能 ：选择模式
** 入口参数 ：mode-->模式		选择模式(软件模式或硬件模式)
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Working_Mode(uint8_t mode)
{
  if (mode == HARDWARE_MODE)
	{
    GPIO_WriteBit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, Bit_SET);
  } 
	else if (mode == SOFTWARE_MODE)
	{
    GPIO_WriteBit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, Bit_RESET);
  }
}
/**************************************************************** 
** 函数名称 ：AD7616_Set_Pin_Output
** 函数功能 ：设置控制引脚为推完输出 范围（连接AD7616模块引脚DB0~DB15）
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Set_Pin_Output(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	uint8_t i = 0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

  for (i = 0; i < sizeof(pin_list) / sizeof(uint16_t); i++)
	{
    GPIO_InitStruct.GPIO_Pin = pin_list[i];
    GPIO_Init(port_list[i], &GPIO_InitStruct);
  }
}
/**************************************************************** 
** 函数名称 ：AD7616_Set_Pin_Input
** 函数功能 ：设置控制引脚为浮空输入 范围（连接AD7616模块引脚DB0~DB15）
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
 void AD7616_Set_Pin_Input(void)
{
	uint8_t i;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

  for ( i = 0; i < sizeof(pin_list) / sizeof(uint16_t); i++)
	{
    GPIO_InitStruct.GPIO_Pin = pin_list[i];
    GPIO_Init(port_list[i], &GPIO_InitStruct);
  }
}

/**************************************************************** 
** 函数名称 ：AD7616_Read_Data_16Bit
** 函数功能 ：并行接口获取AD7616数据
** 入口参数 ：无
** 出口参数 ：input_level-->读取到AD7616数据
** 函数说明 ：无
*****************************************************************/
uint16_t AD7616_Read_Data_16Bit(void)
{
  uint16_t input_level = 0;
//	uint8_t state;
//	uint8_t i;
//	//当控制板引脚连接AD7616的DB0~DB15不连续时将下面的函数注释取消
//  for ( i = 0; i < sizeof(pin_list) / sizeof(uint16_t); i++)
//	{
//     state = GPIO_ReadInputDataBit(port_list[i], pin_list[i]);
//    if (state)
//		{
//      input_level |= 0x0001<<i;
//    }
//	}
//	return input_level;
//如果AD7616的DB0~DB15对应接在PC0~PC15那么只需要使用此函数即可
	input_level = GPIO_ReadInputData(GPIOC);
	return input_level;

}

/**************************************************************** 
** 函数名称 ：AD7616_Reset
** 函数功能 ：AD7616 模块复位
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Reset(void)
{
	GPIO_WriteBit(AD7616_RST_GPIO_Port, AD7616_RST_Pin, Bit_RESET);
	delay_ms(1);
	GPIO_WriteBit(AD7616_RST_GPIO_Port, AD7616_RST_Pin, Bit_SET);
	delay_ms(20);
}
/**************************************************************** 
** 函数名称 ：AD7616_Conversion
** 函数功能 ：AD7616 模块启动转换
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Conversion(void)
{
  GPIO_WriteBit(AD7616_CONV_GPIO_Port, AD7616_CONV_Pin, Bit_RESET);
  delay_ms(1);
  GPIO_WriteBit(AD7616_CONV_GPIO_Port, AD7616_CONV_Pin, Bit_SET);
}
/**************************************************************** 
** 函数名称 ：AD7616_Parallel_Set_voltage
** 函数功能 ：并行配置AD7616量程
** 入口参数 ：range--> 量程 范围（Range_10_V,Range_2_5_V,Range_5_V）
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Parallel_Set_voltage(uint8_t range)
{
  ad7616_range = range;
	switch (ad7616_range)
	{
		case Range_2_5_V:
					GPIO_WriteBit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, Bit_SET);
					GPIO_WriteBit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, Bit_RESET);
					break;
		case Range_5_V:
					GPIO_WriteBit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, Bit_RESET);
					GPIO_WriteBit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, Bit_SET);
					break;
		case Range_10_V:
					GPIO_WriteBit(AD7616_RNG0_GPIO_Port, AD7616_RNG0_Pin, Bit_SET);
					GPIO_WriteBit(AD7616_RNG1_GPIO_Port, AD7616_RNG1_Pin, Bit_SET);
				  break;
		default:
					 break;
	}
}

/**************************************************************** 
** 函数名称 ：AD7616_Read_Data_16Bit
** 函数功能 ：并行 获取AD7616单次采集数据
** 入口参数 ：无
** 出口参数 ：data_A--> A通道数组指针	 范围（0~65535）
							data_B--> B通道数组指针	范围（0~65535）
** 函数说明 ：根据 AD7616_Init()函数确定串行读取数据还是并行读取数据
*****************************************************************/
void AD7616_Read_Data(int16_t *data_A,int16_t *data_B) 
{
  uint8_t state = Bit_SET;
	do
	{
    state = GPIO_ReadInputDataBit(AD7616_BUSY_GPIO_Port, AD7616_BUSY_Pin);
  } while (state);
  GPIO_WriteBit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, Bit_RESET);
  delay_ms(1);
	
	if(Mode==HARDWARE_MODE)
	{
	GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_RESET);
	data_A[0] = AD7616_Read_Data_16Bit();
	GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_SET);

	GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_RESET);
	data_B[0] = AD7616_Read_Data_16Bit();
	GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_SET);
	}
	
	else if(Mode==SOFTWARE_MODE)
	{
		AD7616_Read_Serial(data_A,data_B);
	}
	GPIO_WriteBit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, Bit_SET);
}
/**************************************************************** 
** 函数名称 ：AD7616_Read_Serial
** 函数功能 ：并行读取AD7616单次A,B通道数据
** 入口参数 ：无
** 出口参数 ：data_A--> A通道数组指针	 范围（0~65535）
							data_B--> B通道数组指针	范围（0~65535）
** 函数说明 ：无
*****************************************************************/
void AD7616_Read_Serial(int16_t *data_A,int16_t *data_B)
{
  uint16_t shift = 0x8000,i=0;
  int16_t input_data_a = 0;
  int16_t input_data_b = 0;
  uint8_t state = Bit_RESET;
  for ( i = 0; i < 16; i++)
	{
    GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_RESET);
    state = GPIO_ReadInputDataBit(AD7616_D12_GPIO_Port, AD7616_D12_Pin);
    if (state == Bit_SET)
		{
      input_data_a |= shift;
    }
		else if (state == Bit_RESET)
		{
      input_data_a &= (~shift);
    }
    state = GPIO_ReadInputDataBit(AD7616_D11_GPIO_Port, AD7616_D11_Pin);
    if (state == Bit_SET)
		{
      input_data_b |= shift;
    }
		else if (state == Bit_RESET)
		{
      input_data_b &= (~shift);
    }
    shift >>= 1;
    GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_SET);
  }
  data_A[0] = input_data_a;
  data_B[0] = input_data_b;
}
/**************************************************************** 
** 函数名称 ：AD7616_Parallel_Channel_Select
** 函数功能 ：并行AD7616通道组选择  共8组
** 入口参数 ：channel--> 通道组 范围（0~7）
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Parallel_Channel_Select(uint8_t channel)
{
  switch (channel)
	{
  case 0:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_RESET);
    break;
  case 1:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_RESET);
    break;
  case 2:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_RESET);
    break;
  case 3:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_RESET);
    break;
  case 4:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_SET);
    break;
  case 5:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_SET);
    break;
  case 6:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_SET);
    break;
  case 7:
    GPIO_WriteBit(AD7616_CHS0_GPIO_Port, AD7616_CHS0_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS1_GPIO_Port, AD7616_CHS1_Pin, Bit_SET);
    GPIO_WriteBit(AD7616_CHS2_GPIO_Port, AD7616_CHS2_Pin, Bit_SET);
    break;
  }
}

/**************************************************************** 
** 函数名称 ：AD7616_Init
** 函数功能 ：AD7616 模块初始化
** 入口参数 ：mode -->模式
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Init(uint8_t mode)
{
	Mode=mode;
	if(mode==HARDWARE_MODE)
	{
		AD7616_Parallel_GPIO_Init();
		AD7616_Set_Pin_Input();
		GPIO_WriteBit(AD7616_SER_GPIO_Port, AD7616_SER_Pin, Bit_RESET);
	}
	else if(mode==SOFTWARE_MODE)
	{
		AD7616_Serial_GPIO_Init();
		GPIO_WriteBit(AD7616_SER_GPIO_Port, AD7616_SER_Pin, Bit_SET);
	}
  GPIO_WriteBit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, Bit_SET);
  GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_SET);
  AD7616_Working_Mode(mode);
}



/**************************************************************** 
** 函数名称 AD7616_Write_Serial
** 函数功能 ：串行向AD7616写入16位数据
** 入口参数 ：data-->数据
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Write_Serial(uint16_t data)
{
	
  uint16_t shift = 0x8000,level;
	uint8_t i = 0;
  for (i = 0; i < 16; i++)
	{
     level = shift & data;
    if (level)
		{
      GPIO_WriteBit(AD7616_D10_GPIO_Port, AD7616_D10_Pin, Bit_SET);
    } 
		else
		{
      GPIO_WriteBit(AD7616_D10_GPIO_Port, AD7616_D10_Pin, Bit_RESET);
    }
    shift >>= 1;
    GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_RESET);
    GPIO_WriteBit(AD7616_RD_GPIO_Port, AD7616_RD_Pin, Bit_SET);
  }
}

/**************************************************************** 
** 函数名称 AD7616_Write_Serial
** 函数功能 ：串行写寄存器 
** 入口参数 ：address-->地址 范围（0~63）
							data-->数据 范围（0~255）
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
  void AD7616_Serial_Write_Rang(uint8_t address, uint8_t data)
{
  /* 选择写命令，必须将位D15设为1 */
  uint16_t wrtie_data = (((uint16_t)address << 9) | 0x8000) | (uint16_t)data;
  GPIO_WriteBit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, Bit_RESET);
  AD7616_Write_Serial(wrtie_data);
  GPIO_WriteBit(AD7616_CS_GPIO_Port, AD7616_CS_Pin, Bit_SET);
}

/**************************************************************** 
** 函数名称 ：AD7616_Serial_Set_voltage
** 函数功能 ：串行配置AD7616量程
** 入口参数 ：range-->寄存器
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Serial_Set_voltage(uint8_t range)//
{

  uint8_t range_data = (range << 6) | (range << 4) | (range << 2) | range;//
	ad7616_range = range;
  AD7616_Serial_Write_Rang(Input_Range_Register_A1, range_data);//
  AD7616_Serial_Write_Rang(Input_Range_Register_A2, range_data);//
  AD7616_Serial_Write_Rang(Input_Range_Register_B1, range_data);//
  AD7616_Serial_Write_Rang(Input_Range_Register_B2, range_data);//

}
/**************************************************************** 
** 函数名称 ：AD7616_Seria_Channel
** 函数功能 ：串行AD7616通道组选择
** 入口参数 ：channel-->通道组  范围（0~7）
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Seria_Channel(uint8_t channel)
{
  uint8_t channel_data = (channel << 4) | channel;
	AD7616_Serial_Write_Rang(Channel_Register, channel_data);
}

/**************************************************************** 
** 函数名称 ：AD7616_Serial_Output_Format
** 函数功能 ：串行设置AD7616串行输出模式
** 入口参数 ：format-->格式
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void AD7616_Serial_Output_Format(uint8_t format)
{
  if (format == Serial_Line_1_Output)
	{
		//当D4/SER1W为低电平时，串行输出仅工作在SDOA上。
    GPIO_WriteBit(AD7616_D4_GPIO_Port, AD7616_D4_Pin, Bit_RESET);
  }
	else if (format == Serial_Line_2_Output)
	{
		//当D4/SER1W为高电平时，串行输出工作在SDOA和SDOB上。
    GPIO_WriteBit(AD7616_D4_GPIO_Port, AD7616_D4_Pin, Bit_SET);
  }
}




















