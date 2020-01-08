#include <stdint.h>
#include "string.h"

#include "sys_utils.h"
#include "gap_api.h"
#include "user_task.h"
#include "os_msg_q.h"

#include "driver_gpio.h"
#include "driver_uart.h"
#include "driver_plf.h"
#include "driver_system.h"
#include "driver_ssp.h"
#include "driver_pmu.h"
#include "driver_efuse.h"
#include "driver_frspim.h"

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
            ssp_test((uint8_t *)0x01000000, 240*240*2);
            gpio_portc_write(gpio_portc_read() & (~(1<<5)));
            break;
        case 'S':
            co_printf("VAL: 0x%02x.\r\n", frspim_rd(FR_SPI_RF_COB_CHAN, ascii_strn2val((const char *)&data[0], 16, 2), 1));
            break;
        case 'T':
            frspim_wr(FR_SPI_RF_COB_CHAN, ascii_strn2val((const char *)&data[0], 16, 2), 1, ascii_strn2val((const char *)&data[3], 16, 2));
            co_printf("OK\r\n");
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

void app_at_cmd_recv_handler(uint8_t *data, uint16_t length)
{
    switch(data[0])
    {
        case 'A':
            app_at_recv_cmd_A(data[1], &data[2]);
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

void app_at_uart_recv(void*dummy, uint8_t status)
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

