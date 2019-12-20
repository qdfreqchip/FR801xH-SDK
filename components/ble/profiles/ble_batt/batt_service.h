/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef BATT_SERVICE_H
#define BATT_SERVICE_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdio.h>
#include <string.h>

#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"


/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */
// Battery Server Profile attributes index.
enum
{
    IDX_BATT_SERVICE,

    IDX_BATT_LEVEL_CHAR_DECLARATION,
    IDX_BATT_LEVEL_CHAR_VALUE,
    IDX_BATT_LEVEL_CCCD,

    IDX_BATT_NB,
};

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
 * PUBLIC FUNCTIONS (全局函数)
 */
 /*********************************************************************
 * @fn      batt_gatt_add_service
 *
 * @brief   Create battery server.
 *
 * @param   None.
 * 
 * @return  None.
 */
void batt_gatt_add_service(void);

/*********************************************************************
 * @fn      batt_gatt_notify
 *
 * @brief   Send batt level notification to peer, and update batt level
 *
 *
 * @param   conidx  - link idx.
 *          batt_level  - battery energy percentage.
 *
 * @return  none.
 */
void batt_gatt_notify(uint8_t conidx,uint8_t batt_level);


#endif







