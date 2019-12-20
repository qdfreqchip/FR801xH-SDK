/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef _DRIVER_TIMER_H
#define _DRIVER_TIMER_H

/*
 * INCLUDES
 */
#include <stdint.h>
#include "driver_plf.h"

/*
 * MACROS
 */
#define TIMER0          (TIMER_BASE)
#define TIMER1          (TIMER_BASE + 0x20)

/*
 * TYPEDEFS
 */
enum timer_run_type
{
    TIMER_FREE_RUN = 0,
    TIMER_PERIODIC = 1
};

enum timer_prescale_t
{
    TIMER_PRESCALE_1,
    TIMER_PRESCALE_16,
    TIMER_PRESCALE_256,
    TIMER_PRESCALE_MAX,
};

/*********************************************************************
 * @fn      timer_run
 *
 * @brief   start a pending timer.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_run(uint32_t timer_addr);

/*********************************************************************
 * @fn      timer_stop
 *
 * @brief   stop a running timer.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_stop(uint32_t timer_addr);

/*********************************************************************
 * @fn      timer_reload
 *
 * @brief   reset initial count value with load value.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_reload(uint32_t timer_addr);

/*********************************************************************
 * @fn      timer_clear_interrupt
 *
 * @brief   clear timer interrupt.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  None.
 */
void timer_clear_interrupt(uint32_t timer_addr);

/*********************************************************************
 * @fn      timer_get_load_value
 *
 * @brief   get current load value.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  current load value.
 */
uint32_t timer_get_load_value(uint32_t timer_addr);

/*********************************************************************
 * @fn      timer_get_current_value
 *
 * @brief   get current count value.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *
 * @return  current count value.
 */
uint32_t timer_get_current_value(uint32_t timer_addr);

/*********************************************************************
 * @fn      timer_init
 *
 * @brief   Initialize timer instance.
 *
 * @param   timer_addr  - timer base address: TIMER0 or TIMER1.
 *          count_us    - timer period with us as unit
 *          run_mode    - single mode or period mode(@see enum timer_run_type)
 *
 * @return  initialization result: true or false.
 */
uint8_t timer_init(uint32_t timer_addr, uint32_t count_us, uint8_t run_mode);

#endif

