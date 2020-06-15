#include <stdint.h>
#include <string.h>
#include "co_printf.h"
#include "co_log.h"

#include "at_cmd_task.h"
#include "at_recv_cmd.h"

#include "driver_uart.h"
#include "driver_system.h"
#include "os_mem.h"

#include "sys_utils.h"
#include "at_profile_spsc.h"
#include "at_profile_spss.h"
#include "at_gap_event.h"

#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"

#define AT_ASSERT(v) do { \
    if (!(v)) {             \
        co_printf("%s %s \n", __FILE__, __LINE__); \
        while (1) {};   \
    }                   \
} while (0);
#undef LOG_LEVEL_MODULE
#define LOG_LEVEL_MODULE        LOG_LEVEL_INFO

enum
{
    AT_CMD_IDX_NAME,
    AT_CMD_IDX_MODE,
    AT_CMD_IDX_MAC,
    AT_CMD_IDX_CIVER,
    AT_CMD_IDX_UART,
    AT_CMD_IDX_Z,
    AT_CMD_IDX_CLR_BOND,
    AT_CMD_IDX_LINK,
    AT_CMD_IDX_ENC,
    AT_CMD_IDX_SCAN,
    AT_CMD_IDX_APP,
    AT_CMD_IDX_CONNADD,
    AT_CMD_IDX_CONN,
    AT_CMD_IDX_SLEEP,
    AT_CMD_IDX_UUID,
    AT_CMD_IDX_DISCONN,
    AT_CMD_IDX_FLASH,
    AT_CMD_IDX_SEND,
    AT_CMD_IDX_TRANSPARENT,
    AT_CMD_IDX_AUTO_TRANSPARENT,
    AT_CMD_IDX_POWER,
    AT_CMD_IDX_ADVINT,
};
const char *cmds[] =
{
    [AT_CMD_IDX_NAME] = "NAME",
    [AT_CMD_IDX_MODE] = "MODE",
    [AT_CMD_IDX_MAC] = "MAC",
    [AT_CMD_IDX_CIVER] = "CIVER",
    [AT_CMD_IDX_UART] = "UART",
    [AT_CMD_IDX_Z] = "Z",
    [AT_CMD_IDX_CLR_BOND] = "CLR_BOND",
    [AT_CMD_IDX_LINK] = "LINK",
    [AT_CMD_IDX_ENC] = "ENC",
    [AT_CMD_IDX_SCAN] = "SCAN",
    [AT_CMD_IDX_APP] = "ADP",
    [AT_CMD_IDX_CONNADD] = "CONNADD",
    [AT_CMD_IDX_CONN] = "CONN",
    [AT_CMD_IDX_SLEEP] = "SLEEP",
    [AT_CMD_IDX_UUID] = "UUID",
    [AT_CMD_IDX_DISCONN] = "DISCONN",
    [AT_CMD_IDX_FLASH] = "FLASH",
    [AT_CMD_IDX_SEND] = "SEND",
    [AT_CMD_IDX_TRANSPARENT] = "+++",
    [AT_CMD_IDX_AUTO_TRANSPARENT] = "AUTO+++",
    [AT_CMD_IDX_POWER] = "POWER",
    [AT_CMD_IDX_ADVINT] = "ADVINT",
};
struct at_buff_env gAT_buff_env = {0};
struct at_ctrl gAT_ctrl_env = {0};

const uint8_t rf_power_arr[6] = {0x28,0x13,0x10,0x08,0x04,0x02};
const uint16_t adv_int_arr[6] = {80,160,320,800,1600,3200};

/*********************************************************************
 * @fn      at_send_rsp
 *
 * @brief   Common function for at command execution result response sending 
 *			
 *
 * @param   str - AT command execution result string. 
 *       	 
 *
 * @return  None
 */
void at_send_rsp(char *str)
{
    uart_put_data_noint(UART0,(uint8_t *)"\r\n", 2);
    uart_put_data_noint(UART0,(uint8_t *)str, strlen(str));
    uart_put_data_noint(UART0,(uint8_t *)"\r\n", 2);
}

/*********************************************************************
 * @fn      find_int_from_str
 *
 * @brief   Misc function, search and strim an integer number string from a string buffer, 
 *			
 *
 * @param   buff - pointer to string buffer, which will be searched.  
 *       	 
 *
 * @return  buffer start address of next string.
 */
static uint8_t *find_int_from_str(uint8_t *buff)
{
    uint8_t *pos = buff;
    while(1)
    {
        if(*pos == ',' || *pos == '\r')
        {
            *pos = 0;
            break;
        }
        pos++;
    }
    return pos;
}

/*********************************************************************
 * @fn      find_uart_idx_from_baudrate
 *
 * @brief   Misc function, tranfer actual baudrate value to uart_baud_map array index. 
 *			
 *
 * @param   baudrate - Actual baudrate value  
 *       	 
 *
 * @return  Array index in the uart_baud_map array
 */
uint8_t find_uart_idx_from_baudrate(uint32_t baudrate)
{
    uint8_t i;
    uint32_t uart_baud_map[12] = {1200,2400,4800,9600,14400,19200,38400,57600,115200,230400,460800,921600};
    for(i=0; i<12; i++)
    {
        if(baudrate == uart_baud_map[i])
            return i;
    }
    return 8;
}

/*********************************************************************
 * @fn      at_spss_recv_data_ind_func
 *
 * @brief   This function will send data which is received from AT service profile to UART0.
 *			
 *
 * @param   value  - point to data buffer received from AT service profile  
 *       	length - data buffer length
 *
 * @return   None
 */
