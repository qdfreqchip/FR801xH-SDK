/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef HID_SERVICE_H
#define HID_SERVICE_H

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
/// Keyboard Device
#define HID_DEV_KEYBOARD    0x01
/// Mouse Device
#define HID_DEV_MOUSE       0x02
/// Protocol Mode present
#define HID_DEV_PROTO_MODE  0x04
/// Extended Reference Present
#define HID_DEV_MAP_EXT_REF 0x08


/*
*NOTICE: User shold define hid device and report information array number. 
*        Current only support keyboard device.
*/
#define HID_DEV (HID_DEV_KEYBOARD)
// Number of HID reports defined in the service
#define HID_NUM_REPORTS          4      

/*
 * CONSTANTS (常量定义)
 */

/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE           0x01 // RemoteWake
#define HID_FLAGS_NORMALLY_CONNECTABLE  0x02 // NormallyConnectable

/* HID protocol mode values */
#define HID_PROTOCOL_MODE_BOOT      0x00 // Boot Protocol Mode
#define HID_PROTOCOL_MODE_REPORT    0x01 // Report Protocol Mode

/** @defgroup HID_REPORT_TYPE_GROUP */
#define HID_REPORT_TYPE_INPUT       0x01
#define HID_REPORT_TYPE_OUTPUT      0x02
#define HID_REPORT_TYPE_FEATURE     0x03
#define HID_REPORT_TYPE_WR          0x10    /// Input report with Write capabilities


// HID service attributes index.
// Attribute index enumeration-- these indexes match array elements above
enum
{
    HID_SERVICE_IDX,                // HID Service

#if ((HID_DEV & HID_DEV_MAP_EXT_REF) == HID_DEV_MAP_EXT_REF)
    HID_INCLUDED_SERVICE_IDX,       // Included Service
#endif

    HID_INFO_DECL_IDX,              // HID Information characteristic declaration
    HID_INFO_IDX,                   // HID Information characteristic

    HID_CONTROL_POINT_DECL_IDX,     // HID Control Point characteristic declaration
    HID_CONTROL_POINT_IDX,          // HID Control Point characteristic

    HID_REPORT_MAP_DECL_IDX,        // HID Report Map characteristic declaration
    HID_REPORT_MAP_IDX,             // HID Report Map characteristic

#if ((HID_DEV & HID_DEV_MAP_EXT_REF) == HID_DEV_MAP_EXT_REF)
    HID_REPORT_MAP_EXT_REP_REF,
#endif

#if ((HID_DEV & HID_DEV_PROTO_MODE) == HID_DEV_PROTO_MODE)
    HID_PROTOCOL_MODE_DECL_IDX,     // HID Protocol Mode characteristic declaration
    HID_PROTOCOL_MODE_IDX,          // HID Protocol Mode characteristic
#endif

#if ((HID_DEV & HID_DEV_KEYBOARD) == HID_DEV_KEYBOARD)
    HID_BOOT_KEY_IN_DECL_IDX,       // HID Boot Keyboard Input Report declaration
    HID_BOOT_KEY_IN_IDX,            // HID Boot Keyboard Input Report
    HID_BOOT_KEY_IN_CCCD_IDX,       // HID Boot Keyboard Input Report characteristic client characteristic configuration

    HID_BOOT_KEY_OUT_DECL_IDX,      // HID Boot Keyboard Output Report declaration
    HID_BOOT_KEY_OUT_IDX,           // HID Boot Keyboard Output Report
#endif

#if ((HID_DEV & HID_DEV_MOUSE) == HID_DEV_MOUSE)
    HID_BOOT_MOUSE_IN_DECL_IDX,     // HID Boot Mouse Input Report declaration
    HID_BOOT_MOUSE_IN_IDX,          // HID Boot Mouse Input Report
    HID_BOOT_MOUSE_IN_CCCD_IDX,     // HID Boot Mouse Input Report characteristic client characteristic configuration
#endif
    HID_IDX_ATT_UNIQ_NB,

    HID_FEATURE_DECL_IDX = HID_IDX_ATT_UNIQ_NB,           // Feature Report declaration
    HID_FEATURE_IDX,                // Feature Report
    HID_REPORT_REF_FEATURE_IDX,     // HID Report Reference characteristic descriptor, feature
    HID_FEATURE_CCCD_IDX,

    HID_ATT_NB = HID_IDX_ATT_UNIQ_NB + (4* (HID_NUM_REPORTS)) - 1,
};
/*
 * TYPEDEFS (类型定义)
 */

// HID report mapping table
typedef struct
{
    ///Report ID, refer to report map.
    uint8_t report_id;
    ///Report Type, define report id type, see HID Report type
    uint8_t report_type;
} hid_report_ref_t;

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
 * @fn      hid_gatt_add_service
 *
 * @brief   Create hid server.
 *
 * @param   None.
 * 
 * @return  None.
 */
void hid_gatt_add_service(void);
/*********************************************************************
 * @fn      hid_gatt_report_notify
 *
 * @brief   Send HID notification, keys, mouse values, etc.
 *
 *
 * @param   rpt_info_id - report idx, see hid_report_ref_t hid_rpt_info[HID_NUM_REPORTS].
 *          len         - length of the HID information data.
 *          p_data      - data of the HID information to be sent.
 *
 * @return  none.
 */
void hid_gatt_report_notify(uint8_t conidx, uint8_t rpt_info_id, uint8_t *p_data, uint16_t len);

#endif







