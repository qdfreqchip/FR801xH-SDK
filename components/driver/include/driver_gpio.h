/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_GPIO_H
#define _DRIVER_GPIO_H

/*
 * INCLUDES
 */
#include <stdint.h>

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_iomux.h"

/*
 * MACROS
 */
#define GPIO_PORTA_DATA         (GPIOAB_BASE + 0x00)
#define GPIO_PORTB_DATA         (GPIOAB_BASE + 0x04)
#define GPIO_PORTA_DIR          (GPIOAB_BASE + 0x08)
#define GPIO_PORTB_DIR          (GPIOAB_BASE + 0x0c)
#define GPIO_PORTC_DATA         (GPIOCD_BASE + 0x00)
#define GPIO_PORTD_DATA         (GPIOCD_BASE + 0x04)
#define GPIO_PORTC_DIR          (GPIOCD_BASE + 0x08)
#define GPIO_PORTD_DIR          (GPIOCD_BASE + 0x0c)

/*********************************************************************
 * @fn      gpio_porta_write
 *
 * @brief   set gpio PORTA output value.
 *
 * @param   value   - output value.
 *
 * @return  None.
 */
__INLINE void gpio_porta_write(uint8_t value)
{
    REG_PL_WR(GPIO_PORTA_DATA, value);
}

/*********************************************************************
 * @fn      gpio_portb_write
 *
 * @brief   set gpio PORTB output value.
 *
 * @param   value   - output value.
 *
 * @return  None.
 */
__INLINE void gpio_portb_write(uint8_t value)
{
    REG_PL_WR(GPIO_PORTB_DATA, value);
}

/*********************************************************************
 * @fn      gpio_portc_write
 *
 * @brief   set gpio PORTC output value.
 *
 * @param   value   - output value.
 *
 * @return  None.
 */
__INLINE void gpio_portc_write(uint8_t value)
{
    REG_PL_WR(GPIO_PORTC_DATA, value);
}

/*********************************************************************
 * @fn      gpio_portd_write
 *
 * @brief   set gpio PORTD output value.
 *
 * @param   value   - output value.
 *
 * @return  None.
 */
__INLINE void gpio_portd_write(uint8_t value)
{
    REG_PL_WR(GPIO_PORTD_DATA, value);
}

/*********************************************************************
 * @fn      gpio_porta_read
 *
 * @brief   get current value of gpio PORTA.
 *
 * @param   None.
 *
 * @return  current value of gpio PORTA.
 */
__INLINE uint8_t gpio_porta_read(void)
{
    return REG_PL_RD(GPIO_PORTA_DATA);
}

/*********************************************************************
 * @fn      gpio_portb_read
 *
 * @brief   get current value of gpio PORTB.
 *
 * @param   None.
 *
 * @return  current value of gpio PORTB.
 */
__INLINE uint8_t gpio_portb_read(void)
{
    return REG_PL_RD(GPIO_PORTB_DATA);
}

/*********************************************************************
 * @fn      gpio_portc_read
 *
 * @brief   get current value of gpio PORTC.
 *
 * @param   None.
 *
 * @return  current value of gpio PORTC.
 */__INLINE uint8_t gpio_portc_read(void)
{
    return REG_PL_RD(GPIO_PORTC_DATA);
}

 /*********************************************************************
  * @fn      gpio_portd_read
  *
  * @brief   get current value of gpio PORTD.
  *
  * @param   None.
  *
  * @return  current value of gpio PORTD.
  */
__INLINE uint8_t gpio_portd_read(void)
{
    return REG_PL_RD(GPIO_PORTD_DATA);
}

/*********************************************************************
 * @fn      gpio_porta_set_dir
 *
 * @brief   set gpio works in output or input mode.
 *
 * @param   dir - the in-out direction of gpio, each bit represent one channel,
 *                0: output, 1:input.
 *
 * @return  None.
 */
__INLINE void gpio_porta_set_dir(uint8_t dir)
{
    REG_PL_WR(GPIO_PORTA_DIR, dir);
}

