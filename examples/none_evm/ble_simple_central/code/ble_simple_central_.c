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
/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */

#define SPSC_UUID128_ARR_TX   {0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}
#define SPSC_UUID128_ARR_RX   {0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}

const gatt_uuid_t client_att_tb[] =
{
    [0]  =
    { UUID_SIZE_16, SPSC_UUID128_ARR_TX},
    [1]  =
    { UUID_SIZE_16, SPSC_UUID128_ARR_RX},
};

/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */
uint8_t client_id;
uint8_t slave_link_conidx;
uint8_t master_link_conidx;

/*
 * LOCAL VARIABLES (本地变量)
 */


 
/*
 * LOCAL FUNCTIONS (本地函数)
 */

void proj_ble_gap_evt_func(gap_event_t *event)
{
    switch(event->type)
    {
        case GAP_EVT_ADV_END:
        {
            co_printf("adv_end,status:0x%02x\r\n",event->param.adv_end_status);
#if 0
            uint8_t adv_data[]="\x09\x08\x46\x52\x38\x30\x31\x30\x48\x00";
            uint8_t rsp_data[]="\x09\xFF\x00\x60\x52\x57\x2D\x42\x4C\x22";

            gap_adv_param_t adv_param;
            adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
            adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
            adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
            adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
            adv_param.adv_intv_min = 32;
            adv_param.adv_intv_max = 32;
            adv_param.adv_data = adv_data;
            adv_param.adv_data_len = sizeof(adv_data)-1;
            adv_param.adv_rsp_data = rsp_data;
            adv_param.adv_rsp_data_len = sizeof(rsp_data)-1;
            gap_set_advertising_param(&adv_param);
#endif
            //gap_start_advertising(0);
        }
        break;
        case GAP_EVT_SCAN_END:
            co_printf("scan_end,status:0x%02x\r\n",event->param.scan_end_status);
            break;
        case GAP_EVT_ADV_REPORT:
        {
            //if(memcmp(event->param.adv_rpt->src_addr.addr.addr,"\x0C\x0C\x0C\x0C\x0C\x0B",6)==0)
            {
                co_printf("evt_type:0x%02x,rssi:%d\r\n",event->param.adv_rpt->evt_type,event->param.adv_rpt->rssi);

                co_printf("content:");
                show_reg(event->param.adv_rpt->data,event->param.adv_rpt->length,1);
            }

        }
        break;

        case GAP_EVT_ALL_SVC_ADDED:
        {
            co_printf("all svc added\r\n");
        }
        break;

        case GAP_EVT_MASTER_CONNECT:
        {
            co_printf("master[%d],connect. link_num:%d\r\n",event->param.master_connect.conidx,gap_get_connect_num());
            master_link_conidx = (event->param.master_connect.conidx);
            gap_conn_param_update(event->param.master_connect.conidx,6,6,50,500);
#if 1
            if (gap_security_get_bond_status())
                gap_security_enc_req(event->param.master_connect.conidx);
            else
                gap_security_pairing_req(event->param.master_connect.conidx);
#else
            extern uint8_t client_id;
            gatt_discovery_all_peer_svc(client_id,event->param.master_encrypt_conidx);
#endif
        }
        break;

        case GAP_EVT_SLAVE_CONNECT:
        {
            co_printf("slave[%d],connect. link_num:%d\r\n",event->param.slave_connect.conidx,gap_get_connect_num());
            slave_link_conidx = event->param.slave_connect.conidx;
            gatt_mtu_exchange_req(event->param.slave_connect.conidx);
            gap_conn_param_update(event->param.slave_connect.conidx, 12, 12, 0, 500);
        }
        break;

        case GAP_EVT_DISCONNECT:
        {
            //gap_bond_manager_info_clr("\x0C\x0C\x0C\x0C\x0C\x0B", 0);
            co_printf("Link[%d] disconnect,reason:0x%02X\r\n",event->param.disconnect.conidx
                      ,event->param.disconnect.reason);
#ifdef USER_MEM_API_ENABLE
            show_mem_list();
            //show_msg_list();
            show_ke_malloc();
#endif
            gap_start_advertising(0);
        }
        break;

        case GAP_EVT_LINK_PARAM_REJECT:
            co_printf("Link[%d]param reject,status:0x%02x\r\n"
                      ,event->param.link_reject.conidx,event->param.link_reject.status);
            break;

        case GAP_EVT_LINK_PARAM_UPDATE:
            co_printf("Link[%d]param update,interval:%d,latency:%d,timeout:%d\r\n",event->param.link_update.conidx
                      ,event->param.link_update.con_interval,event->param.link_update.con_latency,event->param.link_update.sup_to);
            break;

        case GAP_EVT_CONN_END:
            co_printf("conn_end,reason:0x%02x\r\n",event->param.conn_end_reason);
            break;

        case GAP_EVT_PEER_FEATURE:
            co_printf("peer[%d] feats ind\r\n",event->param.peer_feature.conidx);
            show_reg((uint8_t *)&(event->param.peer_feature.features),8,1);
            break;

        case GAP_EVT_MTU:
            co_printf("mtu update,conidx=%d,mtu=%d\r\n"
                      ,event->param.mtu.conidx,event->param.mtu.value);
            break;
        case GAP_EVT_LINK_RSSI:
            co_printf("link rssi %d\r\n",event->param.link_rssi);
            break;
        case GAP_SEC_EVT_MASTER_AUTH_REQ:
            co_printf("link[%d],recv auth req:0x%02x\r\n",event->param.auth_req.conidx,event->param.auth_req.auth);
            break;
        case GAP_SEC_EVT_MASTER_ENCRYPT:
            co_printf("master[%d]_encrypted\r\n",event->param.master_encrypt_conidx);
            extern uint8_t client_id;
            gatt_discovery_all_peer_svc(client_id,event->param.master_encrypt_conidx);
            //uint8_t group_uuid[] = {0xb7, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07};
            //gatt_discovery_peer_svc(client_id,event->param.master_encrypt_conidx,16,group_uuid);
            break;
        case GAP_SEC_EVT_SLAVE_ENCRYPT:
            co_printf("slave[%d]_encrypted\r\n",event->param.slave_encrypt_conidx);
            break;

        default:
            break;
    }
}


uint16_t client_msg_handler(gatt_msg_t *p_msg)
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

void prf_client_create(void)
{
    gatt_client_t client;
    
    client.p_att_tb = client_att_tb;
    client.att_nb = 2;
    client.gatt_msg_handler = client_msg_handler;
    client_id = gatt_add_client(&client);
}




