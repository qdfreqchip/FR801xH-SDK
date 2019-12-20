#ifndef _DRIVER_PMU_QDEC_H
#define _DRIVER_PMU_QDEC_H

#include <stdint.h>
#include <stdbool.h>

#include "driver_pmu.h"

enum pmu_qdec_direction
{
    QDEC_DIR_A,
    QDEC_DIR_B,
};

void pmu_qdec_init(void);
void pmu_qdec_set_debounce_cnt(uint8_t cnt);
void pmu_qdec_autorest_cnt_flag(bool flag_read_rest,bool flag_lc_rest);
void pmu_qdec_set_pin(enum pmu_qdec_la_pin_t pin_a,enum pmu_qdec_lb_pin_t pin_b,enum pmu_qdec_lc_pin_t pin_c);
void pmu_qdec_set_threshold(uint8_t threshold);
void pmu_qdec_set_irq_type(enum pmu_qdec_irq_type irq_type);
uint8_t pmu_qdec_get_cnt(enum pmu_qdec_direction dir);

#endif

