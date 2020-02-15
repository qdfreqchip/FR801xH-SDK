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
#include <stdint.h>

#include "driver_exti.h"
#include "driver_plf.h"
#include "driver_system.h"
#include "co_printf.h"


volatile struct ext_int_t *const ext_int_reg = (struct ext_int_t *)EXTI_BASE;

void ext_int_set_port_mux(enum exti_channel_t exti_channel,enum exti_mux_t exti_io)
{
    uint32_t *exti_mux = (uint32_t *)0x50000038;
    *exti_mux = (*exti_mux  & ~(0x3<< (2*exti_channel) ) ) | (exti_io << (2*exti_channel));
}

void ext_int_enable(enum exti_channel_t exti_channel)
{
    ext_int_reg->ext_int_en |= (1<<exti_channel);
}

void ext_int_disable(enum exti_channel_t exti_channel)
{
    ext_int_reg->ext_int_en &= (~(1<<exti_channel));
}

uint32_t ext_int_get_src(void)
{
    return ext_int_reg->ext_int_status;
}

void ext_int_clear(uint32_t bit)
{
    ext_int_reg->ext_int_status = bit;
}

void ext_int_set_type(enum exti_channel_t exti_channel, enum ext_int_type_t type)
{
    uint8_t offset, index;
    uint32_t value;

    index = exti_channel / 16;
    offset = (exti_channel % 16) << 1;

    value = ext_int_reg->ext_int_type[index];
    value &= (~(EXT_INT_TYPE_MSK<<offset));
    value |= (type << offset);
    ext_int_reg->ext_int_type[index] = value;
}

void ext_int_set_control(enum exti_channel_t exti_channel, uint32_t clk, uint8_t counter)
{
    uint32_t pclk;

    pclk = system_get_pclk();

    ext_int_reg->ext_int_control[exti_channel] = (((pclk/clk-1)<<4) | (counter-1));
}

#if (0)
uint32_t pressed_button_isr = 0;
void button_reverse_int_type(uint32_t changed_button,uint32_t current_value)
{
    uint8_t i;
    for(i=0; i<32; i++)
    {
        if(changed_button & BIT(i))
        {
            changed_button &= (~(BIT(i)));
            if(current_value & BIT(i))
            {
                ext_int_set_type(i, EXT_INT_TYPE_HIGH);
            }
            else
            {
                ext_int_set_type(i, EXT_INT_TYPE_LOW);
            }
        }
        if(changed_button == 0)
        {
            break;
        }
    }
}
#include "app.h"
#include "rwip.h"

void exti_isr_ram(void)
{
    uint32_t status = ext_int_get_src();
    uint32_t gpio_button_sel = __jump_table.gpio_button_sel;
    uint32_t int_status =  status;

    pressed_button_isr ^= status;

    button_reverse_int_type(status, pressed_button_isr);

    if(pressed_button_isr)
    {
        co_printf("k:%x\r\n",pressed_button_isr);
    }
    else
    {
        co_printf("l\r\n");
        show_ke_malloc();

        rwip_prevent_sleep_clear(RW_GPIO_INT_ONGOING);
    }
    //co_printf("%x,%x\r\n",ext_int_get_src(),gpio_portd_read());
    ext_int_clear(status);
}

#endif
__attribute__((weak)) __attribute__((section("ram_code"))) void exti_isr_ram(void)
{
    uint32_t status;

    status = ext_int_get_src();
    ext_int_clear(status);
    co_printf("exti_usr:%x\r\n",status);
}


