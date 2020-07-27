/**
 * Copyright (c) 2019, Tsingtao Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
* INCLUDES (包含头文件)
*/
#include <stdbool.h>
#include "gap_api.h"
#include "gatt_api.h"
#include "hid_service.h"
#include "ble_hid_kbd_mice.h"
#include "gatt_sig_uuid.h"
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

/*********************************************************************
 * @fn      hid_start_adv
 *
 * @brief   Set advertising data, advertising response data
 *					and set adv configration parameters.
 *
 * @param   None
 *       	
 *
 * @return  None
 */
void hid_start_adv(void)
{
    // GAP - Advertisement data (max size = 31 bytes, though this is
    // best kept short to conserve power while advertisting)
    // GAP-广播包的内容,最长31个字节.短一点的内容可以节省广播时的系统功耗.
    uint8_t adv_data[0x1C] =
    {
        // appearance
        0x03,   // length of this data
        GAP_ADVTYPE_APPEARANCE,
        LO_UINT16(GAP_APPEARE_GENERIC_HID),
        HI_UINT16(GAP_APPEARE_GENERIC_HID),

        // service UUIDs, to notify central devices what services are included
        // in this peripheral. 告诉central本机有什么服务, 但这里先只放一个主要的.
        0x03,   // length of this data
        GAP_ADVTYPE_16BIT_COMPLETE,
        LO_UINT16(HID_SERV_UUID),
        HI_UINT16(HID_SERV_UUID),
    };
    *(uint16_t *)(adv_data+2) = gap_get_dev_appearance();

    // GAP - Scan response data (max size = 31 bytes, though this is
    // best kept short to conserve power while advertisting)
    // GAP-Scan response内容,最长31个字节.短一点的内容可以节省广播时的系统功耗.
    uint8_t scan_rsp_data[0x1F] =
    {
        // complete name 设备名字
        0x11,   // length of this data
        GAP_ADVTYPE_LOCAL_NAME_COMPLETE,
        'B',
        'L',
        'E',
        ' ',
        'H',
        'I',
        'D',
        ' ',
        'K',
        'B',
        'D',
        ' ',
        'M',
        'I',
        'C',
        'E',

        // Tx power level 发射功率
        0x02,   // length of this data
        GAP_ADVTYPE_POWER_LEVEL,
        0,       // 0dBm
    };


    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 80;
    adv_param.adv_intv_max = 80;
    gap_set_advertising_param(&adv_param);

    gap_set_advertising_data(adv_data,sizeof(adv_data));
    gap_set_advertising_rsp_data(scan_rsp_data,sizeof(scan_rsp_data) );

    gap_start_advertising(0);
}




