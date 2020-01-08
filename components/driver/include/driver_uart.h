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
Bit     Name    Type        Function Description
7:4     RSVD    R           These bits are always cleared (Reserved bits).
3       EMSI     W/R        “1”: Enable Modem Status Interrupt (EMSI)
                                    “0”: Disable EMSI
2       ERLSI   W/R         “1”: Enable Receive Line Status Interrupt (ERLSI)
                                    “0”: Disable ERLSI
1       ETI      W/R          “1”: Enable THR Empty Interrupt (ETI)
                                    “0”: Disable ETI
0       ERDI    W/R          “1”: Enable Received Data Available Interrupt (ERDI)
                                    “0”: Disable ERDI
*/
/*address: 0x04*/
struct uart_ier_t   /*Interrupt Enable Register*/
{
    uint32_t erdi:1;
    uint32_t eti:1;
    uint32_t erlsi:1;
    uint32_t emsi:1;
    uint32_t unused:28;
};

/*
Bit     Name    Type    Function Description
7:3     SIZE    RO          FIFO SIZE
                                    BIT[7:3] BYTES
                                    000          0
                                    110         16
                                    111         64
                                    (Option Enhance)
4       RSVD     RO        The reserved bit is always cleared (Reserved).
3:0 Interrupt   RO       Bit 0: When interrupt is pending this bit is zero else it is one
        ID Bit                Bit 1~3: Interrupt ID
                                Priority Interrupt ID Bit[3:0]
                                *           0001    No interrupt is pending
                                1           0110    Receiver line status
                                2           0100    Receiver data available
                                2           1100    Character time-out indication
                                3           0010    THR empty
                                4           0000    Modem status
*/
/*address: 0x08*/
struct uart_iir_t   /*Interrupt Enable Register*/
{
    uint32_t int_id:4;
    uint32_t unused:28;
};


/*
Bit     Name        Type    Function Description
7:6     Rx Trigger WO       Indicate the trigger level for the receiver FIFO interrupt
                                        00 --- 1 character in the FIFO
                                        01 --- FIFO 1/4 full
                                        10 --- FIFO 1/2 full
                                        11 --- FIFO 1/8 less full
5:4     Tx Trigger WO       Indicate the trigger level for the transmit FIFO interrupt.
                                        00 --- Empty FIFO
                                        01 --- 2 character in the FIFO
                                        10 --- FIFO 1/4 full
                                        11 --- FIFO 1/2 full
3       DMA          WO         “0” TXRDYN,RXRDYN signal work in DMA mode 0
        Mode                        “1” TXRDYN,RXRDYN signal work in DMA mode 1

2      Tx FIFO      WO          When this bit is set, transmitter FIFO reset. The logic one
        Reset                        written to this bit is self clearing.

1       Rx FIFO     WO          When this bit is set, receiver FIFO reset. The logic one
        Reset                        written to this bit is self clearing.
0       FIFO          WO          Enables transmitter or receiver FIFOs when set. This bit
        EN                             must be set when other FCR bits are written to or they are
                                         no programmed. Changing this bit to zero clears the FIFOs
*/
/*address: 0x08*/
#define FCR_RX_TRIGGER_00 0x00
#define FCR_RX_TRIGGER_01 0x40
#define FCR_RX_TRIGGER_10 0x80
#define FCR_RX_TRIGGER_11 0xc0
#define FCR_TX_TRIGGER_00 0x00
#define FCR_TX_TRIGGER_01 0x10
#define FCR_TX_TRIGGER_10 0x20
#define FCR_TX_TRIGGER_11 0x30
#define FCR_TX_FIFO_RESET 0x04
#define FCR_RX_FIFO_RESET 0x02
#define FCR_FIFO_ENABLE   0x01

