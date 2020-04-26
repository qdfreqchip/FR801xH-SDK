/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
 * INCLUDES (包含头文件)
 */
#include <stdio.h>
#include <string.h>
#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"
#include "sys_utils.h"

#include "hid_service.h"


/*
 * MACROS (宏定义)
 */
#define CFG_CON                     20

/*
 * CONSTANTS (常量定义)
 */
const uint8_t hid_svc_uuid[] = UUID16_ARR(HID_SERV_UUID);

// HID Information data
const uint8_t hid_info_value[] =
{
    LO_UINT16(0x0111), HI_UINT16(0x0111),             // bcdHID (USB HID version)
    0x00,                                             // bCountryCode
    HID_FLAGS_REMOTE_WAKE | HID_FLAGS_NORMALLY_CONNECTABLE     // Flags
};

/******************************* HID Report Map characteristic defination */
static const uint8_t hid_report_map[] =
{
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x02,  // Usage (Mouse)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x01,  // Report Id (1)
    0x09, 0x01,  //   Usage (Pointer)
    0xA1, 0x00,  //   Collection (Physical)

//rpt id == 1
    0x05, 0x09,  //     Usage Page (Mouse)
    0x19, 0x01,  //     Usage Minimum (01) - Button 1
    0x29, 0x03,  //     Usage Maximum (03) - Button 3
    0x15, 0x00,  //     Logical Minimum (0)
    0x25, 0x01,  //     Logical Maximum (1)
    0x75, 0x01,  //     Report Size (1)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x02,  //     Input (Data, Variable, Absolute) - Button states

    0x75, 0x05,  //     Report Size (5)
    0x95, 0x01,  //     Report Count (1)
    0x81, 0x01,  //     Input (Constant) - Padding or Reserved bits

    0x05, 0x01,  //     Usage Page (Generic Desktop)
    0x09, 0x30,  //     Usage (X)
    0x09, 0x31,  //     Usage (Y)
    0x09, 0x38,  //     Usage (Wheel)
    0x15, 0x81,  //     Logical Minimum (-127)
    0x25, 0x7F,  //     Logical Maximum (127)
    0x75, 0x08,  //     Report Size (8)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x06,  //     Input (Data, Variable, Relative) - X & Y wheal  3bytes
    0xC0,        //   End Collection
    0xC0,        // End Collection

//rpt id == 2
    0x05,0x0c,               // Usage Page (Consumer Devices)
    0x09,0x01,               // Usage index(Consumer Control)
    0xa1,0x01,               // Collection (Application)
    0x85,0x02,               //  Report ID (2)
    0x15,0x00,         //  Logical Minimum (0)
    0x25,0x01,         //  Logical Maximum (1)
    0x75,0x01,         //  Report Size (1)
    0x95,0x01,         //  Report Count (1)

    0x09,0xcd,        // Usage index(Play/Pause)
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)
    //0x0a,0x83,0x01,   //  Usage (AL Consumer Control Configuration)
    0x0a, 0x21, 0x02,   //   Usage (OPEN_SEARCHA) 5
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)
    0x09,0xb5,        //  Usage (Scan Next Track)
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)
    0x09,0xb6,        //  Usage (Scan Previous Track)
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)

    0x09,0xea,        //  Usage (Volume -)
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)
    0x09,0xe9,        //  Usage (Volume +)
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)
    0x0a,0x25,0x02,     //Usage(AC Forward)
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)
    0x0a,0x24,0x02,   //Usage(AC Back)
    0x81,0x06,              //input(Data,Value,Relative,Bit,Filed)
    0xc0,                           // End Collection

