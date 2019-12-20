#ifndef _DRIVER_RTC_H
#define _DRIVER_RTC_H

#include <stdint.h>

#define PMU_REG_RTC_SETTINGS        0x3d
#define PMU_RTC_EN                  CO_BIT(0)
#define PMU_RTC_UPDATE_EN           CO_BIT(1)
#define PMU_RTC_ALARM_A_EN          CO_BIT(2)
#define PMU_RTC_ALARM_B_EN          CO_BIT(3)
#define PMU_RTC_ALMA_INT_EN         CO_BIT(4)
#define PMU_RTC_ALMB_INT_EN         CO_BIT(5)
#define PMU_RTC_ALMA_PO_EN          CO_BIT(6)   //rtc alarm a power on
#define PMU_RTC_ALMB_PO_EN          CO_BIT(7)

#define PMU_REG_RTC_UPDATE_0        0x3e
#define PMU_REG_RTC_UPDATE_1        0x3f
#define PMU_REG_RTC_UPDATE_2        0x40
#define PMU_REG_RTC_UPDATE_3        0x41

#define PMU_REG_RTC_ALMA_VALUE_0    0x42
#define PMU_REG_RTC_ALMA_VALUE_1    0x43
#define PMU_REG_RTC_ALMA_VALUE_2    0x44
#define PMU_REG_RTC_ALMA_VALUE_3    0x45

#define PMU_REG_RTC_ALMB_VALUE_0    0x46
#define PMU_REG_RTC_ALMB_VALUE_1    0x47
#define PMU_REG_RTC_ALMB_VALUE_2    0x48
#define PMU_REG_RTC_ALMB_VALUE_3    0x49

#define PMU_REG_RTC_VALUE_0    0x72
#define PMU_REG_RTC_VALUE_1    0x73
#define PMU_REG_RTC_VALUE_2    0x74
#define PMU_REG_RTC_VALUE_3    0x75

enum rtc_idx_t
{
    RTC_A,
    RTC_B,
};

/*********************************************************************
 * @fn      rtc_init
 *
 * @brief   Initialize rtc module.
 *
 * @param   None
 *
 * @return  None.
 */
void rtc_init(void);

/*********************************************************************
 * @fn      rtc_alarm
 *
 * @brief   start running a rtc.
 *
 * @param   rtc_idx     - RTC_A or RTC_B, @ref rtc_idx_t
 *          count_ms    - timer duration with ms as unit. range [1,4294967]
 *
 * @return  None.
 */
void rtc_alarm(enum rtc_idx_t rtc_idx, uint32_t count_ms);

/*********************************************************************
 * @fn      rtc_disalarm
 *
 * @brief   stop a running rtc.
 *
 * @param   rtc_idx     - RTC_A or RTC_B, @ref rtc_idx_t
 *
 * @return  None.
 */
void rtc_disalarm(enum rtc_idx_t rtc_idx);

#endif

