#include <stdint.h>
#include "string.h"

#include "sys_utils.h"
#include "gap_api.h"
#include "user_task.h"
#include "os_msg_q.h"
#include "os_mem.h"

#include "driver_gpio.h"
#include "driver_uart.h"
#include "driver_plf.h"
#include "driver_system.h"
#include "driver_ssp.h"
#include "driver_pmu.h"
#include "driver_efuse.h"
#include "driver_frspim.h"
#include "driver_pwm.h"
#include "driver_flash.h"
#include "driver_adc.h"

#define AT_RECV_MAX_LEN             32

uint8_t app_at_recv_char;
uint8_t at_recv_buffer[AT_RECV_MAX_LEN];
uint8_t at_recv_index = 0;
uint8_t at_recv_state = 0;

#define FLASH_READ_TEST_LENGTH      1024
uint8_t flash_read_test_buffer[FLASH_READ_TEST_LENGTH];

static void app_at_recv_cmd_A(uint8_t sub_cmd, uint8_t *data)
{
    //struct bd_addr addr;
    //uint8_t tmp_data = 0;
    uint32_t data0, data1, data2;

    switch(sub_cmd)
    {
        #if 0
        case 'A':
            appm_create_conn_act(NULL);
            break;
        case 'B':
            // AT#AB01_XXXXXXXXXXXX_YY
            addr.addr[0] = str_to_val((const char *)&data[3], 16, 2);
            addr.addr[1] = str_to_val((const char *)&data[5], 16, 2);
            addr.addr[2] = str_to_val((const char *)&data[7], 16, 2);
            addr.addr[3] = str_to_val((const char *)&data[9], 16, 2);
            addr.addr[4] = str_to_val((const char *)&data[11], 16, 2);
            addr.addr[5] = str_to_val((const char *)&data[13], 16, 2);
            appm_start_conn(str_to_val((const char *)&data[0], 10, 2), &addr, str_to_val((const char *)&data[16], 16, 2));
            break;
        case 'C':
            appm_stop_conn(str_to_val((const char *)&data[0], 10, 2));
            break;
        case 'D':
            appm_conn_send_data(str_to_val((const char *)&data[0], 10, 2), 1, &tmp_data);
            break;
        case 'E':
            gap_start_advertising(0);
            break;
        #endif
        case 'G':
            co_printf("hello world!\r\n");
            break;
        case 'H':
            co_printf("VAL: 0x%08x.\r\n", REG_PL_RD(ascii_strn2val((const char *)&data[0], 16, 8)));
            break;
        case 'I':
            REG_PL_WR(ascii_strn2val((const char *)&data[0], 16, 8), ascii_strn2val((const char *)&data[9], 16, 8));
            co_printf("OK\r\n");
            break;
        case 'J':
            co_printf("OOL VAL: 0x%02x.\r\n", ool_read(ascii_strn2val((const char *)&data[0], 16, 2)));
            break;
        case 'K':
            ool_write(ascii_strn2val((const char *)&data[0], 16, 2), ascii_strn2val((const char *)&data[3], 16, 2));
            co_printf("OK\r\n");
            break;
        case 'L':
            co_printf("VAL: 0x%02x.\r\n", *(uint8_t *)(ascii_strn2val((const char *)&data[0], 16, 8)));
            break;
        case 'M':
            *(uint8_t *)(ascii_strn2val((const char *)&data[0], 16, 8)) = ascii_strn2val((const char *)&data[9], 16, 2);
            co_printf("OK\r\n");
            break;
        case 'N':
            efuse_read(&data0, &data1, &data2);
            co_printf("data0:%x\r\n", data0);
            co_printf("data1:%x\r\n", data1);
            co_printf("data2:%x\r\n", data2);
            break;
        case 'O':
            //new_efuse_value = ascii_strn2val((const char *)&cmd->data[0], 16, 8);
            co_printf("OK\r\n");
            break;
        case 'P':
            co_printf("VAL: 0x%02x.\r\n", *(uint8_t *)(MODEM_BASE + ascii_strn2val((const char *)&data[0], 16, 2)));
            break;
        case 'Q':
            *(uint8_t *)(MODEM_BASE + ascii_strn2val((const char *)&data[0], 16, 2)) = ascii_strn2val((const char *)&data[3], 16, 2);
            co_printf("OK\r\n");
            break;
        case 'R':
            system_set_port_mux(GPIO_PORT_C, GPIO_BIT_5, PORTC5_FUNC_C5);
            gpio_set_dir(GPIO_PORT_C, GPIO_BIT_5, GPIO_DIR_OUT);
            gpio_portc_write(gpio_portc_read() & (~(1<<5)));
            #if 0
            gpio_portc_write(gpio_portc_read() | ((1<<5)));
            flash_read(0, FLASH_READ_TEST_LENGTH, flash_read_test_buffer);
            gpio_portc_write(gpio_portc_read() & (~(1<<5)));
            #endif
            gpio_portc_write(gpio_portc_read() | ((1<<5)));
            ssp_test((uint8_t *)ascii_strn2val((const char *)&data[0], 16, 8), 240*240*2);
            gpio_portc_write(gpio_portc_read() & (~(1<<5)));
            break;
        case 'S':
            co_printf("VAL: 0x%02x.\r\n", frspim_rd(FR_SPI_RF_COB_CHAN, ascii_strn2val((const char *)&data[0], 16, 2), 1));
            break;
        case 'T':
            frspim_wr(FR_SPI_RF_COB_CHAN, ascii_strn2val((const char *)&data[0], 16, 2), 1, ascii_strn2val((const char *)&data[3], 16, 2));
            co_printf("OK\r\n");
            break;
        case 'U':
            {
                uint32_t *ptr = (uint32_t *)(ascii_strn2val((const char *)&data[0], 16, 8) & (~3));
                uint8_t count = ascii_strn2val((const char *)&data[9], 16, 2);
                uint32_t *start = (uint32_t *)((uint32_t)ptr & (~0x0f));
                for(uint8_t i=0; i<count;) {
                    if(((uint32_t)start & 0x0c) == 0) {
                        co_printf("0x%08x: ", start);
                    }
                    if(start < ptr) {
                        co_printf("        ");
                    }
                    else {
                        i++;
                        co_printf("%08x", *start);
                    }
                    if(((uint32_t)start & 0x0c) == 0x0c) {
                        co_printf("\r\n");
                    }
                    else {
                        co_printf(" ");
                    }
                    start++;
                }
            }
            break;
    }
}

