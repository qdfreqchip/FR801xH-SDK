#include <stdint.h>
#include <string.h>

#include "sys_utils.h"
#include "os_task.h"
#include "os_msg_q.h"
#include "os_timer.h"
#include "os_mem.h"

#include "at_recv_cmd.h"
#include "at_cmd_task.h"
#include "at_profile_spss.h"
#include "at_profile_spsc.h"

#include "driver_uart.h"
#include "co_printf.h"
#include "co_log.h"

#include "driver_flash.h"
#include "driver_system.h"
/*
* No need recv flow_ctrl, because if no buff to recv, ble will block recving, peer device
can't send date anymore.
need send flow_ctrl? no need
*/

#define AT_ASSERT(v) do { \
    if (!(v)) {             \
        co_printf("%s %s \n", __FILE__, __LINE__); \
        while (1) {};   \
    }                   \
} while (0);
#undef LOG_LEVEL_MODULE
#define LOG_LEVEL_MODULE        LOG_LEVEL_INFO


#define AT_RECV_MAX_LEN     244
#define AT_TRANSPARENT_DOWN_LEVEL   (AT_RECV_MAX_LEN - 40)

struct at_env
{
    uint8_t at_recv_buffer[AT_RECV_MAX_LEN];
    uint8_t at_recv_index;
    uint8_t at_recv_state;
    uint16_t at_task_id;
    uint8_t transparent_data_send_ongoing;
    uint8_t upgrade_data_processing;
    os_timer_t transparent_timer;       //recv timer out timer.    50ms
    os_timer_t exit_transparent_mode_timer;     //send "+++" ,then 500ms later, exit transparent mode;
} gAT_env = {0};

/*********************************************************************
 * @fn      transparent_timer_handler
 *
 * @brief   Timer handle function, when uart received char numbers less than AT_TRANSPARENT_DOWN_LEVEL for 50ms,
 *			transparent_timer will be timer out, and call this function to send uart buffed data immediately
 *
 * @param   arg - parameter for timer handle 
 *       	 
 *
 * @return  None
 */
void transparent_timer_handler(void *arg)
{
    //LOG_INFO("r_Tout\r\n");
    if( gAT_env.transparent_data_send_ongoing == 0 )
    {
        gAT_env.transparent_data_send_ongoing = 1;
        os_event_t evt;
        evt.event_id = AT_RECV_TRANSPARENT_DATA;
        evt.param_len = 0;
        evt.param = NULL;
        evt.src_task_id = TASK_ID_NONE;
        os_msg_post(gAT_env.at_task_id,&evt);
    }
}

/*********************************************************************
 * @fn      exit_trans_tim_fn
 *
 * @brief   Timer handle function for gAT_env.exit_transparent_mode_timer, if +++ is received, and there is no char comming for 500ms,
 *			this function will be carried on, and transparent mode will end.
 *
 * @param   arg - parameter for timer handle 
 *       	 
 *
 * @return  None
 */
void exit_trans_tim_fn(void *arg)
{
    os_timer_stop(&gAT_env.transparent_timer);
    gAT_ctrl_env.transparent_start = 0;
    gAT_env.at_recv_index = 0;
    //spss_recv_data_ind_func = NULL;
    //spsc_recv_data_ind_func = NULL;
    uint8_t at_rsp[] = "OK";
    at_send_rsp((char *)at_rsp);
}

/*********************************************************************
 * @fn      at_clr_uart_buff
 *
 * @brief   Reset uart char receive index to zero.
 *			
 *
 * @param   None
 *       	 
 *
 * @return  None
 */
void at_clr_uart_buff(void)
{
    gAT_env.at_recv_index = 0;
}

/*********************************************************************
 * @fn      at_task_func
 *
 * @brief   Task to handle uart transmit commands, which are posted in UART interruption
 *			
 *
 * @param   msg - message to be handled by this task
 *       	 
 *
 * @return  None
 */
