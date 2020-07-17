/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_PMU_H
#define _DRIVER_PMU_H

/*
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>

#include "co_math.h"

#include "driver_iomux.h"
#include "driver_pmu_regs.h"
#include "driver_frspim.h"

/*
 * MACROS
 */
#define ool_write(addr, data)       frspim_wr(FR_SPI_PMU_CHAN,(addr),1, (data))
#define ool_read(addr)              (uint8_t)frspim_rd(FR_SPI_PMU_CHAN,(addr),1)

#define ool_write16(addr,data)      frspim_wr(FR_SPI_PMU_CHAN,(addr),2, (data))
#define ool_read16(addr)            (uint16_t)frspim_rd(FR_SPI_PMU_CHAN,(addr),2)

#define ool_write32(addr,data)      frspim_wr(FR_SPI_PMU_CHAN,(addr),4, (data))
#define ool_read32(addr)            (uint32_t)frspim_rd(FR_SPI_PMU_CHAN,(addr),4)

/*
 * CONSTANTS
 */
#define PMU_ALDO_OUT_3_3            0xe0
#define PMU_ALDO_OUT_2_9            0xc0
#define PMU_ALDO_OUT_2_5            0xa0
#define PMU_ALDO_OUT_1_8            0x30

/*
 * TYPEDEFS
 */
enum pmu_sys_pow_mode_t
{
    PMU_SYS_POW_BUCK,
    PMU_SYS_POW_LDO,
};

enum pmu_aldo_work_mode_t
{
    PMU_ALDO_MODE_BYPASS,   // aldo bypass to vbat
    PMU_ALDO_MODE_NORMAL,
};

enum pmu_aldo_voltage_t
{
    PMU_ALDO_VOL_3_5 = 0xf0,
    PMU_ALDO_VOL_3_3 = 0xe0,
    PMU_ALDO_VOL_3_1 = 0xd0,
    PMU_ALDO_VOL_2_9 = 0xc0,
    PMU_ALDO_VOL_2_7 = 0xb0,
    PMU_ALDO_VOL_2_5 = 0xa0,
    PMU_ALDO_VOL_2_3 = 0x90,
    PMU_ALDO_VOL_2_1 = 0x80,
};

enum pmu_lp_clk_src_t
{
    PMU_LP_CLK_SRC_EX_32768,
    PMU_LP_CLK_SRC_IN_RC,
};