static void app_at_recv_cmd_M(uint8_t sub_cmd, uint8_t *data)
{
    switch(sub_cmd)
    {
#if BLE_APP_MESH
        case 'A':
            extern void app_mesh_publish_on_off_msg(uint8_t on_off);
            app_mesh_publish_on_off_msg(false);
            break;
        case 'B':
            extern void app_mesh_publish_on_off_msg(uint8_t on_off);
            app_mesh_publish_on_off_msg(true);
            break;
        case 'C':
            extern void app_mesh_publish_on_off_msg2(uint8_t on_off);
            app_mesh_publish_on_off_msg_2(false);
            break;
        case 'D':
            extern void app_mesh_publish_on_off_msg2(uint8_t on_off);
            app_mesh_publish_on_off_msg_2(true);
            break;
#endif
        default:
            break;
    }

    co_printf("OK\r\n");
}

#define ADC_SAMPLE_COUNT        5000
static uint16_t adc_result[ADC_SAMPLE_COUNT];

void adc_sample_result_callback(uint16_t *buffer, uint32_t length)
{
    co_printf("adc_sample_result_callback\r\n");
    /*for(uint32_t i=0; i<length;) {
        co_printf("0x%04x ", buffer[i]);
        i++;
        
        if((i % 4) == 0) {
            co_printf("\r\n");
        }
    }*/
}

static void app_at_recv_cmd_D(uint8_t sub_cmd, uint8_t *data)
{
    switch(sub_cmd)
    {
        case 'A':
            flash_write(ascii_strn2val((const char *)&data[0], 16, 8), ascii_strn2val((const char *)&data[9], 16, 2), (void *)0);
            break;
        case 'B':
            flash_erase(ascii_strn2val((const char *)&data[0], 16, 8), ascii_strn2val((const char *)&data[9], 16, 2));
            break;
        case 'C':
            {
                struct adc_cfg_t cfg;
                cfg.src = ADC_TRANS_SOURCE_VBAT;
                cfg.ref_sel = ADC_REFERENCE_INTERNAL;
                cfg.int_ref_cfg = ADC_INTERNAL_REF_1_2;
                adc_init(&cfg);
            }
            break;
        case 'D':
            adc_enable(NULL, NULL, 0);
            break;
        case 'E':
            adc_enable(adc_sample_result_callback, adc_result, ADC_SAMPLE_COUNT);
            break;
        case 'F':
            {
                uint16_t result;
                adc_get_result(ADC_TRANS_SOURCE_VBAT, 0, &result);
                co_printf("0x%04x.\r\n", result);
            }
            return;
        case 'G':
            {
                system_set_port_mux(GPIO_PORT_D, GPIO_BIT_4, 0x0c);
                system_set_port_mux(GPIO_PORT_D, GPIO_BIT_5, 0x0c);
                system_set_port_mux(GPIO_PORT_D, GPIO_BIT_6, 0x0c);
                system_set_port_mux(GPIO_PORT_D, GPIO_BIT_7, 0x0c);
                struct adc_cfg_t cfg;
                memset((void*)&cfg, 0, sizeof(cfg));
                cfg.src = ADC_TRANS_SOURCE_PAD;
                cfg.ref_sel = ADC_REFERENCE_AVDD;
                cfg.channels = ascii_strn2val((const char *)&data[0], 16, 2);
                cfg.route.pad_to_sample = 1;
                adc_init(&cfg);
            }
            break;
        case 'H':
            {
                uint16_t result[4];
                adc_get_result(ADC_TRANS_SOURCE_PAD, 0x0f, result);
                co_printf("0x%04x, 0x%04x, 0x%04x, 0x%04x.\r\n", result[0], result[1], result[2], result[3]);
            }
            return;
        default:
            break;
    }

    co_printf("OK\r\n");
}

