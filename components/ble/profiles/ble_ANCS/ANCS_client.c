/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 * ANCS client profile. protocol websit:
 * https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification
 * /Specification/Specification.html#//apple_ref/doc/uid/TP40013460-CH1-SW14
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
#include "ANCS_client.h"


/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */

/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */


/*
 * LOCAL VARIABLES (本地变量)
 */
uint8_t ANCS_client_id;

#define CTL_POINT_UUID     {0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98, \
                                            0xa8, 0x49, 0xe1, 0x45, 0xf3,0xd8, 0xd1, 0x69}
#define NTF_SRC_UUID       {0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, \
                                            0xd9, 0x42, 0x01, 0x63, 0x0d,0x12, 0xbf, 0x9f}
#define DATA_SRC_UUID      {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, \
                                            0xb5, 0x4b, 0xd6, 0x24, 0xe9,0xc6, 0xea, 0x22}

/*********************************************************************
 * Profile Attributes - Table
 * 每一项都是一个characteristic attribute对应的UUID的定义。
 * 第一个是UUID的长度，
 * 每二个是UUID的值。
 */
const gatt_uuid_t ANCS_att_tb[] =
{
    [ANCS_ATT_IDX_CTL_POINT] = { UUID_SIZE_16, CTL_POINT_UUID},
    [ANCS_ATT_IDX_NTF_SRC] = { UUID_SIZE_16, NTF_SRC_UUID},
    [ANCS_ATT_IDX_DATA_SRC] = { UUID_SIZE_16, DATA_SRC_UUID},
};

#define EVT_FLAG_SILENT         BIT(0)
#define EVT_FLAG_IMPORTANT      BIT(1)
#define EVT_FLAG_PRE_EXSITING   BIT(2)
#define EVT_FLAG_POSITIVE       BIT(3)
#define EVT_FLAG_NEGATIVE       BIT(4)

#define CATGRY_ID_OTHER         (0)
#define CATGRY_ID_CALL          (1)
#define CATGRY_ID_MISS_CALL     (2)
#define CATGRY_ID_VOICE_MAIL    (3)
#define CATGRY_ID_SOCIAL        (4)
#define CATGRY_ID_SCHEDULE      (5)
#define CATGRY_ID_EMAIL         (6)
#define CATGRY_ID_NEWS          (7)
#define CATGRY_ID_HEALTH        (8)
#define CATGRY_ID_BUSINESS      (9)
#define CATGRY_ID_LOCATION      (10)
#define CATGRY_ID_ENTERTAINMENT (11)

struct ancs_ntf_src
{
    uint8_t event_id;
    uint8_t event_flags;
    uint8_t category_id;
    uint8_t category_cnt;
    uint32_t ntf_uid;
};

#define NTF_ATT_ID_APPLE        0
#define NTF_ATT_ID_TITLE        1
#define NTF_ATT_ID_SUBTITLE     2
#define NTF_ATT_ID_MSG          3
#define NTF_ATT_ID_MSG_SIZE     4
#define NTF_ATT_ID_DATE         5
#define NTF_ATT_ID_POSITIVE_ACT 6
#define NTF_ATT_ID_NEGATIVE_ACT 7

void ANCS_recv_ntf_src(uint8_t conidx,uint8_t *p_data, uint16_t len)
{
    struct ancs_ntf_src *ntf_src = (struct ancs_ntf_src *)p_data;
    co_printf("event_id:%d,event_flags:%x,category_id:%d,category_cnt:%d,ntf_uid:%x\r\n",ntf_src->event_id
           ,ntf_src->event_flags,ntf_src->category_id,ntf_src->category_cnt,ntf_src->ntf_uid);

    if( ( ntf_src->event_flags == EVT_FLAG_NEGATIVE && ntf_src->category_id == CATGRY_ID_SOCIAL)
        || (ntf_src->event_flags == (EVT_FLAG_NEGATIVE|EVT_FLAG_POSITIVE|EVT_FLAG_IMPORTANT) && ntf_src->category_id == CATGRY_ID_CALL)
      )
    {
    //co_printf("Send RSP\r\n");
        uint8_t rsp[12];
        uint8_t i = 0;
        rsp[i++] = 0;
        *(uint32_t *)(rsp + i) = ntf_src->ntf_uid;
        i+=4;
        rsp[i++] = NTF_ATT_ID_APPLE;
        rsp[i++] = NTF_ATT_ID_TITLE;
        *(uint16_t *)(rsp + i) = 32;
        i+=2;
        rsp[i++] = NTF_ATT_ID_MSG;
        *(uint16_t *)(rsp + i) = MIN( (gatt_get_mtu(conidx) - 32 - 3), 185);
        
        co_printf("Send RSP:%d\r\n",MIN( (gatt_get_mtu(conidx) - 32 - 3), 185) );

        ANCS_gatt_write_req(conidx,ANCS_ATT_IDX_CTL_POINT,rsp,sizeof(rsp));
    }
}

