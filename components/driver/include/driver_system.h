/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_SYSTEM_H
#define _DRIVER_SYSTEM_H

#include <stdint.h>

#include "driver_iomux.h"

struct system_osc_pll_cfg_t
{
    uint32_t bg_pd:1;
    uint32_t osc_pd:1;
    uint32_t osc_32k_pd:1;
    uint32_t clk_12m_enn:1;
    uint32_t clk_oscX2_enn:1;
    uint32_t clk_bb_enn:1;
    uint32_t clk_adc_enn:1;
    uint32_t rfpll_refclk_enn:1;
    uint32_t reserved:8;
    uint32_t irq_wkup_gate:16;
};

enum system_clk_t
{
    SYSTEM_SYS_CLK_6M,
    SYSTEM_SYS_CLK_12M,
    SYSTEM_SYS_CLK_24M,
    SYSTEM_SYS_CLK_48M,
};

struct system_clk_cfg_t
{
    uint32_t sys_clk_sel:2;
    uint32_t mdm_clk_sel:2;
    uint32_t out_clk_sel:1;
    uint32_t reserved0:3;
    uint32_t clk_out_div:8;
    uint32_t reserved1:16;
};

struct system_clk_enable_t
{
    uint32_t uart0_clk_en:1;
    uint32_t uart1_clk_en:1;
    uint32_t mm_clk_en:1;
    uint32_t trng_clk_en:1;
    uint32_t gpio_clk_en:1;
    uint32_t out_clk_en:1;
    uint32_t cdc_clk_en:1;
    uint32_t qspi_ref_clk_en:1;
    uint32_t efuse_clk_en:1;
    uint32_t reserved:23;
};

struct system_rst_t
{
    uint32_t bb_mas_rst:1;
    uint32_t bb_cry_soft_rst:1;
    uint32_t mm_soft_rst:1;
    uint32_t mm_reg_soft_rst:1;
    uint32_t auxadc_soft_rst:1;
    uint32_t cdc_soft_rst:1;
    uint32_t pdm_soft_rst:1;
    uint32_t qspi_ref_soft_rst:1;
    uint32_t trng_soft_rst:1;
    uint32_t efuse_soft_rst:1;
    uint32_t reserved:22;
};

struct system_codec_mode_t
{
    uint32_t codec_mode:1;  //0: use external codec, 1: use internal codec
    uint32_t mdm_if_sel:1;
    uint32_t fll_cfg_hw:1;
    uint32_t fll_cfg_cpu:1;
    uint32_t mdm_dyn_rrst:1;
    uint32_t mdm_dyn_trst:1;
    uint32_t reserved:26;
};

#define SYSTEM_PORT_MUX_MSK     0xF
#define SYSTEM_PORT_MUX_LEN     4

#define SYSTEM_ONKEY_MAP_EXI11_POS  CO_BIT(7)
#define SYSTEM_ONKEY_MAP_EXI10_POS  CO_BIT(6)
#define SYSTEM_ONKEY_MAP_EXI9_POS   CO_BIT(5)
#define SYSTEM_ONKEY_MAP_EXI8_POS   CO_BIT(4)
#define SYSTEM_ONKEY_MAP_EXI3_POS   CO_BIT(3)
#define SYSTEM_ONKEY_MAP_EXI2_POS   CO_BIT(2)
#define SYSTEM_ONKEY_MAP_EXI1_POS   CO_BIT(1)
#define SYSTEM_ONKEY_MAP_EXI0_POS   CO_BIT(0)

struct system_led_cntl_t
{
    uint32_t led0_en:1;
    uint32_t led1_en:1;
    uint32_t led2_en:1;
    uint32_t led3_en:1;
    uint32_t led4_en:1;
    uint32_t led0_inv:1;
    uint32_t led1_inv:1;
    uint32_t led2_inv:1;
    uint32_t led3_inv:1;
    uint32_t led4_inv:1;
    uint32_t led_clk_div:6;
    uint32_t reserved:16;
};

struct system_led_cfg_t
{
    uint32_t led_shift:24;
    uint32_t led_low_cnt:8;
};

