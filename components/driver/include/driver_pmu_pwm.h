#ifndef _DRIVER_PMU_PWM_H
#define _DRIVER_PMU_PWM_H

#include <stdint.h>
#include <stdbool.h>
#include "driver_iomux.h"

/*********************************************************************
 * @fn      pmu_pwm_start
 *
 * @brief   start one pmu pwm channel
 *
 * @param   port  - IO port which is mapped with pmu pwm, @ref enum system_port_t.
 *          bit   - IO bit which is mapped with pmu pwm, @ref enum system_port_t.
 *          repeat_flag   - true = pmu pwm will output continously. 0 = pmu pwm will output one period.
 *          reverse_flag   - true = pmu pwm start from low voltage. fasle = pmu pwm start from high voltage.
 * 
 * @return  None.
 */
void pmu_pwm_start(enum system_port_t port, enum system_port_bit_t bit,bool repeat_flag,bool reverse_flag);
/*********************************************************************
 * @fn      pmu_pwm_stop
 *
 * @brief   stop one pmu pwm channel
 *
 * @param   port  - IO port which is mapped with pmu pwm, @ref enum system_port_t.
 *          bit   - IO bit which is mapped with pmu pwm, @ref enum system_port_t.
 *
 * @return  None.
 */
void pmu_pwm_stop(enum system_port_t port, enum system_port_bit_t bit);
/*********************************************************************
 * @fn      pmu_pwm_set_param
 *
 * @brief   set one pmu pwm channel with parameter.
 *
 * @param   port  - IO port which is mapped with pmu pwm, @ref enum system_port_t.
 *          bit   - IO bit which is mapped with pmu pwm, @ref enum system_port_t.
 *          high_count  - high level output time, unit: RC_Period / 2;  range: 0~65535
 *                        where RC_CLK can be accessed by pmu_get_rc_clk(false)
 *          low_count   - low level output time, unit: RC_Period / 2;   range: 0~65535
 *                        where RC_CLK can be accessed by pmu_get_rc_clk(false)
 *
 * @return  None.
 */
void pmu_pwm_set_param(enum system_port_t port, enum system_port_bit_t bit,uint16_t high_count,uint16_t low_count);
/*********************************************************************
 * @fn      pmu_pwm_init
 *
 * @brief   init pmu pwm block.
 *
 * @param   None.
 *
 * @return  None.
 */
void pmu_pwm_init(void);


#endif  //_PMU_PWM_H