struct uart_fcr_t   /*FIFO Control Register*/
{
    uint32_t data;
};
/*
Bit     Name        Type    Function Description
7       DLAB         W/R    Divisor Registers Access Bit.
                                    This bit must be set to access the divisor latches of baud
                                    generator. Bit 7 must be cleared during access the receiver
                                    buffers, THR or IER.
6       BC            W/R     Break Control Bit.
                                    BC is set to force a break condition; i.e. where SOUT is
                                    forced to the low state. When bit 6 is cleared break condition
                                    is disabled.
5       SP             W/R    Stick Parity Bit .
                                    When bits 3,4,5 are set parity bit is transmitted and checked
                                    as cleared. When bits 3 and 5 are set and bit 4 is cleared, the
                                    parity bit is transmitted and checked as set. When bit 5 is
                                    cleared, stick parity is disabled.
4       EPS           W/R    Even Parity Enable
                                    When parity is enabled and bit 4 is set even parity is
                                    selected. When parity is enabled and bit 4 is cleared odd
                                    parity is selected.
3       PE             W/R    Parity Enable Bit
                                    When bit 3 is set parity bit is generated in data transmitted
                                    between last data word bit and first stop bit and parity is
                                    checked in the received data. When this bit is cleared no
                                    parity is generated or checked.
2       STOP        W/R     Stop Bits
                                    0= 1 stop bit after data bits
                                    1= 1.5 stop bit for a 5-bit data character, or 2 stop bits for a
                                    6-,7-,or 8-bit character. The receiver checks the first stop bit
                                    only regardless of the number of stop bits selected.
1:0 Word Length W/R     Word Length
                                    00: 5 bits
                                    01: 6 bits
                                    10: 7 bits
                                    11: 8 bits

*/
/*address: 0x0C*/
struct uart_lcr_t   /*Line Control Register*/
{
    uint32_t word_len:2;
    uint32_t stop:1;
    uint32_t parity_enable:1;
    uint32_t even_enable:1;
    uint32_t sp:1;
    uint32_t bc:1;
    uint32_t divisor_access:1;
    uint32_t unused:24;
};

/*
Bit     Name        Type    Function Description
7:6     RSVD        00       These bits are always cleared (Reserved bits).
5       AFE            W/R     Auto Flow control Enable
                                        “1”: Enable AFE
                                        “0”: Disable AFE
4       LOOP         W/R      Loop Back Mode
                                        When set, this bit provides local loop back feature for
                                        diagnostic testing of the UART.
3       OUT2        W/R       User Designated Output
                                        “1”: set OUT2n to “0”, active
                                        “0”: set OUT2n to “1”, normal
2       OUT1        W/R       User Designated Output
                                        “1”: set OUT1n to “0”, active
                                        “0”: set OUT1n to “1”, normal
1       RTS          W/R        Request to Send
                                        “1”: set RTSn to “0”, active
                                        “0”: set RTSn to “1”, normal
0       DTR         W/R         Data Terminal Ready
                                        “1”: set DTRn to “0”, active
                                        “0”: set DTRn to “1”, normal
*/
/*address: 0x10*/
struct uart_mcr_t  /*Modem Control Register*/
{
    uint32_t dtr:1;
    uint32_t rts:1;
    uint32_t out1:1;
    uint32_t out2:1;
    uint32_t loop:1;
    uint32_t afe:1;
    uint32_t unused:26;
};
/*
Bit     Name        Type    Function Description
7       ERROR       RO      Error Flag
                                    In No-FIFO mode, this bit is always cleared. In FIFO mode,
                                    LSR[7] is set when there is at least. one parity, framing or
                                    break error in the FIFO. It is cleared when reads the LSR
                                    and there are no subsequent errors in the FIFO.
6       TEMT         RO       Transmitter Empty Indicator
                                    TEMT is set when the THR and TSR are both empty. When
                                    either the THR or the TSR contains a data character, TEMT
                                    is cleared. In the FIFO mode, TEMT is set when the
                                    transmitter FIFO and TSR are both empty.
5       THRE        RO       Transmitter Holding Register Empty Indicator
                                    THRE is set when the THR is empty. Indicating that ACE is
                                    ready to accept the new character. In FIFO mode THRE is
                                    set when transmitter FIFO is empty, it is cleared when at
                                    least one byte is written to the transmitter FIFO. In Non-
                                    FIFO mode THRE is set when contents of THR are
                                    transferred into TSR.THRE is cleared concurrent with the
                                    loading of THR by the CPU.
4       BI             RO       Break Interrupt Indicator
                                    When BI is set, it indicates that received data input was held
                                    in the low state for longer than the full transmission time. BI
                                    is cleared every time the CPU reads the contents of the LSR.
                                    In FIFO mode it indicates the BI status of the character on
                                    the top of the FIFO. When BI occurs, only one 0 character is
                                    loaded into the FIFO. The next character transfer is enabled
                                    after SIN goes to the marking state for at least two RCLK
                                    samples, and then receives the next valid start bit.
                                    Full Transmission Time = Start + Data + Parity + Stop bits
3       FE              RO      Framing Error indicator
                                    When FE is set it indicates that received character does not
                                    have a valid stop bit. FE is cleared every time CPU reads the
                                    contents of the LSR. In FIFO mode it indicates the FE status
                                    of the character on top of the FIFO. Under the condition, the
                                    UART will attempt to resynchronize by assuming the
                                    framing error was due to the next start bit, so it samples this
                                    start bit twice and then accepts the input data.
2       PE             RO       Parity Error
                                    When PE is set it indicates that parity of received
                                    character does not Match the parity selected in LCR[4]. PE
                                    is cleared every time the CPU reads the
                                    contents of LSR. In FIFO mode it indicates the parity of the
                                    character on the top of the FIFO.
1       OE            RO       Overrun Error Indicator
                                    When OE is set, it indicates that before the character in the
                                    RBR was read, it was overwritten by the next character
                                    transferred into the register. OE is cleared every time the
                                    CPU reads the contents of the LSR. If the FIFO mode data
                                    continues to fill the FIFO beyond the trigger level, an
                                    overrun error occurs only after the FIFO is full and the next
                                    character has been completely received in the shift register.
                                    An overrun error is indicated to the CPU as soon as it
                                    happens. The character in the shift register is overwritten,
                                    but it is not transferred to the FIFO.
0       DR           RO        Data Ready Indicator
                                    DR is set when the complete incoming character is
                                    received and transferred into RBR or the FIFO. It is cleared
                                    by reading all data in the FIFO or RBR.
*/
/*address: 0x14*/
struct uart_lsr_t  /*Line Status Register*/
{
    uint32_t data_ready:1;
    uint32_t overrun_err:1;
    uint32_t parity_err:1;
    uint32_t framing_err:1;
    uint32_t break_ind:1;
    uint32_t trans_holding_empty:1;
    uint32_t error:1;
    uint32_t unused2:24;
};

