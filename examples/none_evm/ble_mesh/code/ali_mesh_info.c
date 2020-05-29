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
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "driver_flash.h"

#include "os_timer.h"
#include "os_mem.h"
#include "mesh_api.h"
#include "ali_mesh_info.h"
#include "co_printf.h"

/*
 * MACROS
 */

/*
 * CONSTANTS
 */

/*
 * TYPEDEFS
 */
typedef struct app_mesh_user_data
{
    uint8_t magic_start[4];
    uint8_t on_off_state;
    uint16_t level;
    uint16_t ctl_lightness;
    uint16_t ctl_temperature;
    uint16_t ctl_hue;
    uint8_t magic_end[4];
} app_mesh_user_data_t;


/*
 * GLOBAL VARIABLES
 */

/*
 * LOCAL VARIABLES
 */
/* 
 * use a timer to do mesh network information storage to avoid
 * execute flash operation too frequently.
 */
static os_timer_t mesh_store_info_timer;
static os_timer_t mesh_store_user_data_timer;

static uint8_t *user_data_ptr = NULL;
static uint8_t user_data_len = 0;

/*********************************************************************
 * @fn      app_mesh_store_info_timer_handler
 *
 * @brief   mesh_store_info_timer timeout handler, store mesh network
 *          information into flash, such as app key, network key, etc.
 *
 * @param   arg     - timer parameters
 *
 * @return  None.
 */
static void app_mesh_store_info_timer_handler(void * arg)
{
    mesh_info_store_into_flash();
}

/*********************************************************************
 * @fn      app_mesh_store_info_timer_start
 *
 * @brief   once MESH_EVT_UPDATE_IND message is receivec, this function
 *          should be called to start delay storage timer.
 *
 * @param   duration    - delay storage timer duration
 *
 * @return  None.
 */
void app_mesh_store_info_timer_start(uint16_t duration)
{
    os_timer_stop(&mesh_store_info_timer);
    os_timer_start(&mesh_store_info_timer, duration, 0);
}

/*********************************************************************
 * @fn      app_mesh_store_info_timer_init
 *
 * @brief   initialize delay storage timer.
 *
 * @param   None
 *
 * @return  None.
 */
void app_mesh_store_info_timer_init(void)
{
	os_timer_init(&mesh_store_info_timer, app_mesh_store_info_timer_handler, NULL);
}

/*********************************************************************
 * @fn      app_mesh_ali_info_check_valid
 *
 * @brief   used to check whether valid ali mesh information is stored
 *          in flash.
 *
 * @param   None
 *
 * @return  valid information is existed or not.
 */
bool app_mesh_ali_info_check_valid(void)
{
    uint8_t magic[4];

    flash_read(APP_MESH_ALI_INFO_OFFSET, 4, magic);
    if(memcmp(magic, "freq", 4) == 0) 
    {
        flash_read(APP_MESH_ALI_INFO_MAGIC_END, 4, magic);
        if(memcmp(magic, "chip", 4) == 0) 
        {
            return true;
        }
    }

    return false;
}

/*********************************************************************
 * @fn      app_mesh_ali_info_load_pid
 *
 * @brief   get PID from flash.
 *
 * @param   data    - used to store result
 *
 * @return  None.
 */
void app_mesh_ali_info_load_pid(uint8_t *data)
{
    flash_read(APP_MESH_ALI_INFO_PID, 4, data);
}

/*********************************************************************
 * @fn      app_mesh_ali_info_load_bdaddr
 *
 * @brief   get bdaddr from flash.
 *
 * @param   data    - used to store result
 *
 * @return  None.
 */
void app_mesh_ali_info_load_bdaddr(uint8_t *data)
{
    flash_read(APP_MESH_ALI_INFO_ADDRESS, 6, data);
}

/*********************************************************************
 * @fn      app_mesh_ali_info_load_key
 *
 * @brief   get secret key from flash.
 *
 * @param   data    - used to store result
 *
 * @return  None.
 */
void app_mesh_ali_info_load_key(uint8_t *data)
{
    flash_read(APP_MESH_ALI_INFO_KEY, 16, data);
    #if 0
    co_printf("app_mesh_ali_info_load_key: ");
    for(uint8_t i=0; i<16; i++) 
    {
        co_printf("0x%02x,", data[i]);
    }
    co_printf("\r\n");
    #endif
}

/*********************************************************************
 * @fn      app_mesh_user_data_load
 *
 * @brief   get user data from flash, such as on_off state, lightness, etc.
 *
 * @param   msg     - user data pointer
 *          msg_len - user data length
 *
 * @return  None.
 */
