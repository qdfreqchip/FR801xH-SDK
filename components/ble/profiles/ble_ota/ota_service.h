/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef OTA_SERVICE_H
#define OTA_SERVICE_H

/*
 * INCLUDES (包含头文件)
 */

/*
 * MACROS (宏定义)
 */
#define OTA_SVC_UUID                {0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x02} 
    
#define OTA_CHAR_UUID_TX            {0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x02}
#define OTA_CHAR_UUID_RX            {0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x02}
#define OTA_CHAR_UUID_NOTI          {0x02, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x02}
#define OTA_CHAR_UUID_VERSION_INFO  {0x03, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x02}
    
#define OTAS_MAX_DATA_SIZE              600
#define OTAS_NOTIFY_DATA_SIZE           20

/*
 * CONSTANTS (常量定义)
 */
enum
{
    OTA_ATT_IDX_SERVICE,

    OTA_ATT_IDX_CHAR_DECLARATION_VERSION_INFO,
    OTA_ATT_IDX_CHAR_VALUE_VERSION_INFO,

    OTA_ATT_IDX_CHAR_DECLARATION_NOTI,
    OTA_ATT_IDX_CHAR_VALUE_NOTI,
    OTA_ATT_IDX_CHAR_CFG_NOTI,
    OTA_IDX_CHAR_USER_DESCRIPTION_NOTI,

    OTA_ATT_IDX_CHAR_DECLARATION_TX,
    OTA_ATT_IDX_CHAR_VALUE_TX,

    OTA_ATT_IDX_CHAR_DECLARATION_RX,
    OTA_ATT_IDX_CHAR_VALUE_RX,

    OTA_ATT_NB,
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
 * @fn      ota_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *          添加GATT service到ATT的数据库里面。
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void ota_gatt_add_service(void);
/*********************************************************************
 * @fn      ota_gatt_report_notify
 *
 * @brief   Send ota protocol response data.
 *
 *
 * @param   rpt_info_id - report idx of the hid_rpt_info array.
 *          len         - length of the HID information data.
 *          p_data      - data of the HID information to be sent.
 *
 * @return  none.
 */
void ota_gatt_report_notify(uint8_t conidx, uint8_t *p_data, uint16_t len);

#endif

