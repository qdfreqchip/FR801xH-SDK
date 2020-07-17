/**
 * Copyright (c) 2020, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef _DEMO_CLOCK_H_
#define _DEMO_CLOCK_H_

/*
 * INCLUDES (包含头文件)
 */

/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */

/*
 * TYPEDEFS (类型定义)
 */
typedef struct clock_param
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t week;
}clock_param_t;

/*
 * GLOBAL VARIABLES (全局变量)
 */
extern clock_param_t clock_env;


/*
 * LOCAL VARIABLES (本地变量)
 */


/*
 * PUBLIC FUNCTIONS (全局函数)
 */

/*********************************************************************
 * @fn      get_sec_from_time
 *
 * @brief   get the seconds based on the current beijing time.
 *
 * @param   current year/month/day/hour/min/sec.
 *
 * @return  seconds number.
 */
int get_sec_from_time(uint16_t year,uint16_t month,uint16_t day,uint16_t hour,uint16_t min,uint16_t sec);

/*********************************************************************
 * @fn      get_current_time_sec
 *
 * @brief   get the seconds based on the current system time.
 *
 * @param   None.
 *
 * @return  seconds number.
 */
int get_current_time_sec(void);

/*********************************************************************
 * @fn      set_data_form_timestamp
 *
 * @brief   convert to time based on the unix timestamp.
 *
 * @param   unix_time - the unix timestamp.
 *
 * @return  None.
 */
void set_data_form_timestamp(int unix_time);

/*********************************************************************
 * @fn      set_sys_clock
 *
 * @brief   sync and set the system time.
 *
 * @param   set_time - set time param.
 *
 * @return  None.
 */
void set_sys_clock(clock_param_t set_time);

/*********************************************************************
 * @fn      sys_timer_init
 *
 * @brief   system clock loop timer init.
 *
 * @param   None.
 *
 * @return  None.
 */
void sys_timer_init(void);

#endif


