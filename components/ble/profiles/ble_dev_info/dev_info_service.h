/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef DEV_INFO_SERVICE_H
#define DEV_INFO_SERVICE_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdint.h>

/*
 * MACROS (宏定义)
 */

/// Manufacturer Name Value
#define DIS_MANUFACTURER_NAME       ("Freqchip")
#define DIS_MANUFACTURER_NAME_LEN   (8)

/// Model Number String Value
#define DIS_MODEL_NB_STR            ("Fr-BLE-1.0")
#define DIS_MODEL_NB_STR_LEN        (10)

/// Firmware Revision
#define DIS_FIRM_REV_STR            ("6.1.2")
#define DIS_FIRM_REV_STR_LEN        (5)

/// System ID Value - LSB -> MSB
#define DIS_SYSTEM_ID               ("\x12\x34\x56\xFF\xFE\x9A\xBC\xDE")
#define DIS_SYSTEM_ID_LEN           (8)

/// Software Revision String
#define DIS_SW_REV_STR              ("6.3.0")
#define DIS_SW_REV_STR_LEN          (5)

/**
 * PNP ID Value - LSB -> MSB
 *      Vendor ID Source : 0x02 (USB Implementer鈥檚 Forum assigned Vendor ID value)
 *      Vendor ID : 0x045E      (Microsoft Corp)
 *      Product ID : 0x0040
 *      Product Version : 0x0300
 */
#define DIS_PNP_ID               ("\x01\x17\x27\xb0\x32\x10\x24")
#define DIS_PNP_ID_LEN           (7)


/// Serial Number
#define DIS_SERIAL_NB_STR           ("1.0.0.0-LE")
#define DIS_SERIAL_NB_STR_LEN       (10)

/// Hardware Revision String
#define DIS_HARD_REV_STR           ("1.0.0")
#define DIS_HARD_REV_STR_LEN       (5)

/// IEEE
#define DIS_IEEE                    ("\xFF\xEE\xDD\xCC\xBB\xAA")
#define DIS_IEEE_LEN                (6)



///Indicate if Manufacturer Name String Char. is supported
#define    DIS_MANUFACTURER_NAME_CHAR_SUP       0x0001
    ///Indicate if Model Number String Char. is supported
#define    DIS_MODEL_NB_STR_CHAR_SUP            0x0002
    ///Indicate if Serial Number String Char. is supported
#define    DIS_SERIAL_NB_STR_CHAR_SUP           0x0004
    ///Indicate if Hardware Revision String Char. supports indications
#define    DIS_HARD_REV_STR_CHAR_SUP            0x0008
    ///Indicate if Firmware Revision String Char. is writable
#define    DIS_FIRM_REV_STR_CHAR_SUP            0x0010
    ///Indicate if Software Revision String Char. is writable
#define    DIS_SW_REV_STR_CHAR_SUP              0x0020
    ///Indicate if System ID Char. is writable
#define    DIS_SYSTEM_ID_CHAR_SUP               0x0040
    ///Indicate if IEEE Char. is writable
#define    DIS_IEEE_CHAR_SUP                    0x0080
    ///Indicate if PnP ID Char. is writable
#define    DIS_PNP_ID_CHAR_SUP                  0x0100
    ///All features are supported
#define    DIS_ALL_FEAT_SUP                     0x01FF

#define DIS_FEATURES             (DIS_MANUFACTURER_NAME_CHAR_SUP |\
                                      DIS_MODEL_NB_STR_CHAR_SUP      |\
                                      DIS_SYSTEM_ID_CHAR_SUP         |\
                                      DIS_FIRM_REV_STR_CHAR_SUP     |\
                                      DIS_SW_REV_STR_CHAR_SUP |\
                                      DIS_PNP_ID_CHAR_SUP)

/*
 * CONSTANTS (常量定义)
 */
// Simple Profile attributes index.
enum
{
    DIS_IDX_SERVICE,

#if ((DIS_FEATURES & DIS_MANUFACTURER_NAME_CHAR_SUP) == DIS_MANUFACTURER_NAME_CHAR_SUP)
    DIS_IDX_MANUFACTURER_NAME_CHAR_DECLARATION,
    DIS_IDX_MANUFACTURER_NAME_VALUE,
#endif

#if ((DIS_FEATURES & DIS_MODEL_NB_STR_CHAR_SUP) == DIS_MODEL_NB_STR_CHAR_SUP)
    DIS_IDX_MODEL_NUMBER_CHAR_DECLARATION,
    DIS_IDX_MODEL_NUMBER_VALUE,
#endif

#if ((DIS_FEATURES & DIS_SERIAL_NB_STR_CHAR_SUP) == DIS_SERIAL_NB_STR_CHAR_SUP)
    DIS_IDX_SERIAL_NUMBER_CHAR_DECLARATION,
    DIS_IDX_SERIAL_NUMBER_VALUE,
#endif

#if ((DIS_FEATURES & DIS_HARD_REV_STR_CHAR_SUP) == DIS_HARD_REV_STR_CHAR_SUP)
    DIS_IDX_HARDWARE_REVISION_CHAR_DECLARATION,
    DIS_IDX_HARDWARE_REVISION_VALUE,
#endif

#if ((DIS_FEATURES & DIS_FIRM_REV_STR_CHAR_SUP) == DIS_FIRM_REV_STR_CHAR_SUP)
    DIS_IDX_FIRMWARE_REVISION_CHAR_DECLARATION,
    DIS_IDX_FIRMWARE_REVISION_VALUE,
#endif

#if ((DIS_FEATURES & DIS_SW_REV_STR_CHAR_SUP) == DIS_SW_REV_STR_CHAR_SUP)
    DIS_IDX_SOFTWARE_REVISION_CHAR_DECLARATION,
    DIS_IDX_SOFTWARE_REVISION_VALUE,
#endif

#if ((DIS_FEATURES & DIS_SYSTEM_ID_CHAR_SUP) == DIS_SYSTEM_ID_CHAR_SUP)
    DIS_IDX_SYSTEM_ID_CHAR_DECLARATION,
    DIS_IDX_SYSTEM_ID_VALUE,
#endif

#if ((DIS_FEATURES & DIS_IEEE_CHAR_SUP) == DIS_IEEE_CHAR_SUP)
    DIS_IDX_IEEE_11073_CHAR_DECLARATION,
    DIS_IDX_IEEE_11073_VALUE,
#endif

#if ((DIS_FEATURES & DIS_PNP_ID_CHAR_SUP) == DIS_PNP_ID_CHAR_SUP)
    DIS_IDX_PNP_ID_CHAR_DECLARATION,
    DIS_IDX_PNP_ID_VALUE,
#endif

    DIS_IDX_NB,
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
* @fn      dis_gatt_add_service
*
* @brief   Create device information server.
*
* @param   None.
* 
* @return  None.
*/
void dis_gatt_add_service(void);



#endif







