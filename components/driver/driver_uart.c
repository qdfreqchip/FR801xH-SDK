#include <stdint.h>
#include "driver_plf.h"
#include "driver_uart.h"


__attribute__((weak)) __attribute__((section("ram_code"))) void uart1_isr_ram(void)
{
    uint8_t int_id;
    uint8_t c;
    volatile struct uart_reg_t *uart_reg = (volatile struct uart_reg_t *)UART1_BASE;

    int_id = uart_reg->u3.iir.int_id;

    if(int_id == 0x04 || int_id == 0x0c )   /* Receiver data available or Character time-out indication */
    {
        c = uart_reg->u1.data;
        uart_putc_noint(UART1,c);
    }
    else if(int_id == 0x06)
    {
        uart_reg->u3.iir.int_id = int_id;
    }
}

__attribute__((weak)) __attribute__((section("ram_code"))) void uart0_isr_ram(void)
{
    uint8_t int_id;
    uint8_t c;
    volatile struct uart_reg_t *uart_reg = (volatile struct uart_reg_t *)UART0_BASE;

    int_id = uart_reg->u3.iir.int_id;

    if(int_id == 0x04 || int_id == 0x0c )   /* Receiver data available or Character time-out indication */
    {
        c = uart_reg->u1.data;
        uart_putc_noint(UART0,c);
    }
    else if(int_id == 0x06)
    {
        uart_reg->u3.iir.int_id = int_id;
    }
}
