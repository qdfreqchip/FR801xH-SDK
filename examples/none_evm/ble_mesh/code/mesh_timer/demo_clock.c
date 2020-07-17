/*
 * INCLUDES (包含头文件)
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver_system.h"
#include "os_timer.h"
#include "co_printf.h"
#include "mesh_api.h"

#include "demo_clock.h"
#include "vendor_timer_ctrl.h"

/*
 * MACROS (宏定义)
 */
#define BLE_BASETIMECNT_MASK   ((uint32_t)0x33333330)
#define DAYS_PER_WEEK 7
#define MONTHS 12
#define DATE_HEADER "   Sun   Mon   Tues  Wed   Thur  Fri   Sat"

/*
 * CONSTANTS (常量定义)
 */

/*
 * TYPEDEFS (类型定义)
 */


/*
 * GLOBAL VARIABLES (全局变量)
 */

/*
 * LOCAL VARIABLES (本地变量)
 */
static os_timer_t sys_timer_t;
static clock_param_t clock_env =
{
    .year = 2020,
    .month = 1,
    .day = 1,
    .week = 3,
    .hour = 0,
    .min = 0,
    .sec = 0,
};
static uint32_t last_ke_time = 0;

static bool is_leap_year(int year);
static int days_of_year(int year, int month, int day);
static int days_of_month(int year, int month);
void show_clock_func(void);
/*
 * PUBLIC FUNCTIONS (全局函数)
 */


#if 0
int time_cal_test(void)
{
   // int days = get_days(2019, 2, 19);
    //day_of_week(2019, 3, 4),
    printf("%d,%d,%d\n",get_days(1, 1, 1),get_days(2010, 1, 1),get_days(2019, 2, 19));

    //print_date_of_month(2019, 8);

    return 0;
}
#endif

/*********************************************************************
 * @fn      get_days
 *
 * @brief   how many days are there since 1.1.1970 --- UNIX time.
 *
 * @param   current year/month/day.
 *
 * @return  number of days.
 */
static int get_days(int year, int month, int day)
{
    int days = days_of_year(year, month, day);
    int temp = year-1970;
    return (temp * 365 + temp / 4 - temp / 100 + temp / 400 +  days -1);
}

/*********************************************************************
 * @fn      get_sec_from_time
 *
 * @brief   get the seconds based on the current beijing time.
 *
 * @param   current year/month/day/hour/min/sec.
 *
 * @return  seconds number.
 */
int get_sec_from_time(uint16_t year,uint16_t month,uint16_t day,uint16_t hour,uint16_t min,uint16_t sec)
{
    // beijing time = UTC time + 8
    return (get_days(year, month, day)/* - 733772*/) * 86400 + hour * 3600 + min * 60 + sec - 8*3600;
}

/*********************************************************************
 * @fn      get_current_time_sec
 *
 * @brief   get the seconds based on the current system time.
 *
 * @param   None.
 *
 * @return  seconds number.
 */
int get_current_time_sec(void)
{
    return get_sec_from_time(clock_env.year,clock_env.month,clock_env.day,clock_env.hour,clock_env.min,clock_env.sec);
}

/*********************************************************************
 * @fn      days_of_year
 *
 * @brief   which day of the year.
 *
 * @param   year - the current year.
 *
 * @param   month - the current month.
 *
 * @param   day - the current day.
 *
 * @return  number of days.
 */
static int days_of_year(int year, int month, int day)
{
    int i;
    int days = 0;
    for(i = 1; i < month; i++)
    {
        days += days_of_month(year, i);

    }
    return days + day;
}

/*********************************************************************
 * @fn      days_of_month
 *
 * @brief   the number of days in a month.
 *
 * @param   year - the current year.
 *
 * @param   month - the current month.
 *
 * @return  days of the month.
 */
static int days_of_month(int year, int month)
{
    // days of month
    const int  month_days[MONTHS] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if(2 == month && is_leap_year(year))
        return 29;
    else
        return month_days[month-1];
}

/*********************************************************************
 * @fn      is_leap_year
 *
 * @brief   check if it's a leap year.
 *
 * @param   year - the current year.
 *
 * @return  ture-it's a leap year,false-it's not.
 */
static bool is_leap_year(int year)
{
    return (year % 400 == 0 ) || (year % 4 == 0 && year % 100 != 0);
}

/*********************************************************************
 * @fn      day_of_week
 *
 * @brief   convert to day of the week based on the days.
 *
 * @param   year/month/day.
 *
 * @return  day of the week.
 */
static int day_of_week(int year, int month, int day)
{
    // 1970,1.1---week4
    return (get_days(year, month, day)+4) % DAYS_PER_WEEK;
}

