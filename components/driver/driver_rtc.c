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

#include "co_math.h"
#include "co_printf.h"

#include "sys_utils.h"
#include "driver_rtc.h"
#include "driver_plf.h"
#include "driver_pmu.h"

extern void rwip_sleep_mul_64(uint32_t *low, uint32_t *high, uint32_t mul1, uint32_t mul2);
extern uint32_t rwip_sleep_div_64(uint32_t low, uint32_t high, uint32_t div);

/*********************************************************************
 * @fn      rtc_init
 *
 * @brief   Initialize rtc module.
 *
 * @param   None
 *
 * @return  None.
 */
void rtc_init(void)
{
    ool_write(PMU_REG_RST_CTRL, ool_read(PMU_REG_RST_CTRL) | PMU_RST_RTC);
    ool_write(PMU_REG_CLK_CTRL, ool_read(PMU_REG_CLK_CTRL) | PMU_RTC_CLK_EN);
    
    /* set RTC initial value */
    ool_write32(PMU_REG_RTC_UPDATE_0, 0);
    ool_write(PMU_REG_RTC_SETTINGS, PMU_RTC_UPDATE_EN);
    co_delay_10us(7);
    ool_write(PMU_REG_RTC_SETTINGS, 0);
    
    ool_write(PMU_REG_RTC_SETTINGS, PMU_RTC_EN);
}

/*********************************************************************
 * @fn      rtc_alarm
 *
 * @brief   start running a rtc.
 *
 * @param   rtc_idx     - RTC_A or RTC_B, @ref rtc_idx_t
 *          count_ms    - timer duration with ms as unit. range [1,4294967]
 *
 * @return  None.
 */
void rtc_alarm(enum rtc_idx_t rtc_idx, uint32_t count_ms)
{
    /*
        max count_us          0xffffff87 us
        min count_us          150us

        pmu clk src: low power RC(62.5K)
        accuracy        16us
        pmu clk src: low power RC/2(31.25K)
        accuracy        32us

        pmu clk src: external crystal ( 32768 mostly )
        accuracy        3125/1024us ==> 30.52us

        pmu clk src: 32K
        accuracy        31.25us

        pmu clk src: external crystal ( 16384 mostly )
        accuracy        3125/512us ==> 61.04us
    */

    uint32_t tmp_high, tmp_low;
    uint32_t tmp, cnt;
    
    rwip_sleep_mul_64(&tmp_low, &tmp_high, pmu_get_rc_clk(false), count_ms);
    cnt = rwip_sleep_div_64(tmp_low, tmp_high, 1000);

    tmp = ool_read32(PMU_REG_RTC_VALUE_0);
    if(rtc_idx == RTC_A)
        ool_write32(PMU_REG_RTC_ALMA_VALUE_0, cnt + tmp);
    else
        ool_write32(PMU_REG_RTC_ALMB_VALUE_0, cnt + tmp);
    
    ool_write(PMU_REG_RTC_SETTINGS, ool_read(PMU_REG_RTC_SETTINGS)|CO_BIT(rtc_idx+2)|CO_BIT(rtc_idx+4));
}

/*********************************************************************
 * @fn      rtc_disalarm
 *
 * @brief   stop a running rtc.
 *
 * @param   rtc_idx     - RTC_A or RTC_B, @ref rtc_idx_t
 *
 * @return  None.
 */
void rtc_disalarm(enum rtc_idx_t rtc_idx)
{
    ool_write(PMU_REG_RTC_SETTINGS, ool_read(PMU_REG_RTC_SETTINGS) & (~ CO_BIT(rtc_idx+2) ) );
}

__attribute__((weak)) __attribute__((section("ram_code"))) void rtc_isr_ram(uint8_t rtc_idx)
{
    if(rtc_idx == RTC_A)
    {

        ;
    }
    if(rtc_idx == RTC_B)
    {
        ;
    }
}