void at_spss_recv_data_ind_func(uint8_t *value, uint16_t length)
{
    if( os_get_free_heap_size()>11264 )
        uart_put_data_noint(UART0,value,length);
    else
        uart_putc_noint(UART1,'Y');
    //co_printf("%d\r\n",length);
}

/*********************************************************************
 * @fn      at_spsc_recv_data_ind_func
 *
 * @brief   This function will send data which is received from AT client profile to UART0.
 *			
 *
 * @param   value  - point to data buffer received from AT service profile  
 *       	length - data buffer length
 *
 * @return   None
 */
void at_spsc_recv_data_ind_func(uint8_t *value, uint16_t length)
{
    if( os_get_free_heap_size()>11264 )
        uart_put_data_noint(UART0,value,length);    //uart is no limited
    else
        uart_putc_noint(UART1,'Y');
    //co_printf("%d\r\n",length);
}


/***********SCAN Handle***************/

/*********************************************************************
 * @fn      at_get_adv
 *
 * @brief   at event call back function, handle after advertising report is posted
 *			
 *
 * @param   arg  - pointer to advertising report buffer
 *       	
 *
 * @return   None
 */
void at_get_adv(void *arg)
{
    gap_evt_adv_report_t *param = (gap_evt_adv_report_t *)arg;
    uint8_t free_rpt_idx = 0xff;
    for(uint8_t idx = 0; idx<ADV_REPORT_NUM; idx++)
    {
        if(gAT_buff_env.adv_rpt[idx].evt_type == 0xff && free_rpt_idx == 0xff)
            free_rpt_idx = idx;
        if(memcmp(gAT_buff_env.adv_rpt[idx].adv_addr.addr,param->src_addr.addr.addr,sizeof(mac_addr_t)) == 0 )
            goto _exit;
    }
    gAT_buff_env.adv_rpt[free_rpt_idx].evt_type = param->evt_type;
    gAT_buff_env.adv_rpt[free_rpt_idx].adv_addr_type = param->src_addr.addr_type;
    memcpy(gAT_buff_env.adv_rpt[free_rpt_idx].adv_addr.addr,param->src_addr.addr.addr,sizeof(mac_addr_t));
    gAT_buff_env.adv_rpt[free_rpt_idx].rssi = param->rssi;
    gAT_buff_env.adv_rpt[free_rpt_idx].data_len = param->length;
    memcpy(gAT_buff_env.adv_rpt[free_rpt_idx].data,param->data,param->length);
_exit:
    ;
}

/*********************************************************************
 * @fn      at_scan_done
 *
 * @brief   at event call back function, handle after scan operation end
 *			
 *
 * @param   arg  - pointer to buffer, which store scan done status
 *       	
 *
 * @return  None
 */
void at_scan_done(void *arg)
{
    uint8_t *at_rsp = os_malloc(150);
    uint8_t *addr_str = os_malloc(MAC_ADDR_LEN*2+1);
    uint8_t *rsp_data_str = os_malloc(0x1F*2+1);        //adv data len

    sprintf((char *)at_rsp,"+SCAN:ON\r\nOK");
    at_send_rsp((char *)at_rsp);

    for(uint8_t idx = 0; idx<ADV_REPORT_NUM; idx++)
    {
        if(gAT_buff_env.adv_rpt[idx].evt_type ==0)
        {
            hex_arr_to_str(gAT_buff_env.adv_rpt[idx].adv_addr.addr,MAC_ADDR_LEN,addr_str);
            addr_str[MAC_ADDR_LEN * 2] = 0;

            if(gAT_buff_env.adv_rpt[idx].data_len != 0)
            {
                hex_arr_to_str(gAT_buff_env.adv_rpt[idx].data,gAT_buff_env.adv_rpt[idx].data_len,rsp_data_str);
                rsp_data_str[gAT_buff_env.adv_rpt[idx].data_len * 2] = 0;
            }
            else
                memcpy(rsp_data_str,"NONE",sizeof("NONE"));

            sprintf((char *)at_rsp,"\n\nNo: %d Addr:%s Rssi:%ddBm\n\n\r\nAdv data: \r\n %s\r\n",idx
                    ,addr_str
                    ,(signed char)gAT_buff_env.adv_rpt[idx].rssi
                    ,rsp_data_str);
            uart_put_data_noint(UART0,(uint8_t *)at_rsp, strlen((const char *)at_rsp));
        }
        else
            break;
    }
    os_free(rsp_data_str);
    os_free(addr_str);
    os_free(at_rsp);
    gAT_ctrl_env.async_evt_on_going = false;
}


/*********************************************************************
 * @fn      at_start_scan
 *
 * @brief   Start a active scan opertaion, duration is deceided by gAT_ctrl_env.scan_duration. Or 10s if gAT_ctrl_env.scan_duration is 0
 *			
 *
 * @param   None
 *       	
 *
 * @return  None
 */
void at_start_scan(void)
{
    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    if(gAT_ctrl_env.scan_duration ==0)
        scan_param.duration = 1000;     //10s
    else
        scan_param.duration = gAT_ctrl_env.scan_duration;
    gap_set_link_rssi_report(true);
    gap_start_scan(&scan_param);
    gAT_ctrl_env.scan_ongoing = true;
}
/***********SCAN Handle***************/

/***********CONNECTION Handle***************/

/*********************************************************************
 * @fn      at_start_connecting
 *
 * @brief   Start a active connection opertaion
 *			
 *
 * @param   arg - reseved
 *       	
 *
 * @return  None
 */