int at_task_func(os_event_t *msg)
{
    if(gAT_ctrl_env.transparent_start == 0)
        co_printf("at_id:%d\r\n",msg->event_id);

    switch(msg->event_id)
    {
        case AT_RECV_CMD:
        {
            //struct recv_cmd_t *tmp =  (struct recv_cmd_t *)msg->param;
            //show_reg2((uint8_t *)tmp->recv_data,tmp->recv_length,1);
            at_recv_cmd_handler(msg->param);
        }
        break;
        case AT_RECV_TRANSPARENT_DATA:
        {
            os_timer_stop(&gAT_env.transparent_timer);
            GLOBAL_INT_DISABLE();
            if(gap_get_connect_status(gAT_ctrl_env.transparent_conidx))
            {
                if( os_get_free_heap_size()>11264 )
                {
                    //printf("c:%d\r\n",gAT_env.at_recv_index);
                    if(gAT_buff_env.peer_param[gAT_ctrl_env.transparent_conidx].link_mode == SLAVE_ROLE)
                        at_spss_send_data(gAT_ctrl_env.transparent_conidx, gAT_env.at_recv_buffer,gAT_env.at_recv_index);
                    else if(gAT_buff_env.peer_param[gAT_ctrl_env.transparent_conidx].link_mode == MASTER_ROLE)      //master
                        at_spsc_send_data(gAT_ctrl_env.transparent_conidx, gAT_env.at_recv_buffer,gAT_env.at_recv_index);
                }
                else
                    uart_putc_noint(UART1,'X');
            }
            gAT_env.at_recv_index = 0;
            gAT_env.transparent_data_send_ongoing = 0;
            GLOBAL_INT_RESTORE();
            if(gAT_ctrl_env.one_slot_send_start && gAT_ctrl_env.one_slot_send_len == 0)
            {
                gAT_ctrl_env.one_slot_send_start = false;
                gAT_ctrl_env.one_slot_send_len = 0;
                uint8_t at_rsp[] = "SEND OK";
                at_send_rsp((char *)at_rsp);
            }
        }
        break;
        case AT_TRANSPARENT_START_TIMER:
        {
            os_timer_start(&gAT_env.transparent_timer,50,0);
        }
        break;
        case AT_RECV_UPGRADE_DATA:
        {

        }
        break;
        default:
            break;
    }

    return (EVT_CONSUMED);
}

/*********************************************************************
 * @fn      app_at_recv_c
 *
 * @brief   process uart buffer further, this function is call in UART interruption
 *			
 *
 * @param   c - character received from uart FIFO
 *       	 
 *
 * @return  None
 */
