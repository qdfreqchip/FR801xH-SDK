/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_EXTI_H_
#define _DRIVER_EXTI_H_

/*
 * INCLUDE
 */
#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions

#include "driver_plf.h"
#define EXT_INT_TYPE_MSK        0x03
#define EXT_INT_TYPE_LEN        2
enum ext_int_type_t
{
    EXT_INT_TYPE_LOW,
    EXT_INT_TYPE_HIGH,
    EXT_INT_TYPE_POS,
    EXT_INT_TYPE_NEG,
};
enum exti_mux_t
{
    EXTI_0_PA0  = 0,
    EXTI_0_PC0,
    EXTI_0_PC7,

    EXTI_1_PA1  = 0,
    EXTI_1_PC1,
    EXTI_1_PC6,

    EXTI_2_PA2  = 0,
    EXTI_2_PC2,
    EXTI_2_PC5,

    EXTI_3_PA3  = 0,
    EXTI_3_PC3,
    EXTI_3_PC4,

    EXTI_4_PA4  = 0,
    EXTI_4_PC4,
    EXTI_4_PC3,

    EXTI_5_PA5  = 0,
    EXTI_5_PC5,
    EXTI_5_PC2,

    EXTI_6_PA6  = 0,
    EXTI_6_PC6,
    EXTI_6_PC1,

    EXTI_7_PA7  = 0,
    EXTI_7_PC7,
    EXTI_7_PC0,

    EXTI_8_PB0  = 0,
    EXTI_8_PD0,
    EXTI_8_PD7,

    EXTI_9_PB1  = 0,
    EXTI_9_PD1,
    EXTI_9_PD6,

    EXTI_10_PB2  = 0,
    EXTI_10_PD2,
    EXTI_10_PD5,

    EXTI_11_PB3  = 0,
    EXTI_11_PD3,
    EXTI_11_PD4,

    EXTI_12_PB4  = 0,
    EXTI_12_PD4,
    EXTI_12_PD3,

    EXTI_13_PB5  = 0,
    EXTI_13_PD5,
    EXTI_13_PD2,

    EXTI_14_PB6  = 0,
    EXTI_14_PD6,
    EXTI_14_PD2,

    EXTI_15_PB7  = 0,
    EXTI_15_PD7,
    EXTI_15_PD1,
};
enum exti_channel_t
{
    EXTI_0,
    EXTI_1,
    EXTI_2,
    EXTI_3,
    EXTI_4,
    EXTI_5,
    EXTI_6,
    EXTI_7,
    EXTI_8,
    EXTI_9,
    EXTI_10,
    EXTI_11,
    EXTI_12,
    EXTI_13,
    EXTI_14,
    EXTI_15,
};
/*
//ext_int_en, address: 0x00
Bit     Name        Type    Function Description
31:15   reserve     RW      reserve
14:0    exti_en     RW      exti isr src 14~0 enable/disable

//ext_int_status, address: 0x04
Bit     Name        Type    Function Description
31:0    isr src     RW      exti isr src,bit0~bit31,means pin PA0~PD7. write will clear isr.

//ext_int_type, address: 0x08
Bit     Name        Type    Function Description
31:0    isr src     RW      exti isr src,bit0~bit31,means pin PA0~PD7. write will clear isr.

//ext_int_control, address: 0x10
Bit     Name        Type    Function Description
31:0    isr src     RW      exti isr src,bit0~bit31,means pin PA0~PD7. write will clear isr.

//ext_int_raw_status, address: 0x18
Bit     Name        Type    Function Description
31:0    isr src     RW      exti isr src,bit0~bit31,means pin PA0~PD7. write will clear isr.
*/
struct ext_int_t
{
    uint32_t ext_int_en;
    uint32_t ext_int_status;
    uint32_t ext_int_type[2];
    uint32_t ext_int_control[32];
    uint32_t ext_int_raw_status;
};


/*
 * FUNCTION
 */

/*********************************************************************
 * @fn      uart_init
 *
 * @brief   initialize the uart module. Before calling this function, corresponding
 *          IO mux should be configured correctly.
 *
 * @param   uart_addr   - which uart will be initialized, UART0 or UART1.
 *          bandrate    - such as BAUD_RATE_115200.
 *
 * @return  None.
 */
void ext_int_enable(enum exti_channel_t exti_channel);
void ext_int_disable(enum exti_channel_t exti_channel);
uint32_t ext_int_get_src(void);
void ext_int_clear(uint32_t exti_src);
void ext_int_set_type(enum exti_channel_t exti_channel, enum ext_int_type_t type);

void ext_int_set_control(enum exti_channel_t exti_channel, uint32_t clk, uint8_t counter);
void ext_int_set_port_mux(enum exti_channel_t exti_channel,enum exti_mux_t exti_io);


/// @} EXTI
#endif /* _DRIVER_EXTI_H_ */

