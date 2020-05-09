#include <stdint.h>
#include <string.h>

#include "co_printf.h"
#include "os_msg_q.h" //for os_msg_post
#include "os_mem.h"
#include "at_cmd_task.h"
#include "at_gap_event.h"

#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"
#include "driver_uart.h"
#include "at_profile_spss.h"

#define AT_ASSERT(v) do { \
    if (!(v)) {             \
        co_printf("%s %s \n", __FILE__, __LINE__); \
        while (1) {};   \
    }                   \
} while (0);
#undef LOG_LEVEL_MODULE
#define LOG_LEVEL_MODULE        LOG_LEVEL_NONE

static uint16_t ntf_enable_flag[BLE_CONNECTION_MAX] = {0};
at_recv_data_func_t spss_recv_data_ind_func = NULL;
uint16_t spss_svc_id;
uint8_t spss_uuids[64] =
{
    0xb7, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07, //SPS_SERVICE_UUID
    0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07, //SPS_SERVER_TX_UUID
    0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07, //SPS_SERVER_RX_UUID
    0xb9, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07  //SPS_FLOW_CTRL_UUID
};
/// Full SPS Database Description - Used to add attributes into the database
gatt_attribute_t spss_att_db[] =
{
    // serial port Service Declaration
    [0] = { {UUID_SIZE_2,UUID16_ARR(GATT_PRIMARY_SERVICE_UUID)},GATT_PROP_READ,UUID_SIZE_16,NULL},
    //In client side, this attribute is for rx
    [1] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)},GATT_PROP_READ,0,NULL},
    //updata uuid when created
    [2] = { {UUID_SIZE_16,"\xff\xff\xff\xff"},GATT_PROP_READ|GATT_PROP_NOTI,600,NULL},
    [3] = { {UUID_SIZE_2,UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)},GATT_PROP_READ|GATT_PROP_WRITE,sizeof(uint16_t),NULL},
    [4] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHAR_USER_DESC_UUID)},GATT_PROP_READ,0xC,NULL},

    //In client side, this attribute is for tx
    [5] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)},GATT_PROP_READ,0,NULL},
    //updata uuid when created
    [6] = { {UUID_SIZE_16,"\xff\xff\xff\xff"},GATT_PROP_READ|GATT_PROP_WRITE,600,NULL},
    [7] = { {UUID_SIZE_2,UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)},GATT_PROP_READ|GATT_PROP_WRITE,sizeof(uint16_t),NULL},
    [8] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHAR_USER_DESC_UUID)},GATT_PROP_READ,0xC,NULL},

    //This is for flow control
    [9] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)},GATT_PROP_READ,0,NULL},
    //updata uuid when created
    [10] = { {UUID_SIZE_16,"\xff\xff\xff\xff"},GATT_PROP_READ|GATT_PROP_WRITE|GATT_PROP_NOTI,sizeof(uint16_t),NULL},
    [11] = { {UUID_SIZE_2,UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)},GATT_PROP_READ|GATT_PROP_WRITE,sizeof(uint16_t),NULL},
    [12] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHAR_USER_DESC_UUID)},GATT_PROP_READ,0xC,NULL},
};

/*********************************************************************
 * @fn      spss_svc_msg_handler
 *
 * @brief   AT service profile msg handle function, when service profile receive msg, will call this function.
 *
 * @param   p_msg - msg to be handle
 *       
 *
 * @return  response data len for profile read operation, meanless for other operation
 */
uint16_t spss_svc_msg_handler(gatt_msg_t *p_msg)
{
    //co_printf("spss_svc,op:%d,att_idx:%d\r\n",p_msg->msg_evt,p_msg->att_idx);
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
            if(p_msg->att_idx == 3)
            {
                memcpy(p_msg->param.msg.p_msg_data, &ntf_enable_flag[p_msg->conn_idx], 2);
                return 2;
            }
            else if(p_msg->att_idx == 4)
            {
                memcpy(p_msg->param.msg.p_msg_data, "spss_tx_desc", 12);
                return 12;
            }
            else if(p_msg->att_idx == 8)
            {
                memcpy(p_msg->param.msg.p_msg_data, "spss_rx_desc", 12);
                return 12;
            }
            else if(p_msg->att_idx == 12)
            {
                memcpy(p_msg->param.msg.p_msg_data, "spss_fl_desc", 12);
                return 12;
            }
            break;
        case GATTC_MSG_WRITE_REQ:
            if(p_msg->att_idx == 6)
            {
                if(spss_recv_data_ind_func != NULL)
                    spss_recv_data_ind_func(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len);
                else
                {
#if 0
                    co_printf("RX: ");
                    show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
#else
                    uint8_t at_rsp[20] = {0};
                    sprintf((char *)at_rsp,"\r\n+DATA,%d,%d:",p_msg->conn_idx,p_msg->param.msg.msg_len);
                    uart_put_data_noint(UART0,at_rsp,strlen((char *)at_rsp));
                    at_spss_recv_data_ind_func(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len);
#endif
                }
            }
            else if(p_msg->att_idx == 3)
                ntf_enable_flag[p_msg->conn_idx] = *(uint16_t *)(p_msg->param.msg.p_msg_data);
            break;
        case GATTC_MSG_CMP_EVT:
            //co_printf("att_idx[%d],op:%d done\r\n",p_msg->att_idx,p_msg->param.op.operation);
            break;
        case GATTC_MSG_LINK_CREATE:
            gatt_mtu_exchange_req(p_msg->conn_idx);
            break;
        case GATTC_MSG_LINK_LOST:
            ntf_enable_flag[p_msg->conn_idx] = 0;
            break;
    }
    return 0;
};

/*********************************************************************
 * @fn      at_spss_send_data
 *
 * @brief   function to notification date to peer.
 *
 * @param   conidx - link  index
 *       	data   - pointer to data buffer 
 *       	len    - data len
 *
 * @return  None
 */
void at_spss_send_data(uint8_t conidx, uint8_t *data, uint8_t len)
{
    //if(l2cm_get_nb_buffer_available() > 0)
    {
        if(ntf_enable_flag[conidx])
        {
            gatt_ntf_t ntf_att;
            ntf_att.att_idx = 2;
            ntf_att.conidx = conidx;
            ntf_att.svc_id = spss_svc_id;
            ntf_att.data_len = MIN(len,gatt_get_mtu(conidx) - 7);
            ntf_att.p_data = data;
            gatt_notification(ntf_att);
        }
    }
}

/*********************************************************************
 * @fn      at_profile_spss_init
 *
 * @brief   Create AT profile service
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_profile_spss_init(void)
{
    gatt_service_t service;
    spss_att_db[0].p_data = &spss_uuids[0];
    memcpy(spss_att_db[2].uuid.p_uuid,&spss_uuids[16],UUID_SIZE_16);
    memcpy(spss_att_db[6].uuid.p_uuid,&spss_uuids[32],UUID_SIZE_16);
    memcpy(spss_att_db[10].uuid.p_uuid,&spss_uuids[48],UUID_SIZE_16);
    service.p_att_tb = spss_att_db;
    service.att_nb = 13;
    service.gatt_msg_handler = spss_svc_msg_handler;
    spss_svc_id = gatt_add_service(&service);
}

