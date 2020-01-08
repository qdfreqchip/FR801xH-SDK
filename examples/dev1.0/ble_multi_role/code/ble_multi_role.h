/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
#ifndef BLE_MULTI_ROLE_H
#define BLE_MULTI_ROLE_H
 
 /*
 * INCLUDES (包含头文件)
 */

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
 * @fn      multi_role_init
 *
 * @brief   Initialize multi role, including peripheral & central roles, 
 *          and BLE related parameters.
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void multi_role_init(void);

#endif
