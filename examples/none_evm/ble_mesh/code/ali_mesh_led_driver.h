/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _ALI_MESH_LED_DRIVER_H_
#define _ALI_MESH_LED_DRIVER_H_

/*
 * INCLUDES 
 */
#include <stdint.h>

/*
 * MACROS 
 */

/*
 * CONSTANTS 
 */
#define APP_LED_COUNT       3

/*
 * TYPEDEFS 
 */
// LED state
struct app_led_state_t
{
    uint8_t on_off_state[APP_LED_COUNT];
    uint16_t level[APP_LED_COUNT];
    uint16_t ctl_lightness;
    uint16_t ctl_temperature;
    uint16_t hsl_lightness;
    uint16_t hsl_hue;
    uint16_t hsl_saturation;
    uint16_t remote_dev_src;
    uint8_t appkey_id;
};


/*
 * PUBLIC FUNCTIONS 
 */
void app_led_init(void);
void app_led_set_remote_msg(uint16_t src_id,uint8_t appkey);
void app_led_get_remote_msg(uint16_t * src_id,uint8_t * appkey);
uint8_t app_led_get_onoffstate(uint8_t index);
uint16_t app_led_get_level(uint8_t index);
uint16_t app_led_get_ctl_lightness(void);
uint16_t app_led_get_ctl_temperature(void);
uint16_t app_led_get_hsl(void);
void app_led_set_onoffstate(uint8_t state);
void app_led_set_lightness(uint16_t lightness);
void app_led_set_CTL(uint16_t lightness, uint16_t temperature);
void app_led_set_hsl(uint16_t hue,uint16_t saturation,uint16_t lightness);

#endif