/*********************************************************************
 * @fn      gpio_portb_set_dir
 *
 * @brief   set gpio works in output or input mode.
 *
 * @param   dir - the in-out direction of gpio, each bit represent one channel,
 *                0: output, 1:input.
 *
 * @return  None.
 */
__INLINE void gpio_portb_set_dir(uint8_t dir)
{
    REG_PL_WR(GPIO_PORTB_DIR, dir);
}

/*********************************************************************
 * @fn      gpio_portc_set_dir
 *
 * @brief   set gpio works in output or input mode.
 *
 * @param   dir - the in-out direction of gpio, each bit represent one channel,
 *                0: output, 1:input.
 *
 * @return  None.
 */
__INLINE void gpio_portc_set_dir(uint8_t dir)
{
    REG_PL_WR(GPIO_PORTC_DIR, dir);
}

/*********************************************************************
 * @fn      gpio_portd_set_dir
 *
 * @brief   set gpio works in output or input mode.
 *
 * @param   dir - the in-out direction of gpio, each bit represent one channel,
 *                0: output, 1:input.
 *
 * @return  None.
 */
__INLINE void gpio_portd_set_dir(uint8_t dir)
{
    REG_PL_WR(GPIO_PORTD_DIR, dir);
}

/*********************************************************************
 * @fn      gpio_porta_set_dir
 *
 * @brief   get current gpio PORTA in-out setting.
 *
 * @param   None.
 *
 * @return  current setting.
 */
__INLINE uint8_t gpio_porta_get_dir(void)
{
    return REG_PL_RD(GPIO_PORTA_DIR);
}

/*********************************************************************
 * @fn      gpio_portb_set_dir
 *
 * @brief   get current gpio PORTB in-out setting.
 *
 * @param   None.
 *
 * @return  current setting.
 */
__INLINE uint8_t gpio_portb_get_dir(void)
{
    return REG_PL_RD(GPIO_PORTB_DIR);
}

/*********************************************************************
 * @fn      gpio_portc_set_dir
 *
 * @brief   get current gpio PORTC in-out setting.
 *
 * @param   None.
 *
 * @return  current setting.
 */
__INLINE uint8_t gpio_portc_get_dir(void)
{
    return REG_PL_RD(GPIO_PORTC_DIR);
}

/*********************************************************************
 * @fn      gpio_portd_set_dir
 *
 * @brief   get current gpio PORTD in-out setting.
 *
 * @param   None.
 *
 * @return  current setting.
 */
__INLINE uint8_t gpio_portd_get_dir(void)
{
    return REG_PL_RD(GPIO_PORTD_DIR);
}

/*********************************************************************
 * @fn      gpio_set_dir
 *
 * @brief   set specific gpio channel working mode: output or input.
 *
 * @param   port    - which port this channel belongs to. @ref system_port_t
 *          bit     - channel number. @ref system_port_bit_t
 *          dir     - in-out selection, should be GPIO_DIR_IN or GPIO_DIR_OUT.
 *
 * @return  None.
 */
void gpio_set_dir(enum system_port_t port, enum system_port_bit_t bit, uint8_t dir);
/*********************************************************************
 * @fn      gpio_get_pin_value
 *
 * @brief   get specific gpio pin vale.
 *
 * @param   port    - which port this channel belongs to. @ref system_port_t
 *          bit     - channel number. @ref system_port_bit_t
 *
 * @return  gpio pin value. 0, pin is at low voltage; 1, pin is at high voltage;
 */
uint8_t gpio_get_pin_value(enum system_port_t port, enum system_port_bit_t bit);
/*********************************************************************
 * @fn      gpio_set_pin_value
 *
 * @brief   set specific gpio pin vale.
 *
 * @param   port    - which port this channel belongs to. @ref system_port_t
 *          bit     - channel number. @ref system_port_bit_t
 *          value   - high-low voltage value. 0, output as low voltage; 1 output as high voltage
 *
 * @return  None.
 */
void gpio_set_pin_value(enum system_port_t port, enum system_port_bit_t bit, uint8_t value);

#endif //_DRIVER_GPIO_H

