/**********************************************************
                       康威电子
本程序只供学习使用，未经作者许可，不得用于其它任何用途											 
功能：AD7616模块并行硬件读取16路通道数据,串口发送数据
时间：2025/06/23
版本：1.8
作者：康威电子
其他：程序仅供功能验证，如需更精确测量或更复杂功能请自行编写程序。
			更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
			https://kvdz.taobao.com/
**********************************************************/

#include "stm32_config.h"
#include "ad7616.h"
#include "usart.h"
#include <stdio.h>


int main(void)
{

	uint8_t i = 0,channel,Vout_buf[30] = {0};
	int16_t ad7616_A[8],ad7616_B[8];
	double Vout1,Vout2;
	MY_NVIC_PriorityGroup_Config(NVIC_PriorityGroup_2);	//设置中断分组

	delay_init(72);	//初始化延时函数
	USARTx_Init(115200);//串口3初始化
	delay_ms(100);

	AD7616_Init(HARDWARE_MODE);//AD7616模块初始化 硬件模式
  AD7616_Parallel_Set_voltage(Range_10_V);//输入电压范围 ±10V
  AD7616_Parallel_Channel_Select(0);//通道组0
  AD7616_Reset();//复位

	/*********************************************************************/
		//1.在ad7616.h中 将AD7616_D0_Pin,AD7616_D1_Pin...等更改成自己控制板的引脚。例如AD7616_D0_Pin脚改成PC2控制，则定义"#define AD7616_D0_Pin GPIO_Pin_2"
		//2.修改C文件ad7616.c中，AD7616_Parallel_GPIO_Init()函数，所有用到管脚的GPIO输出功能初始化
		//3.完成
	/***************************************************************************************************************/	
	while (1)
	{
		//切换通道后，数据读取的是上一次CONVST的值。例如切换到通道1，得到的数值是通道8的值
    //查看芯片手册31页图51可知。
		for(channel=0;channel<8;channel++)
		{
			AD7616_Parallel_Channel_Select(channel);
			AD7616_Conversion();
			if (channel==0)
			{
				//上一次通道的数据的数据放到数组最后一个元素
        AD7616_Read_Data(&ad7616_A[7],&ad7616_B[7]);
      }
			else
			{
        AD7616_Read_Data(&ad7616_A[channel-1],&ad7616_B[channel-1]);
      }
		}
		for(i=0;i<8;i++)
		{
			Vout1=(double)ad7616_A[i]*20.0/65535.0*1000;//Vout(mv)=AD*量程/2^16*1000  单位毫伏（mv）
			Vout2=(double)ad7616_B[i]*20.0/65535.0*1000;
			sprintf((char *)Vout_buf,"A%d:%.1fmv B%d:%.1fmv",i,Vout1,i, Vout2);
			printf("%s\r\n",Vout_buf);
		}
		printf("\r\n");
		delay_ms(1000);
  }
}





