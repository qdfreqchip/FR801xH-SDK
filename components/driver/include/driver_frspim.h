/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_FRSPIM_H_
#define _DRIVER_FRSPIM_H_

/*
 * INCLUDES (包含头文件)
 */
#include <stdint.h>

struct frspim_ctrl_t
{
    uint32_t go_done:1; /*Read:0--idle,1--busy; Write:0--no action,1--transmit start(hw clear)*/
    uint32_t mode:1;    /*0--codec/pmu access, 1-- modem/rf access*/
    uint32_t sel:2;     /*channel select*/
    uint32_t len:3;     /*TX/RX byte(1:4)*/
    uint32_t rsv0:1;
    uint32_t op:1;      /*1--write,0--read*/
    uint32_t rsv1:3;
    uint32_t ratio:2;   /*sclk = p_clk/(1+ratio)*2*/
    uint32_t rsv2:2;
    uint32_t addr:8;
    uint32_t rsv3:8;
};

struct frspim_wdat_t
{
    uint32_t wbyte0:8;
    uint32_t wbyte1:8;
    uint32_t wbyte2:8;
    uint32_t wbyte3:8;
};

struct frspim_rdat_t
{
    uint32_t rbyte0:8;
    uint32_t rbyte1:8;
    uint32_t rbyte2:8;
    uint32_t rbyte3:8;
};

struct frspim_reg_t
{
    struct frspim_ctrl_t ctrl;
    struct frspim_wdat_t wdat;
    struct frspim_rdat_t rdat;
};

#define FR_SPI_RF_COB_CHAN          2
#define FR_SPI_CODEC_CHAN           1
#define FR_SPI_PMU_CHAN             0

/*********************************************************************
* @fn      frspim_init
*
* @brief   initialize internal frspim module.
*
* @param   ratio    - clock dividor.
*
* @return  None.
*/
void frspim_init(uint8_t ratio);

/*********************************************************************
* @fn      frspim_rd
*
* @brief   read data from frspim module.
*
* @param   chan_num - indicate which internal module is read. Such as
*                     FR_SPI_RF_COB_CHAN.
*          addr     - which register is read
*          len      - this parameter should be 1, 2, 4
*
* @return  return read value.
*/
uint32_t frspim_rd (uint8_t chan_num, uint8_t addr, uint8_t len);

/*********************************************************************
* @fn      frspim_rd
*
* @brief   write data to frspim module.
*
* @param   chan_num - indicate which internal module is read. Such as
*                     FR_SPI_RF_COB_CHAN.
*          addr     - which register is written
*          len      - this parameter should be 1, 2, 4
*          val      - data to be written
*
* @return  None.
*/
void frspim_wr (uint8_t chan_num, uint8_t addr, uint8_t len, uint32_t val);

#endif

