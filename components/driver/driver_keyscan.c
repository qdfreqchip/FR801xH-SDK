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
#include "co_math.h"

#include "driver_keyscan.h"
#include "driver_pmu.h"

/*
 * MACROS
 */
#undef LOG_LEVEL_MODULE
#define LOG_LEVEL_MODULE        LOG_LEVEL_NONE

/*
 *                                                KEYBOARD PIN-VALUE MAPPING
 *
 *            PA0   PA1   PA2   PA3   PA4   PA5   PA6   PA7   PB0   PB1   PB2   PB3   PB4   PB5   PB6   PB7   PC0   PC1   PC2   PC3
 *  PD7/PC7   4/27  4/19  4/11  4/03  3/27  3/19  3/11  3/03  2/27  2/19  2/11  2/03  1/27  1/19  1/11  1/03  0/27  0/19  0/11  0/03
 *  PD6/PC6   4/26  4/18  4/10  4/02  3/26  3/18  3/10  3/02  2/26  2/18  2/10  2/02  1/26  1/18  1/10  1/02  0/26  0/18  0/10  0/02
 *  PD5/PC5   4/25  4/17  4/09  4/01  3/25  3/17  3/09  3/01  2/25  2/17  2/09  2/01  1/25  1/17  1/09  1/01  0/25  0/17  0/09  0/01
 *  PD4/PC4   4/24  4/16  4/08  4/00  3/24  3/16  3/08  3/00  2/24  2/16  2/08  2/00  1/24  1/16  1/08  1/00  0/24  0/16  0/08  0/00
 *  PD3       4/31  4/23  4/15  4/07  3/31  3/23  3/15  3/07  2/31  2/23  2/15  2/07  1/31  1/23  1/15  1/07  0/31  0/23  0/15  0/07
 *  PD2       4/30  4/22  4/14  4/06  3/30  3/22  3/14  3/06  2/30  2/22  2/14  2/06  1/30  1/22  1/14  1/06  0/30  0/22  0/14  0/06
 *  PD1       4/29  4/21  4/13  4/05  3/29  3/21  3/13  3/05  2/29  2/21  2/13  2/05  1/29  1/21  1/13  1/05  0/29  0/21  0/13  0/05
 *  PD0       4/28  4/20  4/12  4/04  3/28  3/20  3/12  3/04  2/28  2/20  2/12  2/04  1/28  1/20  1/12  1/04  0/28  0/20  0/12  0/04
 *
 *  value = Y/XX, Y means KEYSCAN_STATUS group, XX means bit valule in single gourp.
 */

/*********************************************************************
 * @fn      keyscan_init
 *
 * @brief   initial and start keyscan module with row and column selection
 *          indicated in parameter. The interrupt is enabled at the end
 *          of this function, and it will be triggered once one key is
 *          pressed or released.
 *
 * @param   param       - row and column selection
 *
 * @return  None.
 */
