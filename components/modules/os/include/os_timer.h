/**
 * Copyright (c) 2019, Tsingtao Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
#ifndef OS_TIME_H
#define OS_TIME_H

#include <stdint.h>
#include <stdbool.h>


typedef void(* os_timer_func_t)(void *parg);
  /**
 * OS timer definition.
 */
typedef struct os_timer_struct
{
    struct os_timer_struct *timer_next;
    uint32_t timer_period;      //for loop OR period =0 for single run.
    os_timer_func_t timer_func;
    void *timer_arg;
    uint16_t timer_id;
} os_timer_t;

/*********************************************************************
 * @fn      osal_timer_init
 *
 * @brief   Initialize an OS timer and insert this timer into timer list.
 *			
 *
 * @param   ptimer - pointer to the timer buffer.
 *			pfunction - callback of the timer
 *          parg - parameter of timer callback
 * @return  None.
 */
void os_timer_init(os_timer_t *ptimer, os_timer_func_t pfunction, void *parg);

/*********************************************************************
 * @fn      os_timer_destroy
 *
 * @brief   remove an OS timer from timer list.
 *			
 * @param   ptimer - pointer to the timer buffer.
 *
 * @return  None.
 */
void os_timer_destroy(os_timer_t *ptimer);

/*********************************************************************
 * @fn      osal_timer_start
 *
 * @brief   Start a timer.
 *			????Timer.
 *
 * @param   ptimer - pointer to the timer buffer.
 *			ms - expire of the timer with milliseconds
 *          repeat_flag - repeat or one shot timer
 * @return  None.
 */
void os_timer_start(os_timer_t *ptimer,uint32_t ms, bool repeat_flag);


 /*********************************************************************
 * @fn      osal_timer_stop
 *
 * @brief   Stop a timer.
 *			??timer.
 *
 * @param   ptimer - pointer to the timer buffer.
 *
 * @return  None.
 */
void os_timer_stop(os_timer_t *ptimer);

#endif // OS_TIME_H
