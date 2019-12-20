#ifndef _DRIVER_PMU_PWM_H
#define _DRIVER_PMU_PWM_H

#include <stdint.h>
#include <stdbool.h>
#include "driver_iomux.h"

void pmu_pwm_start(enum system_port_t port, enum system_port_bit_t bit,bool repeat_flag,bool reverse_flag);
void pmu_pwm_stop(enum system_port_t port, enum system_port_bit_t bit);
void pmu_pwm_set_param(enum system_port_t port, enum system_port_bit_t bit,uint16_t high_count,uint16_t low_count);
void pmu_pwm_init(void);


#endif  //_PMU_PWM_H