enum pmu_isr_enable_t
{
    PMU_ISR_BIT_BAT =            CO_BIT(0),
    PMU_ISR_BIT_ULV_PO =         CO_BIT(1),
    PMU_ISR_BIT_LVD =            CO_BIT(2),
    PMU_ISR_BIT_OTP =            CO_BIT(3),
    PMU_ISR_BIT_ACOK =           CO_BIT(4),
    PMU_ISR_BIT_CALI =           CO_BIT(5),
    PMU_ISR_BIT_ACOFF =          CO_BIT(6),
    PMU_ISR_BIT_ONKEY_PO =       CO_BIT(7),
    PMU_ISR_BIT_ONKEY_LOW =      CO_BIT(8),
    PMU_ISR_BIT_ONKEY_HIGH =     CO_BIT(9),
    PMU_ISR_BIT_ADKEY0_LOW =     CO_BIT(10),
    PMU_ISR_BIT_ADKEY0_HIGH =    CO_BIT(11),
    PMU_ISR_BIT_ADKEY1_LOW =     CO_BIT(12),
    PMU_ISR_BIT_ADKEY1_HIGH =    CO_BIT(13),
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
 * @fn      pmu_set_pin_to_PMU
 *
 * @brief   Hand over the IO control from main digital core to PMU (always on),
 *          this function can be used to set more than one IO belong
 *          to the same port.
 *          example usage: pmu_set_pin_to_PMU(GPIO_PORT_A, (1<<GPIO_BIT0)|((1<<GPIO_BIT1))
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bits    - io channel number
 *
 * @return  None.
 */
void pmu_set_pin_to_PMU(enum system_port_t port, uint8_t bits);

/*********************************************************************
 * @fn      pmu_set_pin_to_CPU
 *
 * @brief   Hand over the IO control from PMU to main digital core, this function
 *          can be used to set more than one IO belong to the same port.
 *          example usage: pmu_set_pin_to_CPU(GPIO_PORT_A, (1<<GPIO_BIT0)|((1<<GPIO_BIT1))
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bits    - the numbers of io
 *
 * @return  None.
 */
void pmu_set_pin_to_CPU(enum system_port_t port, uint8_t bits);

/*********************************************************************
 * @fn      pmu_set_port_mux
 *
 * @brief   used to set the function of IO controlled by PMU.
 *          example usage:
 *          pmu_set_port_mux(GPIO_PORT_A, GPIO_BIT_0, PMU_PORT_MUX_KEYSCAN)
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bit     - the channel number, @ref system_port_bit_t
 *          func    - the function of gpio, @ref pmu_gpio_mux_t
 *
 * @return  None.
 */
void pmu_set_port_mux(enum system_port_t port, enum system_port_bit_t bit, enum pmu_gpio_mux_t func);

/*********************************************************************
 * @fn      pmu_set_pin_dir
 *
 * @brief   set the in-out of IOs which are controlled by PMU.
 *          example usage:
 *          pmu_set_pin_dir(GPIO_PORT_A, (1<<GPIO_BIT0)|((1<<GPIO_BIT1), GPIO_DIR_OUT)
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bits    - the numbers of io
 *          dir     - the direction of in-out, GPIO_DIR_OUT or GPIO_DIR_IN
 *
 * @return  None.
 */
void pmu_set_pin_dir(enum system_port_t port, uint8_t bits, uint8_t dir);

/*********************************************************************
 * @fn      pmu_set_pin_pull
 *
 * @brief   set pull-up of IOs which are controlled by PMU.
 *          example usage:
 *          pmu_set_pin_pull(GPIO_PORT_A, (1<<GPIO_BIT0)|((1<<GPIO_BIT1), true)
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bits    - the numbers of io
 *          flag    - true: enable pull-up, false: disable pull-up.
 *
 * @return  None.
 */
void pmu_set_pin_pull(enum system_port_t port, uint8_t bits, bool flag);

/*********************************************************************
 * @fn      pmu_set_gpio_value
 *
 * @brief   set value of IOs which are controlled by PMU.
 *          example usage:
 *          pmu_set_gpio_value(GPIO_PORT_A, (1<<GPIO_BIT0)|((1<<GPIO_BIT1), 1)
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bits    - the numbers of io
 *          value   - 1: set the IO to high, 0: set the IO to low.
 *
 * @return  None.
 */
void pmu_set_gpio_value(enum system_port_t port, uint8_t bits, uint8_t value);

/*********************************************************************
 * @fn      pmu_get_gpio_value
 *
 * @brief   get value of IO which are controlled by PMU and in GPIO mode.
 *          example usage:
 *          pmu_get_gpio_value(GPIO_PORT_A, GPIO_BIT_0)
 *
 * @param   port    - which group the io belongs to, @ref system_port_t
 *          bit     - the number of io
 *
 * @return  1: the IO is high, 0: the IO is low..
 */
uint8_t pmu_get_gpio_value(enum system_port_t port, uint8_t bit);

/*********************************************************************
 * @fn      pmu_set_sys_power_mode
 *
 * @brief   used to set system power supply mode, BUCK or LDO
 *
 * @param   mode    - indicate the mode to be used, @ref pmu_sys_pow_mode_t
 *
 * @return  None.
 */
void pmu_set_sys_power_mode(enum pmu_sys_pow_mode_t mode);

/*********************************************************************
 * @fn      pmu_first_power_on
 *
 * @brief   used to check whether current boot-up is the first time power
 *          on or not.
 *
 * @param   clear   - clear the first power on label (true) or not (false).
 *
 * @return  first power on (true) or not (false).
 */
uint8_t pmu_first_power_on(uint8_t clear);

/*********************************************************************
 * @fn      pmu_sub_init
 *
 * @brief   pmu init procedure, sleep control, etc.
 *
 * @param   None
 *
 * @return  None.
 */
void pmu_sub_init(void);

/*********************************************************************
 * @fn      pmu_enable_irq
 *
 * @brief   enable the irq of modules inside PMU.
 *
 * @param   irqs    -- indicate which irq should be enable
 *
 * @return  None.
 */
void pmu_enable_irq(uint16_t irqs);

/*********************************************************************
 * @fn      pmu_disable_irq
 *
 * @brief   disable the irq of modules inside PMU.
 *
 * @param   irqs    -- indicate which irq should be disabled
 *
 * @return  None.
 */
void pmu_disable_irq(uint16_t irqs);

/*********************************************************************
 * @fn      pmu_clear_isr_state
 *
 * @brief   clear PMU interrupt.
 *
 * @param   state_map   - indicate which irq should be clear
 *
 * @return  None.
 */
void pmu_clear_isr_state(uint16_t state_map);

/*********************************************************************
 * @fn      pmu_codec_power_enable
 *
 * @brief   enable codec power supply.
 *
 * @param   None.
 *
 * @return  None.
 */
void pmu_codec_power_enable(void);

/*********************************************************************
 * @fn      pmu_codec_power_disable
 *
 * @brief   remove codec power supply.
 *
 * @param   None.
 *
 * @return  None.
 */
void pmu_codec_power_disable(void);

/*********************************************************************
 * @fn      pmu_set_io_voltage
 *
 * @brief   set the aldo output voltage, also known as IO voltage.
 *
 * @param   mode    - bypass to VBAT or not.
 *          value   - voltage target value, @ref pmu_aldo_voltage_t.
 *
 * @return  None.
 */
void pmu_set_aldo_voltage(enum pmu_aldo_work_mode_t mode, enum pmu_aldo_voltage_t value);

/*********************************************************************
 * @fn      pmu_set_lp_clk_src
 *
 * @brief   select low power clock source.
 *
 * @param   src - internal RC or external 32768, @ref pmu_lp_clk_src_t
 *
 * @return  None.
 */
void pmu_set_lp_clk_src(enum pmu_lp_clk_src_t src);

/*********************************************************************
 * @fn      pmu_port_wakeup_func_set
 *
 * @brief   indicate which ports should be checked by PMU GPIO monitor module.
 *          once the state of corresponding GPIO changes, an PMU interrupt
 *          will be generated.
 *
 * @param   gpios   - 32bit value, bit num corresponding to pin num.
 *                    sample: 0x08080808 means PA3, PB3, PC3, PD3 will be
 *                    checked.
 *
 * @return  None.
 */
void pmu_port_wakeup_func_set(uint32_t gpios);

/*********************************************************************
 * @fn      pmu_port_wakeup_func_clear
 *
 * @brief   indicate which ports should be removed detection by PMU GPIO
 *          monitor module.
 *
 * @param   gpios   - 32bit value, bit num corresponding to pin num.
 *                    sample: 0x08080808 means PA3, PB3, PC3, PD3 will be
 *                    checked.
 *
 * @return  None.
 */
void pmu_port_wakeup_func_clear(uint32_t gpios);

/*********************************************************************
 * @fn      pmu_get_rc_clk
 *
 * @brief   get inner rc clk frequecy
 *
 * @param   redo   - true, do rc clk caculation,then get result as rc clk frequecy.
 *                 - false,do nothing, get last caculated rc clk frequecy.
 *
 * @return  -   rc clk frequecy.  unit: Hz.
 */
uint32_t pmu_get_rc_clk(uint8_t redo);

/*********************************************************************
 * @fn      pmu_calibration_start
 *
 * @brief   start rc clk frequecy calibration.  USER DON'T CALL IT.
 *
 * @param   type    - which rc source will be calibrated. see @enum pmu_cali_src_sel_t
 *          counter - how may rc clk period will be cost util calibration end.
 *
 * @return  None.
 */
void pmu_calibration_start(uint8_t type, uint16_t counter);

/*********************************************************************
 * @fn      pmu_calibration_stop
 *
 * @brief   stop a rc clk frequecy calibration.  USER DON'T CALL IT.
 *
 * @param   type    - which rc source will be calibrated. see @enum pmu_cali_src_sel_t
 *          counter - how may rc clk period will be cost util calibration end.
 *
 * @return  None.
 */
void pmu_calibration_stop(void);

/*********************************************************************
 * @fn      pmu_set_led2_value
 *
 * @brief   Set LED2 output voltage level
 *
 * @param   value  - 1:output high voltage; 0:output low voltage; 
 *
 * @return  None.
 */
void pmu_set_led2_value(uint8_t value);

/*********************************************************************
 * @fn      pmu_set_led2_as_pwm
 *
 * @brief   configure LED2 pin as pmu_pwm2 output.
 *
 * @return  None.
 */
void pmu_set_led2_as_pwm(void);

#endif  //_DRIVER_PMU_H