//rpt id == 3
    0x05, 0x01,     // Usage Pg (Generic Desktop)
    0x09, 0x06,     // Usage (Keyboard)
    0xA1, 0x01,     // Collection: (Application)
    0x85, 0x03,     // Report Id (3 for keyboard)
    //
    0x05, 0x07,     // Usage Pg (Key Codes)
    0x19, 0xE0,     // Usage Min (224)
    0x29, 0xE7,     // Usage Max (231)
    0x15, 0x00,     // Log Min (0)
    0x25, 0x01,     // Log Max (1)
    //
    // Modifier byte
    0x75, 0x01,     // Report Size (1)   1 bit * 8
    0x95, 0x08,     // Report Count (8)
    0x81, 0x02,     // Input: (Data, Variable, Absolute)

    // Reserved byte
    0x95, 0x01, // Report Count (1)
    0x75, 0x08, // Report Size (8)
    0x81, 0x01, // Input: (Constant)

    //LED repor
    0x95, 0x05,    //Report Count (5)
    0x75, 0x01,    //Report Size (1)
    0x05, 0x08,    //Usage Pg (LEDs )
    0x19, 0x01,    //Usage Min
    0x29, 0x05,    //Usage Max
    0x91, 0x02,    //Output (Data, Variable, Absolute)
    //3 bit reserved
    0x95, 0x01,    //Report Count (1)
    0x75, 0x03,    //Report Size (3)
    0x91, 0x01,    //Output (Constant)

    // Key arrays (6 bytes)
    // this is key array,support simultaneously pressing 6keys report,
    // from report_buf[3]~report_buf[3]
    0x95, 0x06,     // Report Count (6)
    0x75, 0x08,     // Report Size (8)
    0x15, 0x00,     // Log Min (0)
    0x25, 0xE7,     // Log Max (237)
    0x05, 0x07,     // Usage Pg (Key Codes) , here is the key page,look usb hid key define
    0x19, 0x00,     // Usage Min (0)
    0x29, 0xE7,     // Usage Max (237)
    0x81, 0x00,     // Input: (Data, Array)
    0xC0,            // End Collection

};
/******************************* External Report Reference defination */
// External Report Reference Descriptor UUID
const uint8_t ext_report_ref_uuid[] = UUID16_ARR(0x00);


static uint8_t hid_svc_id = 0;

// HID report information table
static hid_report_ref_t  hid_rpt_info[HID_NUM_REPORTS];
static bool hid_link_ntf_enable[CFG_CON] = {0};
/*
 * TYPEDEFS (类型定义)
 */


/*
 * EXTERNAL VARIABLES (外部变量)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */


/*
 * LOCAL VARIABLES (本地变量)
 */

/*********************************************************************
 * Profile Attributes - Table
 * 每一项都是一个attribute的定义。
 * 第一个attribute为Service 的的定义。
 * 每一个特征值(characteristic)的定义，都至少包含三个attribute的定义；
 * 1. 特征值声明(Characteristic Declaration)
 * 2. 特征值的值(Characteristic value)
 * 3. 特征值描述符(Characteristic description)
 * 如果有notification 或者indication 的功能，则会包含四个attribute的定义，除了前面定义的三个，还会有一个特征值客户端配置(client characteristic configuration)。
 *
 */
const gatt_attribute_t hid_profile_att_table[] =
{
    [HID_SERVICE_IDX]               =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID) },
                                            GATT_PROP_READ, 
                                            UUID_SIZE_2, 
                                            (uint8_t *)hid_svc_uuid,
                                        },

#if ((HID_DEV & HID_DEV_MAP_EXT_REF) == HID_DEV_MAP_EXT_REF)
    [HID_INCLUDED_SERVICE_IDX]      =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_INCLUDE_UUID) },
                                            GATT_PROP_READ,
                                            0,
                                            NULL,
                                        },
#endif //((HID_DEV & HID_DEV_MAP_EXT_REF) == HID_DEV_MAP_EXT_REF)

    [HID_INFO_DECL_IDX]             =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ,
                                            0, 
                                            NULL,
                                        },
    
    [HID_INFO_IDX]                  =   { 
                                            { UUID_SIZE_2, UUID16_ARR(HID_INFORMATION_UUID) },
                                            GATT_PROP_READ, 
                                            sizeof(hid_info_value), 
                                            NULL,
                                        },


    // HID Control Point characteristic declaration
    [HID_CONTROL_POINT_DECL_IDX]    =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ, 
                                            0,
                                            NULL,
                                        },
                                        
    // HID Control Point characteristic value
    [HID_CONTROL_POINT_IDX]         =   { 
                                            { UUID_SIZE_2, UUID16_ARR(HID_CTRL_PT_UUID) },
                                            GATT_PROP_WRITE, 
                                            sizeof(uint8_t), 
                                            NULL,
                                        },

    // HID Report Map characteristic declaration
    [HID_REPORT_MAP_DECL_IDX]       =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ, 
                                            0, 
                                            NULL,
                                        },

    // HID Report Map characteristic
    [HID_REPORT_MAP_IDX]            =   { 
                                            { UUID_SIZE_2, UUID16_ARR(REPORT_MAP_UUID) },
                                            GATT_PROP_READ,  
                                            512, 
                                            NULL,
                                        },

