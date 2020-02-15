/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef SPEAKER_SERVICE_H
#define SPEAKER_SERVICE_H

/*
 * INCLUDES 
 */
#include <stdio.h>
#include <string.h>
#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"


/*
 * MACROS
 */

/*
 * CONSTANTS 
 */
// Simple Profile attributes index. 
enum
{
    SPEAKER_IDX_SERVICE,

    SPEAKER_IDX_ONOFF_DECLARATION,
    SPEAKER_IDX_ONOFF_VALUE,
    SPEAKER_IDX_ONOFF_USER_DESCRIPTION,

    SPEAKER_IDX_STATUS_REPORT_DECLARATION,
    SPEAKER_IDX_STATUS_REPORT_VALUE,
    SPEAKER_IDX_STATUS_REPORT_CFG,
    SPEAKER_IDX_STATUS_REPORT_USER_DESCRIPTION,

    SPEAKER_IDX_AUDIO_RX_DECLARATION,
    SPEAKER_IDX_AUDIO_RX_VALUE,
    
    SPEAKER_IDX_NB,
};

// Simple GATT Profile Service UUID
#define SPEAKER_SVC_UUID              0xD0ff

#define SPEAKER_ONOFF_UUID            0xD001
#define SPEAKER_STATUS_REPORT_UUID    0xD002
#define SPEAKER_AURIO_RX_UUID         0xD003



/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */
extern const gatt_attribute_t speaker_att_table[];
extern uint8_t slave_link_conidx;

/*
 * LOCAL VARIABLES (本地变量)
 */


/*
 * PUBLIC FUNCTIONS (全局函数)
 */
/*********************************************************************
 * @fn      speaker_gatt_add_service
 *
 * @brief   Speaker Profile add GATT service function.
 *			添加GATT service到ATT的数据库里面。
 *
 * @param   None. 
 *        
 *
 * @return  None.
 */
void speaker_gatt_add_service(void);




#endif







