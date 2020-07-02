#include <stdint.h>
#include <string.h>

#include "co_printf.h"
#include "os_msg_q.h" //for os_msg_post
#include "os_mem.h"
#include "at_cmd_task.h"
#include "at_gap_event.h"

#include "gap_api.h"
#include "gatt_api.h"
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

#define SPSC_UUID128_ARR_RX   {0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}
#define SPSC_UUID128_ARR_TX   {0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}

at_recv_data_func_t spsc_recv_data_ind_func = NULL;
uint16_t spsc_client_id;
gatt_uuid_t spsc_att_tb[] =
{
    [0]  ={ UUID_SIZE_16, SPSC_UUID128_ARR_RX}, //RX,   and for spss_TX
    [1]  ={ UUID_SIZE_16, SPSC_UUID128_ARR_TX}, //TX,,  and for spss_RX
};

/*********************************************************************
 * @fn      spsc_client_msg_handler
 *
 * @brief   AT client profile msg handle function, when client profile receive msg, will call this function.
 *
 * @param   p_msg - msg to be handle
 *       
 *
 * @return  response data len for profile read operation, meanless for other operation
 */
uint16_t spsc_client_msg_handler(gatt_msg_t *p_msg)
{
    //co_printf("spsc_clinet,att_idx:%d,evt:%d\r\n",p_msg->att_idx,p_msg->msg_evt);
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_NTF_REQ:
        {
            if(p_msg->att_idx == 0)
            {
                if(spsc_recv_data_ind_func != NULL)
                    spsc_recv_data_ind_func(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len);
                else
                {
#if 0
                    co_printf("RX: ");
                    show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
#else
                    uint8_t at_rsp[20] = {0};
                    sprintf((char *)at_rsp,"\r\n+DATA,%d,%d:",p_msg->conn_idx,p_msg->param.msg.msg_len);
                    uart_put_data_noint(UART0,at_rsp,strlen((char *)at_rsp));
                    at_spsc_recv_data_ind_func(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len);
#endif
                }
            }
        }
        break;
        case GATTC_MSG_READ_IND:
        {
            ;
        }
        break;
        case GATTC_MSG_CMP_EVT:
        {
            //co_printf("op:%d done\r\n",p_msg->param.op.operation);
            if(p_msg->param.op.operation == GATT_OP_PEER_SVC_REGISTERED)
            {
                uint16_t att_handles[2];
                memcpy(att_handles,p_msg->param.op.arg,4);
                co_printf("att[0]_hdl:%d,att[1]_hdl:%d\r\n",att_handles[0],att_handles[1]);

                gatt_client_enable_ntf_t ntf_enable;
                ntf_enable.conidx = p_msg->conn_idx;
                ntf_enable.client_id = spsc_client_id;
                ntf_enable.att_idx = 0; //enable svc NTF operation for RX
                gatt_client_enable_ntf(ntf_enable);
            }
        }
        break;
        default:
            break;
    }
    return 0;
}

/*********************************************************************
 * @fn      at_spsc_send_data
 *
 * @brief   function to write date to peer. write without response
 *
 * @param   conidx - link  index
 *       	data   - pointer to data buffer 
 *       	len    - data len
 *
 * @return  None
 */
void at_spsc_send_data(uint8_t conidx,uint8_t *data, uint8_t len)
{
    //if(l2cm_get_nb_buffer_available() > 0)
    {
        gatt_client_write_t write;
        write.conidx = conidx;
        write.client_id = spsc_client_id;
        write.att_idx = 1; //TX
        write.p_data = data;
        write.data_len = MIN(len,gatt_get_mtu(conidx) - 3);
        gatt_client_write_cmd(write);
    } 
}

/*********************************************************************
 * @fn      at_profile_spsc_init
 *
 * @brief   Create AT profile client
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_profile_spsc_init(void)
{
    gatt_client_t client;

    memcpy(spsc_att_tb[0].p_uuid,&spss_uuids[16],UUID_SIZE_16);
    memcpy(spsc_att_tb[1].p_uuid,&spss_uuids[32],UUID_SIZE_16);

    client.p_att_tb = spsc_att_tb;
    client.att_nb = 2;
    client.gatt_msg_handler = spsc_client_msg_handler;
    spsc_client_id = gatt_add_client(&client);
}

