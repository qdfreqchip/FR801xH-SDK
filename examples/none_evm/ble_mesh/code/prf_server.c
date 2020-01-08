/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "gatt_api.h"
#include "gatt_sig_uuid.h"
#include "sys_utils.h"

uint8_t svc_id = 0;
const uint8_t svc_group_uuid[] = "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f";
const gatt_attribute_t att_db[] =
{
    [0] = { {UUID_SIZE_2,UUID16_ARR(GATT_PRIMARY_SERVICE_UUID)}
        ,GATT_PROP_READ,UUID_SIZE_16,(uint8_t *)svc_group_uuid
    },

    [1] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [2] = { {UUID_SIZE_16,"\x02\xF0\x13\x14"}
        ,GATT_PROP_READ|GATT_PROP_NOTI,20,NULL
    },
    [3] = { {UUID_SIZE_2,UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)}
        ,GATT_PROP_READ|GATT_PROP_WRITE,0,NULL
    },
    [4] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHAR_USER_DESC_UUID)}
        ,GATT_PROP_READ|GATT_PROP_WRITE,0xC,NULL
    },

    [5] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [6] = { {UUID_SIZE_16,"\x03\xF0\x13\x14"}
        ,GATT_PROP_READ,300,NULL
    },

    [7] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [8] = { {UUID_SIZE_2,"\x04\xF0"}
        ,GATT_PROP_WRITE,300,NULL
    },
};

uint8_t svc_id1 = 0;
const uint8_t svc1_group_uuid[] = "\x10\x11";
const gatt_attribute_t att_db1[] =
{
    [0] = { {UUID_SIZE_2,UUID16_ARR(GATT_PRIMARY_SERVICE_UUID)}
        ,GATT_PROP_READ,UUID_SIZE_2,(uint8_t *)svc1_group_uuid
    },

    [1] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [2] = { {UUID_SIZE_16,"\x03\x00\x13\x14"}
        ,GATT_PROP_READ,300,NULL
    },
};

uint16_t svc_msg_handler(gatt_msg_t *p_msg)
{
    co_printf("att_idx:%d\r\n",p_msg->att_idx);
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
            if(p_msg->att_idx == 6)
            {
                memcpy(p_msg->param.msg.p_msg_data, "\x7\x8\x9", 3);
                gatt_ntf_t ntf_att;
                ntf_att.att_idx = 2;
                ntf_att.conidx = p_msg->conn_idx;
                ntf_att.svc_id = svc_id;
                ntf_att.data_len = 4;
                uint8_t tmp[] = "12345";
                ntf_att.p_data = tmp;
                gatt_notification(ntf_att);
                return 3;
            }
            else if(p_msg->att_idx == 4)
            {
                memcpy(p_msg->param.msg.p_msg_data, "description", 11);
                return 11;
            }
            break;
        case GATTC_MSG_WRITE_REQ:
            if(p_msg->att_idx == 8)
            {
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
            if(p_msg->att_idx == 3)
            {
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
            break;
        case GATTC_MSG_CMP_EVT:
            if(p_msg->att_idx == 2)
            {
                co_printf("op:%d done\r\n",p_msg->param.op.operation);
            }
            break;
    }
    return 0;
};

uint16_t svc1_msg_handler(gatt_msg_t *p_msg)
{
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
            if(p_msg->att_idx == 2)
            {
                memcpy(p_msg->param.msg.p_msg_data, "\x1\x2\x3", 3);
                return 3;
            }
            break;
    }
    return 0;
};

void prf_server_create(void)
{
    gatt_service_t service;
    
    service.p_att_tb = att_db;
    service.att_nb = 9;
    service.gatt_msg_handler = svc_msg_handler;
    svc_id = gatt_add_service(&service);

    service.p_att_tb = att_db1;
    service.att_nb = 3;
    service.gatt_msg_handler = svc1_msg_handler;
    svc_id1 = gatt_add_service(&service);
}

