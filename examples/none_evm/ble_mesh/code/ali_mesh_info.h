/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _ALI_MESH_INFO_H_
#define _ALI_MESH_INFO_H_

/*
 * INCLUDES 
 */
#include <stdbool.h>

/*
 * MACROS 
 */

/*
 * CONSTANTS 
 */
//#define MESH_QUICK_SWITCH_CTRL 
/* flash space used to store ble mesh secret key, such as public key, private key, etc. */
#define MESH_SECRET_KEY_ADDR                0x61000
/* flash space used to store mesh network information, such as app key, network key, etc. */
#define MESH_INFO_STORE_ADDR                0x62000
/* flash space used to store user data information, such as on-off state, lightness, etc. */
#define APP_MESH_USER_DATA_STORED_OFFSET    0x63000
#ifdef MESH_QUICK_SWITCH_CTRL
/* flash space used to store quick switch time,to exit the network */
#define MESH_STORE_SWITCH_TIME              0x64000
#endif
/* 
 * flash space used to store ali mesh information, this value should
 * be same with configuration in config tools.
 */
#define APP_MESH_ALI_INFO_OFFSET            0x60000
#define APP_MESH_ALI_INFO_PID               (APP_MESH_ALI_INFO_OFFSET + 4)
#define APP_MESH_ALI_INFO_ADDRESS           (APP_MESH_ALI_INFO_OFFSET + 8)
#define APP_MESH_ALI_INFO_KEY               (APP_MESH_ALI_INFO_OFFSET + 14)
#define APP_MESH_ALI_INFO_MAGIC_END         (APP_MESH_ALI_INFO_OFFSET + 30)


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
void app_mesh_store_info_timer_start(uint16_t duration);

/*********************************************************************
 * @fn      app_mesh_store_info_timer_init
 *
 * @brief   initialize delay storage timer.
 *
 * @param   None
 *
 * @return  None.
 */
void app_mesh_store_info_timer_init(void);

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
bool app_mesh_ali_info_check_valid(void);

/*********************************************************************
 * @fn      app_mesh_ali_info_load_pid
 *
 * @brief   get PID from flash.
 *
 * @param   data    - used to store result
 *
 * @return  None.
 */
void app_mesh_ali_info_load_pid(uint8_t *data);

/*********************************************************************
 * @fn      app_mesh_ali_info_load_bdaddr
 *
 * @brief   get bdaddr from flash.
 *
 * @param   data    - used to store result
 *
 * @return  None.
 */
void app_mesh_ali_info_load_bdaddr(uint8_t *data);

/*********************************************************************
 * @fn      app_mesh_ali_info_load_key
 *
 * @brief   get secret key from flash.
 *
 * @param   data    - used to store result
 *
 * @return  None.
 */
void app_mesh_ali_info_load_key(uint8_t *data);

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
bool app_mesh_user_data_load(uint8_t *msg, uint8_t msg_len);

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
void app_mesh_user_data_update(uint8_t *msg, uint8_t msg_len, uint32_t delay);

/*********************************************************************
 * @fn      app_mesh_user_data_clear
 *
 * @brief   clear user data storage space in flash.
 *
 * @param   None.
 *
 * @return  None.
 */
void app_mesh_user_data_clear(void);

/*********************************************************************
 * @fn      app_mesh_store_user_data_timer_init
 *
 * @brief   initialize delay storage timer.
 *
 * @param   None
 *
 * @return  None.
 */
void app_mesh_store_user_data_timer_init(void);

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
void app_mesh_store_switch_time(void);

/*********************************************************************
 * @fn      app_mesh_clear_switch_time
 *
 * @brief   clear quick switch time.
 *
 * @param   None
 *
 * @return  None.
 */
void app_mesh_clear_switch_time(void);
#endif

#endif  // _ALI_MESH_INFO_H_

