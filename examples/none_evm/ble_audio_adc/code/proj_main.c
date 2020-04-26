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
#include "button.h"
#include "jump_table.h"

#include "user_task.h"

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_i2s.h"
#include "driver_pmu.h"
#include "driver_uart.h"
#include "driver_rtc.h"
#include "driver_codec.h"

#include "ble_simple_peripheral.h"
#include "simple_gatt_service.h"

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

uint8_t calibration_stage = 0;
float voltage1 = 1.0042;
int32_t adc_value1 = 0;
float voltage2 = 2.00278;
int32_t adc_value2 = 0;
float adc_rate_a;
float adc_rate_b = 0.0;
float adc_value_calib[] = {
    11.07654256 , 11.3718189  , 11.67335142 , 11.97905473 , 12.28684342 , 12.59463211 , 12.90033542 , 13.20186794 ,
    13.49714428 , 13.78407906 , 14.06058689 , 14.32458236 , 14.5739801  , 14.80633927 , 15.01779727 , 15.20413608 ,
    15.36113768 , 15.48458402 , 15.57025709 , 15.61393885 , 15.61271916 , 15.5689195  , 15.48616921 , 15.36809765 ,
    15.21833417 , 15.04050814 , 14.8382489  , 14.61518583 , 14.374451   , 14.11718746 , 13.84404101 , 13.55565742 ,
    13.25268247 , 12.93576195 , 12.60554165 , 12.26266735 , 11.90752047 , 11.53942503 , 11.15744066 , 10.76062703 ,
    10.34804377 , 9.918750542 , 9.471806992 , 9.006177206 , 8.520443018 , 8.013090699 , 7.482606521 , 6.927476753 ,
    6.346187668 , 5.737225535 , 5.099076627 , 4.431026531 , 3.735558101 , 3.01595351  , 2.275494928 , 1.517464527 ,
    0.74514448  , -0.038183043, -0.829104452, -1.62368048 , -2.417840443, -3.207513656, -3.988629435, -4.757117094,
    -5.508905951, -6.239925319, -6.94664712 , -7.6277137  , -8.28231001 , -8.909621003, -9.50883163 , -10.07912684,
    -10.61969159, -11.12971083, -11.60781043, -12.05037997, -12.45324994, -12.81225082, -13.12321312, -13.38196733,
    -13.58434394, -13.72660281, -13.80672124, -13.82310589, -13.77416343, -13.65830052, -13.47392383, -13.21944002,
    -12.89325576, -12.49396433, -12.02090548, -11.47360557, -10.85159099, -10.1543881 , -9.381523259, -8.532522848,
    -7.61005051 , -6.629318995, -5.608678327, -4.566478534, -3.521069642, -2.490801675, -1.494024661, -0.549046708,
    0.330896154 , 1.142418619 , 1.88326717  , 2.551188286 , 3.14392845  , 3.659234142 , 4.094851844 , 4.448528037 ,
    4.71947478  , 4.912766449 , 5.034942997 , 5.092544376 , 5.09211054  , 5.040181441 , 4.943297034 , 4.80723568  ,
    4.634729376 , 4.427748531 , 4.18826355  , 3.918244842 , 3.619662812 , 3.294487867 , 2.944690416 , 2.572591152 ,
    2.18191193  , 1.776724891 , 1.361102177 , 0.939115929 , 0.514838289 , 0.092341399 , -0.324620421, -0.733564139,
    -1.132324542, -1.518736419, -1.890634558, -2.245853749, -2.582228779, -2.897594438, -3.190297898, -3.460735871,
    -3.709817453, -3.93845174 , -4.14754783 , -4.338014817, -4.510761799, -4.666697871, -4.806398273, -4.929102817,
    -5.03371746 , -5.119148157, -5.184300863, -5.228081535, -5.249396129, -5.247432544, -5.222506465, -5.175215519,
    -5.106157332, -5.015929535, -4.905129753, -4.774355614, -4.624204748, -4.455087177, -4.266662513, -4.058402763,
    -3.829779934, -3.580266033, -3.309333068, -3.016453046, -2.70120127 , -2.363566225, -2.003639693, -1.621513456,
    -1.217279293, -0.791028987, -0.342854319, 0.12715293  , 0.618463342 , 1.128796954 , 1.655436166 , 2.195663377 ,
    2.746760988 , 3.306011399 , 3.870697009 , 4.43810022  , 5.005503431 , 5.570189041 , 6.129439452 , 6.680537063 ,
};

