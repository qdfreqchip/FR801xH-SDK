/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * \brief Sensirion SHT3x driver implementation
 *
 * This module provides access to the SHT3x functionality over a generic I2C
 * interface. It supports measurements without clock stretching only.
 */

#include "sht3x.h"
#include "sht3x_arch_config.h"
#include "sht3x_common.h"
#include "sht3x_i2c.h"
#include "lcd.h"
#include "co_printf.h"
#include <stdbool.h>
#include <stdio.h>
#include "sys_utils.h"

/* all measurement commands return T (CRC) RH (CRC) */
#if USE_SENSIRION_CLOCK_STRETCHING
#define SHT3X_CMD_MEASURE_HPM 0x2C06
#define SHT3X_CMD_MEASURE_LPM 0x2C10
#else /* USE_SENSIRION_CLOCK_STRETCHING */
#define SHT3X_CMD_MEASURE_HPM 0x2400
#define SHT3X_CMD_MEASURE_LPM 0x2416
#define SHT3X_MEASUREMENT_DURATION_USEC 15000
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
static const uint16_t SHT3X_CMD_READ_STATUS_REG = 0xF32D;
static const uint16_t SHT3X_CMD_DURATION_USEC = 1000;
#ifdef SHT_ADDRESS
static const uint8_t SHT3X_ADDRESS = SHT_ADDRESS;
#else
static const uint8_t SHT3X_ADDRESS = 0x44;
#endif

static uint16_t sht3x_cmd_measure = SHT3X_CMD_MEASURE_HPM;

/******************************************************************************
      函数说明：先检测SHT3x是否存在  ，如果存在则 读取温湿度，否则返回false
      入口数据：无
      返回值：  成功返回 true    否则返回false

******************************************************************************/

int16_t sht3x_measure_blocking_read(int32_t *temperature, int32_t *humidity)
{
	sensirion_i2c_init();
	int16_t ret = sht3x_measure();
    if (ret == STATUS_OK)
    {
#if !defined(USE_SENSIRION_CLOCK_STRETCHING) || !USE_SENSIRION_CLOCK_STRETCHING
        sensirion_sleep_usec(SHT3X_MEASUREMENT_DURATION_USEC);
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
        ret = sht3x_read(temperature, humidity);
    }
    return ret;
}
/******************************************************************************
      函数说明：先检测SHT3x是否存在  ，
      入口数据：无
      返回值：  成功返回 true    否则返回false

******************************************************************************/

int16_t sht3x_measure()
{
    return sensirion_i2c_write_cmd(SHT3X_ADDRESS, sht3x_cmd_measure);
}

/******************************************************************************
      函数说明 读取温湿度，
      入口数据：无
      返回值：  成功返回 true    否则返回false

******************************************************************************/

int16_t sht3x_read(int32_t *temperature, int32_t *humidity)
{
    uint16_t words[2];
    int16_t ret = sensirion_i2c_read_words(SHT3X_ADDRESS, words,
                                           SENSIRION_NUM_WORDS(words));
    /**
     * formulas for conversion of the sensor signals, optimized for fixed point
     * algebra: Temperature       = 175 * S_T / 2^16 - 45 Relative Humidity =
     * 100 * S_RH / 2^16
     */


    *temperature = 2.67* ((int32_t)words[0]) - 45000;
    *humidity = 1.526 * ((int32_t)words[1]);
    return ret;
}

/******************************************************************************
      函数说明 带延时读取寄存器，
      入口数据：无
      返回值：  成功返回 true    否则返回false

******************************************************************************/

int16_t sht3x_probe(void)
{
    uint16_t status;
    return sensirion_i2c_delayed_read_cmd(
               SHT3X_ADDRESS, SHT3X_CMD_READ_STATUS_REG, SHT3X_CMD_DURATION_USEC,
               &status, 1);
}
/******************************************************************************
      函数说明 SHT3x 使能低功耗状态
      入口数据：无
      返回值：无

******************************************************************************/

void sht3x_enable_low_power_mode(uint8_t enable_low_power_mode)
{
    sht3x_cmd_measure =
        enable_low_power_mode ? SHT3X_CMD_MEASURE_LPM : SHT3X_CMD_MEASURE_HPM;
}

/*const char *sht3x_get_driver_version(void)
{
//    return SHT_DRV_VERSION_STR;
}*/

/******************************************************************************
	  函数说明 获取传感器I2C地址
	  入口数据：无
	  返回值：	成功返回 true	 否则返回false

******************************************************************************/

uint8_t sht3x_get_configured_address(void)
{
    return SHT3X_ADDRESS;
}


/*
void timer_SHT3x_FUN(void *arg)
{
    // handle_tft_pic();
    int32_t temperature, humidity;
    uint8_t LCD_ShowStringBuff[30];

    int8_t ret = sht3x_measure_blocking_read(&temperature, &humidity);//Read temperature   humidity
    if (ret == STATUS_OK)
    {
        co_printf("temperature = %d,humidity = %d\r\n",temperature,humidity);
        sprintf((char *)LCD_ShowStringBuff,"T= %0.1f H=%0.1f%%",temperature/1000.0,humidity/1000.0);
        LCD_ShowString(10,80,LCD_ShowStringBuff,RED);

    }
    else
    {
        co_printf("error reading measurement\n");
    }
}
*/
/******************************************************************************
      函数说明：demo_SHT3x_APP函数
      入口数据：无
      返回值：

******************************************************************************/

uint8_t demo_SHT3x_APP(void)
{
    sensirion_i2c_init();//SH3x I2C Init
//    os_timer_init(&timer_SHT3x,timer_SHT3x_FUN,NULL);//每1s更新一次温湿度
//    os_timer_start(&timer_SHT3x,1000,1);
    return true;
}