struct system_charger_state_t
{
    uint32_t bat_qs_data:3;
    uint32_t reserved1:1;
    uint32_t bat_state:1;
    uint32_t reserved2:3;
    uint32_t charger_state:1;
    uint32_t reserved3:23;
};

struct system_keyscan_ctrl_t
{
    uint32_t int_en:1;
    uint32_t reserved:31;
};

struct system_regs_t
{
    struct system_osc_pll_cfg_t osc_pll_cfg;            //0x00
    struct system_clk_cfg_t clk_cfg;                    //0x04
    struct system_clk_enable_t clk_gate;                //0x08
    struct system_rst_t rst;                            //0x0c
    struct system_codec_mode_t codec_mode;              //0x10
    uint32_t remap_virtual_addr;                        //0x14
    uint32_t remap_length;                              //0x18
    uint32_t remap_physical_addr;                       //0x1c
    uint32_t port_pull;                                 //0x20
    uint32_t qspi_pull;                                 //0x24
    uint32_t port_mux[4];                               //0x28
    uint32_t ext_int_mux;                             //0x38
    uint32_t reserved2[7];
    struct system_charger_state_t charger_state;        //0x58
    uint32_t reserved3;
    uint32_t key_scan_value[5];                         //0x60
    struct system_keyscan_ctrl_t key_scan_ctrl;
};

extern volatile struct system_regs_t *const system_regs;

/*********************************************************************
 * @fn      system_get_pclk
 *
 * @brief   get current system clock, the value should be 12M, 24M, 48M.
 *
 * @param   None
 *
 * @return  current system clock.
 */
uint32_t system_get_pclk(void);

/*********************************************************************
 * @fn      system_set_pclk
 *
 * @brief   change current system clock, some peripheral clock settings need
 *          to be reconfig if neccessary.
 *
 * @param   clk - @ref system_clk_t
 *
 * @return  None.
 */
void system_set_pclk(uint8_t clk);

/*********************************************************************
 * @fn      system_get_pclk_config
 *
 * @brief   get current system clock configuration.
 *
 * @param   None
 *
 * @return  current system clock setting, @ref system_clk_t.
 */
uint8_t system_get_pclk_config(void);

/*********************************************************************
 * @fn      system_set_port_pull
 *
 * @brief   set pull-up of IOs which are controlled by main digital core,
 *          only effect the pull state of IOs indicated by port parameter.
 *          example usage:
 *          system_set_port_pull((GPIO_PA0 | GPIO_PA1), true)
 *
 * @param   port    - each bit represents one GPIO channel
 *          flag    - true: enable pull-up, false: disable pull-up.
 *
 * @return  None.
 */
void system_set_port_pull(uint32_t port, uint8_t pull);

/*********************************************************************
 * @fn      system_set_port_mux
 *
 * @brief   set function of IO which is controlled by main digital core,
 *          example usage:
 *          system_set_port_mux(GPIO_PORT_A, GPIO_BIT_0, PMU_PORT_MUX_KEYSCAN)
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bit     - the channel number, @ref system_port_bit_t
 *          func    - such as PORTA0_FUNC_I2C0_CLK, PORTA3_FUNC_PDM_DAT
 *
 * @return  None.
 */
void system_set_port_mux(enum system_port_t port, enum system_port_bit_t bit, uint8_t func);

/*********************************************************************
 * @fn      system_sleep_enable
 *
 * @brief   enable system enter deep sleep mode when all conditions are satisfied.
 *
 * @param   None.
 *
 * @return  None.
 */
void system_sleep_enable(void);

/*********************************************************************
 * @fn      system_sleep_disable
 *
 * @brief   disable system enter deep sleep mode.
 *
 * @param   None.
 *
 * @return  None.
 */
void system_sleep_disable(void);

/*********************************************************************
 * @fn      system_get_curr_time
 *
 * @brief   get how many milliseconds have passed after system start-up,
 *          and the value will loop back to 0 after reaching 858993456.
 *
 * @param   None.
 *
 * @return  None.
 */
uint32_t system_get_curr_time(void);

#endif // _DRIVER_IOMUX_H

