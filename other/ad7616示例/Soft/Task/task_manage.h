#ifndef __task_manage_H
#define __task_manage_H
#include "stm32f10x.h"
#include "lcd.h"
#include "key.h"

//extern u32 SysTimer;

void Copybuf2dis(uint8_t source[5], uint8_t dis[10], uint8_t  dispoint);
void Set_Voltage(uint32_t Key_Value, uint8_t* Task_ID);

#endif
