
/*
 * INCLUDES
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "gatt_api.h"
#include "gatt_sig_uuid.h"
#include "gap_api.h"
#include "gap_api.h"

#include "sys_utils.h" 
#include "os_mem.h"

#include "jump_table.h"
#include "driver_system.h"
#include "driver_plf.h"
#include "driver_flash.h"

#include "sha256.h"
#include "aes_cbc.h"
/*
 * MACROS
 */
#define  TOTALFRAME  0x0f

/*
 * TYPEDEFS
 */
__packed struct ali_gatt_header_t{
    uint8_t msg_id;
    uint8_t cmd;
    uint8_t frame_count;
    uint8_t frame_length;
};
__packed struct ali_gatt_packet_t{
    struct ali_gatt_header_t ali_gatt_header;
    uint8_t ali_gatt_paload[__ARRAY_EMPTY]; 
};
struct ali_gatt_packet_res_t{
    struct ali_gatt_header_t ali_gatt_header;
    uint8_t ali_gatt_paload[16]; 
};
struct ali_ota_env_t{
    uint8_t ali_ota_svc_id;
    uint8_t ali_ota_totalframe; //接收的总帧数
    uint32_t ali_ota_total_size; //接收的固件大小
    uint32_t ali_ota_firmware_size; //固件的总大小
    uint32_t ali_ota_storage_addr;
    uint8_t *ali_ota_version_buf;
    uint16_t ali_ota_version_buf_len;
    uint32_t ali_ota_version_current_buf_len;
    bool ali_ota_finish;
    uint8_t ali_ota_key[16];
};    

/*
 * GLOBAL VARIABLES
 */
    
/*
 * LOCAL VARIABLES
 */
static struct ali_ota_env_t ali_ota_env;

/*
 * CONSTANTS
 */
const uint8_t ali_ota_svc_group_uuid[] = "\xB3\xFE";

const gatt_attribute_t ali_ota_att_db[] =
{
    [0] = { {UUID_SIZE_2,UUID16_ARR(GATT_PRIMARY_SERVICE_UUID)}
        ,GATT_PROP_READ,UUID_SIZE_2,(uint8_t *)ali_ota_svc_group_uuid
    },

    [1] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [2] = { {UUID_SIZE_2,"\xD4\xFE"}
        ,GATT_PROP_READ,300,NULL
    },
    [3] = { {UUID_SIZE_2,UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)}
        ,GATT_PROP_READ|GATT_PROP_WRITE,0,NULL
    },  

    [4] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [5] = { {UUID_SIZE_2,"\xD5\xFE"}
        ,GATT_PROP_READ | GATT_PROP_WRITE_REQ,300,NULL
    },

    [6] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [7] = { {UUID_SIZE_2,"\xD6\xFE"}
        ,GATT_PROP_READ | GATT_PROP_INDI,300,NULL
    },
    [8] = { {UUID_SIZE_2,UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)}
        ,GATT_PROP_READ|GATT_PROP_WRITE,0,NULL
    },
      [9] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [10] = { {UUID_SIZE_2,"\xD7\xFE"}
        ,GATT_PROP_READ | GATT_PROP_WRITE_CMD,300,NULL
    },
     [11] = { {UUID_SIZE_2,UUID16_ARR(GATT_CHARACTER_UUID)}
        ,GATT_PROP_READ,0,NULL
    },
    [12] = { {UUID_SIZE_2,"\xD8\xFE"}
        ,GATT_PROP_READ | GATT_PROP_NOTI,300,NULL
    },
    [13] = { {UUID_SIZE_2,UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)}
        ,GATT_PROP_READ|GATT_PROP_WRITE,0,NULL
    },
};

/*
 * EXTERN FUNCTIONS
 */
extern const uint8_t ali_mesh_key_bdaddr[];
extern const uint8_t ali_mesh_key_pid[];

extern const uint8_t ali_mesh_key_secret[];

/*
 * PUBLIC FUNCTIONS
 */
uint16_t gattc_get_mtu(uint8_t idx);

/*********************************************************************
 * @fn      ali_ota_start_advertising
 *
 * @brief   Start broadcasting.
 *
 * @param   none.
 *                              
 * @return  none.
 */