#if ((HID_DEV & HID_DEV_MAP_EXT_REF) == HID_DEV_MAP_EXT_REF)
    // Report Map Characteristic - External Report Reference Descriptor
    [HID_REPORT_MAP_EXT_REP_REF]    =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_EXT_REPORT_REF_UUID) },
                                            GATT_PROP_WRITE, 
                                            sizeof(ext_report_ref_uuid), 
                                            (uint8_t *)ext_report_ref_uuid,
                                        },
#endif //((HID_DEV & HID_DEV_MAP_EXT_REF) == HID_DEV_MAP_EXT_REF)

#if ((HID_DEV & HID_DEV_PROTO_MODE) == HID_DEV_PROTO_MODE)
    // HID Protocol Mode characteristic declaration
    [HID_PROTOCOL_MODE_DECL_IDX]    =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ, 
                                            0,
                                            NULL,
                                        },
                                        
    // HID Protocol Mode characteristic
    [HID_PROTOCOL_MODE_IDX]         =   { 
                                            { UUID_SIZE_2, UUID16_ARR(PROTOCOL_MODE_UUID) },
                                            GATT_PROP_READ | GATT_PROP_WRITE, 
                                            sizeof(uint8_t),
                                            NULL,
                                        },
#endif //((HID_DEV & HID_DEV_PROTO_MODE) == HID_DEV_PROTO_MODE)

#if ((HID_DEV & HID_DEV_KEYBOARD) == HID_DEV_KEYBOARD)
    // HID Boot Keyboard Input Report declaration
    [HID_BOOT_KEY_IN_DECL_IDX]      =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ,
                                            0,
                                            NULL,
                                        },
    // HID Boot Keyboard Input Report
    [HID_BOOT_KEY_IN_IDX]           =   { 
                                            { UUID_SIZE_2, UUID16_ARR(BOOT_KEY_INPUT_UUID) },
                                            GATT_PROP_READ | GATT_PROP_NOTI,
                                            8,
                                            NULL,
                                        },
    // HID Report characteristic, key input client characteristic configuration
    [HID_BOOT_KEY_IN_CCCD_IDX]      =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },
                                            GATT_PROP_READ | GATT_PROP_WRITE,
                                            0,
                                            NULL,
                                        },

    // HID Boot Keyboard Output Report declaration
    [HID_BOOT_KEY_OUT_DECL_IDX]     =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ,
                                            0,
                                            NULL,
                                        },
                                        
    // HID Boot Keyboard Output Report
    [HID_BOOT_KEY_OUT_IDX]          =   { 
                                            { UUID_SIZE_2, UUID16_ARR(BOOT_KEY_OUTPUT_UUID) },
                                            GATT_PROP_READ | GATT_PROP_WRITE,
                                            8,
                                            NULL,
                                        },
#endif //((HID_DEV & HID_DEV_KEYBOARD) == HID_DEV_KEYBOARD)

#if ((HID_DEV & HID_DEV_MOUSE) == HID_DEV_MOUSE)
    // HID Boot Mouse Input Report declaration
    [HID_BOOT_MOUSE_IN_DECL_IDX]    =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ,
                                            0,
                                            NULL,
                                        },
                                        
    // HID Boot Mouse Input Report
    [HID_BOOT_MOUSE_IN_IDX]         =   { 
                                            { UUID_SIZE_2, UUID16_ARR(BOOT_MOUSE_INPUT_UUID) },
                                            GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_NOTI,
                                            8,
                                            NULL,
                                        },
                                        
    // HID Report characteristic, key input client characteristic configuration
    [HID_BOOT_MOUSE_IN_CCCD_IDX]    =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },
                                            GATT_PROP_READ | GATT_PROP_WRITE,
                                            0,
                                            NULL,
                                        },
#endif // ((HID_DEV & HID_DEV_MOUSE) == HID_DEV_MOUSE)

