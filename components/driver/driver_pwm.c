#include <stdint.h>

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_pwm.h"

struct pwm_ctrl_t {
    uint32_t en:1;
    uint32_t reserved0:2;
    uint32_t out_en:1;
    uint32_t single:1;
    uint32_t reservaed1:27;
};

struct pwm_elt_t {
    uint32_t cur_cnt;
    uint32_t high_cnt;
    uint32_t total_cnt;
    struct pwm_ctrl_t ctrl;
};

struct pwm_regs_t {
    struct pwm_elt_t channel[PWM_CHANNEL_MAX];
};

struct pwm_regs_t *pwm_ctrl = (struct pwm_regs_t *)PWM_BASE;

void pwm_init(enum pwm_channel_t channel, uint32_t frequency, uint8_t high_duty)
{
    uint32_t total_count;
    uint32_t high_count;

    total_count = system_get_pclk() / frequency;
    if(high_duty < 100) {
        high_count = total_count * (100-high_duty) / 100;
    }
    else {
        return;
    }

    *(uint32_t *)&pwm_ctrl->channel[channel].ctrl = 0;
    pwm_ctrl->channel[channel].total_cnt = total_count;
    pwm_ctrl->channel[channel].high_cnt = high_count;
}

void pwm_update(enum pwm_channel_t channel, uint32_t frequency, uint8_t high_duty)
{
    uint32_t total_count;
    uint32_t high_count;
    GLOBAL_INT_DISABLE();

    total_count = system_get_pclk() / frequency;
    if(high_duty < 100) {
        high_count = total_count * (100-high_duty) / 100;
    }
    else {
        GLOBAL_INT_RESTORE();
        return;
    }

    pwm_ctrl->channel[channel].total_cnt = total_count;
    pwm_ctrl->channel[channel].high_cnt = high_count;
    GLOBAL_INT_RESTORE();
}

void pwm_start(enum pwm_channel_t channel)
{
    pwm_ctrl->channel[channel].ctrl.out_en = 1;
    pwm_ctrl->channel[channel].ctrl.en = 1;
}

void pwm_stop(enum pwm_channel_t channel)
{
    pwm_ctrl->channel[channel].ctrl.en = 0;
    pwm_ctrl->channel[channel].ctrl.out_en = 0;
}


