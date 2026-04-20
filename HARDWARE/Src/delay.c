#include "delay.h"

static uint32_t fac_us;
static uint32_t sys_clk;

/* 启用 DWT 计数器作为高精度延时基准 / Enable DWT cycle counter as delay time base. */
static void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL = 0U;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/* 在系统时钟稳定后调用一次 / Call once after system clock becomes stable. */
void delay_init(uint32_t sysclk_freq)
{
    sys_clk = sysclk_freq;
    fac_us = sysclk_freq / 1000000;
    dwt_init();
}

/* 微秒延时 / Microsecond delay for timing-sensitive peripherals. */
void delay_us(uint32_t us)
{
    uint32_t ticks_start = DWT->CYCCNT;
    uint32_t delay_ticks = us * fac_us;

    while ((DWT->CYCCNT - ticks_start) < delay_ticks);
}

/* 毫秒延时 / Millisecond delay with chunk split to avoid overflow. */
void delay_ms(uint32_t ms)
{
    if (ms == 0U)
    {
        return;
    }

    /* 分段避免 ms*1000 溢出 / Split into chunks to avoid ms*1000 overflow. */
    while (ms > 0U)
    {
        uint32_t chunk_ms = (ms > 4000000U) ? 4000000U : ms;
        delay_us(chunk_ms * 1000U);
        ms -= chunk_ms;
    }
}

/* 纳秒延时 / Rough nanosecond delay, only suitable for short approximate waits. */
void delay_ns(uint32_t ns)
{
    uint32_t cycles = (ns * sys_clk) / 1000000000;
    if (cycles == 0) cycles = 1;

    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cycles);
}