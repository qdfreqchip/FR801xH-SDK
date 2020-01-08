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

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_i2s.h"
#include "driver_pmu.h"
#include "driver_uart.h"
#include "driver_rtc.h"

#include "ble_simple_central.h"


extern uint8_t master_link_conidx;

extern uint8_t slave_link_conidx;
uint8_t tick = 1;

static uint8_t local_mac_addr[6] = {0xcb, 0x60, 0x6b, 0x0d, 0xda, 0x78};

extern void initial_static_memory(uint8_t act_num, 
                                    uint8_t adv_act_num, 
                                    uint8_t con_num, 
                                    uint8_t rx_buf_num, 
                                    uint16_t rx_buf_size, 
                                    uint8_t tx_buf_num, 
                                    uint16_t tx_buf_size,
                                    uint16_t max_adv_size,
                                    uint16_t stack_size);

static uint8_t jump_table_param_get(uint8_t param_id, uint8_t * lengthPtr, void *buf)
{
    switch(param_id)
    {
        case PARAM_BD_ADDRESS:
            memcpy(buf,local_mac_addr,6);
            *lengthPtr = 6;
            break;

        case PARAM_LE_PRIVATE_KEY_P256:
        case PARAM_LE_PUBLIC_KEY_P256:
        case PARAM_LE_DBG_FIXED_P256_KEY:
        default:
            return PARAM_FAILED;
    }

    return PARAM_SUCCESS;
}

static uint8_t jump_table_param_set(uint8_t param_id, uint8_t lengthPtr, uint8_t *buf)
{
    switch(param_id)
    {
        case PARAM_BD_ADDRESS:
            memcpy(local_mac_addr, buf, lengthPtr);
            break;

        case PARAM_LE_PRIVATE_KEY_P256:
        case PARAM_LE_PUBLIC_KEY_P256:
        case PARAM_LE_DBG_FIXED_P256_KEY:
        default:
            return PARAM_FAILED;
    }

    return PARAM_SUCCESS;
}

void pmu_gpio_isr_ram111(uint32_t gpio_value)
{
    co_printf("K:0x%08x\r\n", gpio_value);
    if(gpio_value == 0)
        co_printf("L\r\n");
    else
    {
        if(gpio_value == GPIO_PD4)
        {
            if(tick%2)
            {
                gap_disconnect_req(slave_link_conidx);
            }
            else
            {
                ;
            }
        }
        else if (gpio_value == GPIO_PD5)
        {
            if(tick%2)
            {
                gap_disconnect_req(master_link_conidx);
            }
            else
            {
                mac_addr_t addr= {{0x0C,0x0c,0x0c,0x0c,0x0c,0x0B}};
                //struct bd_addr addr = {{0x1B,0x18,0x62,0x8D,0x7C,0xC4}};
                gap_start_conn(&addr,0, 15, 15, 10, 100);
            }
        }
        else if (gpio_value == GPIO_PC5)
        {
            if(tick%2)
            {
                //gap_stop_scan();
                //gap_start_scan(NULL);
                //#ifdef USER_PROFILE_API_ENABLE
                //test_profile_send_ntf();
                //#endif
            }
            else
            {
                //gap_start_scan(NULL);
                //#ifdef USER_PROFILE_API_ENABLE
                //test_profile_send_ntf();
                //#endif
#ifdef USER_MEM_API_ENABLE
                //         show_mem_list();
                //         show_msg_list();
#endif
            }
        }
        tick++;
    }
}

__attribute__((section("ram_code"))) void pmu_gpio_isr_ram(void)
{
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);
    
    button_toggle_detected(gpio_value);
    ool_write32(PMU_REG_PORTA_LAST, gpio_value);
}

__attribute__((section("ram_code"))) void i2s_isr_ram(void)
{
    uint32_t status;
    uint16_t buffer[I2S_FIFO_DEPTH/2];
    
    status = i2s_get_int_status();

    if(status & I2S_INT_STATUS_RX_HFULL)
    {
        i2s_get_data(buffer, I2S_FIFO_DEPTH/2, I2S_DATA_MONO);
        for(uint8_t i=0; i<I2S_FIFO_DEPTH/2; i++)
        {
            uint16_t data = buffer[i];
            uart_putc_noint(UART1, (data>>8)&0xff);
            uart_putc_noint(UART1, (data>>0)&0xff);
        }
    }
}


void user_init_static_memory(void)
{
    initial_static_memory(6, 2, 3, 8, 256, 10, 256, 254, 0x800);
}

void user_custom_jump_table_param(void)
{
    __jump_table.param_get = jump_table_param_get;
    __jump_table.param_set = jump_table_param_set;
}

__attribute__((section("ram_code"))) void rtc_isr_ram(uint8_t rtc_idx)
{
    uart_putc_noint_no_wait(UART1, 's');
    uart_putc_noint_no_wait(UART1, '\r');
    uart_putc_noint_no_wait(UART1, '\n');
    rtc_alarm(RTC_A, 1000);
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
        co_delay_100us(10000);
        co_printf("\r\nb");
        co_delay_100us(10000);
        co_printf("\r\nc");
        co_delay_100us(10000);
        co_printf("\r\nd");
    }

    /* 
     * 取消ALDO的bypass模式，把ALDO配置成最小值，这样aldo就不会跟vbat bypass，否则在射频工作时会拉低电池
     * 电压，从而影响ALDO的稳定性，进而影响codec的工作
     */
    ool_write(PMU_REG_ADKEY_ALDO_CTRL, ool_read(PMU_REG_ADKEY_ALDO_CTRL) & (~(1<<3)));
    //ool_write(PMU_REG_ADKEY_ALDO_CTRL, (ool_read(PMU_REG_ADKEY_ALDO_CTRL)&0x0F));
}

void user_entry_after_ble_init(void)
{
    co_printf("user_entry_after_ble_init\r\n");

    user_task_init();
    
    gap_bond_manager_init(0x32000,0x33000,8,true);
    gap_bond_manager_delete_all();
    
    /* 测试GPIO的唤醒功能 */
    pmu_set_pin_pull(GPIO_PORT_D, (1<<GPIO_BIT_4)|(1<<GPIO_BIT_5), true);
    pmu_port_wakeup_func_set(GPIO_PD4|GPIO_PD5);
    button_init(GPIO_PD4|GPIO_PD5);

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
        .bond = true,
        .password = 123456,
    };
#endif
#if 0
    gap_security_param_t param =
    {
        .mitm = false,
        .ble_secure_conn = false,
        .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond = true,
        .password = 0,
    };
#endif

    //gap_security_param_init(&param);

#if 0
    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 0;
    gap_start_scan(&scan_param);
#endif


    mac_addr_t addr;
    gap_address_get(&addr);
    show_reg(&addr.addr[0], 6, 1);
    
    prf_client_create();
}
