/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef _DRIVER_PWM_H
#define _DRIVER_PWM_H

/*
 * INCLUDES 
 */

/*
 * MACROS 
 */

/*
 * CONSTANTS 
 */

/*
 * TYPEDEFS 
 */
enum pwm_channel_t {
    PWM_CHANNEL_0,
    PWM_CHANNEL_1,
    PWM_CHANNEL_2,
    PWM_CHANNEL_3,
    PWM_CHANNEL_4,
    PWM_CHANNEL_5,
    PWM_CHANNEL_MAX,
};

/*
 * GLOBAL VARIABLES 
 */

/*
 * LOCAL VARIABLES 
 */

/*
 * LOCAL FUNCTIONS 
 */

/*
 * EXTERN FUNCTIONS 
 */

/*
 * PUBLIC FUNCTIONS 
 */

/*********************************************************************
 * @fn      pwm_init
 *
 * @brief   init one pwm channel.
 *
 * @param   channel     - pwm channel, @ref pwm_channel_t.
 *          frequency   - output wave frequency
 *          high_duty   - duty taken by high level, should be from 0 to 99
 *
 * @return  None.
 */
void pwm_init(enum pwm_channel_t channel, uint32_t frequency, uint8_t high_duty);

/*********************************************************************
 * @fn      pwm_update
 *
 * @brief   update one pwm channel with new parameter.
 *
 * @param   channel     - pwm channel, @ref pwm_channel_t.
 *          frequency   - output wave frequency
 *          high_duty   - duty taken by high level, should be from 0 to 99
 *
 * @return  None.
 */
void pwm_update(enum pwm_channel_t channel, uint32_t frequency, uint8_t high_duty);

/*********************************************************************
 * @fn      pwm_start
 *
 * @brief   start one pwm channel.
 *
 * @param   channel     - pwm channel, @ref pwm_channel_t.
 *
 * @return  None.
 */
void pwm_start(enum pwm_channel_t channel);

/*********************************************************************
 * @fn      pwm_stop
 *
 * @brief   stop one working pwm channel.
 *
 * @param   channel     - pwm channel, @ref pwm_channel_t.
 *
 * @return  None.
 */
void pwm_stop(enum pwm_channel_t channel);

#endif  // _DRIVER_PWM_H

