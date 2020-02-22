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

/*
 * TYPEDEFS 
 */
// LED state
struct app_led_state_t
{
    uint8_t on_off_state;
    uint16_t level;
    uint16_t ctl_lightness;
    uint16_t ctl_temperature;
    uint16_t ctl_hue;
};


/*
 * PUBLIC FUNCTIONS 
 */
void app_led_init(void);
uint8_t app_led_get_onoffstate(uint8_t index);
uint16_t app_led_get_level(uint8_t index);
uint16_t app_led_get_ctl_lightness(uint8_t index);
uint16_t app_led_get_ctl_temperature(uint8_t index);
uint16_t app_led_get_hsl(uint8_t index);
void app_led_set_onoffstate(uint8_t index, uint8_t state);
void app_led_set_level(uint8_t index, uint16_t level);
void app_led_set_CTL(uint8_t index, uint16_t lightness, uint16_t temperature);
void app_led_set_hsl(uint8_t index, uint16_t hue);

#endif

