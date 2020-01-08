/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "gatt_api.h"
#include "gatt_sig_uuid.h"
#include "sys_utils.h"

#define SPSC_UUID128_ARR_TX   {0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}
#define SPSC_UUID128_ARR_RX   {0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}


const gatt_uuid_t client_att_tb[] =
{
    [0]  =
    { UUID_SIZE_16, SPSC_UUID128_ARR_TX},
    [1]  =
    { UUID_SIZE_16, SPSC_UUID128_ARR_RX},
};

uint8_t client_id;

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

void prf_client_create(void)
{
    gatt_client_t client;
    
    client.p_att_tb = client_att_tb;
    client.att_nb = 2;
    client.gatt_msg_handler = client_msg_handler;
    client_id = gatt_add_client(&client);
}

