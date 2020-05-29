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

#include "co_log.h"

#include "sys_utils.h"
#include "driver_wdt.h"

/*
 * MACROS 
 */
#undef LOG_LEVEL_MODULE
#define LOG_LEVEL_MODULE        LOG_LEVEL_ERROR

/*********************************************************************
 * @fn      wdt_init
 *
 * @brief   init watchdog before enable this mudule.
 *
 * @param   action      - the next action after watchdog timer expired.
 *          delay_s     - how many seconds does the timer set
 *
 * @return  None.
 */
void wdt_init(enum wdt_action action, uint8_t delay_s)
{
    uint32_t rc_clk = pmu_get_rc_clk(false);
    
    ool_write32(PMU_REG_WTD_LEN_0, delay_s * rc_clk );

    if(action == WDT_ACT_RST_CHIP)
    {
        ool_write(PMU_REG_RST_CTRL, ool_read(PMU_REG_RST_CTRL) & (~ PMU_RST_WDT_EN) );
        ool_write(PMU_REG_WTD_CTRL, ool_read(PMU_REG_WTD_CTRL) & (~ PMU_WTD_IRQ_EN) );
    }
    else
    {
        ool_write(PMU_REG_RST_CTRL, ool_read(PMU_REG_RST_CTRL) | PMU_RST_WDT_EN );
        ool_write(PMU_REG_WTD_CTRL, ool_read(PMU_REG_WTD_CTRL) | PMU_WTD_IRQ_EN );
    }

    LOG_INFO("wdt_t:%d,%d,%d\r\n", delay_s * rc_clk, rc_clk, ool_read32(PMU_REG_WTD_LEN_0));
    LOG_INFO("en:%x,%x\r\n", ool_read(PMU_REG_WTD_CTRL), ool_read(PMU_REG_RST_CTRL));
}

/*********************************************************************
 * @fn      wdt_feed
 *
 * @brief   feed the watchdog.
 *
 * @param   None.
 *
 * @return  None.
 */
void wdt_feed(void)
{
    pmu_clear_isr_state(PMU_ISR_WDT_CLR);
}

/*********************************************************************
 * @fn      wdt_start
 *
 * @brief   start the watchdog after init this mudle.
 *
 * @param   None.
 *
 * @return  None.
 */
void wdt_start(void)
{
    wdt_feed();
    LOG_INFO("wdt_ctl1:%x\r\n", ool_read(PMU_REG_WTD_CTRL));
    ool_write(PMU_REG_WTD_CTRL, ool_read(PMU_REG_WTD_CTRL) | PMU_WTD_EN );
    LOG_INFO("wdt_ctl2:%x\r\n", ool_read(PMU_REG_WTD_CTRL));
}

/*********************************************************************
 * @fn      wdt_stop
 *
 * @brief   stop the watchdog.
 *
 * @param   None.
 *
 * @return  None.
 */
void wdt_stop(void)
{
    ool_write(PMU_REG_WTD_CTRL, ool_read(PMU_REG_WTD_CTRL) & (~ PMU_WTD_EN) );
    LOG_INFO("wdt_ctl3:%x\r\n", ool_read(PMU_REG_WTD_CTRL));
}

/*********************************************************************
 * @fn      wdt_isr_ram
 *
 * @brief   this is an example of watchdog interrupt handler. the actions
 *          are dump current registers and reset the system.
 *
 * @param   current stack pointer.
 *
 * @return  None.
 */
__attribute__((weak)) __attribute__((section("ram_code"))) void wdt_isr_ram(unsigned int* hardfault_args)
{
    co_printf("wdt_rest\r\n\r\n");
    co_printf("PC    = 0x%08X\r\n",hardfault_args[6]);
    co_printf("LR    = 0x%08X\r\n",hardfault_args[5]);
    co_printf("R0    = 0x%08X\r\n",hardfault_args[0]);
    co_printf("R1    = 0x%08X\r\n",hardfault_args[1]);
    co_printf("R2    = 0x%08X\r\n",hardfault_args[2]);
    co_printf("R3    = 0x%08X\r\n",hardfault_args[3]);
    co_printf("R12   = 0x%08X\r\n",hardfault_args[4]);
    /* reset the system */
    wdt_init(WDT_ACT_RST_CHIP,1);
    ool_write(PMU_REG_WTD_CTRL, ool_read(PMU_REG_WTD_CTRL) | PMU_WTD_EN );
    while(1);
}

void wdt_test(void)
{
    wdt_init(WDT_ACT_CALL_IRQ, 4);
    wdt_start();
    co_delay_100us(10000);
    wdt_stop();
}