void at_start_connecting(void *arg)
{
    atuo_transparent_clr();
    mac_addr_t addr;
    memcpy(addr.addr, gAT_buff_env.master_peer_param.conn_param.peer_addr.addr, MAC_ADDR_LEN);
    uint8_t addr_type = gAT_buff_env.master_peer_param.conn_param.addr_type;
    gap_start_conn(&addr,addr_type,9,9,0,400);
    gAT_ctrl_env.initialization_ongoing = true;
}
/***********CONNECTION Handle***************/


/***********ADVERTISIING Handle***************/

/*********************************************************************
 * @fn      at_init_adv_rsp_parameter
 *
 * @brief   Set advertising response data, only include local name element in rsp data
 *			
 *
 * @param   None
 *       	
 *
 * @return  None
 */
static void at_init_adv_rsp_parameter(void)
{
    uint8_t *pos;
    uint8_t scan_rsp_data[0x1F];
    uint8_t scan_rsp_data_len = 0;

    uint8_t local_name[LOCAL_NAME_MAX_LEN];
    uint8_t local_name_len = gap_get_dev_name(local_name);

    pos = &scan_rsp_data[0];
    *pos++ = local_name_len + 1;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, local_name, local_name_len);
    pos += local_name_len;
    scan_rsp_data_len = ((uint32_t)pos - (uint32_t)(&scan_rsp_data[0]));
    gap_set_advertising_rsp_data(scan_rsp_data,scan_rsp_data_len );
}
/*********************************************************************
 * @fn      at_init_adv_rsp_parameter
 *
 * @brief   Set advertising data, include manufacturer_value,HID_SERVICE_UUID, 
 *			and set adv configration parameters.
 *
 * @param   None
 *       	
 *
 * @return  None
 */
void at_init_advertising_parameter(void)
{

    uint8_t *pos;
    uint8_t adv_data[0x1C];
    uint8_t adv_data_len = 0;

    pos = &adv_data[0];
    uint8_t manufacturer_value[] = {0x00,0x00};
    *pos++ = sizeof(manufacturer_value) + 1;
    *pos++  = '\xff';
    memcpy(pos, manufacturer_value, sizeof(manufacturer_value));
    pos += sizeof(manufacturer_value);

    uint16_t uuid_value = HID_SERV_UUID;
    *pos++ = sizeof(uuid_value) + 1;
    *pos++  = '\x03';
    memcpy(pos, (uint8_t *)&uuid_value, sizeof(uuid_value));
    pos += sizeof(uuid_value);
    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = adv_int_arr[gAT_buff_env.default_info.adv_int];
    adv_param.adv_intv_max = adv_int_arr[gAT_buff_env.default_info.adv_int];
    gap_set_advertising_param(&adv_param);
    gAT_ctrl_env.curr_adv_int = adv_param.adv_intv_min;

    gap_set_advertising_data(adv_data,adv_data_len);
    at_init_adv_rsp_parameter();
}

/*********************************************************************
 * @fn      at_start_advertising
 *
 * @brief   Start an advertising action 
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_start_advertising(void *arg)
{
    if(gAT_ctrl_env.curr_adv_int != adv_int_arr[gAT_buff_env.default_info.adv_int])
        at_init_advertising_parameter();
    gap_start_advertising(0);
    gAT_ctrl_env.adv_ongoing = true;
    ADV_LED_ON;
}

/*********************************************************************
 * @fn      at_cb_adv_end
 *
 * @brief   at event call back function, handle after adv action end.
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_cb_adv_end(void *arg)
{
    if(gAT_buff_env.default_info.role & SLAVE_ROLE)     //B mode
        at_start_advertising(NULL);
    else
        ADV_LED_OFF;
}
/***********ADVERTISIING Handle***************/


/***********Idle status Handle***************/

/*********************************************************************
 * @fn      at_idle_status_hdl
 *
 * @brief   at event call back function, handle after all adv actions stop, include advtertising, scan and conn actions.
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_idle_status_hdl(void *arg)
{
    ADV_LED_OFF;
    if(gAT_ctrl_env.async_evt_on_going
       && gAT_ctrl_env.adv_ongoing == false
       && gAT_ctrl_env.scan_ongoing == false
       && gAT_ctrl_env.initialization_ongoing == false)
    {
        uint8_t *at_rsp = os_malloc(150);
        sprintf((char *)at_rsp,"+MODE:I\r\nOK");
        at_send_rsp((char *)at_rsp);
        os_free(at_rsp);
        gAT_ctrl_env.async_evt_on_going = false;
    }
}

/*********************************************************************
 * @fn      at_link_idle_status_hdl
 *
 * @brief   at event call back function, handle after all link is disconnected
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_link_idle_status_hdl(void *arg)
{
    if(gap_get_connect_num()==0)
    {
        LINK_LED_OFF;
        if(gAT_ctrl_env.async_evt_on_going)
        {
            uint8_t *at_rsp = os_malloc(150);
            sprintf((char *)at_rsp,"+DISCONN:A\r\nOK");
            at_send_rsp((char *)at_rsp);
            os_free(at_rsp);
            gAT_ctrl_env.async_evt_on_going = false;
        }
    }
}

/***********Idle status Handle***************/

/*********************************************************************
 * @fn      at_init_gap_cb_func
 *
 * @brief   initialization setting for AT event : AT_GAP_CB_ADV_END & AT_GAP_CB_DISCONNECT
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_init_gap_cb_func(void)
{
    at_set_gap_cb_func(AT_GAP_CB_ADV_END,at_cb_adv_end);
    at_set_gap_cb_func(AT_GAP_CB_DISCONNECT,at_cb_disconnected);
}


/*********************************************************************
 * @fn      at_recv_cmd_handler
 *
 * @brief   Handle at commands , this function is called in at_task when a whole AT cmd is detected
 *			
 *
 * @param   param - pointer to at command data buffer
 *       	
 *
 * @return  None
 */
