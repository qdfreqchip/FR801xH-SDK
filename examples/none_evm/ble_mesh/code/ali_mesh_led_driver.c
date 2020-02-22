/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
 * INCLUDES 
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "driver_gpio.h"
#include "driver_system.h"
#include "driver_pwm.h"
#include "driver_pmu.h"

#include "ali_mesh_info.h"
#include "ali_mesh_led_driver.h"

/*
 * MACROS 
 */

/*
 * CONSTANTS 
 */
#define APP_LED_COUNT       2

/* pwm output frequency is 8K */
#define FREQUENCY_SET	8000
#define PORT_FUNC_PWM	0x03
#define PORT_FUNC_GPIO	0x00

/*
 * TYPEDEFS 
 */

/*
 * GLOBAL VARIABLES 
 */

/*
 * LOCAL VARIABLES 
 */
static struct app_led_state_t app_led_state[APP_LED_COUNT];
/* use PA4 and PA5 for led control */
static const uint8_t app_led_pwm_map[APP_LED_COUNT] = {4, 5};
static uint8_t app_led_inited = false;

static void app_led_calc_pwm_count(uint8_t *high_duty,uint16_t level)
{
  *high_duty = 100*level/0xffff;
}

/*********************************************************************
 * @fn      app_led_set_state
 *
 * @brief   update LED state.
 *
 * @param   index   - which led should be updated.
 *
 * @return  None.
 */
static void app_led_set_state(uint8_t index)
{
    uint8_t high_duty;

    if(app_led_state[index].on_off_state == 0)
    {
        gpio_portd_write(gpio_portd_read() & (~(1<<(app_led_pwm_map[index]))));
        system_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)app_led_pwm_map[index], PORT_FUNC_GPIO);
    }
    else
    {
        if(app_led_state[index].level == 0xFFFF)
        {
            gpio_portd_write(gpio_portd_read() | (1<<(app_led_pwm_map[index])));
            system_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)app_led_pwm_map[index], PORT_FUNC_GPIO);
        }
        else
        {
            app_led_calc_pwm_count(&high_duty, app_led_state[index].level);
            pwm_update((enum pwm_channel_t)app_led_pwm_map[index], FREQUENCY_SET, high_duty);
            pwm_start((enum pwm_channel_t)app_led_pwm_map[index]);
            system_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)app_led_pwm_map[index], PORT_FUNC_PWM);
        }
    }

    if(app_led_inited)
    {
        app_mesh_user_data_update((void *)&app_led_state, sizeof(app_led_state), 2000);
    }
}

/*********************************************************************
 * @fn      app_led_init
 *
 * @brief   initialize LED state after power on.
 *
 * @param   None.
 *
 * @return  None.
 */
void app_led_init(void)
{
    /* set LED output to low level */
    gpio_portd_write(gpio_portd_read() & 0xcf);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_4, PORTD4_FUNC_D4);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_5, PORTD5_FUNC_D5);
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_4, GPIO_DIR_OUT);
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_5, GPIO_DIR_OUT);
    system_set_port_pull(GPIO_PD4, true);
	system_set_port_pull(GPIO_PD5, true);

    /* set lightness control PA2 */
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_PWM2);
    pwm_init(PWM_CHANNEL_2, 8000, 90);
    pwm_start(PWM_CHANNEL_2);

    /* confirm this pin is controlled by CPU */
	pmu_set_pin_to_CPU(GPIO_PORT_D, (1<<GPIO_BIT_4)|(1<<GPIO_BIT_5));

    memset(app_led_state, 0, sizeof(app_led_state));

    if(app_mesh_user_data_load((void *)&app_led_state, sizeof(app_led_state)) == false)
    {
        app_led_state[0].on_off_state = 1;
        app_led_state[0].level = 0xFFFF;
        app_led_state[0].ctl_lightness = 0xFFFF;
        app_led_state[0].ctl_temperature = 0xFFFF;
        app_led_state[1].on_off_state = 1;
        app_led_state[1].level = 0xFFFF;
        app_led_state[1].ctl_lightness = 0xFFFF;
        app_led_state[1].ctl_temperature = 0xFFFF;
    }

    /* use stored information or default value to update LED state */
    app_led_set_state(0);
    app_led_set_state(1);
    app_mesh_store_user_data_timer_init();

    app_led_inited = true;
}

