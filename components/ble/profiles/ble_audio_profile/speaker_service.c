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
#include "co_printf.h"
#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"

#include "speaker_service.h"
#include "driver_flash.h"
#include "decoder.h"

/*
 * MACROS 
 */
#define SPEAKER_DBG     FR_DBG_ON
#define SPEAKER_LOG     FR_LOG(SPEAKER_DBG)

/*
 * CONSTANTS 
 */



// Speaker Service UUID: 0xD0FF
const uint8_t speaker_svc_uuid[] = UUID16_ARR(SPEAKER_SVC_UUID);

/******************************* Characteristic 1 OnOff defination *******************************/
// Characteristic ONOFF UUID: 0xD001
// Characteristic ONOFF data
#define SPEAKER_ONOFF_VALUE_LEN  10
uint8_t speaker_onoff_value[SPEAKER_ONOFF_VALUE_LEN] = {0};
// Characteristic ONOFF User Description
#define SPEAKER_ONOFF_DESC_LEN   6
const uint8_t speaker_onoff_desc[SPEAKER_ONOFF_DESC_LEN] = "OnOff";

/******************************* Characteristic 2 status report defination *******************************/
// Characteristic status report UUID: 0xD002
// Characteristic status report data 
#define SPEAKER_STATUS_REPORT_VALUE_LEN  20
uint8_t speaker_status_report_value[SPEAKER_STATUS_REPORT_VALUE_LEN] = {0};
// Characteristic status report CCC
#define SPEAKER_STATUS_REPORT_CCC_LEN   2

#define SPEAKER_STATUS_REPORT_DESC_LEN   7
const uint8_t speaker_status_report_desc[SPEAKER_STATUS_REPORT_DESC_LEN] = "Status";

/******************************* Characteristic 3 audio RX defination *******************************/
// Characteristic audio RX UUID: 0xD003
// Characteristic audio RX data 
#define SPEAKER_AUDIO_RX_VALUE_LEN  512
uint8_t speaker_audio_rx_value[SPEAKER_AUDIO_RX_VALUE_LEN] = {0};


/*
 * TYPEDEFS 
 */

/*
 * GLOBAL VARIABLES 
 */
uint8_t speaker_svc_id = 0;
extern os_timer_t timer_refresh;// 用于刷新寄存器数据以及显示等


/*
 * LOCAL VARIABLES 
 */
static gatt_service_t speaker_svc;


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