static void app_at_recv_c(uint8_t c)
{
    //AT_LOG("%02x ",c);
    //printf("[%02x]",c);
    if(gAT_ctrl_env.transparent_start)
    {
        if(gAT_env.at_recv_index == 0)
        {
            os_event_t evt;
            evt.event_id = AT_TRANSPARENT_START_TIMER;
            evt.param_len = 0;
            evt.param = NULL;
            evt.src_task_id = TASK_ID_NONE;
            os_msg_post(gAT_env.at_task_id,&evt);
        }
        if( gAT_env.at_recv_index < (AT_RECV_MAX_LEN-2) )
            gAT_env.at_recv_buffer[gAT_env.at_recv_index++] = c;

#if 1       //for special customer  ... exit transparent
        os_timer_stop(&gAT_env.exit_transparent_mode_timer);
        if(gAT_env.at_recv_index ==3)
        {
            if( gAT_env.at_recv_buffer[gAT_env.at_recv_index-1] == '+'
                && gAT_env.at_recv_buffer[gAT_env.at_recv_index-2] == '+'
                && gAT_env.at_recv_buffer[gAT_env.at_recv_index-3] == '+')
                os_timer_start(&gAT_env.exit_transparent_mode_timer,500,0);
        }
#endif

        if( (gAT_env.at_recv_index >AT_TRANSPARENT_DOWN_LEVEL) && (gAT_env.transparent_data_send_ongoing == 0) )
        {
            gAT_env.transparent_data_send_ongoing = 1;
            os_event_t evt;
            evt.event_id = AT_RECV_TRANSPARENT_DATA;
            evt.param_len = 0;
            evt.param = NULL;
            evt.src_task_id = TASK_ID_NONE;
            os_msg_post(gAT_env.at_task_id,&evt);
        }
        goto _exit;
    }

    if(gAT_ctrl_env.one_slot_send_start)        // one slot send
    {
        if(gAT_ctrl_env.one_slot_send_len > 0)
        {
            if(gAT_env.at_recv_index == 0)
            {
                os_event_t evt;
                evt.event_id = AT_TRANSPARENT_START_TIMER;
                evt.param_len = 0;
                evt.param = NULL;
                evt.src_task_id = TASK_ID_NONE;
                os_msg_post(gAT_env.at_task_id,&evt);
            }
            if( gAT_env.at_recv_index < (AT_RECV_MAX_LEN-2) )
                gAT_env.at_recv_buffer[gAT_env.at_recv_index++] = c;
            gAT_ctrl_env.one_slot_send_len--;

            if( ((gAT_env.at_recv_index >AT_TRANSPARENT_DOWN_LEVEL) && (gAT_env.transparent_data_send_ongoing == 0))
                || (gAT_ctrl_env.one_slot_send_len == 0)
              )
            {
                gAT_env.transparent_data_send_ongoing = 1;
                os_event_t evt;
                evt.event_id = AT_RECV_TRANSPARENT_DATA;
                evt.param_len = 0;
                evt.param = NULL;
                evt.src_task_id = TASK_ID_NONE;
                os_msg_post(gAT_env.at_task_id,&evt);
            }
        }
        goto _exit;
    }

    if(gAT_ctrl_env.upgrade_start == true)
    {
        /*
        if( gAT_env.upgrade_data_processing == 0 )
        {
            if( gAT_env.at_recv_index < (AT_RECV_MAX_LEN-2) )
            {
                gAT_env.at_recv_buffer[gAT_env.at_recv_index++] = c;
                uint8_t chk_ret = check_whole_pkt_in_upgrade_mode();
                if( chk_ret > 0 )
                {
                    if(chk_ret == 0xff)
                    {
                        gAT_env.at_recv_index = 0;
                        goto _exit;
                    }
                    gAT_env.upgrade_data_processing = 1;
                    os_event_t evt;
                    evt.event_id = AT_RECV_UPGRADE_DATA;
                    evt.param_len = 0;
                    evt.param = NULL;
                    if( chk_ret == 1 )
                        evt.src_task_id = (TASK_ID_NONE-1);
                    else if (chk_ret == 2)
                        evt.src_task_id = TASK_ID_NONE;
                    os_msg_post(gAT_env.at_task_id,&evt);
                }
            }
            else
                gAT_env.at_recv_index = 0;
        }
        goto _exit;
        */
    }

    switch(gAT_env.at_recv_state)
    {
        case 0:
            if(c == 'A')
            {
                gAT_env.at_recv_state++;
                system_prevent_sleep_set();
            }
            break;
        case 1:
            if(c == 'T')
                gAT_env.at_recv_state++;
            else
                gAT_env.at_recv_state = 0;
            break;
        case 2:
            if(c == '+')
                gAT_env.at_recv_state++;
            else
                gAT_env.at_recv_state = 0;
            break;
        case 3:
            gAT_env.at_recv_buffer[gAT_env.at_recv_index] = c;
            if(  (c == '\n') && (gAT_env.at_recv_buffer[gAT_env.at_recv_index-1] == '\r') )
            {
                system_prevent_sleep_clear();

                struct recv_cmd_t *cmd = (struct recv_cmd_t *)os_malloc(sizeof(struct recv_cmd_t)+(gAT_env.at_recv_index+1));
                cmd->recv_length = gAT_env.at_recv_index+1;
                memcpy(&cmd->recv_data[0], &gAT_env.at_recv_buffer[0], cmd->recv_length);

                // AT_LOG("cmd_found\r\n");

                os_event_t evt;
                evt.event_id = AT_RECV_CMD;
                evt.param_len = sizeof(struct recv_cmd_t) + cmd->recv_length;
                evt.param = cmd;
                evt.src_task_id = TASK_ID_NONE;
                os_msg_post(gAT_env.at_task_id,&evt);

                os_free(cmd);
                gAT_env.at_recv_state = 0;
                gAT_env.at_recv_index = 0;
            }
            else
            {
                gAT_env.at_recv_index++;
                if(gAT_env.at_recv_index >= AT_RECV_MAX_LEN)
                {
                    gAT_env.at_recv_state = 0;
                    gAT_env.at_recv_index = 0;
                }
            }
            break;
    }
_exit:
    ;
}

/*********************************************************************
 * @fn      uart0_isr_ram
 *
 * @brief   UART0 interruption, when uart0 FIFO received charaters, this ISR will be called
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
__attribute__((section("ram_code"))) void uart0_isr_ram(void)
{
    uint8_t int_id;
    volatile struct uart_reg_t * const uart_reg_ram = (volatile struct uart_reg_t *)UART0_BASE;
    int_id = uart_reg_ram->u3.iir.int_id;

    if(int_id == 0x04 || int_id == 0x0c )   /* Receiver data available or Character time-out indication */
    {
        while(uart_reg_ram->lsr & 0x01)
        {
            app_at_recv_c(uart_reg_ram->u1.data);
        }
    }
    else if(int_id == 0x06)
    {
        uart_reg_ram->lsr = uart_reg_ram->lsr;
    }
}