void app_at_cmd_recv_handler(uint8_t *data, uint16_t length)
{
    switch(data[0])
    {
        case 'A':
            app_at_recv_cmd_A(data[1], &data[2]);
            break;
        case 'D':
            app_at_recv_cmd_D(data[1], &data[2]);
            break;
        case 'M':
            app_at_recv_cmd_M(data[1], &data[2]);
        default:
            break;
    }
}

#define __RAM_CODE          __attribute__((section("ram_code")))
__RAM_CODE static void app_at_recv_c(uint8_t c)
{
    switch(at_recv_state)
    {
        case 0:
            if(c == 'A')
            {
                at_recv_state++;
            }
            break;
        case 1:
            if(c == 'T')
                at_recv_state++;
            else
                at_recv_state = 0;
            break;
        case 2:
            if(c == '#')
                at_recv_state++;
            else
                at_recv_state = 0;
            break;
        case 3:
            at_recv_buffer[at_recv_index++] = c;
            if((c == '\n')
               ||(at_recv_index >= AT_RECV_MAX_LEN))
            {
                os_event_t at_cmd_event;
                at_cmd_event.event_id = USER_EVT_AT_COMMAND;
                at_cmd_event.param = at_recv_buffer;
                at_cmd_event.param_len = at_recv_index;
                os_msg_post(user_task_id, &at_cmd_event);

                at_recv_state = 0;
                at_recv_index = 0;
            }
            break;
    }
}

__RAM_CODE void app_at_uart_recv(void*dummy, uint8_t status)
{
    app_at_recv_c(app_at_recv_char);
    uart1_read_for_hci(&app_at_recv_char, 1, app_at_uart_recv, NULL);
}

__attribute__((section("ram_code")))void app_at_init(void)
{
    system_set_port_pull(GPIO_PA2, true);

    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    uart_init(UART1, BAUD_RATE_115200);

    uart1_read_for_hci(&app_at_recv_char, 1, app_at_uart_recv, NULL);
}

typedef void (*rwip_eif_callback) (void*, uint8_t);

struct uart_txrxchannel
{
    /// call back function pointer
    rwip_eif_callback callback;
};

struct uart_env_tag
{
    /// rx channel
    struct uart_txrxchannel rx;
    uint32_t rxsize;
    uint8_t *rxbufptr;
    void *dummy;
    /// error detect
    uint8_t errordetect;
    /// external wakeup
    bool ext_wakeup;
};

__attribute__((section("ram_code"))) void uart1_isr_ram(void)
{
    uint8_t int_id;
    uint8_t c;
    rwip_eif_callback callback;
    void *dummy;
    volatile struct uart_reg_t *uart_reg = (volatile struct uart_reg_t *)UART1_BASE;
    struct uart_env_tag *uart1_env = (struct uart_env_tag *)0x20000a54;

    int_id = uart_reg->u3.iir.int_id;

    if(int_id == 0x04 || int_id == 0x0c )   /* Receiver data available or Character time-out indication */
    {
        while(uart_reg->lsr & 0x01)
        {
            c = uart_reg->u1.data;
            *uart1_env->rxbufptr++ = c;
            uart1_env->rxsize--;
            if((uart1_env->rxsize == 0)
               &&(uart1_env->rx.callback))
            {
                uart_reg->u3.fcr.data = 0xf1;
                NVIC_DisableIRQ(UART1_IRQn);
                uart_reg->u3.fcr.data = 0x21;
                callback = uart1_env->rx.callback;
                dummy = uart1_env->dummy;
                uart1_env->rx.callback = 0;
                uart1_env->rxbufptr = 0;
                callback(dummy, 0);
                break;
            }
        }
    }
    else if(int_id == 0x06)
    {
        volatile uint32_t line_status = uart_reg->lsr;
    }
}