const gatt_attribute_t speaker_att_table[SPEAKER_IDX_NB] =
{
    // Simple gatt Service Declaration
    [SPEAKER_IDX_SERVICE]                               =   {
                                                                { UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID) },     /* UUID */
                                                                GATT_PROP_READ,                                             /* Permissions */
                                                                UUID_SIZE_2,                                                /* Max size of the value */     /* Service UUID size in service declaration */
                                                                (uint8_t*)speaker_svc_uuid,                                      /* Value of the attribute */    /* Service UUID value in service declaration */
                                                            },

        // Characteristic OnOff Declaration           
        [SPEAKER_IDX_ONOFF_DECLARATION]                 =   {
                                                                { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                                GATT_PROP_READ,                                             /* Permissions */
                                                                0,                                                          /* Max size of the value */
                                                                NULL,                                                       /* Value of the attribute */
                                                            },
        // Characteristic OnOff Value                  
        [SPEAKER_IDX_ONOFF_VALUE]                       =   {
                                                                { UUID_SIZE_2, UUID16_ARR(SPEAKER_ONOFF_UUID) },            /* UUID */
                                                                GATT_PROP_WRITE,                                            /* Permissions */
                                                                SPEAKER_ONOFF_VALUE_LEN,                                         /* Max size of the value */
                                                                NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                            },             
        // Characteristic OnOff User Description
        [SPEAKER_IDX_ONOFF_USER_DESCRIPTION]            =   {
                                                                { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
                                                                GATT_PROP_READ,                                             /* Permissions */
                                                                SPEAKER_ONOFF_DESC_LEN,                                          /* Max size of the value */
                                                                (uint8_t *)speaker_onoff_desc,                                   /* Value of the attribute */
                                                            },


        // Characteristic status report Declaration
        [SPEAKER_IDX_STATUS_REPORT_DECLARATION]         =   {
                                                                { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                                GATT_PROP_READ,                                             /* Permissions */
                                                                0,                                                          /* Max size of the value */
                                                                NULL,                                                       /* Value of the attribute */
                                                            },
        // Characteristic status report Value   
        [SPEAKER_IDX_STATUS_REPORT_VALUE]               =   {
                                                                { UUID_SIZE_2, UUID16_ARR(SPEAKER_STATUS_REPORT_UUID) },                 /* UUID */
                                                                GATT_PROP_READ | GATT_PROP_NOTI,                                             /* Permissions */
                                                                SPEAKER_STATUS_REPORT_VALUE_LEN,                                         /* Max size of the value */
                                                                NULL,                                                       /* Value of the attribute */	/* Can assign a buffer here, or can be assigned in the application by user */
                                                            }, 
        // Characteristic status report client characteristic configuration
        [SPEAKER_IDX_STATUS_REPORT_CFG]                 =   {
                                                                { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },     /* UUID */
                                                                GATT_PROP_READ | GATT_PROP_WRITE,                           /* Permissions */
                                                                SPEAKER_STATUS_REPORT_CCC_LEN,                                           /* Max size of the value */
                                                                NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                            }, 
        // Characteristic status report User Description
        [SPEAKER_IDX_STATUS_REPORT_USER_DESCRIPTION]    =   {
                                                                { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },       /* UUID */
                                                                GATT_PROP_READ,                                             /* Permissions */
                                                                SPEAKER_STATUS_REPORT_DESC_LEN,                                          /* Max size of the value */
                                                                (uint8_t *)speaker_status_report_desc,                                   /* Value of the attribute */
                                                            },																																																						


        // Characteristic audio rx Declaration
        [SPEAKER_IDX_AUDIO_RX_DECLARATION]              =   {
                                                                { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                                GATT_PROP_READ,                                             /* Permissions */
                                                                0,                                                          /* Max size of the value */
                                                                NULL,                                                       /* Value of the attribute */
                                                            },
        // Characteristic audio rx Value
        [SPEAKER_IDX_AUDIO_RX_VALUE]                    =   {
                                                                { UUID_SIZE_2, UUID16_ARR(SPEAKER_AURIO_RX_UUID) },                 /* UUID */
                                                                GATT_PROP_WRITE,                                            /* Permissions */
                                                                SPEAKER_AUDIO_RX_VALUE_LEN,                                         /* Max size of the value */
                                                                NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                            },
};

#define SBC_DATA_FLASH_ADDR (USER_FLASH_BASE_ADDR+FLASH_PAGE_SIZE)//音频数据起始地址
/*********************************************************************
 * @fn      store_sbc_to_flash
 *
 * @brief   Stores SBC audio data into flash.
 *
 * @param   p_buf       - SBC audio data to be written to flash.
 *
 *          store_len   - length of SBC audio data to be written to flash.
 *
 * @return  None.
 */
void store_sbc_to_flash(uint8_t *p_buf, uint32_t store_len)
{
    if(sbc_sotre_env.last_offset == 0 && sbc_sotre_env.last_page_idx == 0)
        sbc_sotre_env.start_base = SBC_DATA_FLASH_ADDR;

    uint32_t sample_sz = store_len;
    uint32_t pos = 0;
    SPEAKER_LOG("a,");
    while( sample_sz > 0 )
    {
        if(sbc_sotre_env.last_offset < FLASH_PAGE_SIZE && sbc_sotre_env.last_offset > 0)
        {
            if( (sample_sz + sbc_sotre_env.last_offset) >= FLASH_PAGE_SIZE)
            {
                SPEAKER_LOG("x,");
                flash_write(sbc_sotre_env.start_base + sbc_sotre_env.last_page_idx *FLASH_PAGE_SIZE + sbc_sotre_env.last_offset
                                       ,  FLASH_PAGE_SIZE-sbc_sotre_env.last_offset, p_buf+pos);
                sample_sz -=(FLASH_PAGE_SIZE - sbc_sotre_env.last_offset);
                pos += (FLASH_PAGE_SIZE - sbc_sotre_env.last_offset);
                sbc_sotre_env.last_offset = 0;
                sbc_sotre_env.last_page_idx++;
            }
            else
            {
                SPEAKER_LOG("y,");
                flash_write(sbc_sotre_env.start_base + sbc_sotre_env.last_page_idx *FLASH_PAGE_SIZE + sbc_sotre_env.last_offset
                                       , sample_sz, p_buf+pos);
                pos += sample_sz;
                sbc_sotre_env.last_offset += sample_sz;
                sample_sz = 0;
            }
        }
        else
        {
            if(sample_sz >= FLASH_PAGE_SIZE)
            {
                SPEAKER_LOG("z,");
                flash_erase(sbc_sotre_env.start_base + sbc_sotre_env.last_page_idx *FLASH_PAGE_SIZE, 0);
                flash_write(sbc_sotre_env.start_base + sbc_sotre_env.last_page_idx *FLASH_PAGE_SIZE + sbc_sotre_env.last_offset
                                       , FLASH_PAGE_SIZE, p_buf+pos);
                sample_sz -=FLASH_PAGE_SIZE;
                pos += FLASH_PAGE_SIZE;
                sbc_sotre_env.last_offset = 0;
                sbc_sotre_env.last_page_idx++;
            }
            else
            {
                SPEAKER_LOG("w,");
                flash_erase(sbc_sotre_env.start_base + sbc_sotre_env.last_page_idx *FLASH_PAGE_SIZE, 0);
                flash_write(sbc_sotre_env.start_base + sbc_sotre_env.last_page_idx *FLASH_PAGE_SIZE + sbc_sotre_env.last_offset
                                       , sample_sz, p_buf+pos);
                pos += sample_sz;
                sbc_sotre_env.last_offset = sample_sz;
                sample_sz = 0;
            }
        }
    }
    SPEAKER_LOG("b\r\n");
}

/*********************************************************************
 * @fn      sp_gatt_read_cb
 *
 * @brief   Simple Profile user application handles read request in this callback.
 *			应用层在这个回调函数里面处理读的请求。
 *
 * @param   p_read  - the pointer to read buffer. NOTE: It's just a pointer from lower layer, please create the buffer in application layer.
 *					  指向读缓冲区的指针。 请注意这只是一个指针，请在应用程序中分配缓冲区. 为输出函数, 因此为指针的指针.
 *          len     - the pointer to the length of read buffer. Application to assign it.
 *                    读缓冲区的长度，用户应用程序去给它赋值.
 *          att_idx - index of the attribute value in it's attribute table.
 *					  Attribute的偏移量.
 *
 * @return  读请求的长度.
 */
static void sp_gatt_read_cb(uint8_t *p_read, uint16_t *len, uint16_t att_idx,uint8_t conn_idx )
{
    switch (att_idx)
    {
        case SPEAKER_IDX_ONOFF_VALUE:
            for (int i = 0; i < SPEAKER_ONOFF_VALUE_LEN; i++)
                speaker_onoff_value[i] = speaker_onoff_value[0] + i + 1;
            memcpy(p_read, speaker_onoff_value, SPEAKER_ONOFF_VALUE_LEN);
            *len = SPEAKER_ONOFF_VALUE_LEN;
        break;

        case SPEAKER_IDX_STATUS_REPORT_VALUE:
            for (int i = 0; i < SPEAKER_STATUS_REPORT_VALUE_LEN; i++)
                speaker_status_report_value[i] = speaker_status_report_value[0] + i + 1;
            memcpy(p_read, speaker_status_report_value, SPEAKER_STATUS_REPORT_VALUE_LEN);
            *len = SPEAKER_STATUS_REPORT_VALUE_LEN;
        break;
        
        default:
        break;
    }
    
	co_printf("Read request: len: %d  value: 0x%x 0x%x \r\n", *len, (p_read)[0], (p_read)[*len-1]);
    
}


/*********************************************************************
 * @fn      sp_gatt_write_cb
 *
 * @brief   Simple Profile user application handles write request in this callback.
 *			应用层在这个回调函数里面处理写的请求。
 *
 * @param   write_buf   - the buffer for write
 *			              写操作的数据.
 *					  
 *          len         - the length of write buffer.
 *                        写缓冲区的长度.
 *          att_idx     - index of the attribute value in it's attribute table.
 *					      Attribute的偏移量.
 *
 * @return  写请求的长度.
 */
static void sp_gatt_write_cb(uint8_t *write_buf, uint16_t len, uint16_t att_idx,uint8_t conn_idx)
{
	uint8_t *pos  =  write_buf;
	switch(att_idx){
		case SPEAKER_IDX_ONOFF_VALUE:
			if(*pos == 0x01)
			{
                SPEAKER_LOG("start\r\n");
                co_printf("t1\r\n");

                if( sbc_buff == NULL )
                {
                    //更新连接间隔
                    gap_conn_param_update(conn_idx, 6, 6, 0, 500);//Fr8010H
                    memset((void *)&sbc_sotre_env, 0, sizeof(sbc_sotre_env));
                }
                if( sbc_buff != NULL )
                {
                    gatt_ntf_t ntf_att;
                    uint8_t tmp1[] = "PLAY";
                    ntf_att.p_data = tmp1;
                    ntf_att.data_len = sizeof(tmp1);
                    ntf_att.att_idx = SPEAKER_IDX_STATUS_REPORT_VALUE;
                    ntf_att.conidx = conn_idx;//Connection index
                    ntf_att.svc_id = speaker_svc_id;
                    gatt_notification(ntf_att);
                }
                else
                {
                    gatt_ntf_t ntf_att;
                    uint8_t tmp[] = "OK";
                    ntf_att.p_data = tmp;
                    ntf_att.data_len = sizeof(tmp);

//						start_recv_flag = true;
                    ntf_att.att_idx = SPEAKER_IDX_STATUS_REPORT_VALUE;
                    ntf_att.conidx = conn_idx;//Connection index
                    ntf_att.svc_id = speaker_svc_id;
                    gatt_notification(ntf_att);
                }
			}
			else if(*pos == 0x02)
			{
                co_printf("t2\r\n");
                gatt_ntf_t ntf_att;
                ntf_att.att_idx = SPEAKER_IDX_STATUS_REPORT_VALUE;
                ntf_att.conidx = conn_idx;//Connection index
                ntf_att.svc_id = speaker_svc_id;
                uint8_t tmp[] = "finish";
                ntf_att.p_data = tmp;
                ntf_att.data_len = sizeof(tmp);
                gatt_notification(ntf_att);

                flash_erase(USER_FLASH_BASE_ADDR, 0);
                flash_write(USER_FLASH_BASE_ADDR, sizeof(sbc_sotre_env), (uint8_t *)&sbc_sotre_env);
                co_printf("end\r\n");
//				start_recv_flag = false;
                Flash_data_state = true;
			}
			break;

		case SPEAKER_IDX_AUDIO_RX_VALUE:
            {
                sbc_sotre_env.tot_data_len += len;
                if( sbc_sotre_env.tot_data_len >= 3072)
                {
                    sbc_sotre_env.tot_data_len -= 3072;

                    gatt_ntf_t ntf_att;
                    ntf_att.att_idx = SPEAKER_IDX_STATUS_REPORT_VALUE;
                    ntf_att.conidx = conn_idx;//Connection index
                    ntf_att.svc_id = speaker_svc_id;
                    uint8_t tmp[] = "send";
                    ntf_att.p_data = tmp;
                    ntf_att.data_len = sizeof(tmp);
                    gatt_notification(ntf_att);
                    SPEAKER_LOG("s");
                }
                SPEAKER_LOG("%d,",len);
                store_sbc_to_flash(write_buf,len);//将接收的数据写入flash
            }
            break;
    }


}

/*********************************************************************
 * @fn      sp_gatt_msg_handler
 *
 * @brief   Simple Profile callback funtion for GATT messages. GATT read/write
 *			operations are handeled here.
 *
 * @param   p_msg       - GATT messages from GATT layer.
 *
 * @return  uint16_t    - Length of handled message.
 */
static uint16_t sp_gatt_msg_handler(gatt_msg_t *p_msg)
{
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
            sp_gatt_read_cb((uint8_t *)(p_msg->param.msg.p_msg_data), &(p_msg->param.msg.msg_len), p_msg->att_idx,p_msg->conn_idx );
            break;
        
        case GATTC_MSG_WRITE_REQ:
            sp_gatt_write_cb((uint8_t*)(p_msg->param.msg.p_msg_data), (p_msg->param.msg.msg_len), p_msg->att_idx,p_msg->conn_idx);
            break;
            
        default:
            break;
    }
    return p_msg->param.msg.msg_len;
}

/*********************************************************************
 * @fn      speaker_gatt_add_service
 *
 * @brief   Speaker Profile add GATT service function.
 *			添加GATT service到ATT的数据库里面。
 *
 * @param   None. 
 *        
 *
 * @return  None.
 */
void speaker_gatt_add_service(void)
{
    speaker_svc.p_att_tb = speaker_att_table;
    speaker_svc.att_nb = SPEAKER_IDX_NB;
    speaker_svc.gatt_msg_handler = sp_gatt_msg_handler;
    
    speaker_svc_id = gatt_add_service(&speaker_svc);
}




