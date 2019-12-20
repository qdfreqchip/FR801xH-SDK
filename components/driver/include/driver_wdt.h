#ifndef _DRIVER_WDT_H
#define _DRIVER_WDT_H

/*
 * INCLUDES
 */
#include <stdint.h>

#include "co_math.h"

#include "driver_pmu.h"

/*
 * TYPEDEFS
 */
enum wdt_action
{
    WDT_ACT_RST_CHIP,   /* reset the chip */
    WDT_ACT_CALL_IRQ,   /* generate an interrupt */ 
};

/*********************************************************************
 * @fn      wdt_init
 *
 * @brief   init watchdog before enable this mudule.
 *
 * @param   action      - the next action after watchdog timer expired.
 *          delay_s     - how many seconds does the timer set
 *
 * @return  None.
 */
void wdt_init(enum wdt_action action, uint8_t delay_s);

/*********************************************************************
 * @fn      wdt_feed
 *
 * @brief   feed the watchdog.
 *
 * @param   None.
 *
 * @return  None.
 */
void wdt_feed(void);

/*********************************************************************
 * @fn      wdt_start
 *
 * @brief   start the watchdog after init this mudle.
 *
 * @param   None.
 *
 * @return  None.
 */
void wdt_start(void);

/*********************************************************************
 * @fn      wdt_stop
 *
 * @brief   stop the watchdog.
 *
 * @param   None.
 *
 * @return  None.
 */
void wdt_stop(void);

#endif  // _DRIVER_WDT_H

