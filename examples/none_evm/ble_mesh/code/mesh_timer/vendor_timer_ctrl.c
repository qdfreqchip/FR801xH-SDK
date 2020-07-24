/**
 * Copyright (c) 2020, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

/*
 * INCLUDES (包含头文件)
 */
#include <stdio.h>

#include "os_mem.h"
#include "mesh_api.h"
#include "mesh_model_msg.h"
#include "ali_mesh_led_driver.h"
#include "co_printf.h"
#include "sys_utils.h"

#include "vendor_timer_ctrl.h"
/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */

/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */

/*
 * LOCAL VARIABLES (本地变量)
 */
static uint8_t vendor_tid = 0;
static struct vendor_set_timer_s timer_buff[VENDOR_TIMER_MAX];

/*
 * PUBLIC FUNCTIONS (全局函数)
 */
extern void app_mesh_start_publish_msg_resend(uint8_t * p_msg,uint8_t p_len);

void vendor_indication_rsp(uint8_t opcode,uint16_t attr_type,uint8_t * attr_param,uint8_t len)
{
    struct app_mesh_led_vendor_model_indication_t *indication;
    mesh_publish_msg_t *msg = (mesh_publish_msg_t *)os_malloc(sizeof(mesh_publish_msg_t) + len);
    msg->element_idx = 0;
    msg->model_id = MESH_MODEL_ID_VENDOR_ALI;
    msg->opcode = MESH_VENDOR_INDICATION;

    msg->msg_len = sizeof(struct app_mesh_led_vendor_model_status_t)+len;
    indication = (struct app_mesh_led_vendor_model_indication_t *)msg->msg;
    indication->tid = vendor_tid;
    indication->attr_type = attr_type;
    memcpy(indication->attr_parameter,attr_param,len);

    mesh_publish_msg(msg);
    app_mesh_start_publish_msg_resend(msg->msg,msg->msg_len);
    os_free(msg);
    vendor_tid ++;
}

void vendor_sync_time(int unix_time)
{
    set_data_form_timestamp(unix_time);
}

/*********************************************************************
 * @fn      vendor_set_timer_case
 *
 * @brief   timer opration from the Tmall.
 *
 * @param   ind - message received from remote node.
 *
 * @return  None.
 */
