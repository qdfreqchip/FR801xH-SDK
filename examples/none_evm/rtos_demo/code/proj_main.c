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
#include "driver_rtc.h"
/*
 * LOCAL VARIABLES
 */

const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) = 
{
    .firmware_version = 0x00000000,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
{
    .image_type = IMAGE_TYPE_APP,
    .image_size = 0x19000,      
};

uint8_t slave_link_conidx;
uint8_t master_link_conidx;
uint8_t tick = 1;

void app_rtos_entry(uint8_t type, void *arg);

__attribute__((section("ram_code"))) void pmu_gpio_isr_ram(void)
{
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);
    
    button_toggle_detected(gpio_value);
    ool_write32(PMU_REG_PORTA_LAST, gpio_value);
}

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
            //if(memcmp(event->param.adv_rpt->src_addr.addr.addr,"\x0C\x0C\x0C\x0C\x0C\x0B",6)==0)
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
            //gatt_mtu_exchange_req(event->param.slave_connect.conidx);
            //gap_conn_param_update(event->param.slave_connect.conidx, 12, 12, 0, 500);
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
            gap_start_advertising(0);
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
            extern uint8_t client_id;
            gatt_discovery_all_peer_svc(client_id,event->param.master_encrypt_conidx);
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
    __jump_table.system_clk = SYSTEM_SYS_CLK_48M;
    
    /* enable FreeRTOS */
    __jump_table.system_option |= SYSTEM_OPTION_ENABLE_RTOS | SYSTEM_OPTION_RTOS_HEAP_SEL;
    /* set rtos api entry */
    rtos_entry = app_rtos_entry;
}

__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
    uart_putc_noint_no_wait(UART1, 's');
}

__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{
    /* set PA2 and PA3 for AT command interface */
    system_set_port_pull(GPIO_PA2, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    
    //system_sleep_disable();

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

void user_entry_before_ble_init(void)
{    
    /* set system power supply in BUCK mode */
    pmu_set_sys_power_mode(PMU_SYS_POW_BUCK);
    
    //pmu_set_lp_clk_src(PMU_LP_CLK_SRC_EX_32768);
    rtc_init();

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

    /* used for debug, reserve 3S for j-link once sleep is enabled. */
    if(__jump_table.system_option & SYSTEM_OPTION_SLEEP_ENABLE)
    {
        co_printf("\r\na");
        //co_delay_100us(10000);
        co_printf("\r\nb");
        //co_delay_100us(10000);
        co_printf("\r\nc");
        //co_delay_100us(10000);
        co_printf("\r\nd");
    }

    /* 
     * 取消ALDO的bypass模式，把ALDO配置成最小值，这样aldo就不会跟vbat bypass，否则在射频工作时会拉低电池
     * 电压，从而影响ALDO的稳定性，进而影响codec的工作
     */
    //ool_write(PMU_REG_ADKEY_ALDO_CTRL, ool_read(PMU_REG_ADKEY_ALDO_CTRL) & (~(1<<3)));
    //ool_write(PMU_REG_ADKEY_ALDO_CTRL, (ool_read(PMU_REG_ADKEY_ALDO_CTRL)&0x0F));
}

void user_entry_after_ble_init(void)
{
    co_printf("user_entry_after_ble_init\r\n");

    user_task_init();
    
    system_sleep_disable();
    
    gap_set_dev_name("FR8010H", strlen("FR8010H"));
    
    gap_bond_manager_init(0x32000,0x33000,8,true);
    gap_bond_manager_delete_all();
    
    /* 测试GPIO的唤醒功能 */
    //button_init(GPIO_PD4|GPIO_PD5);
    //pmu_set_pin_pull(GPIO_PORT_D, (1<<GPIO_BIT_4)|(1<<GPIO_BIT_5), true);
    //pmu_port_wakeup_func_set(GPIO_PD4|GPIO_PD5);
    
    //rtc_alarm(RTC_A, 1000);

    /* 用于测试随机数生态的随机性 */
    co_printf("random value is 0x%08x.\r\n", rand());

    gap_set_cb_func(proj_ble_gap_evt_func);
#if 0
    gap_security_param_t param =
    {
        .auth = GAP_AUTH_REQ_MITM_BOND,
        .iocap = GAP_IO_CAP_KB_ONLY,
        .oob = GAP_OOB_AUTH_DATA_NOT_PRESENT,
        .sec_req = GAP_SEC1_NOAUTH_PAIR_ENC,
    };
#endif
#if 0
    gap_security_param_t param =
    {
        .mitm = true,
        .ble_secure_conn = false,
        .io_cap = GAP_IO_CAP_DISPLAY_ONLY,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond_auth = true,
        .password = 123456,
    };
#endif
#if 1
    gap_security_param_t param =
    {
        .mitm = false,
        .ble_secure_conn = false,
        .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond_auth = true,
        .password = 0,
    };
#endif

    //gap_security_param_init(&param);

    uint8_t adv_data[]="\x09\x08\x46\x52\x38\x30\x31\x30\x46\x00";
    uint8_t rsp_data[]="\x09\xFF\x00\x60\x52\x57\x2D\x42\x4C\x45";

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PRIVATE;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 60;
    adv_param.adv_intv_max = 60;
    gap_set_advertising_param(&adv_param);
    gap_set_advertising_data(adv_data, sizeof(adv_data)-1);
    gap_set_advertising_rsp_data(rsp_data, sizeof(rsp_data)-1);
    gap_start_advertising(0);

#if 0
    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 0;
    gap_start_scan(&scan_param);
#endif

    prf_server_create();
    prf_client_create();
}