void ali_ota_start_advertising(void)
{
    uint8_t adv_data[0x1f - 3] = {0};
    uint8_t rsp_data[0x1f] = {0};
    uint8_t *pos ;
    uint8_t length;
    
    uint8_t name[18] = {0};
    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 400;
    adv_param.adv_intv_max = 400;
    gap_set_advertising_param(&adv_param);
    pos = rsp_data;
    
    uint8_t uuid[2] = {0xFE,0xB3};
    *pos++ = 0x03;  //pos len;  (payload + type)
    *pos++  = '\x07';   //pos: type
    memcpy(pos, uuid, 16);
    pos += 16;

    pos = adv_data;
    *pos++ = 0x0f;
    *pos++ = 0xff;
    *pos++ = 0xA8;
    *pos++ = 0x01;
    *pos++ = 0x85;
    *pos++ = 0x0f;
    memcpy(pos,ali_mesh_key_pid,4);
    pos += 4;
    memcpy(pos,ali_mesh_key_bdaddr,6);
    pos += 6;
   
    /// 广播设备简称
    gap_get_dev_name(name);
    uint8_t dev_name_len = strlen((const char *)name) + 1; /// app_device_name 不能超过6节字
    //printf("dev_name_len %d %d\r\n",dev_name_len,((uint32_t)pos - (uint32_t)(msg.info.host.adv_data)));
    if((dev_name_len + ((uint32_t)pos - (uint32_t)(adv_data))) > (31 - 3)) /// 这里减３是因为flag 0x02 0x01 0x06 这个是自动添加的 20190731
        dev_name_len = 31 - 3 - ((uint32_t)pos - (uint32_t)(adv_data));
    co_printf("dev_name_len %d %s\r\n",dev_name_len,name);
    *pos++ = dev_name_len;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, name, dev_name_len);
    pos += dev_name_len;
    length = (uint32_t )pos - (uint32_t )adv_data;
    gap_set_advertising_data(adv_data, length);
  
    gap_start_advertising(0);
}

/*********************************************************************
 * @fn      ali_ota_stop_advertising
 *
 * @brief   Stop broadcasting.
 *
 * @param   none.
 *                              
 * @return  none.
 */
void ali_ota_stop_advertising(void)
{
    gap_stop_advertising();
}

/*********************************************************************
 * @fn      ali_ota_get_curr_firmwave_version
 *
 * @brief   get the current version number.
 *
 * @param   none.
 *                              
 * @return  version number.
 */
static uint32_t ali_ota_get_curr_firmwave_version(void)
{
    struct jump_table_t *jump_table_a = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)  // part B
    {
        struct jump_table_t *jump_table_b = (struct jump_table_t *)(0x01000000 + jump_table_a->image_size);
        return jump_table_b->firmware_version;
    }
    else        // part A
        return jump_table_a->firmware_version;
}

/*********************************************************************
 * @fn      ali_ota_save_data
 *
 * @brief   save data to flash.
 *
 * @param   dest   - destination address.
 *          src    - source data.
 *          len    - Source data length.
 *          
 * @return  none.
 */
__attribute__((section("ram_code"))) static void ali_ota_save_data(uint32_t dest, uint8_t *src, uint32_t len)
{
    uint32_t current_remap_address, remap_size;
    current_remap_address = system_regs->remap_virtual_addr;
    remap_size = system_regs->remap_length;

    *(volatile uint32_t *)0x500a0000 = 0x3c;
    while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x00);
    system_regs->remap_virtual_addr = 0;
    system_regs->remap_length = 0;

    flash_write(dest, len, src);

    system_regs->remap_virtual_addr = current_remap_address;
    system_regs->remap_length = remap_size;
    *(volatile uint32_t *)0x500a0000 = 0x3d;
    while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x02);
}

/*********************************************************************
 * @fn      ali_ota_get_storage_address
 *
 * @brief   get storage address.
 *
 * @param   none.
 *                              
 * @return  storage address.
 */
static uint32_t ali_ota_get_storage_address(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)      //partB, then return partA flash Addr
        return 0;
    else
        return jump_table_tmp->image_size;  //partA, then return partB flash Addr
}

/*********************************************************************
 * @fn      ali_ota_indication_data
 *
 * @brief   Send ota response data as notification.
 *
 * @param   conidx - connect index.
 *          data   - data to be sent.
 *          len    - data length.
 *          
 * @return  none.
 */
