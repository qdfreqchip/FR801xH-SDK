/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_PLF_H_
#define _DRIVER_PLF_H_

#include "compiler.h"

/* peripheral regsiters base address */
#define QSPI_DAC_ADDRESS    0x01000000
#define SYSTEM_REG_BASE     0x50000000
#define MODEM_BASE          0x50010000
#define TIMER_BASE          0x50020000
#define I2C0_BASE           0x50030000
#define I2C1_BASE           0x50038000
#define SSP0_BASE           0x50040000
#define UART0_BASE          0x50050000
#define UART1_BASE          0x50058000
#define GPIOAB_BASE         0x50060000
#define GPIOCD_BASE         0x50064000
#define EXTI_BASE           0x50068000
#define I2S_BASE            0x50070000
#define EFUSE_BASE          0x50080000
#define SAR_ADC_BASE        0x50090000
#define CACHE_BASE          0x500A0000
#define QSPI_APB_BASE       0x500B0000
#define TRNG_BASE           0x500C0000
#define PDM_BASE            0x500D0000
#define PWM_BASE            0x500E0000
#define FRSPIM_BASE         0x500F0000
#define TUBE_BASE           0x500F8888
#define APB2SPI_BASE        0x50010000

/* interrupt sequence number */
typedef enum IRQn
{
    /******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
    NonMaskableInt_IRQn           = -14,    /*!< 2 Non Maskable Interrupt                           */
    HardFault_IRQn                = -13,    /*!< 3 Cortex-M3 Hard Fault Interrupt                   */
    SVCall_IRQn                   = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                     */
    PendSV_IRQn                   = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                     */
    SysTick_IRQn                  = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                 */

    /******  CMSDK Specific Interrupt Numbers *******************************************************/
    BLE_IRQn                      = 0,
    TIMER0_IRQn                   = 1,
    TIMER1_IRQn                   = 2,
    UART0_IRQn                    = 3,
    UART1_IRQn                    = 4,
    SSP_IRQn                      = 5,
    IIC0_IRQn                     = 6,
    IIC1_IRQn                     = 7,
    PTC_IRQn                      = 8,
    I2S_IRQn                      = 9,
    EXTI_IRQn                     = 10,
    PMU_IRQn                      = 11,
    ADC_IRQn                      = 12,
    PDM_IRQn                      = 13,
    FC_IRQn                       = 14,
    QSPI_IRQn                     = 15,
} IRQn_Type;

/* Configuration of the Cortex-M3 Processor and Core Peripherals */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          3         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */

/* Macro to read a platform register */
#define REG_PL_RD(addr)              (*(volatile uint32_t *)(addr))

/* Macro to write a platform register */
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)

/* definations and functions relatived MCU dependence */
#include "core_cm3.h"
#include "ll.h"

/// @} PLF

#endif // _PLF_H_

