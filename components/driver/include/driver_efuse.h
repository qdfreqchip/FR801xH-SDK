/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_EFUSE_H
#define _DRIVER_EFUSE_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdint.h>

/*********************************************************************
 * @fn      efuse_write
 *
 * @brief   write data to efuse.
 *
 * @param   data0   - lower 32-bits.
 *          data1   - middle 32-bits.
 *          data2   - higher 32-bits.
 *
 * @return  None.
 */
void efuse_write(uint32_t data0, uint32_t data1, uint32_t data2);

/*********************************************************************
 * @fn      efuse_read
 *
 * @brief   read data from efuse.
 *
 * @param   data0   - pointer used to store lower 32-bits.
 *          data1   - pointer used to store middle 32-bits.
 *          data2   - pointer used to store higher 32-bits.
 *
 * @return  None.
 */
void efuse_read(uint32_t *data0, uint32_t *data1, uint32_t *data2);

#endif  // _DRIVER_EFUSE_H