void vendor_set_timer_case(mesh_model_msg_ind_t const * ind)
{
    //uint16_t attr_type = 0;
    int * unix_time = 0;
    uint8_t i = 0/*,j = 0*/,timer_idx = 0;
    uint8_t timer_msg_len = sizeof(struct vendor_set_timer_s);

    struct app_mesh_led_vendor_model_set_new_t *vendor_set;
    vendor_set = (struct app_mesh_led_vendor_model_set_new_t *)ind->msg;  

    co_printf("=vendor_set_timer_case=%x\r\n",ind->opcode);
    switch(ind->opcode)
    {
        case VENDOR_QUERY_TIME:
            if(vendor_set->attr_type == 0xf01f) // query device time msg
            {
                // upload the time
            }
            break;
        case VENDOR_SET_OR_DEL_TIME:
            switch(vendor_set->attr_type)
            {
                case 0xf010: // set single timer opration
                    for(i = 0;i < (ind->msg_len-3)/sizeof(struct set_timer_s);i++)
                    {
                        timer_idx = vendor_set->attr_parameter[i+i*(sizeof(struct set_timer_s))];
                        co_printf("=set single timer=%d\r\n",timer_idx);
                        
                        if(timer_idx)
                        {
                            memcpy(&(timer_buff[(timer_idx-1)].msg),
                                (struct set_timer_s *)&(vendor_set->attr_parameter[i+1+i*(sizeof(struct set_timer_s))]),
                                sizeof(struct set_timer_s));
                            if((timer_buff[(timer_idx-1)].msg.msg_t.unix_t - get_current_time_sec()) < 60)
                                timer_buff[(timer_idx-1)].msg.msg_t.unix_t = get_current_time_sec()+60;
                            timer_buff[(timer_idx-1)].loop_timer_flag = 0;
                            timer_buff[(timer_idx-1)].timer_idx = timer_idx;
                            timer_buff[(timer_idx-1)].timer_valid = 1;
                            #if 0
                            co_printf("attr=%x,data=%x,t=%x\r\n",timer_buff[(timer_idx-1)].msg.attr_type,\
                            timer_buff[(timer_idx-1)].msg.attr_data,timer_buff[(timer_idx-1)].msg.msg_t.unix_t);
                            #endif
                        }
                    }
                    //vendor_indication_rsp(0,0xf010,(uint8_t *)ind->msg,ind->msg_len);
                    break;
                case 0xf011: // set loop timer opration
                    for(i = 0;i < (ind->msg_len-3)/sizeof(struct set_timer_s);i++)
                    {
                        timer_idx = vendor_set->attr_parameter[i*(sizeof(struct set_timer_s))];
                        if(timer_idx)
                        {
                            timer_buff[(timer_idx-1)].loop_timer_flag = 1;
                            timer_buff[(timer_idx-1)].timer_idx = timer_idx;
                            memcpy(&(timer_buff[(timer_idx-1)].msg),
                                (struct set_timer_s *)&(vendor_set->attr_parameter[i*(sizeof(struct set_timer_s))]),
                                sizeof(struct set_timer_s));
                            timer_buff[(timer_idx-1)].timer_valid = 1;
                        }
                    }
                    //vendor_indication_rsp(0,0xf011,(uint8_t *)ind->msg,ind->msg_len);
                    break;
                case 0xf012: // del timer
                    for(i = 0;i < (ind->msg_len-3);i++)
                    {
                        if(vendor_set->attr_parameter[i])
                            memset(&timer_buff[(vendor_set->attr_parameter[i]-1)],0,timer_msg_len);
                    }
                    //vendor_indication_rsp(0,0xf012,(uint8_t *)ind->msg,ind->msg_len);
                    break;
                case 0xf01f: // sync time
                    unix_time = (int *)vendor_set->attr_parameter;
                    vendor_sync_time(*unix_time);
                    break;
                default:
                    break;
            }
            break;
        case VENDOR_UPDATE_TIME:
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      vendor_check_timer_opration
 *
 * @brief   timer cycle check.
 *
 * @param   check_t - the check time.
 *
 * @return  None.
 */
void vendor_check_timer_opration(clock_param_t check_t)
{
    uint16_t check_min = 0,check_min_b = 0;
    uint32_t check_unix_t = 0/*,check_unix_b = 0*/;
    uint8_t i = 0;
    uint8_t attr_msg[8] = {0}/*,timer_over_idx = 0*/;
    //uint8_t timer_msg_len = sizeof(struct vendor_set_timer_s);

    //co_printf("===check timer===\r\n");
    //show_clock_func();
    for(i = 0;i < VENDOR_TIMER_MAX;i++)
    {
        if(timer_buff[i].timer_valid != 1)
            continue;
            
        if(timer_buff[i].loop_timer_flag)
        {
            if(timer_buff[i].msg.msg_t.loop_t.schedule & (BIT(check_t.week-1))) // week bit
            {
                check_min = timer_buff[i].msg.msg_t.loop_t.time & 0x0fff;
                check_min_b = 60*check_t.hour+check_t.min;
                if(check_min == check_min_b) // loop timer ctrl
                {
                    co_printf("=loop timer on=\r\n");
                    switch(timer_buff[i].msg.attr_type)
                    {
                        case 0x0100: // on-off
                            if(timer_buff[i].msg.attr_data)
                                co_printf("=on=\r\n");
                            else
                                co_printf("=off=\r\n");
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        else
        {
            check_unix_t = get_sec_from_time(check_t.year,check_t.month,check_t.day,\
                                                check_t.hour,check_t.min,check_t.sec);
            //check_unix_b = (timer_buff[i].msg.msg_t.unix_t/60)*60;                                  
            if(timer_buff[i].msg.msg_t.unix_t == check_unix_t)
            {
                co_printf("=single timer on=\r\n");
                switch(timer_buff[i].msg.attr_type)
                {
                    case 0x0100: // on-off
                        if(timer_buff[i].msg.attr_data)
                            co_printf("=on=\r\n");
                        else
                            co_printf("=off=\r\n");
                        break;
                    default:
                        break;
                }

                memset(&timer_buff[i],0,sizeof(struct vendor_set_timer_s));

                attr_msg[0] = 0x11; // timer over
                attr_msg[1] = i+1; // timer idx
                vendor_indication_rsp(1,0xf009,attr_msg,2);
            }
        }
    }
}



