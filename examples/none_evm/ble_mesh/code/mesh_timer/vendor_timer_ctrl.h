/**
 * Copyright (c) 2020, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef VENDOR_TIMER_CTRL_H
#define VENDOR_TIMER_CTRL_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdint.h>
#include "driver_plf.h"
#include "demo_clock.h"
/*
 * MACROS (宏定义)
 */
#define VENDOR_QUERY_TIME                 0x01A8D0
#define VENDOR_SET_OR_DEL_TIME            0x01A8D1
#define VENDOR_UPLOAD_TIME_MSG            0x01A8D3
#define VENDOR_UPDATE_TIME_REQ            0x01A8DE 
#define VENDOR_UPDATE_TIME                0x01A8DF

#define VENDOR_TIMER_MAX                  40
/*
 * CONSTANTS (常量定义)
 */

/*
 * TYPEDEFS (类型定义)
 */
__PACKED struct loop_timer_s
{
    uint32_t reserver:8;
    uint32_t time:16;
    uint32_t schedule:8;
}GCC_PACKED;

__PACKED struct set_timer_s
{
    __PACKED union
    {
        struct loop_timer_s loop_t;
        int unix_t; 
    }GCC_PACKED msg_t;    
    uint16_t attr_type;
    uint8_t attr_data;
}GCC_PACKED;

__PACKED struct vendor_set_timer_s
{
    struct set_timer_s msg;
    uint8_t loop_timer_flag;
    uint8_t timer_idx;
    uint8_t timer_valid;
}GCC_PACKED;

struct app_mesh_led_vendor_model_set_t
{
    uint8_t tid;
    uint8_t num_of_value;
    uint8_t attr_parameter[__ARRAY_EMPTY];
} __attribute__((packed)) ;

struct app_mesh_led_vendor_model_status_t
{
    uint8_t tid;
    uint8_t num_of_value;
    uint8_t attr_parameter[__ARRAY_EMPTY];
} __attribute__((packed)) ;

struct app_mesh_led_vendor_model_set_new_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[__ARRAY_EMPTY];
} __attribute__((packed)) ;

struct app_mesh_led_vendor_model_status_new_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[__ARRAY_EMPTY];
} __attribute__((packed)) ;


__PACKED struct app_mesh_led_vendor_model_indication_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[__ARRAY_EMPTY];
}GCC_PACKED;

/*
 * GLOBAL VARIABLES (全局变量)
 */

/*
 * LOCAL VARIABLES (本地变量)
 */


/*
 * PUBLIC FUNCTIONS (全局函数)
 */

/*********************************************************************
 * @fn      vendor_set_timer_case
 *
 * @brief   timer opration from the Tmall.
 *
 * @param   ind - message received from remote node.
 *
 * @return  None.
 */
void vendor_set_timer_case(mesh_model_msg_ind_t const * ind);

/*********************************************************************
 * @fn      vendor_check_timer_opration
 *
 * @brief   timer cycle check.
 *
 * @param   check_t - the check time.
 *
 * @return  None.
 */
void vendor_check_timer_opration(clock_param_t check_t);



#endif