void ANCS_recv_data_src(uint8_t conidx,uint8_t *p_data, uint16_t len)
{
    uint8_t *str = NULL;
    uint16_t str_len = 0;
    uint8_t msg_type = 0;
    uint16_t i = 0;
    uint16_t data_len = 0;

    uint8_t event_id = p_data[i++];
    uint32_t uid = *(uint32_t *)(p_data + i);
    i+=4;
    co_printf("evt_id:%d,uid:%x,len:%d\r\n",event_id,uid,len);
    while(i < len)
    {
        switch(p_data[i++])
        {
            case NTF_ATT_ID_APPLE:
                data_len = *(uint16_t *)(p_data + i);
                i+=2;
                //show_reg(param->value + i,data_len,1);
                if( memcmp(p_data+i+data_len-3,"xin",3)==0 )
                    msg_type = 3;
                else if( memcmp(p_data+i+data_len-5,"phone",5)==0 )
                    msg_type = 0;
                else if( memcmp(p_data+i+data_len-3,"mqq",3)==0 )
                    msg_type = 2;
                i+=data_len;
                break;
            case NTF_ATT_ID_TITLE:
                data_len = *(uint16_t *)(p_data + i);
                i+=2;
                //show_reg(param->value + i,data_len,1);

                ////get str
                str = (uint8_t *)(p_data + i);
                if(msg_type != 0 && msg_type != 2)
                    str_len += data_len;
                else    //get phone num str
                {
                    for(uint8_t j=0; j<data_len; j++)
                    {
                        if(p_data[i+j] >= '0' && p_data[i+j] <= '9')
                        {
                            *(str+str_len) =  p_data[i+j];
                            str_len++;
                        }
                    }
                }
                ////get str
                i+=data_len;
                break;
            case NTF_ATT_ID_MSG:
                data_len = *(uint16_t *)(p_data + i);
                i+=2;
                //show_reg(param->value + i,data_len,1);

                if(msg_type != 0)
                {
                    memcpy(str+str_len,p_data + i,data_len);
                    str_len += data_len;
                }

                i+=data_len;
                break;
            default:
                co_printf("ERRR\r\n");
                break;
        }
    }
    *(str + str_len) = 0;

    if(msg_type == 0)
    {
        //ui_show_phone_call(3000,str);
    }
    else
    {
        //ui_show_chinese_str(msg_type,3000,str);
    }
}

/*********************************************************************
 * @fn      dis_gatt_msg_handler
 *
 * @brief   Device information gatt message handler.
 *
 *
 * @param   p_msg  - messages from GATT layer.
 *
 * @return  none.
 */
uint16_t ANCS_gatt_msg_handler(gatt_msg_t *p_msg)
{
    co_printf("CCC:%x\r\n",p_msg->msg_evt);
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_NTF_REQ:
        {
            if(p_msg->att_idx == ANCS_ATT_IDX_CTL_POINT)
            {
                ;
            }
            else if(p_msg->att_idx == ANCS_ATT_IDX_NTF_SRC)
            {
                ANCS_recv_ntf_src(p_msg->conn_idx,p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len);
            }
            else if(p_msg->att_idx == ANCS_ATT_IDX_DATA_SRC)
            {
                ANCS_recv_data_src(p_msg->conn_idx,p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len);
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
            co_printf("op:%d done\r\n",p_msg->param.op.operation);
            if(p_msg->param.op.operation == GATT_OP_PEER_SVC_REGISTERED)
            {
                uint16_t att_handles[3];
                memcpy(att_handles,p_msg->param.op.arg,6);
                show_reg((uint8_t *)att_handles,6,1);

                gatt_client_enable_ntf_t ntf_enable;
                ntf_enable.conidx = p_msg->conn_idx;
                ntf_enable.client_id = ANCS_client_id;
                ntf_enable.att_idx = ANCS_ATT_IDX_NTF_SRC;
                gatt_client_enable_ntf(ntf_enable);
                
                ntf_enable.att_idx = ANCS_ATT_IDX_DATA_SRC;
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
 * @fn      batt_gatt_notify
 *
 * @brief   Send batt level notification to peer.
 *
 *
 * @param   conidx  - link idx.
 *          batt_level  - battery energy percentage.
 *
 * @return  none.
 */
void ANCS_gatt_write_cmd(uint8_t conidx,enum ancs_att_idx att_idx,uint8_t *p_data, uint16_t len)
{
    gatt_client_write_t write;
    write.conidx = conidx;
    write.client_id = ANCS_client_id;
    write.att_idx = att_idx;
    write.p_data = p_data;
    write.data_len = len;
    gatt_client_write_cmd(write);
}
void ANCS_gatt_write_req(uint8_t conidx,enum ancs_att_idx att_idx,uint8_t *p_data, uint16_t len)
{
    gatt_client_write_t write;
    write.conidx = conidx;
    write.client_id = ANCS_client_id;
    write.att_idx = att_idx;
    write.p_data = p_data;
    write.data_len = len;
    gatt_client_write_req(write);
}

void ANCS_gatt_read(uint8_t conidx,enum ancs_att_idx att_idx)
{
    gatt_client_read_t read;
    read.conidx = conidx;
    read.client_id = ANCS_client_id;
    read.att_idx = att_idx;
    gatt_client_read(read);
}

/*********************************************************************
 * @fn      batt_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *          添加GATT service到ATT的数据库里面。
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void ANCS_gatt_add_client(void)
{
    gatt_client_t client;

    client.p_att_tb = ANCS_att_tb;
    client.att_nb = ANCS_ATT_IDX_MAX;
    client.gatt_msg_handler = ANCS_gatt_msg_handler;
    ANCS_client_id = gatt_add_client(&client);
}





