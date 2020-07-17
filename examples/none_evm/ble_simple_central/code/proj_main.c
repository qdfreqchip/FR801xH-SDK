/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "sys_utils.h"
#include "jump_table.h"

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_pmu.h"
#include "driver_uart.h"
#include "driver_flash.h"
#include "flash_usage_config.h"

#include "ble_simple_central.h"

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
		memcpy(__jump_table.addr.addr,"\xBD\xAD\xD0\xF0\x80\x10",MAC_ADDR_LEN);
    __jump_table.system_clk = SYSTEM_SYS_CLK_12M;
    jump_table_set_static_keys_store_offset(JUMP_TABLE_STATIC_KEY_OFFSET);
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

    uart_init(UART1, BAUD_RATE_115200);
    NVIC_EnableIRQ(UART1_IRQn);

    // Do some things here, can be uart print
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
#ifdef FLASH_PROTECT
    flash_protect_enable(1);
#endif    
    NVIC_EnableIRQ(PMU_IRQn);
    // Enable UART print.
    system_set_port_pull(GPIO_PA2, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    uart_init(UART1, BAUD_RATE_115200);    
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
    co_printf("BLE Central\r\n");
		
    // Application layer initialization, can included bond manager init, 
    // advertising parameters init, scanning parameter init, GATT service adding, etc.
    simple_central_init();
}
