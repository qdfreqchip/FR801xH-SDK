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
#include <stdbool.h>
#include <stdlib.h> // for rand()

#include "driver_iic.h"
#include "driver_plf.h"
#include "driver_system.h"
#include "sys_utils.h"

/*
 * MACROS 
 */
#define IIC_TRAN_START      0x100
#define IIC_TRAN_STOP       0x200

#define IIC0_REG_BASE           ((volatile struct iic_reg_t *)I2C0_BASE)
#define IIC1_REG_BASE           ((volatile struct iic_reg_t *)I2C1_BASE)

/*
 * TYPEDEFS 
 */
struct iic_data_t
{
    uint32_t data:8;
    uint32_t start:1;
    uint32_t stop:1;
    uint32_t unused:22;
};

struct iic_status_t
{
    uint32_t trans_done:1;
    uint32_t arb_fail:1;
    uint32_t no_ack:1;
    uint32_t data_req_mst:1;
    uint32_t data_req_slv:1;
    uint32_t bus_atv:1;
    uint32_t sts_scl:1;
    uint32_t sts_sda:1;
    uint32_t rec_full:1;
    uint32_t rec_emp:1;
    uint32_t trans_ful:1;
    uint32_t trans_emp:1;
    uint32_t slv_trans_ful:1;
    uint32_t slv_trans_emp:1;
    uint32_t unused:18;
};

struct iic_control_t
{
    uint32_t trans_done_ie:1;
    uint32_t arb_fail_ie:1;
    uint32_t no_ack_ie:1;
    uint32_t req_mst_ie:1;
    uint32_t req_slv_ie:1;
    uint32_t rec_full_ie:1;
    uint32_t rec_noemp_ie:1;
    uint32_t trans_noful_ie:1;
    uint32_t soft_reset:1;
    uint32_t seven_bit:1;
    uint32_t slv_noful_ie:1;
    uint32_t unused:21;
};

struct iic_clkdiv_t
{
    uint32_t clk_div:9;
    uint32_t unused:23;
};

struct iic_address_t
{
    uint32_t slv_addr:10;
    uint32_t unused:22;
};

struct iic_rxlevel_t
{
    uint32_t rx_level:3;
    uint32_t unused:29;
};

struct iic_txlevel_t
{
    uint32_t tx_level:3;
    uint32_t unused:29;
};

struct iic_rxbytecnt_t
{
    uint32_t rx_byte_cnt:16;
    uint32_t unused:16;
};

struct iic_txbytecnt_t
{
    uint32_t tx_byte_cnt:16;
    uint32_t unused:16;
};

struct iic_slavedata_t
{
    uint32_t slave_data:8;
    uint32_t unused:24;
};

struct iic_reg_t
{
    uint32_t data;
    struct iic_status_t status;
    struct iic_control_t control;
    struct iic_clkdiv_t clkdiv;
    struct iic_address_t address;
    struct iic_rxlevel_t rxlevel;
    struct iic_txlevel_t txlevel;
    struct iic_rxbytecnt_t rx_bytecnt;
    struct iic_txbytecnt_t tx_bytecnt;
    struct iic_slavedata_t slavedata;
};

/*
 * LOCAL VARIABLES 
 */
/// how many 10us will be taken on IIC bus for one byte
static uint16_t iic_byte_period[IIC_CHANNEL_MAX] = {10, 10};

/*********************************************************************
 * @fn      iic_write_byte
 *
 * @brief   write one byte to slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be writen
 *          data        - data to be writen
 *
 * @return  None.
 */
uint8_t iic_write_byte(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    volatile struct iic_reg_t *iic_reg;

    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->data = slave_addr | IIC_TRAN_START;
    while(iic_reg->status.trans_emp != 1);
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;
    iic_reg->data = data | IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    return true;
}

/*********************************************************************
 * @fn      iic_write_bytes
 *
 * @brief   write multi-bytes to slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be writen
 *          buffer      - pointer to data buffer
 *          length      - how many bytes to be written
 *
 * @return  None.
 */
uint8_t iic_write_bytes(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;

    if(length == 0)
    {
        return true;
    }
    length--;

    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->data = slave_addr | IIC_TRAN_START;
    while(iic_reg->status.trans_emp != 1);
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;

    while(length)
    {
        while(iic_reg->status.trans_ful == 1);
        iic_reg->data = *buffer++;
        length--;
    }

    while(iic_reg->status.trans_ful == 1);
    iic_reg->data = *buffer | IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    return true;
}