/*********************************************************************
 * @fn      app_led_get_onoffstate
 *
 * @brief   Get specified current LED OnOff state.
 *
 * @param   index       - which LED OnOff state should get.
 *
 * @return  uint8_t     - returned LED OnOff state.
 */
uint8_t app_led_get_onoffstate(uint8_t index)
{
    return app_led_state[index].on_off_state;
}

/*********************************************************************
 * @fn      app_led_get_level
 *
 * @brief   Get specified current LED level.
 *
 * @param   index       - which LED level should get.
 *
 * @return  uint8_t     - returned LED level.
 */
uint16_t app_led_get_level(uint8_t index)
{
    return app_led_state[index].level;
}

/*********************************************************************
 * @fn      app_led_get_ctl_lightness
 *
 * @brief   Get specified current LED ctl lightness number.
 *
 * @param   index       - which LED lightness should get.
 *
 * @return  uint8_t     - returned LED lightness.
 */
uint16_t app_led_get_ctl_lightness(uint8_t index)
{
    return app_led_state[index].ctl_lightness;
}

/*********************************************************************
 * @fn      app_led_get_hsl
 *
 * @brief   Get specified current LED hsl number.
 *
 * @param   index        - which LED hsl should get.
 *
 * @return  uint16_t     - returned LED hsl number.
 */
uint16_t app_led_get_hsl(uint8_t index)
{
    return app_led_state[index].ctl_hue;
}

/*********************************************************************
 * @fn      app_led_get_ctl_temperature
 *
 * @brief   Get specified current LED temperature number.
 *
 * @param   index       - which LED temperature should get.
 *
 * @return  uint16_t     - returned LED temperature number.
 */
uint16_t app_led_get_ctl_temperature(uint8_t index)
{
    return app_led_state[index].ctl_temperature;
}

/*********************************************************************
 * @fn      app_led_set_onoffstate
 *
 * @brief   Set specified LED OnOff state, used to turn on/off LED.
 *
 * @param   index       - which LED should set.
 *
 * @return  None.
 */
void app_led_set_onoffstate(uint8_t index, uint8_t state)
{
    if(app_led_state[index].on_off_state != state)
    {
        app_led_state[index].on_off_state = state;
        app_led_set_state(index);
    }
}

/*********************************************************************
 * @fn      app_led_set_hsl
 *
 * @brief   Set specified LED hsl number.
 *
 * @param   index       - which LED should set.
 *          hue         - hue value to set.
 *
 * @return  None.
 */
void app_led_set_hsl(uint8_t index, uint16_t hue)
{
    if(app_led_state[index].ctl_hue == hue)
    {
        return;
    }

    app_led_state[index].ctl_hue = hue;
    app_led_set_state(index);
}

/*********************************************************************
 * @fn      app_led_set_level
 *
 * @brief   Set specified LED level.
 *
 * @param   index       - which LED should set.
 *          level       - level value to set.
 *
 * @return  None.
 */
void app_led_set_level(uint8_t index, uint16_t level)
{
    if(app_led_state[index].level == level)
    {
        return;
    }

    app_led_state[index].level = level;
    app_led_set_state(index);
}

/*********************************************************************
 * @fn      app_led_set_CTL
 *
 * @brief   Set specified LED level.
 *
 * @param   index           - which LED should set.
 *          lightness       - lightness value to set.
 *          temperature     - temperature valueto set
 *
 * @return  None.
 */
void app_led_set_CTL(uint8_t index, uint16_t lightness, uint16_t temperature)
{
    if((app_led_state[index].ctl_lightness == lightness)
        && (app_led_state[index].ctl_temperature == temperature))
    {
        return;
    }

    app_led_state[index].ctl_lightness = lightness;
    app_led_state[index].ctl_temperature = temperature;
    app_led_set_state(index);
}