static void ali_ota_notification_data(uint8_t conidx, uint8_t *data, uint16_t len)
{
    uint16_t pakect_size;
    uint16_t offset = 0; 
    gatt_ntf_t ntf_att;
    ntf_att.att_idx = 12;
    ntf_att.conidx = conidx;
    ntf_att.svc_id = ali_ota_env.ali_ota_svc_id;  
    do{
        pakect_size = MIN(len, gattc_get_mtu(conidx) -7);  
        ntf_att.data_len = pakect_size;   
        ntf_att.p_data = data + offset;
        gatt_notification(ntf_att);
        offset += pakect_size;
        len -= pakect_size;
    }while(len > 0);
}

/*********************************************************************
 * @fn      ali_ota_indication_data
 *
 * @brief   Send ota response data as indication.
 *
 * @param   conidx - connect index.
 *          data   - data to be sent.
 *          len    - data length.
 *
 * @return  none.
 */
static void ali_ota_indication_data(uint8_t conidx, uint8_t *data, uint16_t len)
{
    uint16_t pakect_size;
    uint16_t offset = 0; 
    gatt_ind_t ind_att;
    ind_att.att_idx = 7;
    ind_att.conidx = conidx;
    ind_att.svc_id = ali_ota_env.ali_ota_svc_id;  
    do{
        pakect_size = MIN(len, gattc_get_mtu(conidx) -7);  
        ind_att.data_len = pakect_size;   
        ind_att.p_data = data + offset;
        gatt_indication(ind_att);
        offset += pakect_size;
        len -= pakect_size;
    }while(len > 0);
}

/*********************************************************************
 * @fn      ali_ota_cmd_res
 *
 * @brief   Send ota response data.
 *
 * @param   msg_id    - message id.
 *          cmd       - command.
 *          paload    - Content of respond.
 *          indcation - 0end using notification  1、Send using indication
 *          
 * @return  none.
 */
void ali_ota_cmd_res(uint8_t msg_id,uint8_t cmd,uint8_t *paload,bool indcation)
{
    uint8_t out_put[16] = {0};
    struct ali_gatt_packet_res_t ali_gatt_packet_tx;
    aes_encrypt_data(ali_ota_env.ali_ota_key,paload,out_put);
    ali_gatt_packet_tx.ali_gatt_header.msg_id = msg_id;
    ali_gatt_packet_tx.ali_gatt_header.cmd = cmd;
    ali_gatt_packet_tx.ali_gatt_header.frame_count = 0x00;
    ali_gatt_packet_tx.ali_gatt_header.frame_length = 0x10;
    memcpy(ali_gatt_packet_tx.ali_gatt_paload,out_put,0x10);
    if(indcation)
    {
        ali_ota_indication_data(0,(uint8_t *)&ali_gatt_packet_tx,sizeof(ali_gatt_packet_tx)); 
    }else
    {
        ali_ota_notification_data(0,(uint8_t*)&ali_gatt_packet_tx,sizeof(ali_gatt_packet_tx));
    }
}

/*********************************************************************
 * @fn      ali_ota_svc_msg_handler
 *
 * @brief   receive message processing.
 *
 * @param   p_msg  - received message data.
 *                              
 * @return  none.
 */
