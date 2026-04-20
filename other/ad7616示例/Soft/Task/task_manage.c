/**********************************************************
                       康威电子
本程序只供学习使用，未经作者许可，不得用于其它任何用途											 
功能：DAC8550模块，配合康威数字模块主控板按键调整输出电压，LCD显示
时间：2024/12/9
版本：1.6
作者：康威电子
其他：程序仅供功能验证，如需更精确测量或更复杂功能请自行编写程序。
			更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
			https://kvdz.taobao.com/
**********************************************************/
#include "task_manage.h"
#include "delay.h"
#include "key.h"
#include "ad7616.h"

uint8_t Firt_In = 1;
uint8_t Task_Index = 0;
uint8_t _return=0;
/**************************************************************** 
** 函数名称 ：Copybuf2dis
** 函数功能 ：将整型数据转换成字符串
** 入口参数 ：*source-->整型指针数据
							dis[10]-->字符数组存放转换后的数据
							dispoint-->颜色反转使能
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void Copybuf2dis(uint8_t *source, uint8_t dis[10], uint8_t  dispoint)
{
	dis[0] = *source + 		 '0';
	dis[1] = '.';
	dis[2] = *(source+1) + '0';
	dis[3] = *(source+2) + '0';
	dis[4] = *(source+3) + '0';
	dis[5] = *(source+4) + '0';
	dis[6] = *(source+5) + '0';
	dis[7] = *(source+6) + '0';
	dis[8] = 'V';
	dis[9] = 0;

	if(dispoint < 1) dis[dispoint] += 128;
	else dis[dispoint+1] += 128;	
}
/**************************************************************** 
** 函数名称 ：Set_PointVoltage
** 函数功能 ：任务处理
** 入口参数 ：Key_Value-->按键状态（长按或短按）
						* Task_ID-->任务ID
** 出口参数 ：无
** 函数说明 ：无
*****************************************************************/
void Set_Voltage(uint32_t Key_Value, uint8_t* Task_ID)
{
	static uint8_t P_Index = 0;
	static uint32_t DAC8550_Voltage = 0;
	
	int16_t DAC8550_value;
	uint8_t V_buf[8];
	uint8_t display[20];

	if(Firt_In) 
	{
		Key_Value = K_2_L;
		LCD_Show_CEStr(30,0,"DAC5880");
		LCD_Show_CEStr(30,2,"输出电压");
		LCD_Show_CEStr(0,6,"↑    ←→    ↓");
		Firt_In = 0;
		_return=1;		
	}
		V_buf[0] = DAC8550_Voltage%10000000/1000000;
		V_buf[1] = DAC8550_Voltage%1000000/100000;
		V_buf[2] = DAC8550_Voltage%100000/10000;
		V_buf[3] = DAC8550_Voltage%10000/1000;
		V_buf[4] = DAC8550_Voltage%1000/100;
		V_buf[5] = DAC8550_Voltage%100/10;
		V_buf[6] = DAC8550_Voltage%10;
	
	switch(Key_Value)
	{
		case K_4_S: V_buf[P_Index]++;break;
		case K_4_L: V_buf[P_Index]++;break;
		case K_5_L: P_Index++;break;
		case K_5_S: P_Index++;break;
		case K_1_L: P_Index--;break;
		case K_1_S: P_Index--;break;
		case K_3_S: V_buf[P_Index]--;break;
		case K_3_L: V_buf[P_Index]--;break;
	}
	if(P_Index == 255) P_Index = 6;
	P_Index %= 7;
	if(V_buf[P_Index] == 255) V_buf[P_Index] = 9;
	if(V_buf[P_Index] ==  10) V_buf[P_Index] = 0;

	if(Key_Value != K_NO)
	{
		DAC8550_Voltage = V_buf[0]*1000000 + V_buf[1]*100000 + V_buf[2]*10000+ V_buf[3]*1000+V_buf[4]*100 + V_buf[5]*10 + V_buf[6];
		if(DAC8550_Voltage>5000000) DAC8550_Voltage=5000000;
		V_buf[0] = DAC8550_Voltage%10000000/1000000;
		V_buf[1] = DAC8550_Voltage%1000000/100000;
		V_buf[2] = DAC8550_Voltage%100000/10000;
		V_buf[3] = DAC8550_Voltage%10000/1000;
		V_buf[4] = DAC8550_Voltage%1000/100;
		V_buf[5] = DAC8550_Voltage%100/10;
		V_buf[6] = DAC8550_Voltage%10;
		
		Copybuf2dis(V_buf, display, P_Index);
		OLED_ShowString(25, 4, display);
		
		//计算DAC值
		DAC8550_value =((double)DAC8550_Voltage/5000000*65535)-32768;
		voltage_output(DAC8550_value);
		
		_return=1;
	}
}















