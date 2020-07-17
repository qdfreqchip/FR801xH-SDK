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
#include "at_gap_event.h"
#include "at_cmd_task.h"
#include "at_recv_cmd.h"

#include "at_profile_spss.h"
#include "at_profile_spsc.h"
#include "ota_service.h"

#include "dev_info_service.h"
#include "batt_service.h"
#include "hid_service.h"

static os_timer_t button_anti_shake_timer;  //Timer for button anti-sahke
static uint32_t curr_button_before_anti_shake = 0;  //before anti-sahke timeout, store the pressed button value
const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) =
{
    .firmware_version = 0x00000000,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
{
    .image_type = IMAGE_TYPE_APP,
    .image_size = 0x19000,
};

/*********************************************************************
 * @fn      user_custom_parameters
 *
 * @brief   entry for configuration of jumptable parameter  .
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void user_custom_parameters(void)
{
    memcpy(__jump_table.addr.addr,"\x0F\x09\x07\x09\x17\x20",MAC_ADDR_LEN);
    __jump_table.system_clk = SYSTEM_SYS_CLK_12M;
    __jump_table.local_drift = 600;
    __jump_table.sleep_algo_dur = 8;
    jump_table_set_static_keys_store_offset(0x49000);
}


/*********************************************************************
 * @fn      user_entry_before_sleep_imp
 *
 * @brief   User entry function before goto deep sleep status.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
    //uart_putc_noint_no_wait(UART1, 's');
}


/*********************************************************************
 * @fn      user_entry_after_sleep_imp
 *
 * @brief   User entry function after wakeup from deep sleep status.
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
    uart_init(UART1, BAUD_RATE_115200);
    //NVIC_EnableIRQ(UART1_IRQn);
    //uart_putc_noint_no_wait(UART1, 'w');

    system_set_port_pull(GPIO_PD4, true);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_4, PORTD4_FUNC_UART0_RXD);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_5, PORTD5_FUNC_UART0_TXD);
    uart_init(UART0, find_uart_idx_from_baudrate(gAT_buff_env.uart_param.baud_rate));
    //uart_init_x(gAT_buff_env.uart_param);
    NVIC_EnableIRQ(UART0_IRQn);

    NVIC_EnableIRQ(PMU_IRQn);
}

/*********************************************************************
 * @fn      user_entry_before_ble_init
 *
 * @brief   User entry function before ble stack initialization.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
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
    system_set_pclk(SYSTEM_SYS_CLK_12M);
}



/*********************************************************************
 * @fn      button_anti_shake_timeout_handler
 *
 * @brief   Anti-shake-timer execute fuinction.
 *
 * @param   argument for timer function.
 *
 *
 * @return  None.
 */
static void button_anti_shake_timeout_handler(void *param)
{
    uint32_t pmu_int_pin_setting = ool_read32(PMU_REG_PORTA_TRIG_MASK);
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);
    gpio_value &= pmu_int_pin_setting;
    gpio_value ^= pmu_int_pin_setting;

    if(gpio_value == curr_button_before_anti_shake)
    {
        if(gpio_value != 0)
        {
            co_printf("press_key:%08X\r\n",gpio_value);
            if((gpio_value & GPIO_PC5))    //PC5, wakeup
            {
                system_sleep_disable();
            }
            if((gpio_value & GPIO_PD6))      //PD6
            {
                if(gap_get_connect_num()==1)
                {
                    for(uint8_t i=0; i<BLE_CONNECTION_MAX; i++)
                    {
                        if(gap_get_connect_status(i))
                            gap_disconnect_req(i);
                    }
                }
            }
        }
        else
        {
            co_printf("release_key:%08X\r\n",gpio_value);
            if(gAT_buff_env.default_info.auto_sleep)
                system_sleep_enable();
            else
                system_sleep_disable();
        }
    }
}


/*********************************************************************
 * @fn      pmu_gpio_isr_ram
 *
 * @brief   pmu gpio interruption.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void pmu_gpio_isr_ram(void)
{
    uint32_t pmu_int_pin_setting = ool_read32(PMU_REG_PORTA_TRIG_MASK);
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);

    ool_write32(PMU_REG_PORTA_LAST, gpio_value);
    uint32_t tmp = gpio_value & pmu_int_pin_setting;
    curr_button_before_anti_shake = tmp^pmu_int_pin_setting;
    os_timer_start(&button_anti_shake_timer, 10, false);
}

/*********************************************************************
 * @fn      user_entry_after_ble_init
 *
 * @brief   User entry function after ble stack initialization.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void user_entry_after_ble_init(void)
{
    co_printf("ota2\r\n");
    gap_bond_manager_init(0x4A000,0x4B000,20,true);

    //gap_set_dev_name("FR8010H_AT", strlen("FR8010H_AT"));
    gap_set_cb_func(proj_ble_gap_evt_func);
    //mac_addr_t addr = {0x0f,0x09,0x07,0x09,0x17,0x20};
    //gap_address_set(&addr);

    //at_clr_flash_info();
    at_load_info_from_flash();
    at_init();

//Set two button, PC5,PD6
    os_timer_init(&button_anti_shake_timer, button_anti_shake_timeout_handler, NULL);
    pmu_set_pin_pull(GPIO_PORT_D, BIT(6), true);     //PD6, disconnect,
    pmu_set_pin_pull(GPIO_PORT_C, BIT(5), true);     //PC5 wakeup
    pmu_port_wakeup_func_set(GPIO_PD6|GPIO_PC5);

    ADV_LED_INIT;
    LINK_LED_INIT;

#if 1
//stop sleep
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
    gap_security_param_t param =
    {
        .mitm = false,
        .ble_secure_conn = false,
        .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond_auth = true,
        .password = 0,
    };
//initialize bonding configuration
    gap_security_param_init(&param);

    at_init_gap_cb_func();
    at_init_advertising_parameter();

//Add AT service and client profile
    at_profile_spsc_init();
    at_profile_spss_init();

//Add OTA, Device info, batt, hid profile
    ota_gatt_add_service();
    dis_gatt_add_service();
    batt_gatt_add_service();
    hid_gatt_add_service();
    co_printf("re_len:%d\r\n",system_regs->remap_length);
}