/*********************************************************************
 * @fn      at_init
 *
 * @brief   Initializate gAT_env elements and assign UART0 pins
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_init(void)
{
    system_set_port_pull(GPIO_PD4, true);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_4, PORTD4_FUNC_UART0_RXD);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_5, PORTD5_FUNC_UART0_TXD);
    uart_init(UART0, find_uart_idx_from_baudrate(gAT_buff_env.uart_param.baud_rate));
    //uart_init_x(gAT_buff_env.uart_param);
    NVIC_EnableIRQ(UART0_IRQn);

    gAT_env.at_task_id = os_task_create( at_task_func );

    memset(&gAT_ctrl_env,0x0,sizeof(gAT_ctrl_env));
    os_timer_init(&gAT_env.transparent_timer,transparent_timer_handler,NULL);
    os_timer_init(&gAT_env.exit_transparent_mode_timer,exit_trans_tim_fn,NULL);
}


/********Below is flash storage/restore**************/
#define USER_FLASH_BASE_ADDR 0x4C000    //208K
#define FLASH_PAGE_SIZE 0x1000
#define USER_FLASH_MAX_PAGE_ADDR (0x80000 - 0x1000) //512K -4K

#define AT_INFO_FLASH_BASE_ADDR     (USER_FLASH_BASE_ADDR)

#define  WRITE_CNT_OFFSET ( 0 )
#define  DEV_NAME_OFFSET  ( WRITE_CNT_OFFSET + sizeof(uint16_t) )
#define  UART_PARAM_OFFSET  ( DEV_NAME_OFFSET + (LOCAL_NAME_MAX_LEN) )
#define  DEFAULT_INFO_OFFSET    ( UART_PARAM_OFFSET + sizeof(uart_param_t) )
#define  MAC_ADDR_OFFSET  ( DEFAULT_INFO_OFFSET + sizeof(default_info_t) )
#define  PEER_MAC_ADDR_OFFSET  ( MAC_ADDR_OFFSET + sizeof(mac_addr_t) )
#define  SPSS_UUID_OFFSET  ( PEER_MAC_ADDR_OFFSET + sizeof(struct at_conn_peer_param) )