// HID Feature Report No 0
    [HID_FEATURE_DECL_IDX]          =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                            GATT_PROP_READ, 
                                            0,
                                            NULL,
                                        },
                                        
    [HID_FEATURE_IDX]               =   { 
                                            { UUID_SIZE_2, UUID16_ARR(REPORT_UUID) },
                                            GATT_PROP_READ | GATT_PROP_NOTI,
                                            60,
                                            NULL,
                                        },
                                        
    [HID_REPORT_REF_FEATURE_IDX]    =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_REPORT_REF_UUID) },
                                            GATT_PROP_READ,
                                            sizeof(hid_report_ref_t), 
                                            NULL,
                                        },
                                        
    [HID_FEATURE_CCCD_IDX]          =   { 
                                            { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },
                                            GATT_PROP_READ | GATT_PROP_WRITE, 
                                            0, 
                                            NULL,
                                        },

    // HID Feature Report No 1
    [HID_FEATURE_DECL_IDX + 4 * 1]          =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                                    GATT_PROP_READ, 
                                                    0,
                                                    NULL,
                                                },
                                        
    [HID_FEATURE_IDX + 4 * 1]               =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(REPORT_UUID) },
                                                    GATT_PROP_READ | GATT_PROP_NOTI,
                                                    60,
                                                    NULL,
                                                },
                                        
    [HID_REPORT_REF_FEATURE_IDX + 4 * 1]    =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_REPORT_REF_UUID) },
                                                    GATT_PROP_READ,
                                                    sizeof(hid_report_ref_t), 
                                                    NULL,
                                                },
                                                
    [HID_FEATURE_CCCD_IDX + 4 * 1]          =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },
                                                    GATT_PROP_READ | GATT_PROP_WRITE, 
                                                    0, 
                                                    NULL,
                                                },

    // HID Feature Report No 2
    [HID_FEATURE_DECL_IDX + 4 * 2]          =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                                    GATT_PROP_READ, 
                                                    0,
                                                    NULL,
                                                },
                                                
    [HID_FEATURE_IDX + 4 * 2]               =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(REPORT_UUID) },
                                                    GATT_PROP_READ | GATT_PROP_NOTI,
                                                    60,
                                                    NULL,
                                                },
                                                
    [HID_REPORT_REF_FEATURE_IDX + 4 * 2]    =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_REPORT_REF_UUID) },
                                                    GATT_PROP_READ,
                                                    sizeof(hid_report_ref_t), 
                                                    NULL,
                                                },
                                                
    [HID_FEATURE_CCCD_IDX + 4 * 2]          =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },
                                                    GATT_PROP_READ | GATT_PROP_WRITE, 
                                                    0, 
                                                    NULL,
                                                },

    // HID Feature Report No 3
    [HID_FEATURE_DECL_IDX + 4 * 3]          =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },
                                                    GATT_PROP_READ, 
                                                    0,
                                                    NULL,
                                                },
                                                
    [HID_FEATURE_IDX + 4 * 3]               =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(REPORT_UUID) },
                                                    GATT_PROP_READ | GATT_PROP_WRITE,
                                                    60,
                                                    NULL,
                                                },
                                                
    [HID_REPORT_REF_FEATURE_IDX + 4 * 3]    =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_REPORT_REF_UUID) },
                                                    GATT_PROP_READ,
                                                    sizeof(hid_report_ref_t), 
                                                    NULL,
                                                },
                                                
    [HID_FEATURE_CCCD_IDX + 4 * 3]          =   { 
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },
                                                    GATT_PROP_READ | GATT_PROP_WRITE, 
                                                    0, 
                                                    NULL,
                                                },
};


/*********************************************************************
 * @fn      hid_gatt_op_cmp_handler
 *
 * @brief   Gatt operation complete handler.
 *
 *
 * @param   p_operation  - operation that has compeleted
 *
 * @return  none.
 */
void hid_gatt_op_cmp_handler(gatt_op_cmp_t *p_operation)
{
    if (p_operation->status == 0)
    {}
}

/*********************************************************************
 * @fn      hid_gatt_msg_handler
 *
 * @brief   HID gatt message handler.
 *
 *
 * @param   p_msg  - messages from GATT layer.
 *
 * @return  none.
 */
