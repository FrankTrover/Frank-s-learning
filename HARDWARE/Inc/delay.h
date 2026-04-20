#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx_hal.h"

/*
 * 延时模块使用规则 / Delay module usage rule.
 *   1) 先调用 delay_init(HAL_RCC_GetSysClockFreq())
 *      Call delay_init(HAL_RCC_GetSysClockFreq()) first.
 *   2) 再调用 delay_us/delay_ms/delay_ns
 *      Then call delay_us/delay_ms/delay_ns.
 */

void delay_init(uint32_t sysclk_freq);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay_ns(uint32_t ns);

#endif