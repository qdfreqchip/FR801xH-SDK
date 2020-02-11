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
#include "driver_gpio.h"

#include "ali_mesh_info.h"

uint8_t slave_link_conidx;
uint8_t master_link_conidx;
uint8_t tick = 1;

void app_mesh_led_init(void);

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

        case GAP_EVT_SLAVE_CONNECT:
        {
            co_printf("slave[%d],connect. link_num:%d\r\n",event->param.slave_connect.conidx,gap_get_connect_num());
            slave_link_conidx = event->param.slave_connect.conidx;
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
    app_mesh_ali_info_load_bdaddr(&__jump_table.addr.addr[0]);
    
    __jump_table.image_size = 0x19000;  // 100KB
    __jump_table.firmware_version = 0x00010000;
    __jump_table.system_clk = SYSTEM_SYS_CLK_48M;

    __jump_table.diag_port = 0x83000000;

    jump_table_set_static_keys_store_offset(MESH_SECRET_KEY_ADDR);
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

    /* process in normal procedure after wakeup */
    ool_write(PMU_REG_SYSTEM_STATUS, 0xaa);

    /* sleep forever */
    ool_write32(PMU_REG_SLP_VAL_0, 0);
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

    co_delay_100us(500);
    ool_write(PMU_REG_SYSTEM_STATUS, PMU_SYS_PO_MAGIC);
    platform_reset();

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
    
    /* AT command interface */
    app_at_init();
	
    if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_HCI_MODE)
    {
        /* use PC4 and PC5 for HCI interface */
        system_set_port_pull(GPIO_PA4, true);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_4, PORTA4_FUNC_UART0_RXD);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_5, PORTA5_FUNC_UART0_TXD);
    }

    /* set BUCK voltage to higher level */
    ool_write(PMU_REG_BUCK_CTRL1, 0x65);
    /* set DLDO voltage to higher level */
    ool_write(PMU_REG_DLDO_CTRL, 0x72);

    ool_write(0x1c, ool_read(0x1c) | 0x74);
        
#if 0
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_4, PORTD4_FUNC_D4);
    gpio_portd_write(gpio_portd_read() & 0xef);
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_4, GPIO_DIR_OUT);
    uint32_t delay = 13000;    // 1.3s~2.0s
    while(1)
    {
        gpio_portd_write(gpio_portd_read() & 0xef);
        co_delay_100us(delay);
        gpio_portd_write(gpio_portd_read() | 0x10);
        co_delay_100us(20000);
        delay += 500;
        if(delay >= 20000)
        {
            delay = 5000;
        }
    }
#endif
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

    //prf_server_create();
    app_mesh_led_init();

    mac_addr_t addr;
    gap_address_get(&addr);
    show_reg(&addr.addr[0], 6, 1);
}