static uint16_t hid_gatt_msg_handler(gatt_msg_t *p_msg)
{
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
            if(p_msg->att_idx == HID_REPORT_MAP_IDX)
            {
                co_printf("report_map request:%d\r\n",sizeof(hid_report_map));
                memcpy(p_msg->param.msg.p_msg_data, hid_report_map, sizeof(hid_report_map));
                return sizeof(hid_report_map);
            }
            else if (p_msg->att_idx == HID_INFO_IDX)
            {
                co_printf("hid info request\r\n");
                memcpy(p_msg->param.msg.p_msg_data, hid_info_value, sizeof(hid_info_value));
                return sizeof(hid_info_value);
            }
            else
            {
                for(uint8_t idx=0; idx<HID_NUM_REPORTS; idx++)
                {
                    if(p_msg->att_idx == HID_REPORT_REF_FEATURE_IDX + 4 * idx)
                    {
                        co_printf("report_ref[%d] request\r\n",idx);
                        memcpy(p_msg->param.msg.p_msg_data, (uint8_t *)&hid_rpt_info[idx], sizeof(hid_report_ref_t));
                        return sizeof(hid_report_ref_t);
                    }
                }
            }
            break;

        case GATTC_MSG_WRITE_REQ:

            if(p_msg->att_idx == HID_BOOT_KEY_IN_CCCD_IDX)
            {
                co_printf("boot_key_ntf_enable:");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
            else if(p_msg->att_idx == HID_FEATURE_IDX + 4 * 3)
            {
                co_printf("write report_id 3:");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
                uint8_t *data = p_msg->param.msg.p_msg_data;
                if(data[0] & (1<<0))
                    co_printf("num_lock on\r\n");
                else
                    co_printf("num_lock off\r\n");
                if (data[0] & (1<<1))
                    co_printf("caps_lock on\r\n");
                else
                    co_printf("caps_lock off\r\n");
                hid_link_ntf_enable[p_msg->conn_idx] = true;
            }
            else
            {
                for(uint8_t idx=0; idx<HID_NUM_REPORTS; idx++)
                {
                    if(p_msg->att_idx == HID_FEATURE_CCCD_IDX + 4 * idx)
                    {
                        co_printf("ntf_enable[%d]:",idx);
                        show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
                    }
                    if(idx == 2)
                        hid_link_ntf_enable[p_msg->conn_idx] = true;
                }
            }
            break;

        case GATTC_MSG_CMP_EVT:
            hid_gatt_op_cmp_handler((gatt_op_cmp_t*)&(p_msg->param.op));
            break;
        case GATTC_MSG_LINK_CREATE:
            //co_printf("link[%d] create\r\n",p_msg->conn_idx);
            hid_link_ntf_enable[p_msg->conn_idx] = true;
            break;
        case GATTC_MSG_LINK_LOST:
            //co_printf("link[%d] lost\r\n",p_msg->conn_idx);
            hid_link_ntf_enable[p_msg->conn_idx] = false;
            break;
        default:
            break;
    }
    return 0;
}

/*********************************************************************
 * @fn      hid_gatt_report_notify
 *
 * @brief   Send HID notification, keys, mouse values, etc.
 *
 *
 * @param   rpt_info_id - report idx of the hid_rpt_info array.
 *          len         - length of the HID information data.
 *          p_data      - data of the HID information to be sent.
 *
 * @return  none.
 */
void hid_gatt_report_notify(uint8_t conidx, uint8_t rpt_info_id, uint8_t *p_data, uint16_t len)
{
    if (rpt_info_id < HID_NUM_REPORTS && hid_link_ntf_enable[conidx])
    {
        gatt_ntf_t ntf;
        ntf.conidx = conidx;
        ntf.svc_id = hid_svc_id;
        ntf.att_idx = HID_FEATURE_IDX + 4*(rpt_info_id) ;
        ntf.data_len = len;
        ntf.p_data = p_data;
        gatt_notification(ntf);
    }
}

/*********************************************************************
 * @fn      hid_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *          添加GATT service到ATT的数据库里面。
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void hid_gatt_add_service(void)
{
    gatt_service_t hid_profie_svc;

    hid_profie_svc.p_att_tb = hid_profile_att_table;
    hid_profie_svc.att_nb = HID_ATT_NB;
    hid_profie_svc.gatt_msg_handler = hid_gatt_msg_handler;

    hid_rpt_info[0].report_id = 1;     //refer to report map, this is Mouse
    hid_rpt_info[0].report_type = HID_REPORT_TYPE_INPUT;

    hid_rpt_info[1].report_id = 2;      //refer to report map, this is Cosumer Controller
    hid_rpt_info[1].report_type = HID_REPORT_TYPE_INPUT;

    hid_rpt_info[2].report_id = 3;      //refer to report map, this is Keyboard input.
    hid_rpt_info[2].report_type = HID_REPORT_TYPE_INPUT;     //att_table, perm must be GATT_PROP_READ | GATT_PROP_NOTI

    hid_rpt_info[3].report_id = 3;      //refer to report map, this is Keyboard output.
    hid_rpt_info[3].report_type = HID_REPORT_TYPE_OUTPUT;   //att_table, perm must be GATT_PROP_READ | GATT_PROP_WRITE

    hid_svc_id = gatt_add_service(&hid_profie_svc);
}





