/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "os_timer.h"
#include "os_mem.h"
#include "sys_utils.h"
#include "jump_table.h"

#include "driver_system.h"
#include "driver_uart.h"
#include "driver_pmu.h"
#include "driver_flash.h"
#include "flash_usage_config.h"

#include "gap_api.h"
#include "ota_service.h"

#include "proj_adv_test.h"


const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) =
{
    .firmware_version = 0x0000000A,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
{
    .image_type = IMAGE_TYPE_APP,
    .image_size = 0x19000,
};


void user_custom_parameters(void)
{
    memcpy(__jump_table.addr.addr,"\x1F\x99\x07\x09\x17\x20",MAC_ADDR_LEN);
    __jump_table.system_clk = SYSTEM_SYS_CLK_12M;
    __jump_table.system_option &= (~SYSTEM_OPTION_CODED_PHY_500);   //125K for PHY CODED
    jump_table_set_static_keys_store_offset(JUMP_TABLE_STATIC_KEY_OFFSET);
}
void user_init_static_memory(void)
{
#if TEST_ADV_MODE_UNDIRECT_DOUBLE_ADV
    initial_static_memory(9, 4, 6, 3, 251, 3, 251, 254, 0x800); //adv_act_num must > 3 for double adv
#else
    initial_static_memory(9, 1, 6, 6, 251, 6, 251, 254*4, 0x800);
#endif
}


__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
    uart_putc_noint_no_wait(UART1, 's');
}
__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{
    system_set_port_pull(GPIO_PA2, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    uart_init(UART1, BAUD_RATE_115200);
    uart_putc_noint_no_wait(UART1, 'w');
    NVIC_EnableIRQ(PMU_IRQn);
}


void proj_ble_gap_evt_func(gap_event_t *event)
{
    switch(event->type)
    {
        case GAP_EVT_ADV_END:
        {
            co_printf("adv_end,idx:%d,status:0x%02x\r\n",event->param.adv_end.idx,event->param.adv_end.status);
#if TEST_ADV_MODE_UNDIRECT_DOUBLE_ADV
            if(event->param.adv_end.idx == 0)
            {
                gap_adv_param_t adv_param;
                adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
                adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
                adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
                adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;

                adv_param.adv_intv_min = 80;
                adv_param.adv_intv_max = 80;
                gap_set_advertising_param(&adv_param);

                uint8_t adv_data[]="\x03\x03\x12\x18";
                uint8_t rsp_data[]="\x09\x08\x46\x52\x38\x30\x31\x30\x48\x00";
                gap_set_advertising_data(adv_data,sizeof(adv_data) -1);
                gap_set_advertising_rsp_data(rsp_data,sizeof(rsp_data) -1);
                gap_start_advertising(0);
            }
            else if(event->param.adv_end.idx == 1)
            {
                gap_adv_param_t adv_param;
                adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;

                adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
                adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
                adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;

                adv_param.adv_intv_min = 100;
                adv_param.adv_intv_max = 100;
                gap_set_advertising_param1(&adv_param);

                //uint8_t adv_data[]="\x09\x08\x46\x52\x38\x80\x31\x30\x48\x00";
                uint8_t adv_data1[]="\x03\x03\x02\x18";
                uint8_t rsp_data1[]="\x09\x08\x49\x52\x38\x30\x31\x30\x48\x00";
                gap_set_advertising_data1(adv_data1,sizeof(adv_data1) -1);
                gap_set_advertising_rsp_data1(rsp_data1,sizeof(rsp_data1) -1);
                gap_start_advertising1(0);
            }
#endif
        }
        break;

        case GAP_EVT_SCAN_END:
            gap_set_link_rssi_report(false);
            co_printf("scan_end,status:0x%02x\r\n",event->param.scan_end_status);
            break;
        case GAP_EVT_PER_SYNC_ESTABLISHED:
            co_printf("phy:%d,intv:%d,adv_sid:%d,addr_type:%d\r\n",event->param.per_sync_ready.phy,event->param.per_sync_ready.intv
                      ,event->param.per_sync_ready.adv_sid,event->param.per_sync_ready.addr.addr_type);
            show_reg(event->param.per_sync_ready.addr.addr.addr,MAC_ADDR_LEN,1);
            break;
        case GAP_EVT_PER_SYNC_END:
            co_printf("per_sync_end,status:0x%02x\r\n",event->param.per_sync_end_status);
            break;

        case GAP_EVT_ADV_REPORT:
        {
            co_printf("evt_type:0x%02x,rssi:%d,rpt_len:%d\r\n",event->param.adv_rpt->evt_type,event->param.adv_rpt->rssi
                      ,event->param.adv_rpt->length);
            if(event->param.adv_rpt->evt_type == GAP_SCAN_EVT_EXT_ADV || event->param.adv_rpt->evt_type == GAP_SCAN_EVT_PER_ADV
               || event->param.adv_rpt->evt_type == GAP_SCAN_EVT_EXT_SCAN_RSP)
            {
                //show_reg(event->param.adv_rpt->data,event->param.adv_rpt->length,1);
                co_printf("MAC:");
                show_reg(event->param.adv_rpt->src_addr.addr.addr,MAC_ADDR_LEN,1);
            }
        }
        break;

        case GAP_EVT_ALL_SVC_ADDED:
        {
            co_printf("all svc added\r\n");
#if TEST_BOARD_ADV
            start_adv();
#endif
#if TEST_BOARD_SCAN_OR_CONN
            start_scan_Or_conn();
#endif
        }
        break;

        case GAP_EVT_MASTER_CONNECT:
        {
            co_printf("master[%d],connect. link_num:%d\r\n",event->param.master_connect.conidx,gap_get_connect_num());
            co_printf("Link[%d]param,interval:%d,latency:%d,timeout:%d\r\n",event->param.master_connect.conidx
                      ,event->param.master_connect.con_interval,event->param.master_connect.con_latency,event->param.master_connect.sup_to);
            gap_conn_param_update(event->param.master_connect.conidx, 12, 12, 20, 500);
        }
        break;

        case GAP_EVT_SLAVE_CONNECT:
        {
            co_printf("slave[%d],connect. link_num:%d\r\n",event->param.slave_connect.conidx,gap_get_connect_num());
            co_printf("Link[%d]param,interval:%d,latency:%d,timeout:%d\r\n",event->param.slave_connect.conidx
                      ,event->param.slave_connect.con_interval,event->param.slave_connect.con_latency,event->param.slave_connect.sup_to);
        }
        break;

        case GAP_EVT_DISCONNECT:
        {
            co_printf("Link[%d] disconnect,reason:0x%02X\r\n",event->param.disconnect.conidx
                      ,event->param.disconnect.reason);
        }
        break;

        case GAP_EVT_CONN_END:
            co_printf("conn_end,reason:0x%02x\r\n",event->param.conn_end_reason);
            break;

        case GAP_SEC_EVT_MASTER_ENCRYPT:
            co_printf("master[%d]_encrypted\r\n",event->param.master_encrypt_conidx);
            break;

        case GAP_SEC_EVT_SLAVE_ENCRYPT:
            co_printf("slave[%d]_encrypted\r\n",event->param.slave_encrypt_conidx);
            break;

        default:
            break;
    }
}

void user_entry_before_ble_init(void)
{
    pmu_set_sys_power_mode(PMU_SYS_POW_BUCK);
#ifdef FLASH_PROTECT
    flash_protect_enable(1);
#endif    
    pmu_enable_irq(PMU_ISR_BIT_ACOK
                   | PMU_ISR_BIT_ACOFF
                   | PMU_ISR_BIT_ONKEY_PO
                   | PMU_ISR_BIT_OTP
                   | PMU_ISR_BIT_LVD
                   | PMU_ISR_BIT_BAT
                   | PMU_ISR_BIT_ONKEY_HIGH);
    NVIC_EnableIRQ(PMU_IRQn);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    uart_init(UART1, BAUD_RATE_115200);
    ool_write(PMU_REG_ADKEY_ALDO_CTRL, ool_read(PMU_REG_ADKEY_ALDO_CTRL) & (~(1<<3)));
}
void user_entry_after_ble_init(void)
{
    co_printf("user_entry_after_ble_init\r\n");
    co_printf("user_entry, kkk_OTA5\r\n");

    gap_set_dev_name("FR8010H", strlen("FR8010H"));
    gap_bond_manager_init(BLE_BONDING_INFO_SAVE_ADDR, BLE_REMOTE_SERVICE_SAVE_ADDR, 8, true);

    co_printf("random value is 0x%08x.\r\n", rand());

    gap_set_cb_func(proj_ble_gap_evt_func);

#if 1
    system_sleep_disable();
#else
    if(__jump_table.system_option & SYSTEM_OPTION_SLEEP_ENABLE)
    {
        co_printf("\r\na");
        co_delay_100us(10000);       //must keep it, or pressing reset key may block .
        co_printf("\r\nb");
        co_delay_100us(10000);
        co_printf("\r\nc");
        co_delay_100us(10000);
        co_printf("\r\nd");
    }
#endif
    ota_gatt_add_service();

}