void at_recv_cmd_handler(struct recv_cmd_t *param)
{
    uint8_t *buff;      //cmd buff
    uint8_t index;
    uint8_t *at_rsp;    //scan rsp buff

    buff = param->recv_data;
    if(gAT_ctrl_env.async_evt_on_going)
        goto _out;

    for(index = 0; index < (sizeof(cmds) / 4); index ++)
    {
        if(memcmp(buff,cmds[index],strlen(cmds[index])) == 0)
        {
            buff += strlen(cmds[index]);
            break;
        }
    }
    at_rsp = os_malloc(150);

    switch(index)
    {
        case AT_CMD_IDX_NAME:
        {
            uint8_t local_name[LOCAL_NAME_MAX_LEN];
            uint8_t local_name_len = gap_get_dev_name(local_name);
            local_name[local_name_len] = 0;
            switch(*buff++)
            {
                case '?':
                {
                    sprintf((char *)at_rsp,"+NAME:%s\r\nOK",local_name);
                    at_send_rsp((char *)at_rsp);
                }
                break;
                case '=':
                {
                    uint8_t idx = 0;
                    for(; idx<LOCAL_NAME_MAX_LEN; idx++)
                    {
                        if(*(buff+idx) == '\r')
                            break;
                    }
                    if(idx>=LOCAL_NAME_MAX_LEN)
                    {
                        co_printf("ERR,name_len:%d >=%d",idx,LOCAL_NAME_MAX_LEN);
                        *(buff+idx) = 0x0;
                        sprintf((char *)at_rsp,"+NAME:%s\r\nERR",buff);
                        at_send_rsp((char *)at_rsp);
                        goto _exit;
                    }
                    *(buff+idx) = 0;
                    if(memcmp(local_name,buff,local_name_len)!=0)   //name is different,the set it
                    {
                        gap_set_dev_name(buff,strlen((const char *)buff)+1);
                        at_init_adv_rsp_parameter();
                    }
                    sprintf((char *)at_rsp,"+NAME:%s\r\nOK",buff);
                    at_send_rsp((char *)at_rsp);
                }
                break;
                default:
                    break;
            }
        }
        break;

        case AT_CMD_IDX_MODE:
        {
            switch(*buff++)
            {
                case '?':
                {
                    uint8_t mode_str[3];
                    uint8_t idx = 0;
                    if(gAT_ctrl_env.upgrade_start)
                        mode_str[0] = 'U';         //upgrade
                    else
                        mode_str[0] = 'I';          //idle
                    if(gAT_ctrl_env.adv_ongoing)
                        mode_str[idx++] = 'B';
                    if(gAT_ctrl_env.scan_ongoing)
                        mode_str[idx++] = 'S';
                    if(gAT_ctrl_env.initialization_ongoing)
                        mode_str[idx++] = 'C';

                    if(idx == 1 || idx == 0)
                        sprintf((char *)at_rsp,"+MODE:%c\r\nOK",mode_str[0]);
                    else if(idx == 2)
                        sprintf((char *)at_rsp,"+MODE:%c %c\r\nOK",mode_str[0],mode_str[1]);
                    else if(idx == 3)
                        sprintf((char *)at_rsp,"+MODE:%c %c %c\r\nOK",mode_str[0],mode_str[1],mode_str[2]);
                    at_send_rsp((char *)at_rsp);
                }
                break;
                case '=':
                    if(*buff == 'I')
                    {
                        gAT_ctrl_env.async_evt_on_going = true;
                        gAT_buff_env.default_info.role = IDLE_ROLE;

                        if(gAT_ctrl_env.adv_ongoing)
                        {
                            gap_stop_advertising();
                            at_set_gap_cb_func(AT_GAP_CB_ADV_END,at_idle_status_hdl);
                        }
                        if(gAT_ctrl_env.scan_ongoing)
                        {
                            gap_stop_scan();
                            at_set_gap_cb_func(AT_GAP_CB_SCAN_END,at_idle_status_hdl);
                        }
                        if(gAT_ctrl_env.initialization_ongoing)
                        {
                            gap_stop_conn();
                            at_set_gap_cb_func(AT_GAP_CB_CONN_END,at_idle_status_hdl);
                        }
                        at_set_gap_cb_func(AT_GAP_CB_DISCONNECT,at_cb_disconnected);

                        sprintf((char *)at_rsp,"+MODE:I\r\nOK");
                        at_send_rsp((char *)at_rsp);

                        gAT_ctrl_env.async_evt_on_going = false;
                        if(gAT_ctrl_env.upgrade_start == true)
                        {
                            gAT_ctrl_env.upgrade_start = false;
                            //os_free("1st_pkt_buff\r\n"); todo
                        }
                        if( gAT_buff_env.default_info.auto_sleep)
                            system_sleep_enable();
                        else
                            system_sleep_disable();
                    }
                    else if(*buff == 'B')
                    {
                        if(gAT_ctrl_env.adv_ongoing == false)
                        {
                            gAT_buff_env.default_info.role |= SLAVE_ROLE;
                            at_start_advertising(NULL);
                            at_set_gap_cb_func(AT_GAP_CB_ADV_END,at_cb_adv_end);
                            at_set_gap_cb_func(AT_GAP_CB_DISCONNECT,at_cb_disconnected);

                            if( gAT_buff_env.default_info.auto_sleep)
                                system_sleep_enable();
                            else
                                system_sleep_disable();
                        }
                        sprintf((char *)at_rsp,"+MODE:B\r\nOK");
                        at_send_rsp((char *)at_rsp);
                    }
                    else if(*buff == 'M')
                    {
                        uint8_t i=0;
                        for(; i<BLE_CONNECTION_MAX; i++)
                        {
                            if(gap_get_connect_status(i) && gAT_buff_env.peer_param[i].link_mode ==MASTER_ROLE)
                                break;
                        }
                        if(i >= BLE_CONNECTION_MAX ) //no master link
                        {
                            gAT_buff_env.default_info.role |= MASTER_ROLE;
                            at_set_gap_cb_func(AT_GAP_CB_DISCONNECT,at_start_connecting);
                            //gAT_ctrl_env.async_evt_on_going = true;
                            at_start_connecting(NULL);
                            sprintf((char *)at_rsp,"+MODE:M\r\nOK");
                        }
                        else
                            sprintf((char *)at_rsp,"+MODE:M\r\nERR");
                        at_send_rsp((char *)at_rsp);
                    }
                    else if(*buff == 'U')
                    {
                        if(gap_get_connect_num()==0)
                        {
                            //upgrade mode, stop sleep
                            system_sleep_disable();
                            //set_sleep_flag_after_key_release(false);
                            gAT_ctrl_env.upgrade_start = true;
                            //at_ota_init();
                            sprintf((char *)at_rsp,"+MODE:U\r\nOK");
                        }
                        else
                            sprintf((char *)at_rsp,"+MODE:U\r\nERR");
                        at_send_rsp((char *)at_rsp);
                    }
                    break;
                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_SCAN:
        {
            switch(*buff++)
            {
                case '=':
                {
                    uint8_t scan_time = atoi((const char *)buff);
                    if(scan_time > 0 && scan_time < 100)
                        gAT_ctrl_env.scan_duration = scan_time*100;
                }
                break;
            }
            at_start_scan();

            at_set_gap_cb_func(AT_GAP_CB_SCAN_END,at_scan_done);
            at_set_gap_cb_func(AT_GAP_CB_ADV_RPT,at_get_adv);
            memset(&(gAT_buff_env.adv_rpt[0]),0xff,sizeof(struct at_adv_report)*ADV_REPORT_NUM);
            gAT_ctrl_env.async_evt_on_going = true;
        }
        break;
        case AT_CMD_IDX_LINK:
        {
            switch(*buff++)
            {
                case '?':
                {
                    uint8_t mac_str[MAC_ADDR_LEN*2+1];
                    uint8_t link_mode;
                    uint8_t encryption = 'N';
                    sprintf((char *)at_rsp,"+LINK\r\nOK");
                    at_send_rsp((char *)at_rsp);
                    for(uint8_t i=0; i< BLE_CONNECTION_MAX; i++)
                    {
                        if(gap_get_connect_status(i))
                        {
                            if(gAT_buff_env.peer_param[i].link_mode == SLAVE_ROLE)
                                link_mode = 'S';
                            else
                                link_mode = 'M';
                            if(gAT_buff_env.peer_param[i].encryption)
                                encryption = 'Y';
                            else
                                encryption = 'N';

                            hex_arr_to_str(gAT_buff_env.peer_param[i].conn_param.peer_addr.addr,MAC_ADDR_LEN,mac_str);
                            mac_str[MAC_ADDR_LEN*2] = 0;
                            sprintf((char *)at_rsp,"Link_ID: %d LinkMode:%c Enc:%c PeerAddr:%s\r\n",i,link_mode,encryption,mac_str);
                            uart_put_data_noint(UART0,(uint8_t *)at_rsp, strlen((const char *)at_rsp));
                        }
                        else
                        {
                            encryption = 'N';
                            link_mode = 'N';
                        }
                    }
                }
                break;
            }
        }
        break;
        case AT_CMD_IDX_ENC:
        {
            switch(*buff++)
            {
                case '?':
                {
                    if(gAT_buff_env.default_info.encryption_link == 'M'
                       || gAT_buff_env.default_info.encryption_link == 'B')
                        sprintf((char *)at_rsp,"+ENC:%c\r\nOK",gAT_buff_env.default_info.encryption_link);
                    else
                        sprintf((char *)at_rsp,"+ENC:N\r\nOK");
                    at_send_rsp((char *)at_rsp);
                }
                break;
                case '=':
                {
                    if(*buff == 'M' || *buff == 'B')
                    {
                        gAT_buff_env.default_info.encryption_link = *buff;
                        sprintf((char *)at_rsp,"+ENC:%c\r\nOK",*buff);
                    }
                    else
                    {
                        gAT_buff_env.default_info.encryption_link = 0;
                        sprintf((char *)at_rsp,"+ENC:N\r\nOK");
                    }
                    at_send_rsp((char *)at_rsp);
                }
                break;
            }
        }
        break;
        case AT_CMD_IDX_DISCONN:
        {
            switch(*buff++)
            {
                case '=':
                {
                    if(*buff == 'A')
                    {
                        if(gap_get_connect_num()>0)
                        {
                            gAT_ctrl_env.async_evt_on_going = true;
                            for(uint8_t i = 0; i<BLE_CONNECTION_MAX; i++)
                            {
                                if(gap_get_connect_status(i))
                                    gap_disconnect_req(i);
                            }
                            at_set_gap_cb_func(AT_GAP_CB_DISCONNECT,at_link_idle_status_hdl);
                        }
                    }
                    else
                    {
                        uint8_t link_num = atoi((const char *)buff);

                        if(gap_get_connect_status(link_num))
                        {
                            gAT_ctrl_env.async_evt_on_going = true;
                            if(gap_get_connect_status(link_num))
                                gap_disconnect_req(link_num);
                            at_set_gap_cb_func(AT_GAP_CB_DISCONNECT,at_cb_disconnected);
                        }
                        else
                        {
                            sprintf((char *)at_rsp,"+DISCONN:%d\r\nERR",link_num);
                            at_send_rsp((char *)at_rsp);
                        }
                    }
                }
                break;
            }
        }
        break;
        case AT_CMD_IDX_MAC:
        {
            uint8_t mac_str[MAC_ADDR_LEN*2+1];
            mac_addr_t addr;
            switch(*buff++)
            {
                case '?':
                    gap_address_get(&addr);
                    hex_arr_to_str(addr.addr,MAC_ADDR_LEN,mac_str);
                    mac_str[MAC_ADDR_LEN*2] = 0;

                    sprintf((char *)at_rsp,"+MAC:%s\r\nOK",mac_str);
                    at_send_rsp((char *)at_rsp);
                    break;
                case '=':
                {
                    str_to_hex_arr(buff,addr.addr,MAC_ADDR_LEN);
                    gap_address_set(&addr);
                    hex_arr_to_str(addr.addr,MAC_ADDR_LEN,mac_str);
                    mac_str[MAC_ADDR_LEN*2] = 0;

                    sprintf((char *)at_rsp,"+MAC:%s\r\nOK",mac_str);
                    at_send_rsp((char *)at_rsp);
                }
                break;
                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_CIVER:
        {
            switch(*buff++)
            {
                case '?':
                    sprintf((char *)at_rsp,"+VER:%d\r\nOK",AT_MAIN_VER);
                    at_send_rsp((char *)at_rsp);
                    break;
                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_UART:
        {
            switch(*buff++)
            {
                case '?':
                    sprintf((char *)at_rsp,"+UART:%d,%d,%d,%d\r\nOK",gAT_buff_env.uart_param.baud_rate,gAT_buff_env.uart_param.data_bit_num
                            ,gAT_buff_env.uart_param.pari,gAT_buff_env.uart_param.stop_bit);
                    at_send_rsp((char *)at_rsp);
                    break;
                case '=':
                {
                    uint8_t *pos_int_end;
                    gAT_buff_env.uart_param.uart_idx = 0;
                    pos_int_end = find_int_from_str(buff);
                    gAT_buff_env.uart_param.baud_rate = atoi((const char *)buff);

                    buff = pos_int_end+1;
                    pos_int_end = find_int_from_str(buff);
                    gAT_buff_env.uart_param.data_bit_num = atoi((const char *)buff);

                    buff = pos_int_end+1;
                    pos_int_end = find_int_from_str(buff);
                    gAT_buff_env.uart_param.pari = atoi((const char *)buff);

                    buff = pos_int_end+1;
                    pos_int_end = find_int_from_str(buff);
                    gAT_buff_env.uart_param.stop_bit = atoi((const char *)buff);
                    //at_store_info_to_flash();

                    sprintf((char *)at_rsp,"+UART:%d,%d,%d,%d\r\nOK",gAT_buff_env.uart_param.baud_rate,
                            gAT_buff_env.uart_param.data_bit_num,gAT_buff_env.uart_param.pari,gAT_buff_env.uart_param.stop_bit);
                    at_send_rsp((char *)at_rsp);
                    uart_init(UART0,find_uart_idx_from_baudrate(gAT_buff_env.uart_param.baud_rate));
                }
                break;
                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_Z:
        {
            sprintf((char *)at_rsp,"+Z\r\nOK");
            at_send_rsp((char *)at_rsp);
            uart_finish_transfers(UART0);
            NVIC_SystemReset();
        }
        break;
        case AT_CMD_IDX_CLR_BOND:
        {
            gap_bond_manager_delete_all();
            sprintf((char *)at_rsp,"+CLR_BOND\r\nOK");
            at_send_rsp((char *)at_rsp);
        }
        break;
        case AT_CMD_IDX_SLEEP:
        {
            switch(*buff++)
            {
                case '?':
                    if(gAT_buff_env.default_info.auto_sleep)
                        sprintf((char *)at_rsp,"+SLEEP:S\r\nOK");
                    else
                        sprintf((char *)at_rsp,"+SLEEP:E\r\nOK");
                    at_send_rsp((char *)at_rsp);
                    break;
                case '=':
                    if(*buff == 'S')
                    {
                        system_sleep_enable();
                        gAT_buff_env.default_info.auto_sleep = true;
                        //set_sleep_flag_after_key_release(true);
                        for(uint8_t i=0; i< BLE_CONNECTION_MAX; i++)
                        {
                            if(gap_get_connect_status(i))
                                at_con_param_update(i,50);
                        }
                        sprintf((char *)at_rsp,"+SLEEP:S\r\nOK");
                        at_send_rsp((char *)at_rsp);
                    }
                    else if(*buff == 'E')
                    {
                        system_sleep_disable();
                        gAT_buff_env.default_info.auto_sleep = false;
                        //set_sleep_flag_after_key_release(false);
                        for(uint8_t i=0; i< BLE_CONNECTION_MAX; i++)
                        {
                            if(gap_get_connect_status(i))
                                at_con_param_update(i,0);
                        }
                        sprintf((char *)at_rsp,"+SLEEP:E\r\nOK");
                        at_send_rsp((char *)at_rsp);
                    }
                    break;
                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_CONNADD:
        {
            uint8_t peer_mac_addr_str[MAC_ADDR_LEN*2+1];
            switch(*buff++)
            {
                case '?':
                    break;
                case '=':
                    str_to_hex_arr(buff,gAT_buff_env.master_peer_param.conn_param.peer_addr.addr,MAC_ADDR_LEN);
                    break;
            }
            hex_arr_to_str(gAT_buff_env.master_peer_param.conn_param.peer_addr.addr,MAC_ADDR_LEN,peer_mac_addr_str);
            peer_mac_addr_str[MAC_ADDR_LEN*2] = 0;
            sprintf((char *)at_rsp,"\r\n+CONNADD:%s\r\nOK",peer_mac_addr_str );
            at_send_rsp((char *)at_rsp);
        }
        break;
        case AT_CMD_IDX_CONN:
        {
            switch(*buff++)
            {
                case '=':
                {
                    uint8_t connect_idx = atoi((const char *)buff);

                    if(gAT_ctrl_env.initialization_ongoing == false) //no master link
                    {
                        memcpy(gAT_buff_env.master_peer_param.conn_param.peer_addr.addr, gAT_buff_env.adv_rpt[connect_idx].adv_addr.addr, MAC_ADDR_LEN);
                        gAT_buff_env.master_peer_param.conn_param.addr_type = gAT_buff_env.adv_rpt[connect_idx].adv_addr_type;
                        gAT_buff_env.default_info.role |= MASTER_ROLE;
                        at_set_gap_cb_func(AT_GAP_CB_DISCONNECT,at_start_connecting);
                        gAT_ctrl_env.async_evt_on_going = true;
                        at_start_connecting(NULL);
                    }
                    else
                    {
                        sprintf((char *)at_rsp,"+CONN:%d\r\nERR",connect_idx);
                        at_send_rsp((char *)at_rsp);
                    }
                }
                break;
            }
        }
        break;

        case AT_CMD_IDX_UUID:
        {
            uint8_t uuid_str_svc[UUID_SIZE_16*2+1];
            uint8_t uuid_str_tx[UUID_SIZE_16*2+1];
            uint8_t uuid_str_rx[UUID_SIZE_16*2+1];
            switch(*buff++)
            {
                case '?':
                    hex_arr_to_str(spss_uuids,UUID_SIZE_16,uuid_str_svc);
                    uuid_str_svc[UUID_SIZE_16*2] = 0;
                    hex_arr_to_str(spss_uuids + UUID_SIZE_16,UUID_SIZE_16,uuid_str_tx);
                    uuid_str_tx[UUID_SIZE_16*2] = 0;
                    hex_arr_to_str(spss_uuids + UUID_SIZE_16*2,UUID_SIZE_16,uuid_str_rx);
                    uuid_str_rx[UUID_SIZE_16*2] = 0;

                    sprintf((char *)at_rsp,"+%s:\r\nDATA:UUID\r\n\r\n+%s:\r\nDATA:UUID\r\n\r\n+%s:\r\nDATA:UUID\r\n\r\nOK"
                            ,uuid_str_svc,uuid_str_tx,uuid_str_rx);
                    at_send_rsp((char *)at_rsp);
                    break;

                case '=':
                    if( *buff == 'A' && *(buff+1) == 'A')
                    {
                        str_to_hex_arr(buff+3,spss_uuids,UUID_SIZE_16);
                        gatt_change_svc_uuid(spss_svc_id,0,spss_uuids,UUID_SIZE_16);
                    }
                    if( *buff == 'B' && *(buff+1) == 'B')
                    {
                        str_to_hex_arr(buff+3,spss_uuids + UUID_SIZE_16,UUID_SIZE_16);
                        gatt_change_svc_uuid(spss_svc_id,2,spss_uuids + UUID_SIZE_16,UUID_SIZE_16);
                    }
                    if( *buff == 'C' && *(buff+1) == 'C')
                    {
                        str_to_hex_arr(buff+3,spss_uuids + UUID_SIZE_16*2,UUID_SIZE_16);
                        gatt_change_svc_uuid(spss_svc_id,6,spss_uuids + UUID_SIZE_16*2,UUID_SIZE_16);
                    }
                    *(buff+3 + UUID_SIZE_16*2) = 0;

                    sprintf((char *)at_rsp,"+%s:\r\nDATA:UUID\r\n\r\nsuccessful",buff+3);
                    at_send_rsp((char *)at_rsp);
                    break;

                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_FLASH:            //store param
        {
            at_store_info_to_flash();
            sprintf((char *)at_rsp,"+FLASH\r\nOK");
            at_send_rsp((char *)at_rsp);
        }
        break;
        case AT_CMD_IDX_SEND:
        {
            switch(*buff++)
            {
                case '=':
                {
                    uint8_t *pos_int_end;
                    pos_int_end = find_int_from_str(buff);
                    uint8_t conidx = atoi((const char *)buff);

                    buff = pos_int_end+1;
                    pos_int_end = find_int_from_str(buff);
                    uint32_t len = atoi((const char *)buff);

                    if(gap_get_connect_status(conidx) && gAT_ctrl_env.one_slot_send_start == false)
                    {
                        gAT_ctrl_env.transparent_conidx = conidx;
                        gAT_ctrl_env.one_slot_send_len = len;
                        gAT_ctrl_env.one_slot_send_start = true;
                        at_clr_uart_buff();
                        sprintf((char *)at_rsp,">");
                    }
                    else
                        sprintf((char *)at_rsp,"+SEND\r\nERR");

                    at_send_rsp((char *)at_rsp);
                }
                break;
            }
        }
        break;
        case AT_CMD_IDX_TRANSPARENT:            //go to transparent transmit
        {
            if(gap_get_connect_num()==1)
            {
                //printf("%d,%d\r\n",app_env.conidx,gAT_buff_env.peer_param[app_env.conidx].link_mode);
                gAT_ctrl_env.transparent_start = true;
                at_clr_uart_buff();

                uint8_t i;
                //find which conidx is connected
                for(i = 0; i<BLE_CONNECTION_MAX; i++)
                {
                    if(gap_get_connect_status(i))
                        break;
                }

                gAT_ctrl_env.transparent_conidx = i;
                if(gAT_buff_env.peer_param[i].link_mode == SLAVE_ROLE)
                    spss_recv_data_ind_func = at_spss_recv_data_ind_func;
                else if(gAT_buff_env.peer_param[i].link_mode == MASTER_ROLE)
                    spsc_recv_data_ind_func = at_spsc_recv_data_ind_func;
                //app_spss_send_ble_flowctrl(160);
                sprintf((char *)at_rsp,"+++\r\nOK");
            }
            else
                sprintf((char *)at_rsp,"+++\r\nERR");
            at_send_rsp((char *)at_rsp);
        }
        break;
        case AT_CMD_IDX_AUTO_TRANSPARENT:            //go to transparent transmit
        {
            switch(*buff++)
            {
                case '?':
                    if(gAT_buff_env.default_info.auto_transparent == true)
                        sprintf((char *)at_rsp,"+AUTO+++:Y\r\nOK");
                    else
                        sprintf((char *)at_rsp,"+AUTO+++:N\r\nOK");
                    at_send_rsp((char *)at_rsp);
                    break;
                case '=':
                    if(*buff == 'Y')
                    {
                        gAT_buff_env.default_info.auto_transparent = true;
                        sprintf((char *)at_rsp,"+AUTO+++:Y\r\nOK");
                    }
                    else if(*buff == 'N')
                    {
                        gAT_buff_env.default_info.auto_transparent = false;
                        sprintf((char *)at_rsp,"+AUTO+++:N\r\nOK");
                    }
                    at_send_rsp((char *)at_rsp);
                    break;
                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_POWER:            //rf_power set/req
        {
            switch(*buff++)
            {
                case '?':
                    sprintf((char *)at_rsp,"+POWER:%d\r\nOK",gAT_buff_env.default_info.rf_power);
                    at_send_rsp((char *)at_rsp);
                    break;
                case '=':
                    gAT_buff_env.default_info.rf_power = atoi((const char *)buff);
                    if(gAT_buff_env.default_info.rf_power > 5)
                        sprintf((char *)at_rsp,"+POWER:%d\r\nERR",gAT_buff_env.default_info.rf_power);
                    else
                    {
                        //rf_set_tx_power(rf_power_arr[gAT_buff_env.default_info.rf_power]);
                        sprintf((char *)at_rsp,"+POWER:%d\r\nOK",gAT_buff_env.default_info.rf_power);
                    }
                    at_send_rsp((char *)at_rsp);
                    break;
                default:
                    break;
            }
        }
        break;
        case AT_CMD_IDX_ADVINT:            //rf_power set/req
        {
            switch(*buff++)
            {
                case '?':
                    sprintf((char *)at_rsp,"+ADVINT:%d\r\nOK",gAT_buff_env.default_info.adv_int);
                    at_send_rsp((char *)at_rsp);
                    break;
                case '=':
                {
                    uint8_t tmp = gAT_buff_env.default_info.adv_int;
                    gAT_buff_env.default_info.adv_int = atoi((const char *)buff);
                    if(gAT_buff_env.default_info.adv_int > 5)
                    {
                        sprintf((char *)at_rsp,"+ADVINT:%d\r\nERR",gAT_buff_env.default_info.adv_int);
                        gAT_buff_env.default_info.adv_int = tmp;
                    }
                    else
                    {
                        sprintf((char *)at_rsp,"+ADVINT:%d\r\nOK",gAT_buff_env.default_info.adv_int);
                    }
                    at_send_rsp((char *)at_rsp);
                }
                break;
                default:
                    break;
            }
        }
        break;
        default:
            break;
    }
_exit:
    os_free(at_rsp);
_out:
    ;
}

/*********************************************************************
 * @fn      atuo_transparent_set
 *
 * @brief   Set flag and preparate to enter into transparent mode
 *			
 *
 * @param   param - pointer to at command data buffer
 *       	
 *
 * @return  None
 */
void atuo_transparent_set(void)
{
    if(gAT_buff_env.default_info.auto_transparent == true)
    {
        at_clr_uart_buff();
        gAT_ctrl_env.transparent_start = true;

        uint8_t i;
        //find which conidx is connected
        for(i = 0; i<BLE_CONNECTION_MAX; i++)
        {
            if(gap_get_connect_status(i))
                break;
        }
        if(gAT_buff_env.peer_param[i].link_mode == SLAVE_ROLE)
            spss_recv_data_ind_func = at_spss_recv_data_ind_func;
        else if(gAT_buff_env.peer_param[i].link_mode == MASTER_ROLE)
            spsc_recv_data_ind_func = at_spsc_recv_data_ind_func;
    }
}

/*********************************************************************
 * @fn      atuo_transparent_clr
 *
 * @brief   Clear transparent mode flag and clear related profile data receive function. 
 *			
 *
 * @param   None
 *       	
 *
 * @return  None
 */
void atuo_transparent_clr(void)
{
    if(gAT_ctrl_env.transparent_start)
    {
        gAT_ctrl_env.transparent_start = 0;
        at_clr_uart_buff();
        spss_recv_data_ind_func = NULL;
        spsc_recv_data_ind_func = NULL;
    }
}



