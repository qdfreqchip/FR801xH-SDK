/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_UART_H_
#define _DRIVER_UART_H_

/*
 * INCLUDE
 */
#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions

#include "driver_plf.h"

/*
 * ENUMERATION
 */

/* BAUD_RATE configuration */
#define BAUD_RATE_921600                 11
#define BAUD_RATE_460800                 10
#define BAUD_RATE_230400                 9
#define BAUD_RATE_115200                 8
#define BAUD_RATE_57600                  7
#define BAUD_RATE_38400                  6
#define BAUD_RATE_19200                  5
#define BAUD_RATE_14400                  4
#define BAUD_RATE_9600                   3
#define BAUD_RATE_4800                   2
#define BAUD_RATE_2400                   1
#define BAUD_RATE_1200                   0

#define UART0                            UART0_BASE
#define UART1                            UART1_BASE

typedef void (*uart_int_callback) (void*, uint8_t);

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
void uart_init(uint32_t uart_addr, uint8_t bandrate);

/*********************************************************************
 * @fn      uart_finish_transfers
 *
 * @brief   wait for tx fifo empty.
 *
 * @param   uart_addr   - which uart will be checked, UART0 or UART1.
 *
 * @return  None.
 */
void uart_finish_transfers(uint32_t uart_addr);

/*********************************************************************
 * @fn      uart_putc_noint
 *
 * @brief   send a byte to UART bus, return until tx fifo is empty.
 *
 * @param   uart_addr   - which uart will be used, UART0 or UART1.
 *          c           - data to be send
 *
 * @return  None.
 */
void uart_putc_noint(uint32_t uart_addr, uint8_t c);

/*********************************************************************
 * @fn      uart_putc_noint
 *
 * @brief   send a byte to UART bus without the byte is sent.
 *
 * @param   uart_addr   - which uart will be used, UART0 or UART1.
 *          c           - data to be send
 *
 * @return  None.
 */
void uart_putc_noint_no_wait(uint32_t uart_addr, uint8_t c);

/*********************************************************************
 * @fn      uart_put_data_noint
 *
 * @brief   send several bytes to UART bus, return until tx fifo is empty.
 *
 * @param   uart_addr   - which uart will be used, UART0 or UART1.
 *          d           - data buffer pointer
 *          size        - how many bytes to be send.
 *
 * @return  None.
 */
void uart_put_data_noint(uint32_t uart_addr, const uint8_t *d, int size);

/*********************************************************************
 * @fn      uart_get_data_noint
 *
 * @brief   get data with specified length, return until all data are acquired.
 *
 * @param   uart_addr   - which uart will be used, UART0 or UART1.
 *          d           - data buffer pointer
 *          size        - how many bytes to be read.
 *
 * @return  None.
 */
void uart_get_data_noint(uint32_t uart_addr, uint8_t *buf, int size);

/*********************************************************************
 * @fn      uart_get_data_nodelay_noint
 *
 * @brief   get data with specified length, return until rx fifo is empty 
 *          or all data are acquired.
 *
 * @param   uart_addr   - which uart will be used, UART0 or UART1.
 *          d           - data buffer pointer
 *          size        - how many bytes to be read.
 *
 * @return  how many bytes have been read, may be less than parameter size.
 */
int uart_get_data_nodelay_noint(uint32_t uart_addr, uint8_t *buf, int size);

/*********************************************************************
 * @fn      uart0_read
 *
 * @brief   get data with specified length. Interrup will be enable inside this function,
 *          callback will be called once all data are acquired.
 *
 * @param   bufptr  - ram pointer used to store read data.
 *          size    - how many bytes to be read.
 *          callback- callback function when all data are acquired.
 *
 * @return  None.
 */
void uart0_read_for_hci(uint8_t *bufptr, uint32_t size, uart_int_callback callback, void *dummy);
#define uart0_read(bufptr, size, callback)      uart0_read_for_hci(bufptr, size, callback, NULL)

/*********************************************************************
 * @fn      uart1_read
 *
 * @brief   get data with specified length. Interrup will be enable inside this function,
 *          callback will be called once all data are acquired.
 *
 * @param   bufptr  - ram pointer used to store read data.
 *          size    - how many bytes to be read.
 *          callback- callback function when all data are acquired.
 *
 * @return  None.
 */
void uart1_read_for_hci(uint8_t *bufptr, uint32_t size, uart_int_callback callback, void *dummy);
#define uart1_read(bufptr, size, callback)      uart1_read_for_hci(bufptr, size, callback, NULL)

/*********************************************************************
 * @fn      uart0_write
 *
 * @brief   send data with specified length. callback will be called once all 
 *          data are sent.
 *
 * @param   bufptr  - ram pointer used to store send data.
 *          size    - how many bytes to be sent.
 *          callback- callback function when all data are sent.
 *
 * @return  None.
 */
void uart0_write_for_hci(uint8_t *bufptr, uint32_t size, uart_int_callback callback, void *dummy);
#define uart0_write(bufptr, size, callback)      uart0_write_for_hci(bufptr, size, callback, NULL)

/*********************************************************************
 * @fn      uart1_write
 *
 * @brief   send data with specified length. callback will be called once all 
 *          data are sent.
 *
 * @param   bufptr  - ram pointer used to store send data.
 *          size    - how many bytes to be sent.
 *          callback- callback function when all data are sent.
 *
 * @return  None.
 */
void uart1_write_for_hci(uint8_t *bufptr, uint32_t size, uart_int_callback callback, void *dummy);
#define uart1_write(bufptr, size, callback)      uart1_write_for_hci(bufptr, size, callback, NULL)

/// @} UART
#endif /* _DRIVER_UART_H_ */

