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

#include "co_printf.h"

#include "driver_pmu_qdec.h"
#include "driver_system.h"
#include "driver_pmu.h"

/*
    func        init pmu_qdec function,main job is enalbe edge_check anti-shake function
*/
void pmu_qdec_init(void)
{
    ool_write(PMU_REG_QDEC_CTRL, ool_read(PMU_REG_QDEC_CTRL)|PMU_QDEC_DEB_LA_EN|PMU_QDEC_DEB_LB_EN|PMU_QDEC_DEB_LC_EN);
}

/*
    func        set pmu_qdec edge_check anti-shake time.
    cnt         0x0 ~ 0xff
                actual anti-shake time = (debounce period) * (2+cnt)
*/
void pmu_qdec_set_debounce_cnt(uint8_t cnt)
{
    /*
        debounce period = 0.24us , see pmu_set_debounce_clk()
        qdec anti-shake time = (debounce period) * (2+cnt)
        max:0xff
        min:0
    */
    ool_write(PMU_REG_QDEC_DEB_LEN, cnt);
}

/*
    func        set flag to reset rotation cnt.
    flag_read_rest
                    - true      after read rotation cnt,will clear cnt.
                    - false     after read rotation cnt,will not clear cnt.
    flag_lc_rest
                    - true      when lc pin is low voltage ,will keep reseting cnt.
                    - false     no matter what lc pin voltage is ,will not reseting cnt.
*/
void pmu_qdec_autorest_cnt_flag(bool flag_read_rest,bool flag_lc_rest)
{
    ool_write(PMU_REG_LVD_BAT_DEB_CFG, (ool_read(PMU_REG_LVD_BAT_DEB_CFG) & (~ PMU_QDEC_AUTO_RST)) | flag_read_rest ); //auto reset
    ool_write(PMU_REG_QDEC_CTRL, (ool_read(PMU_REG_QDEC_CTRL) & (~ PMU_QDEC_LC_RST_CNT_EN)) | (flag_lc_rest<<7) );
    //ool_write(PMU_REG_QDEC_CTRL, ool_read(PMU_REG_QDEC_CTRL) | PMU_QDEC_LC_RST_CNT_EN  );
}
/*
    func        set pmu_qdec pin.
    pin_a       fill PMU_QDEC_LA_PIN_PA0 ~ PMU_QDEC_LA_PIN_PD4
    pin_b       fill PMU_QDEC_LB_PIN_PA1 ~ PMU_QDEC_LA_PIN_PD5
    pin_c       fill PMU_QDEC_LC_RESV ~ PMU_QDEC_LC_PIN_PD7
*/
void pmu_qdec_set_pin(enum pmu_qdec_la_pin_t pin_a,enum pmu_qdec_lb_pin_t pin_b,enum pmu_qdec_lc_pin_t pin_c)
{
    ool_write(PMU_REG_QDEC_PIN_MUX, (pin_a<<PMU_QDEC_LA_PIN_MUX_POS) | (pin_b<<PMU_QDEC_LB_PIN_MUX_POS));
    ool_write(PMU_REG_QDEC_LC_PIN_MUX, pin_c);
}

/*
    func        set threshold cnt value for MULTI-step mode.
    threshold   0~0xff.  value of threshold.
*/
void pmu_qdec_set_threshold(uint8_t threshold)
{
    ool_write(PMU_REG_QDEC_MULTI_LEN,threshold);
}
/*
    func        set pmu_qdec irq type.
    irq_type
                - PMU_ISR_QDEC_SINGLE_EN    if rotation cnt > 0, generate irq, SINGLE-step mode.
                - PMU_ISR_QDEC_MULTI_EN     if rotation cnt > threshold, generate irq, MULTI-step mode.  refer to pmu_qdec_set_threshold().
*/
void pmu_qdec_set_irq_type(enum pmu_qdec_irq_type irq_type)    //PMU_ISR_QDEC_SINGLE_EN
{
    ool_write(PMU_REG_QDEC_CTRL, (ool_read(PMU_REG_QDEC_CTRL) & (~ PMU_ISR_QDEC_MASK))| irq_type);
    co_printf("QDEC_CTRL:%x\r\n",ool_read(PMU_REG_QDEC_CTRL));
}

/*
    func        get rotation cnt at a certain direction.
    dir         enum pmu_qdec_direction
                - DIR_A    get rotation cnt at direction A
                - DIR_B    get rotation cnt at direction B
*/
uint8_t pmu_qdec_get_cnt(enum pmu_qdec_direction dir)
{
    if(dir == QDEC_DIR_A)
        return (ool_read(PMU_REG_QDEC_CNTA_VALUE));
    else
        return (ool_read(PMU_REG_QDEC_CNTB_VALUE));
}



/*
    weak func    pmu_qdec isr entry
*/
__attribute__((weak)) __attribute__((section("ram_code"))) void qdec_isr_ram(void)
{
    co_printf("%x,%x\r\n",pmu_qdec_get_cnt(QDEC_DIR_A),pmu_qdec_get_cnt(QDEC_DIR_B));
}

