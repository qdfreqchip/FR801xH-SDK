/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
//***********gyro driver**************

/*
 * INCLUDES
 */
#include <stdio.h>
#include <stdint.h>

#include "os_timer.h"
#include "driver_iic.h"
#include "driver_iomux.h"
#include "gyro_alg.h"
#include "driver_system.h"
#include "sys_utils.h"

#ifdef FOR_GYRO_DRIVER
os_timer_t gyroscope_loop_timer;
uint32_t gyroscope_loop_count = 0;

/******************************************************************************
      函数说明 开启OS timer
      入口数据：无
      返回值：  无

******************************************************************************/
void start_gyroscope_timer(void)
{
	os_timer_start(&gyroscope_loop_timer,10,1);
}

/******************************************************************************
      函数说明 创建OS timer
      入口数据：无
      返回值：  无

******************************************************************************/
void gyroscope_timer_loop(void * arg)
{
	gyroscope_i2c_init(GYRO_IIC_CHL);
	gyroscope_loop_count++;
	gyroscope_loop();
}

/******************************************************************************
      函数说明 创建OS timer
      入口数据：无
      返回值：  无

******************************************************************************/
void gyroscope_timer_init()
{
	os_timer_init(&gyroscope_loop_timer,gyroscope_timer_loop,NULL);
}

/******************************************************************************
      函数说明 get_dt
      入口数据：无
      返回值：  无

******************************************************************************/
uint32_t get_dt(void)
{
	uint32_t cur_tick = 0;
	cur_tick = gyroscope_loop_count;
	gyroscope_loop_count = 0;
	return cur_tick;
}

/******************************************************************************
      函数说明 延时函数
      入口数据：
      uint32_t delayTime  延时长度  /ms
      返回值：  无

******************************************************************************/
void delay_ms(uint32_t delayTime)
{
	co_delay_100us((10*delayTime));
}

/******************************************************************************
      函数说明 I2C读函数，
      入口数据：
      uint8_t deviceAddr,I2C设备地址
      uint8_t regAddr,寄存器地址
      uint8_t readLen,读取长度
      uint8_t *readBuf 读取buffer指针
      返回值：  无

******************************************************************************/
void I2C_Read_NBytes(uint8_t deviceAddr,uint8_t regAddr,uint8_t readLen,uint8_t *readBuf)
{
	uint8_t i = 0;

	for(i = 0;i < readLen;i++)
	{
		iic_read_byte(GYRO_IIC_CHL,deviceAddr,(regAddr+i),&readBuf[i]);
	}
}

/******************************************************************************
      函数说明 I2C写函数，I2C_Write_NBytes_imp修改过的I2C 写函数
      入口数据：无
      返回值：  无

******************************************************************************/
void I2C_Write_NBytes(uint8_t deviceAddr,uint8_t regAddr,uint8_t writeLen,uint8_t *writeBuf)
{
	iic_write_bytes(GYRO_IIC_CHL,deviceAddr,regAddr,writeBuf,writeLen);
}

void I2C_Write_NBytes_imp(uint8_t deviceAddr,uint8_t regAddr,uint8_t writeLen,uint8_t *writeBuf)
{
	iic_write_bytes_imp(GYRO_IIC_CHL,deviceAddr,regAddr,writeBuf,writeLen);
}

void gyroscope_i2c_init(uint8_t chl)
{
	system_set_port_mux(GPIO_PORT_C,GPIO_BIT_6,PORTC6_FUNC_I2C1_CLK);
	system_set_port_mux(GPIO_PORT_C,GPIO_BIT_7,PORTC7_FUNC_I2C1_DAT);
	system_set_port_pull(GPIO_PC6,true);
	system_set_port_pull(GPIO_PC7,true);
	iic_init(GYRO_IIC_CHL,350,GYRO_ADDRESS);
}

/******************************************************************************
      函数说明 g-sensor初始化，
      入口数据：无
      返回值：  无

******************************************************************************/
void gyro_dev_init(void)
{
	printf("=gyroscope start=\r\n");
	gyroscope_i2c_init(GYRO_IIC_CHL);//I2C初始化
	gyroscope_init();//g-sensor init
	gyroscope_timer_init();//创建os_timer
	start_gyroscope_timer();//开启os_timer
}
#endif


