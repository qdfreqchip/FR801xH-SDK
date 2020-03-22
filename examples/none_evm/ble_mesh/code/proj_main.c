/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

/*
 * INCLUDES
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
#include "driver_gpio.h"

#include "ali_mesh_info.h"

/*
 * MACROS
 */

/*
 * CONSTANTS
 */

/*
 * TYPEDEFS
 */

/*
 * GLOBAL VARIABLES
 */
uint8_t slave_link_conidx;
uint8_t master_link_conidx;
uint8_t tick = 1;

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
    .image_size = 0x30000,      
};


/*
 * EXTERN FUNCTIONS
 */
extern  const uint8_t ali_mesh_key_bdaddr[];


/*
 * PUBLIC FUNCTIONS
 */

void app_mesh_led_init(void);
void initial_static_memory(uint8_t act_num, 
                                    uint8_t adv_act_num, 
                                    uint8_t con_num, 
                                    uint8_t rx_buf_num, 
                                    uint16_t rx_buf_size, 
                                    uint8_t tx_buf_num, 
                                    uint16_t tx_buf_size,
                                    uint16_t max_adv_size,
                                    uint16_t stack_size);


/*********************************************************************
 * @fn      user_init_static_memory
 *
 * @brief   set memory.
 *
 * @param   None.
 *       
 *
 * @return  None.
 */

void user_init_static_memory(void)
{
    initial_static_memory(6, 3, 2, 8, 251, 8, 251, 64, 0x1000);
}


/*********************************************************************
 * @fn      proj_ble_gap_evt_func
 *
 * @brief   Application layer GAP event callback function. Handles GAP evnets.
 *
 * @param   event - GAP events from BLE stack.
 *       
 *
 * @return  None.
 */
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

        case GAP_EVT_ALL_SVC_ADDED:
            ali_ota_start_advertising();
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
            ali_ota_start_advertising();
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
    if(app_mesh_ali_info_check_valid())
        app_mesh_ali_info_load_bdaddr(&__jump_table.addr.addr[0]);
    else
        memcpy(&__jump_table.addr.addr[0],ali_mesh_key_bdaddr,6);
        
    __jump_table.system_clk = SYSTEM_SYS_CLK_48M;
    __jump_table.diag_port = 0x00000000;
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

    //pmu_set_pin_to_PMU(GPIO_PORT_D, (1<<GPIO_BIT_4) | (1<<GPIO_BIT_5));
    //pmu_set_pin_to_PMU(GPIO_PORT_A, (1<<GPIO_BIT_2));

    /* process in normal procedure after wakeup */
    ool_write(PMU_REG_SYSTEM_STATUS, 0xc6);

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

    /* set CPU start delay afte wakeup */
    ool_write(PMU_REG_BUCK_ON_DLY, 0xc0);
    ool_write(PMU_REG_WKUP_PWO_DLY, 0xfc);
    ool_write(PMU_REG_WKUP_PMUFSM_CHG_DLY, 0xfd);
    ool_write(PMU_REG_BT_TIMER_WU_IRQ_PROTECT, 0xfe);

    /* set BUCK voltage to higher level */
    ool_write(PMU_REG_BUCK_CTRL1, 0x65);
    /* set DLDO voltage to higher level */
    ool_write(PMU_REG_DLDO_CTRL, 0x72);

    /* 
     * RC will be disabled if vbat is lower than a threshold, set the threshold
     * to maximum value.
     */
    ool_write(0x1c, ool_read(0x1c) | 0x74);

    /* lower RC frequency */
    ool_write(0x1c, 0x74);
    ool_write(0x1b, 0x00);

    /*
    pmu_set_pin_dir(GPIO_PORT_D, (1<<GPIO_BIT_4) | (1<<GPIO_BIT_5), GPIO_DIR_OUT);
    pmu_set_pin_dir(GPIO_PORT_A, (1<<GPIO_BIT_2), GPIO_DIR_OUT);
    pmu_set_gpio_value(GPIO_PORT_D, (1<<GPIO_BIT_4) | (1<<GPIO_BIT_5), 0);
    pmu_set_gpio_value(GPIO_PORT_A, (1<<GPIO_BIT_2), 0);
    pmu_set_pin_to_CPU(GPIO_PORT_D, (1<<GPIO_BIT_4) | (1<<GPIO_BIT_5));
    pmu_set_pin_to_CPU(GPIO_PORT_A, (1<<GPIO_BIT_2));*/

#if 0   //generate 50Hz for debug
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_6, PORTC6_FUNC_C6);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_7, PORTC7_FUNC_C7);
    system_set_port_pull(GPIO_PC7, true);
    gpio_set_dir(GPIO_PORT_C, GPIO_BIT_6, GPIO_DIR_OUT);
    gpio_set_dir(GPIO_PORT_C, GPIO_BIT_7, GPIO_DIR_IN);
    gpio_portc_write(gpio_portc_read() & 0xbf);
    while(1) {
        while((gpio_portc_read() & 0x80) == 0);
        gpio_portc_write(gpio_portc_read() | 0x40);
        co_delay_100us(100);
        gpio_portc_write(gpio_portc_read() & 0xbf);
        co_delay_100us(100);
    }
#endif

#if 0   // simulate switch on-off
    uint32_t counter;
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_6, PORTC6_FUNC_C6);
    gpio_set_dir(GPIO_PORT_C, GPIO_BIT_6, GPIO_DIR_OUT);
#define COUNTER_BEGIN       13000   // unit: 100us
#define COUNTER_END         18000   // unit: 100us
#define COUNTER_STEP        200    // unit: 100us
    counter = COUNTER_BEGIN;
    while(1) {
        gpio_portc_write(gpio_portc_read() | 0x40);
        co_delay_100us(20000);
        gpio_portc_write(gpio_portc_read() & 0xbf);
        co_delay_100us(counter);
        counter += COUNTER_STEP;
        if(counter > COUNTER_END) {
            counter = COUNTER_BEGIN;
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

    if(ool_read(PMU_REG_SYSTEM_STATUS) == 0xc6) {
        co_printf("wake up+++++++++++++++++++++++++\r\n");
    }
    else {
        co_printf("Normal--------------------------\r\n");
    }

    // User task initialization, for buttons.
    user_task_init();
    
    system_sleep_disable();
  
    // set local device name
    gap_set_dev_name("FR8010H", strlen("FR8010H"));

    gap_set_cb_func(proj_ble_gap_evt_func);

    app_mesh_led_init();

    mac_addr_t addr;
    gap_address_get(&addr);
    show_reg(&addr.addr[0], 6, 1);

    ali_ota_server_create();
}

