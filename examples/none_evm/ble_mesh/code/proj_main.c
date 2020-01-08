/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "gap_api.h"
#include "gatt_api.h"

#include "os_timer.h"
#include "os_mem.h"
#include "sys_utils.h"
#include "app_at.h"
#include "button.h"
#include "jump_table.h"

#include "user_task.h"
#include "prf_server.h"
#include "prf_client.h"

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_i2s.h"
#include "driver_pmu.h"
#include "driver_uart.h"

uint8_t slave_link_conidx;
uint8_t master_link_conidx;
uint8_t tick = 1;

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

            if(memcmp(event->param.adv_rpt->src_addr.addr.addr,"\x0C\x0C\x0C\x0C\x0C\x0B",6)==0)
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
            extern uint8_t svc_id;
            gatt_change_svc_uuid(svc_id,0,"\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55",16);
            gatt_change_svc_uuid(svc_id,2,"\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44",16);
            gatt_change_svc_uuid(svc_id,6,"\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33",16);
            gatt_change_svc_uuid(svc_id,8,"\x22\x22",2);
#ifdef USER_MEM_API_ENABLE
            //    show_mem_list();
            //      show_msg_list();
            //      show_ke_malloc();
#endif
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
            //gap_start_advertising(0);
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
            //extern uint8_t client_id;
            //gatt_discovery_all_peer_svc(client_id,event->param.master_encrypt_conidx);
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

/*********************************************************************
 * @fn      user_custom_parameters
 *
 * @brief   initialize several parameters, this function will be called 
 *          at the beginning of the program. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void user_custom_parameters(void)
{
    __jump_table.addr.addr[0] = 0x01;
    __jump_table.addr.addr[1] = 0x01;
    __jump_table.addr.addr[2] = 0x01;
    __jump_table.addr.addr[3] = 0x01;
    __jump_table.addr.addr[4] = 0x01;
    __jump_table.addr.addr[5] = 0xc1;
    
    __jump_table.image_size = 0x19000;  // 100KB
    __jump_table.firmware_version = 0x00010000;
    __jump_table.system_clk = SYSTEM_SYS_CLK_48M;
}

/*********************************************************************
 * @fn      user_entry_before_sleep_imp
 *
 * @brief   Before system goes to sleep mode, user_entry_before_sleep_imp()
 *          will be called, MCU peripherals can be configured properly before 
 *          system goes to sleep, for example, some MCU peripherals need to be
 *          used during the system is in sleep mode. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
    uart_putc_noint_no_wait(UART1, 's');
}

/*********************************************************************
 * @fn      user_entry_after_sleep_imp
 *
 * @brief   When system wakes up from sleep, user_entry_after_sleep_imp()
 *          will be called, MCU peripherals need to be initialized again, 
 *          this can be done in user_entry_after_sleep_imp(). MCU peripherals
 *          status will not be kept during the sleep. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{
    /* set PA2 and PA3 for AT command interface */
    system_set_port_pull(GPIO_PA2, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);

    if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_HCI_MODE)
    {
        system_set_port_pull(GPIO_PA4, true);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_4, PORTA4_FUNC_UART0_RXD);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_5, PORTA5_FUNC_UART0_TXD);
        uart_init(UART0, BAUD_RATE_115200);
        NVIC_EnableIRQ(UART0_IRQn);

        system_sleep_disable();
    }

    uart_init(UART1, BAUD_RATE_115200);
    NVIC_EnableIRQ(UART1_IRQn);

    uart_putc_noint_no_wait(UART1, 'w');

    NVIC_EnableIRQ(PMU_IRQn);
}

/*********************************************************************
 * @fn      user_entry_before_ble_init
 *
 * @brief   Code to be executed before BLE stack to be initialized.
 *          Power mode configurations, PMU part driver interrupt enable, MCU 
 *          peripherals init, etc. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void user_entry_before_ble_init(void)
{    
    /* set system power supply in BUCK mode */
    pmu_set_sys_power_mode(PMU_SYS_POW_BUCK);

    pmu_enable_irq(PMU_ISR_BIT_ACOK
                   | PMU_ISR_BIT_ACOFF
                   | PMU_ISR_BIT_ONKEY_PO
                   | PMU_ISR_BIT_OTP
                   | PMU_ISR_BIT_LVD
                   | PMU_ISR_BIT_BAT
                   | PMU_ISR_BIT_ONKEY_HIGH);
    NVIC_EnableIRQ(PMU_IRQn);
    
    /* AT command interface */
    app_at_init();
	
    if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_HCI_MODE)
    {
        /* use PC4 and PC5 for HCI interface */
        system_set_port_pull(GPIO_PA4, true);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_4, PORTA4_FUNC_UART0_RXD);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_5, PORTA5_FUNC_UART0_TXD);
    }
}

/*********************************************************************
 * @fn      user_entry_after_ble_init
 *
 * @brief   Main entrancy of user application. This function is called after BLE stack
 *          is initialized, and all the application code will be executed from here.
 *          In that case, application layer initializtion can be startd here. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void user_entry_after_ble_init(void)
{
    co_printf("user_entry_after_ble_init\r\n");

    // User task initialization, for buttons.
    user_task_init();
    
    system_sleep_disable();
    
    // set local device name
    gap_set_dev_name("FR8010H", strlen("FR8010H"));

    gap_set_cb_func(proj_ble_gap_evt_func);

    prf_server_create();
    ali_mesh_led_init();

    mac_addr_t addr;
    gap_address_get(&addr);
    show_reg(&addr.addr[0], 6, 1);
}