/*********************************************************************
 * @fn      at_store_info_to_flash
 *
 * @brief   Store AT infomations to flash, AT+FLASH cmd will carry out this function
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_store_info_to_flash(void)
{
    uint16_t at_flash_buf_size = SPSS_UUID_OFFSET + sizeof(spss_uuids);

    flash_read(USER_FLASH_MAX_PAGE_ADDR, 2, (uint8_t *)&gAT_buff_env.flash_page_idx);

    uint8_t first_store = 0;
    if( gAT_buff_env.flash_page_idx > 20 )
    {
        gAT_buff_env.flash_page_idx = 0;
        gAT_buff_env.flash_write_cnt = 0;
        flash_erase(USER_FLASH_MAX_PAGE_ADDR, FLASH_PAGE_SIZE);
        flash_write(USER_FLASH_MAX_PAGE_ADDR, 2, (uint8_t *)&gAT_buff_env.flash_page_idx);
        first_store = 1;
    }

    uint8_t *at_flash_buf = os_malloc(at_flash_buf_size);
//read stored at_flash_buf
    flash_read(AT_INFO_FLASH_BASE_ADDR + gAT_buff_env.flash_page_idx * FLASH_PAGE_SIZE, at_flash_buf_size, at_flash_buf);
    if(first_store == 0)
        gAT_buff_env.flash_write_cnt = *(uint16_t *)(at_flash_buf + WRITE_CNT_OFFSET);

    LOG_INFO("MAC:\r\n");
    show_reg(at_flash_buf + MAC_ADDR_OFFSET,MAC_ADDR_LEN,1);

    LOG_INFO("PEER_MAC:\r\n");
    show_reg(at_flash_buf + PEER_MAC_ADDR_OFFSET,sizeof(struct at_conn_peer_param),1);

    mac_addr_t local_mac;
    gap_address_get(&local_mac);

    uint8_t local_name[LOCAL_NAME_MAX_LEN];
    uint8_t local_name_len = 0;
    local_name_len = gap_get_dev_name(local_name);

    LOG_INFO("%d ",memcmp(at_flash_buf + DEV_NAME_OFFSET, local_name, local_name_len ) );
    LOG_INFO("%d ",memcmp(at_flash_buf + UART_PARAM_OFFSET, (uint8_t *)&gAT_buff_env.uart_param, sizeof(gAT_buff_env.uart_param)) );
    LOG_INFO("%d ",memcmp(at_flash_buf + DEFAULT_INFO_OFFSET, (uint8_t *)&gAT_buff_env.default_info, sizeof(gAT_buff_env.default_info)) );
    LOG_INFO("%d ",memcmp(at_flash_buf + MAC_ADDR_OFFSET, local_mac.addr, sizeof(mac_addr_t)) );
    LOG_INFO("%d ",memcmp(at_flash_buf + PEER_MAC_ADDR_OFFSET, (uint8_t *)&gAT_buff_env.master_peer_param, sizeof(gAT_buff_env.master_peer_param)) );
    LOG_INFO("%d ",memcmp(at_flash_buf + SPSS_UUID_OFFSET, spss_uuids, sizeof(spss_uuids)) );


    if ( memcmp(at_flash_buf + DEV_NAME_OFFSET, local_name, local_name_len ) == 0
         && memcmp(at_flash_buf + UART_PARAM_OFFSET, (uint8_t *)&gAT_buff_env.uart_param, sizeof(gAT_buff_env.uart_param) ) == 0
         && memcmp(at_flash_buf + DEFAULT_INFO_OFFSET, (uint8_t *)&gAT_buff_env.default_info, sizeof(gAT_buff_env.default_info) ) == 0
         && memcmp(at_flash_buf + MAC_ADDR_OFFSET, local_mac.addr, sizeof(mac_addr_t)) == 0
         && memcmp(at_flash_buf + PEER_MAC_ADDR_OFFSET, (uint8_t *)&gAT_buff_env.master_peer_param, sizeof(gAT_buff_env.master_peer_param) ) == 0
         && memcmp(at_flash_buf + SPSS_UUID_OFFSET, spss_uuids, sizeof(spss_uuids) ) == 0
       )
    {
        LOG_INFO("flash_exit\r\n");
        goto _exit;
    }
    else
    {
//copy new at_flash_buff.
        memcpy(at_flash_buf + DEV_NAME_OFFSET, local_name, local_name_len);
        memcpy(at_flash_buf + UART_PARAM_OFFSET, (uint8_t *)&gAT_buff_env.uart_param, sizeof(gAT_buff_env.uart_param) );
        memcpy(at_flash_buf + DEFAULT_INFO_OFFSET, (uint8_t *)&gAT_buff_env.default_info, sizeof(gAT_buff_env.default_info) );
        memcpy(at_flash_buf + MAC_ADDR_OFFSET, local_mac.addr, sizeof(mac_addr_t));
        memcpy(at_flash_buf + PEER_MAC_ADDR_OFFSET, (uint8_t *)&gAT_buff_env.master_peer_param, sizeof(gAT_buff_env.master_peer_param) );
        memcpy(at_flash_buf + SPSS_UUID_OFFSET, spss_uuids, sizeof(spss_uuids) );

        gAT_buff_env.flash_write_cnt++;
        if(gAT_buff_env.flash_write_cnt > 20000)
        {
            gAT_buff_env.flash_write_cnt = 0;
            gAT_buff_env.flash_page_idx++;
            if(gAT_buff_env.flash_page_idx > 20)
                gAT_buff_env.flash_page_idx = 0;
//store at_flash_ctrl_info
            flash_erase(USER_FLASH_MAX_PAGE_ADDR, FLASH_PAGE_SIZE);
            flash_write(USER_FLASH_MAX_PAGE_ADDR, 2, (uint8_t *)&gAT_buff_env.flash_page_idx);
        }
        LOG_INFO("flash_store\r\n");
//store at_flash_buff
        memcpy(at_flash_buf,(uint8_t *)&gAT_buff_env.flash_write_cnt,sizeof(uint16_t));
        flash_erase(AT_INFO_FLASH_BASE_ADDR + gAT_buff_env.flash_page_idx * FLASH_PAGE_SIZE, FLASH_PAGE_SIZE);
        flash_write(AT_INFO_FLASH_BASE_ADDR + gAT_buff_env.flash_page_idx * FLASH_PAGE_SIZE, at_flash_buf_size, at_flash_buf);
    }
    co_printf("at_flash,page_idx:%d,write_cnt:%d\r\n",gAT_buff_env.flash_page_idx,gAT_buff_env.flash_write_cnt);
_exit:
    os_free(at_flash_buf);
}


/*********************************************************************
 * @fn      at_load_info_from_flash
 *
 * @brief   Load AT infomations from flash, this function should be called at entry: user_entry_after_ble_init
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_load_info_from_flash(void)
{
    uint16_t at_flash_buf_size = SPSS_UUID_OFFSET + sizeof(spss_uuids);
    uint8_t *at_flash_buf;
    uint8_t local_name[LOCAL_NAME_MAX_LEN]= {0};

    flash_read(USER_FLASH_MAX_PAGE_ADDR, 2, (uint8_t *)&gAT_buff_env.flash_page_idx);

    LOG_INFO("flash_page_idx:%d\r\n",gAT_buff_env.flash_page_idx);

    if( gAT_buff_env.flash_page_idx > 20)
    {
        gAT_buff_env.uart_param.baud_rate = 115200;
        gAT_buff_env.uart_param.uart_idx = 0;
        gAT_buff_env.uart_param.data_bit_num = 8;
        gAT_buff_env.uart_param.pari = 0;
        gAT_buff_env.uart_param.stop_bit = 1;
        gAT_buff_env.default_info.role = SLAVE_ROLE;     //B mode
        gAT_buff_env.default_info.rf_power = 1;
        gAT_buff_env.default_info.adv_int = 0;

        gap_set_dev_name("FR8010H_AT",sizeof("FR8010H_AT"));
        //rf_set_tx_power(rf_power_arr[gAT_buff_env.default_info.rf_power]);
        mac_addr_t addr = {0x1f,0x09,0x07,0x09,0x17,0x20};
        gap_address_set(&addr);

        memset( (uint8_t *)&gAT_buff_env.master_peer_param, 0x0, sizeof(gAT_buff_env.master_peer_param) );
        //gAT_buff_env.master_peer_param.conn_param.bond = APP_SEC_NO_BOND;
        goto _exit;
    }

    at_flash_buf = os_malloc(at_flash_buf_size);
    //read stored at_flash_buf
    flash_read(AT_INFO_FLASH_BASE_ADDR + gAT_buff_env.flash_page_idx * FLASH_PAGE_SIZE, at_flash_buf_size, at_flash_buf);

    LOG_INFO("MAC:\r\n");
    show_reg(at_flash_buf + MAC_ADDR_OFFSET,MAC_ADDR_LEN, 1);

    LOG_INFO("PEER_MAC:\r\n");
    show_reg(at_flash_buf + PEER_MAC_ADDR_OFFSET,sizeof(struct at_conn_peer_param),1 );

    memcpy( local_name, at_flash_buf + DEV_NAME_OFFSET, LOCAL_NAME_MAX_LEN );
    gap_set_dev_name(local_name,strlen((const char *)local_name)+1);
    memcpy( (uint8_t *)&gAT_buff_env.uart_param, at_flash_buf + UART_PARAM_OFFSET, sizeof(gAT_buff_env.uart_param) );
    memcpy( (uint8_t *)&gAT_buff_env.default_info, at_flash_buf + DEFAULT_INFO_OFFSET, sizeof(gAT_buff_env.default_info) );
    //rf_set_tx_power(rf_power_arr[gAT_buff_env.default_info.rf_power]);
    gap_address_set((mac_addr_t *)(at_flash_buf + MAC_ADDR_OFFSET));
    memcpy( (uint8_t *)&gAT_buff_env.master_peer_param, at_flash_buf + PEER_MAC_ADDR_OFFSET, sizeof(gAT_buff_env.master_peer_param) );
    memcpy( spss_uuids, at_flash_buf + SPSS_UUID_OFFSET, sizeof(spss_uuids) );

    os_free(at_flash_buf);

_exit:
    ;
}

/*********************************************************************
 * @fn      at_clr_flash_info
 *
 * @brief   Clear AT infomations from flash, this function is reserved for future uasge
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_clr_flash_info(void)
{
    flash_erase(USER_FLASH_MAX_PAGE_ADDR, FLASH_PAGE_SIZE);
    LOG_INFO("at_clr_flash_info\r\n");
}

/********Above is flash storage/restore**************/