uint16_t ali_ota_svc_msg_handler(gatt_msg_t *p_msg)
{
    struct ali_gatt_packet_t *ali_gatt_packet_rx;
    struct ali_gatt_packet_res_t ali_gatt_packet_tx;
    uint8_t src_data[16] = {0};
    uint8_t in_put[16] = {0};
    uint8_t erase_count = 0;
    uint8_t erase_total_count;
    uint32_t erase_addr;
    co_printf("att_idx:%d,%d\r\n",p_msg->att_idx,p_msg->msg_evt);
	//show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);

    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
            break;
        case GATTC_MSG_WRITE_REQ:            
            ali_gatt_packet_rx = (struct ali_gatt_packet_t *)p_msg->param.msg.p_msg_data;
            switch(ali_gatt_packet_rx->ali_gatt_header.cmd){
                case 0x10:
                    gatt_mtu_exchange_req(0);
                    ali_ota_env.ali_ota_finish = false;
                    sha256_gatt_gen_auth_value((BYTE *)ali_gatt_packet_rx->ali_gatt_paload,(BYTE *)ali_mesh_key_pid,(BYTE *)ali_mesh_key_bdaddr,(BYTE *)ali_mesh_key_secret,ali_ota_env.ali_ota_key);
                    ali_ota_cmd_res(ali_gatt_packet_rx->ali_gatt_header.msg_id,0x11,ali_gatt_packet_rx->ali_gatt_paload,true);
                    
                    break;
                case 0x20:
    
                    memset(in_put,0x0b,16);
                    in_put[0] = 0x00;
                    memcpy(&in_put[1],&__jump_table.firmware_version,4);
                    ali_ota_cmd_res(ali_gatt_packet_rx->ali_gatt_header.msg_id,0x21,in_put,false);
                    break;
                case 0x12:
                    ali_gatt_packet_tx.ali_gatt_header.msg_id = ali_gatt_packet_rx->ali_gatt_header.msg_id;
                    ali_gatt_packet_tx.ali_gatt_header.cmd = 0x13;
                    ali_gatt_packet_tx.ali_gatt_header.frame_count = 0x00;
                    ali_gatt_packet_tx.ali_gatt_header.frame_length = 0x01;
                    ali_gatt_packet_tx.ali_gatt_paload[0] = 0x00;
                    ali_ota_indication_data(0,(uint8_t*)&ali_gatt_packet_tx,5);
                    break;
                case 0x22:
                    
                    aes_decrypt_data(ali_ota_env.ali_ota_key,ali_gatt_packet_rx->ali_gatt_paload,src_data);
                    ali_ota_env.ali_ota_firmware_size = src_data[5] | ((src_data[6] << 8) & 0xFF00) | ((src_data[7] << 16) & 0xFF0000) | ((src_data[8] << 24) & 0xFF0000);
                    ali_ota_env.ali_ota_storage_addr = ali_ota_get_storage_address();
                    erase_addr = ali_ota_env.ali_ota_storage_addr;
                    erase_total_count = ROUND(ali_ota_env.ali_ota_firmware_size,0x1000);
                    while(erase_count <= erase_total_count)
                    {
                        flash_erase(erase_addr,0x1000);                       
                        erase_count ++;
                        erase_addr += 0x1000;
                    }
                    memset(in_put,0x0a,16);
                    in_put[0] = 0x01;
                    in_put[1] = 0x00;
                    in_put[2] = 0x00;
                    in_put[3] = 0x00;
                    in_put[4] = 0x00;
                    in_put[5] = TOTALFRAME;
                    ali_ota_cmd_res(ali_gatt_packet_rx->ali_gatt_header.msg_id,0x23,in_put,false);
                    ali_ota_env.ali_ota_totalframe = 0;
                    ali_ota_env.ali_ota_total_size = 0;
                    break;
                case 0x2F:
                    ali_ota_env.ali_ota_totalframe++;
                    if(ali_ota_env.ali_ota_total_size == 0)
                    {
                        ali_ota_env.ali_ota_version_buf_len = ROUND(380,ali_gatt_packet_rx->ali_gatt_header.frame_length) * ali_gatt_packet_rx->ali_gatt_header.frame_length;
                        ali_ota_env.ali_ota_version_buf = (uint8_t *)os_malloc(ali_ota_env.ali_ota_version_buf_len);
                        ali_ota_env.ali_ota_version_current_buf_len += ali_gatt_packet_rx->ali_gatt_header.frame_length;
                        memcpy(ali_ota_env.ali_ota_version_buf,ali_gatt_packet_rx->ali_gatt_paload,ali_ota_env.ali_ota_version_current_buf_len);
                    }else{
                        if(ali_ota_env.ali_ota_version_current_buf_len < ali_ota_env.ali_ota_version_buf_len)
                        {
                            co_printf("ota_version:%d,%d\r\n",ali_ota_env.ali_ota_version_current_buf_len,ali_ota_env.ali_ota_version_buf_len);
                            memcpy(ali_ota_env.ali_ota_version_buf + ali_ota_env.ali_ota_version_current_buf_len,ali_gatt_packet_rx->ali_gatt_paload,ali_gatt_packet_rx->ali_gatt_header.frame_length);
                            ali_ota_env.ali_ota_version_current_buf_len += ali_gatt_packet_rx->ali_gatt_header.frame_length;
                        }else
                        {
                            ali_ota_save_data(ali_ota_env.ali_ota_storage_addr + ali_ota_env.ali_ota_total_size,
                                       ali_gatt_packet_rx->ali_gatt_paload,
                                       ali_gatt_packet_rx->ali_gatt_header.frame_length);
                        }
                    }
                    if(ali_ota_env.ali_ota_version_current_buf_len == ali_ota_env.ali_ota_version_buf_len)
                    {
                        uint32_t firmware_offset = (uint32_t)&((struct jump_table_t *)0x01000000)->firmware_version- 0x01000000;
                        if( *(uint32_t *)((uint32_t)ali_ota_env.ali_ota_version_buf + firmware_offset) <= ali_ota_get_curr_firmwave_version() )
                        {
                            uint32_t new_bin_ver = ali_ota_get_curr_firmwave_version() + 1;
                            co_printf("old_ver:%08X\r\n",*(uint32_t *)((uint32_t)ali_ota_env.ali_ota_version_buf + firmware_offset));
                            co_printf("new_ver:%08X\r\n",new_bin_ver);
                            //checksum_minus = new_bin_ver - *(uint32_t *)((uint32_t)first_pkt.buf + firmware_offset);
                            *(uint32_t *)((uint32_t)ali_ota_env.ali_ota_version_buf + firmware_offset) = new_bin_ver;
                        }
                    }
                    ali_ota_env.ali_ota_total_size += ali_gatt_packet_rx->ali_gatt_header.frame_length;
                    co_printf("total:%d,%d\r\n",ali_ota_env.ali_ota_totalframe,ali_ota_env.ali_ota_total_size);
                        
                        
                    if(((ali_ota_env.ali_ota_totalframe - 1) == TOTALFRAME) 
                        || (((ali_gatt_packet_rx->ali_gatt_header.frame_count & 0x0f))  == TOTALFRAME)
                        || (ali_ota_env.ali_ota_firmware_size == ali_ota_env.ali_ota_total_size))
                    {   
                        //co_printf("total:%d,%d\r\n",ali_ota_totalframe,ali_gatt_packet_rx->ali_gatt_header.frame_count);
                        memset(in_put,0x0b,16);
                        ali_ota_env.ali_ota_totalframe = 0;
                        in_put[0] = (TOTALFRAME << 4) | (ali_gatt_packet_rx->ali_gatt_header.frame_count & 0x0f);
                        in_put[1] = ali_ota_env.ali_ota_total_size & 0xff;
                        in_put[2] = (ali_ota_env.ali_ota_total_size >> 8) & 0xff;
                        in_put[3] = (ali_ota_env.ali_ota_total_size >> 16) & 0xff;
                        in_put[4] = (ali_ota_env.ali_ota_total_size >> 24) & 0xff;
                        ali_ota_cmd_res(ali_gatt_packet_rx->ali_gatt_header.msg_id | 0x10,0x24,in_put,false);
                    }
                    break;
                    case 0x25:
                        ali_ota_save_data(ali_ota_env.ali_ota_storage_addr,ali_ota_env.ali_ota_version_buf,ali_ota_env.ali_ota_version_buf_len);
                        os_free(ali_ota_env.ali_ota_version_buf);
                        memset(in_put,0x0f,16);
                        in_put[0] = 0x01;                        
                        ali_ota_cmd_res(ali_gatt_packet_rx->ali_gatt_header.msg_id,0x26,in_put,false);
                        ali_ota_env.ali_ota_finish = true;
                        show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
                        break;
                    default:
                        show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
                        break;
            }
                     
            break;
        case GATTC_MSG_CMP_EVT:
            if(ali_ota_env.ali_ota_finish == true)
            {
                co_delay_100us(10000);
                NVIC_SystemReset();        
            }
            
            break;
    }
    return 0;
};

/*********************************************************************
 * @fn      ali_ota_server_create
 *
 * @brief   create ota server.
 *
 * @param   none.
 *          
 * @return  none.
 */
void ali_ota_server_create(void)
{
    gatt_service_t service;
    
    service.p_att_tb = ali_ota_att_db;
    service.att_nb = 14;
    service.gatt_msg_handler = ali_ota_svc_msg_handler;
    ali_ota_env.ali_ota_svc_id = gatt_add_service(&service);
}
