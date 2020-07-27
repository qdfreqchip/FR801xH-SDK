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

#include "dev_info_service.h"
#include "batt_service.h"
#include "hid_service.h"
#include "jump_table.h"

#include "driver_system.h"
#include "driver_i2s.h"
#include "driver_uart.h"
#include "driver_pmu.h"
#include "driver_flash.h"
#include "flash_usage_config.h"

#include "ble_hid_kbd_mice.h"

uint8_t slave_link_conidx;
const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) =
{
    .firmware_version = 0x00000000,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
{
    .image_type = IMAGE_TYPE_APP,
    .image_size = 0x19000,
};

void user_custom_parameters(void)
{
    memcpy(__jump_table.addr.addr,"\xFF\x09\x07\x09\x17\x20",MAC_ADDR_LEN);
    __jump_table.system_clk = SYSTEM_SYS_CLK_12M;
    __jump_table.local_drift = 600;
}

__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
    //ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) & 0xfd);

    uart_putc_noint_no_wait(UART1, 's');
}

__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{
    //ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) | 0x02);

    /* set PA2 and PA3 for AT command interface */
    system_set_port_pull(GPIO_PA2, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);

    uart_init(UART1, BAUD_RATE_115200);
    NVIC_EnableIRQ(UART1_IRQn);

    uart_putc_noint_no_wait(UART1, 'w');

    NVIC_EnableIRQ(PMU_IRQn);
}

void user_entry_before_ble_init(void)
{
    /* set system power supply in BUCK mode */
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

    /*
    * PA2 is set to be UART RX in ROM code, this pin should be pull up
    * to avoid unexpected UART interrupt.
    */
    system_set_port_pull(GPIO_PA2, true);

    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    uart_init(UART1, BAUD_RATE_115200);

    ool_write(PMU_REG_ADKEY_ALDO_CTRL, ool_read(PMU_REG_ADKEY_ALDO_CTRL) & (~(1<<3)));
    //ool_write(PMU_REG_ADKEY_ALDO_CTRL, (ool_read(PMU_REG_ADKEY_ALDO_CTRL)&0x0F));
}

static os_timer_t button_anti_shake_timer;
static uint32_t curr_button_before_anti_shake = 0;
static void button_anti_shake_timeout_handler(void *param)
{
    uint32_t pmu_int_pin_setting = ool_read32(PMU_REG_PORTA_TRIG_MASK);
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);
    gpio_value &= pmu_int_pin_setting;
    gpio_value ^= pmu_int_pin_setting;

    if(gpio_value == curr_button_before_anti_shake)
    {
        co_printf("press_key:%08X\r\n",gpio_value);
        if(gpio_value == GPIO_PD6)
        {
            uint8_t key[8]= {0};
			#if 1
            key[0] =0xe9;           //vol+, consumer  can photo
            hid_gatt_report_notify(slave_link_conidx,1,key,2);
            key[0] =0x00;
            hid_gatt_report_notify(slave_link_conidx,1,key,2);
			#else
						key[2] =0x29;           //escape,keyboard
            hid_gatt_report_notify(slave_link_conidx,2,key,8);
            key[2] =0x00;
            hid_gatt_report_notify(slave_link_conidx,2,key,8);
			#endif
        }
        if(gpio_value == GPIO_PD7)
        {
						uint8_t key[8]= {0};
			#if 1
            key[0] =0xea;          //vol-, consumer  can photo
            hid_gatt_report_notify(slave_link_conidx,1,key,2);
            key[0] =0x00;
            hid_gatt_report_notify(slave_link_conidx,1,key,2);
			#else
            key[2] =0x4;				//'a',keyboard
            hid_gatt_report_notify(slave_link_conidx,2,key,8);
            key[2] =0x00;
            hid_gatt_report_notify(slave_link_conidx,2,key,8);
            batt_gatt_notify(slave_link_conidx,50);
			#endif
            /*
                uint8_t key[1]= {0};
            key[0] =0x20;           //left
            hid_gatt_report_notify(slave_link_conidx,1,key,1);
            key[0] =0x00;
            hid_gatt_report_notify(slave_link_conidx,1,key,1);
            */
        }
    }
}
__attribute__((section("ram_code"))) void pmu_gpio_isr_ram(void)
{
    uint32_t pmu_int_pin_setting = ool_read32(PMU_REG_PORTA_TRIG_MASK);
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);

    ool_write32(PMU_REG_PORTA_LAST, gpio_value);
    uint32_t tmp = gpio_value & pmu_int_pin_setting;
    curr_button_before_anti_shake = tmp^pmu_int_pin_setting;
    os_timer_start(&button_anti_shake_timer, 10, false);
}
os_timer_t update_timer;
void updata_tim_fn(void *arg)
{
    gap_conn_param_update((uint8_t)arg, 24, 24, 33, 600);
}

void proj_ble_gap_evt_func(gap_event_t *event)
{
    switch(event->type)
    {
        case GAP_EVT_ADV_END:
        {
            co_printf("adv_end,status:0x%02x\r\n",event->param.adv_end.status);
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
        }
        break;

        case GAP_EVT_SLAVE_CONNECT:
        {
            co_printf("slave[%d],connect. link_num:%d\r\n",event->param.slave_connect.conidx,gap_get_connect_num());
            slave_link_conidx = event->param.slave_connect.conidx;
            gatt_mtu_exchange_req(event->param.slave_connect.conidx);
            gap_security_req(event->param.slave_connect.conidx);
        }
        break;

        case GAP_EVT_DISCONNECT:
        {
            co_printf("Link[%d] disconnect,reason:0x%02X\r\n",event->param.disconnect.conidx
                      ,event->param.disconnect.reason);
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
            break;
        case GAP_SEC_EVT_SLAVE_ENCRYPT:
					  os_timer_init(&update_timer,updata_tim_fn,(void *)(event->param.slave_connect.conidx));
						if(gap_security_get_bond_req())
							os_timer_start(&update_timer,6000,0);
						else
							os_timer_start(&update_timer,3000,0);
            co_printf("slave[%d]_encrypted\r\n",event->param.slave_encrypt_conidx);
            hid_service_enable(event->param.slave_encrypt_conidx);
            break;

        default:
            break;
    }
}


extern void hid_start_adv(void);

void user_entry_after_ble_init(void)
{
    co_printf("user_entry_after_ble_init\r\n");
    gap_bond_manager_init(0x4A000,0x4B000,8,true);

    gap_set_dev_name("BLE HID KBD MICE", strlen("BLE HID KBD MICE"));
    gap_set_dev_appearance(GAP_APPEARE_HID_KEYBOARD);
    gap_set_cb_func(proj_ble_gap_evt_func);

    os_timer_init(&button_anti_shake_timer, button_anti_shake_timeout_handler, NULL);
    pmu_set_pin_pull(GPIO_PORT_D, BIT(6)|BIT(7), true);
    pmu_port_wakeup_func_set(GPIO_PD6|GPIO_PD7);

    gap_security_param_t param =
    {
        .mitm = false,
        .ble_secure_conn = true,			//true
        .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond_auth = true,
        .password = 0,
    };
    gap_security_param_init(&param);

#if 0
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

    hid_start_adv();
    dis_gatt_add_service();
    batt_gatt_add_service();
    hid_gatt_add_service();

}

