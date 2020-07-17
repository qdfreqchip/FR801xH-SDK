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
#include "flash_usage_config.h"

/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */

#define SP_CHAR1_UUID            0xFFF1
#define SP_CHAR2_UUID            0xFFF2

const gatt_uuid_t client_att_tb[] =
{
    [0]  =
    { UUID_SIZE_2, UUID16_ARR(SP_CHAR1_UUID)},
    [1]  =
    { UUID_SIZE_2, UUID16_ARR(SP_CHAR2_UUID)},
};

/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */
uint8_t client_id;

uint8_t master_link_conidx;

/*
 * LOCAL VARIABLES (本地变量)
 */

 
/*
 * LOCAL FUNCTIONS (本地函数)
 */
static void simple_central_start_scan(void);

/*********************************************************************
 * @fn      app_gap_evt_cb
 *
 * @brief   Application layer GAP event callback function. Handles GAP evnets.
 *
 * @param   p_event - GAP events from BLE stack.
 *       
 *
 * @return  None.
 */
static void app_gap_evt_cb(gap_event_t *p_event)
{
    switch(p_event->type)
    {
        case GAP_EVT_SCAN_END:
            co_printf("scan_end,status:0x%02x\r\n",p_event->param.scan_end_status);
            break;
 
        case GAP_EVT_ADV_REPORT:
        {
            uint8_t scan_name[] = "Simple Peripheral";
            //if(memcmp(event->param.adv_rpt->src_addr.addr.addr,"\x0C\x0C\x0C\x0C\x0C\x0B",6)==0)
            if (p_event->param.adv_rpt->data[0] == 0x12
                && p_event->param.adv_rpt->data[1] == GAP_ADVTYPE_LOCAL_NAME_COMPLETE
                && memcmp(&(p_event->param.adv_rpt->data[2]), scan_name, 0x10) == 0)
            {
                gap_stop_scan();
                
                co_printf("evt_type:0x%02x,rssi:%d\r\n",p_event->param.adv_rpt->evt_type,p_event->param.adv_rpt->rssi);

                co_printf("content:");
                show_reg(p_event->param.adv_rpt->data,p_event->param.adv_rpt->length,1);
                
                gap_start_conn(&(p_event->param.adv_rpt->src_addr.addr),
                                p_event->param.adv_rpt->src_addr.addr_type,
                                12, 12, 0, 300);
            }

        }
        break;

        case GAP_EVT_MASTER_CONNECT:
        {
            co_printf("master[%d],connect. link_num:%d\r\n",p_event->param.master_connect.conidx,gap_get_connect_num());
            master_link_conidx = (p_event->param.master_connect.conidx);
#if 1
            if (gap_security_get_bond_status())
                gap_security_enc_req(p_event->param.master_connect.conidx);
            else
                gap_security_pairing_req(p_event->param.master_connect.conidx);
#else
            extern uint8_t client_id;
            gatt_discovery_all_peer_svc(client_id,event->param.master_encrypt_conidx);
#endif
            simple_central_start_scan();
        }
        break;

        case GAP_EVT_DISCONNECT:
        {
            //gap_bond_manager_info_clr("\x0C\x0C\x0C\x0C\x0C\x0B", 0);
            co_printf("Link[%d] disconnect,reason:0x%02X\r\n",p_event->param.disconnect.conidx
                      ,p_event->param.disconnect.reason);
#ifdef USER_MEM_API_ENABLE
            show_mem_list();
            //show_msg_list();
            show_ke_malloc();
#endif
        }
        break;

        case GAP_EVT_LINK_PARAM_REJECT:
            co_printf("Link[%d]param reject,status:0x%02x\r\n"
                      ,p_event->param.link_reject.conidx,p_event->param.link_reject.status);
            break;

        case GAP_EVT_LINK_PARAM_UPDATE:
            co_printf("Link[%d]param update,interval:%d,latency:%d,timeout:%d\r\n",p_event->param.link_update.conidx
                      ,p_event->param.link_update.con_interval,p_event->param.link_update.con_latency,p_event->param.link_update.sup_to);
            break;

        case GAP_EVT_CONN_END:
            co_printf("conn_end,reason:0x%02x\r\n",p_event->param.conn_end_reason);
            break;

        case GAP_EVT_PEER_FEATURE:
            co_printf("peer[%d] feats ind\r\n",p_event->param.peer_feature.conidx);
            show_reg((uint8_t *)&(p_event->param.peer_feature.features),8,1);
            break;

        case GAP_EVT_MTU:
            co_printf("mtu update,conidx=%d,mtu=%d\r\n"
                      ,p_event->param.mtu.conidx,p_event->param.mtu.value);
            break;
        case GAP_EVT_LINK_RSSI:
            co_printf("link rssi %d\r\n",p_event->param.link_rssi);
            break;

        case GAP_SEC_EVT_MASTER_ENCRYPT:
            co_printf("master[%d]_encrypted\r\n",p_event->param.master_encrypt_conidx);
            extern uint8_t client_id;
            gatt_discovery_all_peer_svc(client_id,p_event->param.master_encrypt_conidx);
            //uint8_t group_uuid[] = {0xb7, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07};
            //gatt_discovery_peer_svc(client_id,event->param.master_encrypt_conidx,16,group_uuid);
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      simple_central_start_scan
 *
 * @brief   Set central role scan parameters and start scanning BLE devices.
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
static void simple_central_start_scan(void)
{
    // Start Scanning
    co_printf("Start scanning...\r\n");
    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 0;
    gap_start_scan(&scan_param);
}

/*********************************************************************
 * @fn      simple_central_msg_handler
 *
 * @brief   Simple Central GATT message handler, handles messages from GATT layer.
 *          Messages like read/write response, notification/indication values, etc.
 *
 * @param   p_msg       - GATT message structure.
 *
 * @return  uint16_t    - Data length of the GATT message handled.
 */
static uint16_t simple_central_msg_handler(gatt_msg_t *p_msg)
{
    co_printf("CCC:%x\r\n",p_msg->msg_evt);
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_NTF_REQ:
        {
            if(p_msg->att_idx == 0)
            {
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
        }
        break;
        
        case GATTC_MSG_READ_IND:
        {
            if(p_msg->att_idx == 0)
            {
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
        }
        break;
        
        case GATTC_MSG_CMP_EVT:
        {
            co_printf("op:%d done\r\n",p_msg->param.op.operation);
            if(p_msg->param.op.operation == GATT_OP_PEER_SVC_REGISTERED)
            {
                uint16_t att_handles[2];
                memcpy(att_handles,p_msg->param.op.arg,4);
                show_reg((uint8_t *)att_handles,4,1);

                gatt_client_enable_ntf_t ntf_enable;
                ntf_enable.conidx = p_msg->conn_idx;
                ntf_enable.client_id = client_id;
                ntf_enable.att_idx = 0; //TX
                gatt_client_enable_ntf(ntf_enable);

                gatt_client_write_t write;
                write.conidx = p_msg->conn_idx;
                write.client_id = client_id;
                write.att_idx = 1; //RX
                write.p_data = "\x1\x2\x3\x4\x5\x6\x7";
                write.data_len = 7;
                gatt_client_write_cmd(write);

                gatt_client_read_t read;
                read.conidx = p_msg->conn_idx;
                read.client_id = client_id;
                read.att_idx = 0; //TX
                gatt_client_read(read);
            }
        }
        break;
        
        default:
        break;
    }

    return 0;
}

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
 * @fn      simple_central_init
 *
 * @brief   Initialize simple central, BLE related parameters.
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void simple_central_init(void)
{
    // set local device name
    uint8_t local_name[] = "Simple Central";
    gap_set_dev_name(local_name, sizeof(local_name));

    gap_set_cb_func(app_gap_evt_cb);
    
    // Initialize security related settings.
    gap_bond_manager_init(BLE_BONDING_INFO_SAVE_ADDR, BLE_REMOTE_SERVICE_SAVE_ADDR, 8, true);
  //  gap_bond_manager_delete_all();    

    gap_security_param_t param =
    {
        .mitm = false,
        .ble_secure_conn = false,
        .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond_auth = true,
        .password = 0,
    };

    gap_security_param_init(&param);
    
    // Initialize GATT 
    gatt_client_t client;
    
    client.p_att_tb = client_att_tb;
    client.att_nb = 2;
    client.gatt_msg_handler = simple_central_msg_handler;
    client_id = gatt_add_client(&client);
    
    simple_central_start_scan();
}