/*********************************************************************
 * @fn      set_data_form_timestamp
 *
 * @brief   convert to time based on the unix timestamp.
 *
 * @param   unix_time - the unix timestamp.
 *
 * @return  None.
 */
void set_data_form_timestamp(int unix_time)
{
    clock_param_t get_time;
    int  month_days[MONTHS] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t i = 0;
    uint16_t days = 0,days_b = 0;

    get_time.year = 1970+unix_time/86400/365;
    if(is_leap_year((int)get_time.year))
        month_days[1] = 29;

    days_b = (unix_time/86400-get_days((int)get_time.year,1,1));
    for(i = 0;i < MONTHS;i++)
    {
        days += month_days[i];
        if(days > days_b)
        {
            get_time.month = i+1;
            get_time.day = days_b-(days-month_days[i])+1;
            break;
        }
    }

    get_time.hour = (unix_time%86400/3600+8)%24; // beijing time = utc time+8
    get_time.min = unix_time%86400%3600/60;
    get_time.sec = unix_time%86400%3600%60;
    get_time.week = day_of_week(get_time.year,get_time.month,get_time.day);

    set_sys_clock(get_time);
}
/*************day calculate************* */

/*********************************************************************
 * @fn      set_sys_clock
 *
 * @brief   sync and set the system time.
 *
 * @param   set_time - set time param.
 *
 * @return  None.
 */
void set_sys_clock(clock_param_t set_time)
{
    memcpy(&clock_env,&set_time,sizeof(clock_param_t));

    show_clock_func();
}

/*********************************************************************
 * @fn      get_sys_ke_basetime
 *
 * @brief   get system basic time.
 *
 * @param   None.
 *
 * @return  the system tick.
 */
static uint32_t get_sys_ke_basetime(void)
{
	uint32_t cur_base_time = system_get_curr_time();
    uint32_t diff;
    if(cur_base_time >= last_ke_time)
        diff = cur_base_time - last_ke_time;
    else
        diff = cur_base_time + BLE_BASETIMECNT_MASK + 1 - last_ke_time;

	return diff;
}

/*********************************************************************
 * @fn      clock_hdl
 *
 * @brief   system clock function.
 *
 * @param   None.
 *
 * @return  change state:1-sec 2-min 3-hour.
 */
static uint8_t clock_hdl(void)
{
	uint8_t update_flag = 0;
	uint32_t diff = get_sys_ke_basetime();

    if( diff > 1000 )
    {
        last_ke_time += 1000;
        if( last_ke_time > BLE_BASETIMECNT_MASK)
            last_ke_time -= (BLE_BASETIMECNT_MASK+1);
        clock_env.sec++;
		update_flag = 1;
        if(clock_env.sec>=60)
        {
            clock_env.sec = 0;
            clock_env.min++;
			update_flag = 2;
            if(clock_env.min>=60)
            {
                clock_env.min = 0;
                clock_env.hour++;
				update_flag = 3;
                if(clock_env.hour>=24)
                {
                    clock_env.hour = 0;
                    clock_env.week++;
                    if(clock_env.week>=8)
                        clock_env.week=1;

                    clock_env.day++;
                    if(clock_env.day >= (days_of_month(clock_env.year,clock_env.month)+1) )
                    {
                        clock_env.day = 1;
                        clock_env.month++;
                        if(clock_env.month>=13)
                        {
                            clock_env.month = 1;
                            clock_env.year++;
                        }
                    }
                }
            }
        }//min hdl
    }

	return update_flag;
}

void show_clock_func(void)
{
	co_printf("Y:%d,M:%d,D:%d,W:%d,h:%d-m:%d-s:%d\r\n",clock_env.year,clock_env.month,clock_env.day,clock_env.week,
	                    clock_env.hour,clock_env.min,clock_env.sec);
	co_printf("sec=%x\r\n",get_sec_from_time(clock_env.year,clock_env.month,clock_env.day,\
	                    clock_env.hour,clock_env.min,clock_env.sec));                   
}

/*********************************************************************
 * @fn      sys_timer_func
 *
 * @brief   system clock loop timer callback.
 *
 * @param   arg - timer callback arg.
 *
 * @return  None.
 */
static void sys_timer_func(void * arg)
{
    uint8_t check_min_flag = 0;
    
    check_min_flag = clock_hdl();
    if(check_min_flag == 1) // sec
        vendor_check_timer_opration(clock_env);
}

/*********************************************************************
 * @fn      sys_timer_init
 *
 * @brief   system clock loop timer init.
 *
 * @param   None.
 *
 * @return  None.
 */
void sys_timer_init(void)
{
    co_printf("=sys_timer_init=\r\n");
    
    os_timer_init(&sys_timer_t,sys_timer_func,NULL);
    os_timer_start(&sys_timer_t,500,true);
}


