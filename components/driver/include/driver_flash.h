/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 * NOTE: This driver is for inner flash operation through inner qspi interface.
 *
 */
#ifndef _DRIVER_FLASH_H
#define _DRIVER_FLASH_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdint.h>

#define FLASH_READ_TYPE_SINGLE          1
#define FLASH_READ_TYPE_DUAL            2
#define FLASH_READ_TYPE_QUAD            4

/*********************************************************************
* @fn      flash_read_status
*
* @brief   read the status register of flash.
*
* @param   None.
*
* @return  8 or 16 bits (depend flash type) flash status.
*/
uint16_t flash_read_status(void);

/*********************************************************************
* @fn      flash_write_status
*
* @brief   change the status of flash.
*
* @param   status   - target status.
*
* @return  None.
*/
void flash_write_status(uint16_t status);

/*********************************************************************
* @fn      flash_write
*
* @brief   write data into flash.
*
* @param   offset   - the start address to be written.
*          length   - how many bytes to be written.
*          buffer   - data source pointer.
*
* @return  None.
*/
void flash_write(uint32_t offset, uint32_t length, uint8_t *buffer);

/*********************************************************************
* @fn      flash_read
*
* @brief   read data from flash.
*
* @param   offset   - the start address to be read.
*          length   - how many bytes to be read.
*          buffer   - pointer to the area used to store data.
*
* @return  None.
*/
void flash_read(uint32_t offset, uint32_t length, uint8_t *buffer);

/*********************************************************************
* @fn      flash_erase
*
* @brief   sector erase.
*
* @param   offset   - the start address to be erase, should be aligned with
*                     sector (0x1000).
*          size     - should be multiple of 0x1000, 0 means one sector.
*
* @return  None.
*/
void flash_erase(uint32_t offset, uint32_t size);

/*********************************************************************
 * @fn      flash_page_erase
 *
 * @brief   erase one page of flash, this function can only be used in
 *          Puya flash.
 *
 * @param   offset  - page offset to be erased
 *
 * @return  None.
 */
uint8_t flash_page_erase(uint32_t offset);

/*********************************************************************
* @fn      qspi_flash_enable_quad
*
* @brief   enable the QUAD read mode.
*
* @param   None.
*
* @return  None.
*/
void qspi_flash_enable_quad(void);

/*********************************************************************
* @fn      qspi_flash_init
*
* @brief   initialize the qspi flash controller.
*
* @param   type - used to set read mode: SINGLE(1), DUAL(2), QUAD(4).
*
* @return  flash ID.
*/
uint32_t qspi_flash_init(uint8_t type);

#endif /* _DRIVER_FLASH_H */