/*********************************************************************
 * @fn      iic_read_byte
 *
 * @brief   read one byte frome slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be written
 *          buffer      - store data to buffer
 *
 * @return  None.
 */
uint8_t iic_read_byte(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer)
{
    volatile struct iic_reg_t *iic_reg;

    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->data = slave_addr | IIC_TRAN_START;
    while(iic_reg->status.trans_emp != 1);
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;
    iic_reg->data = slave_addr | 0x01 | IIC_TRAN_START;
    iic_reg->data = IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    *buffer = iic_reg->data&0xff;

    return true;
}

/*********************************************************************
 * @fn      iic_read_bytes
 *
 * @brief   read multi-bytes frome slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be written
 *          buffer      - buffer pointer to be written
 *          length      - how many bytes to be read
 *
 * @return  None.
 */
uint8_t iic_read_bytes(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;

    if(length == 0)
    {
        return true;
    }

    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->data = slave_addr | IIC_TRAN_START;
    while(iic_reg->status.trans_emp != 1);
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;
    iic_reg->data = slave_addr | 0x01 | IIC_TRAN_START;
	
    while(length > 1)
    {
        iic_reg->data = 0x00;

        while(iic_reg->status.rec_emp != 1)
        {
            *buffer++ = iic_reg->data;
            length--;
        }
        while(iic_reg->status.trans_emp != 1);
    }

    iic_reg->data = IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    while(length)
    {
        *buffer++ = iic_reg->data;
        length--;
    }

    *buffer = iic_reg->data&0xff;

    return true;
}
uint8_t iic_write_bytes_imp(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, 
									uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;

    if(length == 0)
    {
        return true;
    }
    length--;

    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->data = slave_addr | IIC_TRAN_START;
    while(iic_reg->status.trans_emp != 1);
    co_delay_10us(10);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;

    while(length)
    {
        while(iic_reg->status.trans_ful == 1);
        iic_reg->data = *buffer++;
        length--;
    }

    while(iic_reg->status.trans_ful == 1);
    iic_reg->data = *buffer | IIC_TRAN_STOP;

	co_delay_100us(1);
	
	return true;
	
    //while(iic_reg->status.bus_atv == 1);

    //return true;
}

/*********************************************************************
 * @fn      iic_init
 *
 * @brief   Initialize iic instance.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          speed       - SCL speed when working as master, N * 1000
 *          slave_addr  - local address when working as slave
 *
 * @return  None.
 */
void iic_init(enum iic_channel_t channel, uint16_t speed, uint16_t slave_addr)
{
    volatile struct iic_reg_t *iic_reg;

    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->clkdiv.clk_div = (system_get_pclk_config()*1000/speed-10)/2;
    iic_reg->control.soft_reset = 1;
    iic_reg->control.seven_bit = 1;
    iic_reg->address.slv_addr = slave_addr;

    iic_byte_period[channel] = 1000/speed + 1;
}

enum iic_test_tate_t
{
    IIC_TEST_STATE_WAIT_ADDR,
    IIC_TEST_STATE_RW,
};
uint8_t iic_test_state = IIC_TEST_STATE_WAIT_ADDR;
uint8_t iic_test_reg_addr = 0;

__attribute__((section("ram_code"))) void iic0_isr(void)
{
    volatile struct iic_reg_t *iic_reg = IIC0_REG_BASE;
    uint8_t data;

    if(iic_reg->status.rec_emp == 0)
    {
        if(iic_test_state == IIC_TEST_STATE_WAIT_ADDR)
        {
            iic_test_reg_addr = iic_reg->data;
            iic_test_state = IIC_TEST_STATE_RW;
        }
        else
        {
            co_printf("write 0x%02x at 0x%02x.\r\n", iic_reg->data, iic_test_reg_addr++);
        }
    }
    else if(iic_reg->status.data_req_slv)
    {
        data = rand() & 0xFF;
        iic_reg->slavedata.slave_data = data;
        co_printf("send 0x%02x.\r\n", data);
    }
    else if(iic_reg->status.trans_done)
    {
        iic_reg->status.trans_done = 1;
        iic_test_state = IIC_TEST_STATE_WAIT_ADDR;
    }
}