bool app_mesh_user_data_load(uint8_t *msg, uint8_t msg_len)
{
    uint8_t data[4];
    
    flash_read(APP_MESH_USER_DATA_STORED_OFFSET, 4, data);

    if(memcmp(data, "freq", 4) == 0)
    {
        flash_read(APP_MESH_USER_DATA_STORED_OFFSET+4+msg_len, 4, data);
        if(memcmp(data, "chip", 4) == 0)
        {
            flash_read(APP_MESH_USER_DATA_STORED_OFFSET+4, msg_len, msg);

            return true;
        }
    }

    return false;
}

/*********************************************************************
 * @fn      app_mesh_store_user_data_timer_handler
 *
 * @brief   mesh_store_user_data_timer timeout handler, store user data
 *          into flash.
 *
 * @param   arg     - timer parameters
 *
 * @return  None.
 */
static void app_mesh_store_user_data_timer_handler(void *arg)
{
    flash_erase(APP_MESH_USER_DATA_STORED_OFFSET, 0x1000);
    flash_write(APP_MESH_USER_DATA_STORED_OFFSET, 4, "freq");
    flash_write(APP_MESH_USER_DATA_STORED_OFFSET+4, user_data_len, user_data_ptr);
    flash_write(APP_MESH_USER_DATA_STORED_OFFSET+4+user_data_len, 4, "chip");
}

/*********************************************************************
 * @fn      app_mesh_user_data_update
 *
 * @brief   update user data to flash, such as on_off state, lightness, etc.
 *
 * @param   msg     - user data pointer
 *          msg_len - user data length
 *          delay   - use delay to avoid execute flash operation too frequently.
 *
 * @return  None.
 */
void app_mesh_user_data_update(uint8_t *msg, uint8_t msg_len, uint32_t delay)
{
    if(user_data_ptr == NULL)
    {
        user_data_ptr = os_malloc(msg_len);
        user_data_len = msg_len;
    }
    else
    {
        if(user_data_len != msg_len)
        {
            os_free(user_data_ptr);
            user_data_ptr = os_malloc(msg_len);
            user_data_len = msg_len;
        }
    }
    memcpy(user_data_ptr, msg, user_data_len);

    if(delay == 0)
    {
        app_mesh_store_user_data_timer_handler(NULL);
        return;
    }
    
    os_timer_start(&mesh_store_user_data_timer, delay, 0);
}

/*********************************************************************
 * @fn      app_mesh_user_data_clear
 *
 * @brief   clear user data storage space in flash.
 *
 * @param   None.
 *
 * @return  None.
 */
void app_mesh_user_data_clear(void)
{
    flash_erase(APP_MESH_USER_DATA_STORED_OFFSET, 0x1000);
}

/*********************************************************************
 * @fn      app_mesh_store_user_data_timer_init
 *
 * @brief   initialize delay storage timer.
 *
 * @param   None
 *
 * @return  None.
 */
void app_mesh_store_user_data_timer_init(void)
{
	os_timer_init(&mesh_store_user_data_timer, app_mesh_store_user_data_timer_handler, NULL);
}

#ifdef MESH_QUICK_SWITCH_CTRL
/*********************************************************************
 * @fn      app_mesh_store_switch_time
 *
 * @brief   store quick switch time,to exit the network.
 *
 * @param   None
 *
 * @return  None.
 */
void app_mesh_store_switch_time(void)
{
    uint8_t switch_time = 0;
    flash_read(MESH_STORE_SWITCH_TIME,1,&switch_time);
    if(switch_time == 0xff)
        switch_time = 1;
    else
    {
        switch_time++;
    }
    flash_erase(MESH_STORE_SWITCH_TIME,0x1000);
    flash_write(MESH_STORE_SWITCH_TIME,1,&switch_time);
    co_printf("=switch time=%d\r\n",switch_time);
}

/*********************************************************************
 * @fn      app_mesh_clear_switch_time
 *
 * @brief   clear quick switch time.
 *
 * @param   None
 *
 * @return  None.
 */
void app_mesh_clear_switch_time(void)
{
    uint8_t switch_time = 0;
    flash_read(MESH_STORE_SWITCH_TIME,1,&switch_time);

    if(switch_time > 4)
    {
        co_printf("=mesh switch clear=\r\n");
        mesh_info_clear();
		app_mesh_user_data_clear();
    }
    flash_erase(MESH_STORE_SWITCH_TIME,0x1000);
}
#endif