__attribute__((section("ram_code"))) void pmu_gpio_isr_ram(void)
{
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);
    
    button_toggle_detected(gpio_value);
    ool_write32(PMU_REG_PORTA_LAST, gpio_value);
}

__attribute__((section("ram_code"))) void i2s_isr_ram(void)
{
    static int16_t data[I2S_FIFO_DEPTH/2];
    static int total_value = 0;
    static uint16_t sample_count = 0;

    if(i2s_get_int_status() & I2S_INT_STATUS_RX_HFULL) {//codec_ADC
        i2s_get_data(data, I2S_FIFO_DEPTH/2, I2S_DATA_MONO);
        for(uint32_t i=0; i<(I2S_FIFO_DEPTH/2); i++) {
            total_value += data[i];
        }

        sample_count += I2S_FIFO_DEPTH/2;
        if(sample_count >= 24000) {
            if(calibration_stage == 0x81) {
                calibration_stage = 1;
                adc_value1 = total_value / sample_count;
                co_printf("adc_value1 is %d.\r\n", adc_value1);
            }
            else if(calibration_stage == 0x82) {
                calibration_stage = 2;
                adc_value2 = total_value / sample_count;
                co_printf("adc_value2 is %d.\r\n", adc_value2);
            }
            else if(calibration_stage == 0x83) {
                calibration_stage = 3;
                adc_rate_a = (adc_value2 - adc_value1) / (voltage2 - voltage1);
                adc_rate_b = adc_value1 - adc_rate_a * voltage1;
                printf("adc_rate_a is %f.\r\n", adc_rate_a);
                printf("adc_rate_b is %f.\r\n", adc_rate_b);
            }
            else {
                float voltage = 0.0;
                if(adc_rate_b != 0) {
                    total_value /= sample_count;
                    total_value -= adc_value_calib[(total_value/200)+95];
                    voltage = (total_value - adc_rate_b) / adc_rate_a;
                }
                printf("voltage is %f.\r\n", voltage);
            }
            
            total_value = 0;
            sample_count = 0;
        }
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
    __jump_table.addr.addr[0] = 0xBD;
    __jump_table.addr.addr[1] = 0xAD;
    __jump_table.addr.addr[2] = 0xD0;
    __jump_table.addr.addr[3] = 0xF0;
    __jump_table.addr.addr[4] = 0x80;
    __jump_table.addr.addr[5] = 0x10;
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
}

/*********************************************************************
 * @fn      user_entry_after_sleep_imp
 *
 * @brief   After system wakes up from sleep mode, user_entry_after_sleep_imp()
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
    
    system_sleep_disable();

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

    // Do some things here, can be uart print

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
    
    // Enable UART print.
    system_set_port_pull(GPIO_PA2, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    uart_init(UART1, BAUD_RATE_115200);    
 
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
    system_sleep_disable();
    
    pmu_set_pin_pull(GPIO_PORT_D, (1<<GPIO_BIT_4)|(1<<GPIO_BIT_5), true);
    pmu_port_wakeup_func_set(GPIO_PD4|GPIO_PD5);
    button_init(GPIO_PD4|GPIO_PD5);
    
    co_printf("Audio ADC\r\n");

    // User task initialization, for buttons.
    user_task_init();

    pmu_set_aldo_voltage(PMU_ALDO_MODE_BYPASS, 0x00);
    pmu_codec_power_enable();
    codec_adc_init(CODEC_SAMPLE_RATE_48000);
    codec_enable_adc();
    codec_enable_dac();
    i2s_init(I2S_DIR_RX,48000,1);
    NVIC_SetPriority(I2S_IRQn, 2);
    i2s_start();
    NVIC_EnableIRQ(I2S_IRQn);	
}