/*
Bit     Name    Type    Function Description
7       DCD     RO          Data Carrier Detect
                                    This bit is the complement of DCDn. When the UART is in
                                    the diagnostic test mode(LOOP),this bit is equal to the MCR
                                    bit 3(OUT2).
6       RI         RO         Ring Indicator
                                    This bit is the complement of Rin. When the UART is in
                                    diagnostic test mode, this bit is equal to MCR bit 2(RI).
5       DSR      RO         Data Send Request
                                    This is the inverted version of DSRn. When the UART is in
                                    diagnostic test node, this bit is equal to MCR bit 0(DSR).
4       CTS      RO         Clear To Send
                                    This bit is complement of ctsn. When UART is in diagnostic
                                    test mode, this bit is equal to MCR bit1.
3       DCD     RO          DCDn Changed
                                    This bit is change in data carrier detect indicator. It indicates
                                    that dcdn has changed states since it was last read by the
                                    CPU.
2       TERI    RO          RIn Rising Edge
                                    This bit is trailing edge of the ring indicator detector. TERIi
                                    indicates that RI to the chip has changed from low to a high
                                    level. When TERI is set and the modem status interrupt is
                                    enabled, a modem status interrupt is generated.
1       DSR     RO          DSRn Changed
                                    This bit is change in data set ready indicator. It indicates that
                                    dsrn has changed states since last time it was read by the
                                    CPU.
0       CTS     RO          CTSn Changed
                                    This bit is change in clear to send indicator. It indicates the
                                    ctsn has changed states since last time it was Read by the
                                    CPU.
*/
/*address: 0x18*/
struct uart_msr_t  /*Modem Status Register*/
{
    uint32_t cts_change:1;
    uint32_t dsr_change:1;
    uint32_t teri:1;
    uint32_t dcd_change:1;
    uint32_t cts:1;
    uint32_t dsr:1;
    uint32_t ri:1;
    uint32_t dcd:1;
    uint32_t unused:24;
};


/*address: 0x00*/
struct uart_dll_t  /*Divisor Latch LSB*/
{
    uint32_t data:8;
    uint32_t unused:24;
};
/*address: 0x04*/
struct uart_dlm_t  /*Divisor Latch MSB*/
{
    uint32_t data:8;
    uint32_t unused:24;
};


/*address: 0x00*/
union uart_data_dll_t
{

    uint32_t data;   /*low byte works (recv buffer and trans holding register)*/

    struct uart_dll_t dll;
};

/*address: 0x04*/
union uart_ier_dlm_t
{
    struct uart_ier_t ier;
    struct uart_dlm_t dlm;
};

/*address: 0x08*/
union uart_iir_fcr_t
{
    struct uart_iir_t iir;
    struct uart_fcr_t fcr;
};

struct uart_reg_t
{
    union uart_data_dll_t u1;/*data and Divisor Latch LSB*/
    union uart_ier_dlm_t u2;/*Interrupt Enable Register and Divisor Latch LSB*/
    union uart_iir_fcr_t u3;/*Interrupt Identification Register and FIFO Control Register*/

    struct uart_lcr_t lcr; /*Line Control Register*/
    struct uart_mcr_t mcr;/*Modem Control Register*/
    uint32_t lsr; /*Line Status Register*/
    struct uart_msr_t msr; /*Modem Status Register*/
    uint32_t scr;/*Scratch Register, low byte is available*/

};

#define UART_FIFO_SIZE    32

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

