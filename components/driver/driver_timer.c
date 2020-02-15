/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>

#include "co_printf.h"

#include "driver_system.h"
#include "driver_timer.h"

/*
 * MACROS
 */

/*
 * CONSTANTS
 */

/*
 * TYPEDEFS
 */
struct timer_lvr_t
{
    uint32_t load: 16;
    uint32_t unused: 16;
};

struct timer_cvr_t
{
    uint32_t count: 16;
    uint32_t unused: 16;
};

struct timer_cr_t
{
    uint32_t reserved1: 2;
    uint32_t pselect: 2;
    uint32_t reserved2: 2;
    uint32_t count_mode: 1;
    uint32_t count_enable: 1;
    uint32_t unused: 24;
};

struct timer_icr_t
{
    uint32_t data: 16;
    uint32_t unused: 16;
};

struct timer
{
    struct timer_lvr_t load_value;
    struct timer_cvr_t count_value;
    struct timer_cr_t control;
    struct timer_icr_t interrupt_clear;
};

/*
 * GLOBAL VARIABLES
 */

/*
 * LOCAL VARIABLES
 */

/*
 * LOCAL FUNCTIONS
 */

/*
 * EXTERN FUNCTIONS
 */

/*
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      timer_calc_prescale
 *
 * @brief   used to get a befitting prescale.
 *
 * @param   count_us    - timer period with us as unit.
 *
 * @return  prescale.
 */
static uint8_t timer_calc_prescale(uint32_t count_us)
{
    /*
        prescale: 1
        sys_clk         6M      12M     24M     48M
        accuracy        1/6us   1/12us  1/24us  1/48us
        max period      10922us 5461us  2730us  1365us

        prescale: 16
        sys_clk         6M      12M     24M     48M
        accuracy        8/3us   4/3us   2/3us   1/3us
        max period      174.7ms 87.3ms  43.6ms  21.8ms

        prescale: 256
        sys_clk         6M      12M     24M     48M
        accuracy        128/3us 64/3us  32/3us  16/3us
        max period      2796ms  1398ms  699ms   349ms
    */

    const uint32_t timer_max_period[][TIMER_PRESCALE_MAX] =
    {
        1365, 21800, 349000,    // 48M
        2730, 43600, 699000,    // 24M
        5461, 87300, 1398000,   // 12M
        10922, 174700, 2796000
    };// 6M
    uint8_t sys_clk_cfg = system_get_pclk_config();

    if(sys_clk_cfg == 6)
    {
        sys_clk_cfg = 3;
    }
    else if(sys_clk_cfg == 12)
    {
        sys_clk_cfg = 2;
    }
    else if(sys_clk_cfg == 24)
    {
        sys_clk_cfg = 1;
    }
    else
    {
        sys_clk_cfg = 0;
    }

    if(timer_max_period[sys_clk_cfg][TIMER_PRESCALE_256] < count_us)
    {
        return TIMER_PRESCALE_MAX;
    }
    else if(timer_max_period[sys_clk_cfg][TIMER_PRESCALE_16] < count_us)
    {
        return TIMER_PRESCALE_256;
    }
    else if(timer_max_period[sys_clk_cfg][TIMER_PRESCALE_1] < count_us)
    {
        return TIMER_PRESCALE_16;
    }
    else
    {
        return TIMER_PRESCALE_1;
    }
}

/*********************************************************************
 * @fn      timer_run
 *
 * @brief   start a pending timer.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_run(uint32_t timer_addr)
{
    volatile struct timer *timerp = (volatile struct timer *)timer_addr;

    timerp->control.count_enable = 1;
}

/*********************************************************************
 * @fn      timer_stop
 *
 * @brief   stop a running timer.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_stop(uint32_t timer_addr)
{
    volatile struct timer *timerp = (volatile struct timer *)timer_addr;

    timerp->control.count_enable = 0;
}

/*********************************************************************
 * @fn      timer_reload
 *
 * @brief   reset initial count value with load value.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_reload(uint32_t timer_addr)
{
    volatile struct timer *timerp = (volatile struct timer *)timer_addr;

    timerp->load_value.load = timerp->load_value.load;
}

/*********************************************************************
 * @fn      timer_clear_interrupt
 *
 * @brief   clear timer interrupt.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_clear_interrupt(uint32_t timer_addr)
{
    volatile struct timer *timerp = (volatile struct timer *)timer_addr;

    timerp->interrupt_clear.data = 0x01;
}

/*********************************************************************
 * @fn      timer_get_current_value
 *
 * @brief   get current count value.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  current count value.
 */
uint32_t timer_get_current_value(uint32_t timer_addr)
{
    volatile struct timer *timerp = (volatile struct timer *)timer_addr;

    return timerp->count_value.count;
}

/*********************************************************************
 * @fn      timer_get_load_value
 *
 * @brief   get current load value.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  current load value.
 */
uint32_t timer_get_load_value(uint32_t timer_addr)
{
    volatile struct timer *timerp = (volatile struct timer *)timer_addr;

    return timerp->load_value.load;
}

/*********************************************************************
 * @fn      timer_init
 *
 * @brief   Initialize timer instance.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *          count_us    - timer period with us as unit
 *          run_mode    - single mode or period mode(@see enum timer_run_type)
 *
 * @return  None.
 */
uint8_t timer_init(uint32_t timer_addr, uint32_t count_us, uint8_t run_mode)
{
    uint8_t sysclk;
    uint16_t count;
    uint8_t prescale;

    if(count_us < 100)
    {
        return false;
    }

    prescale = timer_calc_prescale(count_us);
    if(prescale == TIMER_PRESCALE_MAX)
    {
        return false;
    }

    sysclk = system_get_pclk_config();
    if(prescale == TIMER_PRESCALE_1)
    {
        count = count_us*sysclk;
    }
    else if(prescale == TIMER_PRESCALE_16)
    {
        count = (count_us * sysclk) >> 4;
    }
    else if(prescale == TIMER_PRESCALE_256)
    {
        count = (count_us * sysclk) >> 8;
    }

    volatile struct timer *timerp = (volatile struct timer *)timer_addr;

    timerp->control.count_enable = 0;
    timerp->control.count_mode = run_mode;
    timerp->control.pselect = prescale;
    timerp->interrupt_clear.data = 0x01;
    timerp->load_value.load = count;

    return true;
}

__attribute__((weak)) __attribute__((section("ram_code"))) void timer0_isr_ram(void)
{
    timer_clear_interrupt(TIMER0);

    co_printf("0\r\n");
}

__attribute__((weak)) __attribute__((section("ram_code"))) void timer1_isr_ram(void)
{
    timer_clear_interrupt(TIMER1);

    co_printf("1\r\n");
}

