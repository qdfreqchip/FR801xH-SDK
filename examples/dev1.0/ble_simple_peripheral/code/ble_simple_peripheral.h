/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef BLE_SIMPLE_PERIPHERAL_H
#define BLE_SIMPLE_PERIPHERAL_H
 
 /*
 * INCLUDES (包含头文件)
 */
#include "gap_api.h"
/*
 * MACROS (宏定义)
 */
#define PIC1_ADDR 0x43000
#define PIC2_ADDR 0x60000

/*
 * CONSTANTS (常量定义)
 */    
extern const unsigned	char * lcd_show_workmode[];

/*
 * TYPEDEFS (类型定义)
 */
enum
{
    GATT_SVC_IDX_SP,
    GATT_SVC_NUM    
};

enum WORK_MODE
{
    PICTURE_UPDATE,
    SENSOR_DATA,
    SPEAKER_FROM_FLASH,
    CODEC_TEST,
    MODE_MAX,
};

/*
 * GLOBAL VARIABLES (全局变量)
 */
extern 	uint8_t App_Mode;



/*
 * LOCAL VARIABLES (本地变量)
 */
 
/*
 * LOCAL FUNCTIONS (本地函数)
 */

/*
 * EXTERN FUNCTIONS (外部函数)
 */

/*
 * PUBLIC FUNCTIONS (全局函数)
 */


/** @function group ble peripheral device APIs (ble外设相关的API)
 * @{
 */
 
void app_gap_evt_cb(gap_event_t *p_event);


/*********************************************************************
 * @fn      simple_peripheral_init
 *
 * @brief   Initialize simple peripheral profile, BLE related parameters.
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void simple_peripheral_init(void);

#endif