void keyscan_init(keyscan_param_t *param)
{
    uint8_t tmp_bits = 0;

    param->col_en &= (~(1<<8));
    
    /* release keyscan reset and enable clock for keyscan */
    ool_write(PMU_REG_RST_CTRL, ool_read(PMU_REG_RST_CTRL) | PMU_RST_KEYSCAN);
    ool_write(PMU_REG_CLK_CTRL, ool_read(PMU_REG_CLK_CTRL) | PMU_KEYSCAN_CLK_EN);

    /* clear all keyscan register first */
    ool_write32(PMU_REG_KEYSCAN_CTRL, 0);
    ool_write16(PMU_REG_KEYSCAN_COL_SEL_2, 0);

    /* setup pin configuration for row */
    for(uint8_t i = 0; i<8; i++)
    {
        if(i<4)
        {
            if(param->row_en & CO_BIT(i))
            {
                pmu_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)i, PMU_PORT_MUX_KEYSCAN);
                pmu_set_pin_to_PMU(GPIO_PORT_D, CO_BIT(i));
                tmp_bits |= CO_BIT(i+4);
                LOG_INFO("ROW-PD[%d]\r\n", i);
            }
        }
        else
        {
            if(param->row_map_sel)
            {
                if(param->row_en & CO_BIT(i))
                {
                    pmu_set_port_mux(GPIO_PORT_C, (enum system_port_bit_t)i, PMU_PORT_MUX_KEYSCAN);
                    pmu_set_pin_to_PMU(GPIO_PORT_C, CO_BIT(i));
                    tmp_bits |= CO_BIT(i-4);
                    LOG_INFO("ROW-PC[%d]\r\n", i);
                }
            }
            else
            {
                if(param->row_en & CO_BIT(i))
                {
                    pmu_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)i, PMU_PORT_MUX_KEYSCAN);
                    pmu_set_pin_to_PMU(GPIO_PORT_D, CO_BIT(i));
                    tmp_bits |= CO_BIT(i-4);
                    LOG_INFO("ROW-PD[%d]\r\n", i);
                }
            }
        }
    }
    ool_write(PMU_REG_KEYSCAN_ROW_SEL, tmp_bits);

    /* setup pin configuration for column[7:0] */
    tmp_bits = 0;
    for(uint8_t i = 0; i<8; i++)
    {
        if(param->col_en & CO_BIT(i))
        {
            pmu_set_port_mux(GPIO_PORT_A, (enum system_port_bit_t)i, PMU_PORT_MUX_KEYSCAN);
            pmu_set_pin_to_PMU(GPIO_PORT_A, CO_BIT(i));
            tmp_bits |= CO_BIT(i);
            LOG_INFO("COL-PA[%d]\r\n", i);
        }
    }
    ool_write(PMU_REG_KEYSCAN_COL_SEL_1, tmp_bits);

    /* setup pin configuration for column[15:8] */
    tmp_bits = 0;
    for(uint8_t i = 0; i<8; i++)
    {
        if(param->col_en & CO_BIT(i+8))
        {
            pmu_set_port_mux(GPIO_PORT_B, (enum system_port_bit_t)i, PMU_PORT_MUX_KEYSCAN);
            pmu_set_pin_to_PMU(GPIO_PORT_B, CO_BIT(i));
            tmp_bits |= CO_BIT(i);
            LOG_INFO("COL-PB[%d]\r\n", i);
        }
    }
    ool_write(PMU_REG_KEYSCAN_COL_SEL_2, tmp_bits);

    /* setup pin configuration for column[19:16] */
    tmp_bits = 0;
    for(uint8_t i = 0; i<4; i++)
    {
        if(param->col_en & CO_BIT(i+16))
        {
            pmu_set_port_mux(GPIO_PORT_C, (enum system_port_bit_t)i, PMU_PORT_MUX_KEYSCAN);
            pmu_set_pin_to_PMU(GPIO_PORT_C, CO_BIT(i));
            tmp_bits |= CO_BIT(i);
            LOG_INFO("COL-PC[%d]\r\n",i);
        }
    }
    ool_write(PMU_REG_KEYSCAN_COL_SEL_3, (ool_read(PMU_REG_KEYSCAN_COL_SEL_3) & 0x0f) | (tmp_bits<<4) );

    if(param->row_map_sel)
        ool_write(PMU_REG_KEYSCAN_CTRL, ool_read(PMU_REG_KEYSCAN_CTRL) | PMU_KEYSCAN_MAP);
    else
        ool_write(PMU_REG_KEYSCAN_CTRL, ool_read(PMU_REG_KEYSCAN_CTRL) & (~ PMU_KEYSCAN_MAP) );

    /* interval between two continuous scanning operation,  unit: RC clock/4 */
    /* actual interval is scan_interval * 4 * RC period + 1.9ms   = 80*38us + 1.9ms = 4940ms  */
    ool_write(PMU_REG_KEYSCAN_INTERVAL, 20);
    
    /* settings for anti-shake and ghost key */
    ool_write(PMU_REG_KEYSCAN_GLICTH_CFG, 0xf2);    //anti-shake tim = 9880ms

    /* enable row pull up */
    ool_write(PMU_REG_PORTD_PUL, ool_read(PMU_REG_PORTD_PUL) & (~param->row_en));

    /* enable keyscan module and interrupt */
    ool_write(PMU_REG_KEYSCAN_CTRL, ool_read(PMU_REG_KEYSCAN_CTRL) | PMU_KEYSCAN_EN | PMU_KEYSCAN_IRQ_EN | PMU_KEYSCAN_LP_EN );
}

/*********************************************************************
 * @fn      keyscan_isr_ram
 *
 * @brief   This is a sample of keyscan isr handler. The interrupt will
 *          be generated once a key is pressed or released.
 *
 * @param   None
 *
 * @return  None.
 */
__attribute__((weak)) __attribute__((section("ram_code"))) void keyscan_isr_ram(void)
{
    uint32_t value;
    uint8_t reg = PMU_REG_KEYSCAN_STATUS_0;
    
    for(uint8_t j=0; j<5; j++)
    {
        value = ool_read32(reg + (j<<2));
        if(value)
        {
            LOG_INFO("grp[%d]:0x%08x.\r\n", j, value);
        }
    }
}

void keyscan_test(void)
{
    keyscan_param_t param;
    
    param.row_en = 0xff;
    param.col_en = 0xfffff;
    param.row_map_sel = 0x0;
    
    keyscan_init(&param);
}

