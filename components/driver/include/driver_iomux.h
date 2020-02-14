/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef DRIVER_IOMUX_H
#define DRIVER_IOMUX_H

/**********************************************************************************************************************************
PX/MUX   4'h0        4'h1         4'h2        4'h3    4'h4         4'h5	        4'h6	     4'h7	    4'h8	    4'ha
PORTA0   gpio_a0     I2C0_CLK     I2S_CLK     PWM0    SSP0_CLK     UART0_RXD    UART1_RXD    CLK_OUT    PDM_CLK     
PORTA1   gpio_a1     I2C0_DAT     I2S_FRM     PWM1    SSP0_CSN     UART0_TXD    UART1_TXD    ant_ctl[0]	PDM_DATA    
PORTA2   gpio_a2     I2C1_CLK     I2S_DOUT    PWM2    SSP0_DOUT    UART0_RXD    UART1_RXD    ant_ctl[0]	PDM_CLK     
PORTA3   gpio_a3     I2C1_DAT     I2S_DIN     PWM3    SSP0_DIN     UART0_TXD    UART1_TXD    ant_ctl[1]	PDM_DATA    
PORTA4   gpio_a4     I2C0_CLK     I2S_CLK     PWM4    SSP0_CLK     UART0_RXD    UART1_RXD    CLK_OUT    PDM_CLK     
PORTA5   gpio_a5     I2C0_DAT     I2S_FRM     PWM5    SSP0_CSN     UART0_TXD    UART1_TXD    ant_ctl[1]	PDM_DATA    
PORTA6   gpio_a6     I2C1_CLK     I2S_DOUT    PWM0    SSP0_DOUT    UART0_RXD    UART1_RXD    CLK_OUT    PDM_CLK     
PORTA7   gpio_a7     I2C1_DAT     I2S_DIN     PWM1    SSP0_DIN     UART0_TXD    UART1_TXD    ant_ctl[0]	PDM_DATA    

PORTB0   gpio_b0     I2C0_CLK     I2S_CLK     PWM0    SSP0_CLK     UART0_RXD    UART1_RXD    ble_tx     PDM_CLK     
PORTB1   gpio_b1     I2C0_DAT     I2S_FRM     PWM1    SSP0_CSN     UART0_TXD    UART1_TXD    ble_rx     PDM_DATA    
PORTB2   gpio_b2     I2C1_CLK     I2S_DOUT    PWM2    SSP0_DOUT    UART0_RXD    UART1_RXD    wlan_tx/in PDM_CLK     
PORTB3   gpio_b3     I2C1_DAT     I2S_DIN     PWM3    SSP0_DIN     UART0_TXD    UART1_TXD    wlan_rx/in PDM_DATA    
PORTB4   gpio_b4     I2C0_CLK     I2S_CLK     PWM4    SSP0_CLK     UART0_RXD    UART1_RXD    CLK_OUT    PDM_CLK     
PORTB5   gpio_b5     I2C0_DAT     I2S_FRM     PWM5    SSP0_CSN     UART0_TXD    UART1_TXD    ant_ctl[0] PDM_DATA    
PORTB6   gpio_b6     I2C1_CLK     I2S_DOUT    PWM2    SSP0_DOUT    UART0_RXD    UART1_RXD    ant_ctl[1] PDM_CLK     
PORTB7   gpio_b7     I2C1_DAT     I2S_DIN     PWM3    SSP0_DIN     UART0_TXD    UART1_TXD    CLK_OUT    PDM_DATA    

PORTC0   gpio_c0     I2C0_CLK     I2S_CLK     PWM0    SSP0_CLK     UART0_RXD    UART1_RXD    SWV        PDM_CLK     
PORTC1   gpio_c1     I2C0_DAT     I2S_FRM     PWM1    SSP0_CSN     UART0_TXD    UART1_TXD    SWV        PDM_DATA    
PORTC2   gpio_c2     I2C1_CLK     I2S_DOUT    PWM2    SSP0_DOUT    UART0_RXD    UART1_RXD    SWV        PDM_CLK     
PORTC3   gpio_c3     I2C1_DAT     I2S_DIN     PWM3    SSP0_DIN     UART0_TXD    UART1_TXD    SWV        PDM_DATA    
PORTC4   gpio_c4     I2C0_CLK     I2S_CLK     PWM4    SSP0_CLK     UART0_RXD    UART1_RXD    ant_ctl[1] PDM_CLK     
PORTC5   gpio_c5     I2C0_DAT     I2S_FRM     PWM5    SSP0_CSN     UART0_TXD    UART1_TXD    SWV        PDM_DATA    
PORTC6   gpio_c6     I2C1_CLK     I2S_DOUT    PWM4    SSP0_DOUT    UART0_RXD    UART1_RXD    SW_TCK     PDM_CLK     
PORTC7   gpio_c7     I2C1_DAT     I2S_DIN     PWM5    SSP0_DIN     UART0_TXD    UART1_TXD    SW_DIO     PDM_DATA    

PORTD0   gpio_d0     I2C0_CLK     I2S_CLK     PWM0    SSP0_CLK     UART0_RXD    UART1_RXD    ble_tx     PDM_CLK     
PORTD1   gpio_d1     I2C0_DAT     I2S_FRM     PWM1    SSP0_CSN     UART0_TXD    UART1_TXD    ble_rx     PDM_DATA    
PORTD2   gpio_d2     I2C1_CLK     I2S_DOUT    PWM2    SSP0_DOUT    UART0_RXD    UART1_RXD    wlan_tx/in PDM_CLK     
PORTD3   gpio_d3     I2C1_DAT     I2S_DIN     PWM3    SSP0_DIN     UART0_TXD    UART1_TXD    wlan_rx/in PDM_DATA    
PORTD4   gpio_d4     I2C0_CLK     I2S_CLK     PWM4    SSP0_CLK     UART0_RXD    UART1_RXD    ant_ctl[0] PDM_CLK     ADC0
PORTD5   gpio_d5     I2C0_DAT     I2S_FRM     PWM5    SSP0_CSN     UART0_TXD    UART1_TXD    ant_ctl[0] PDM_DATA    ADC1
PORTD6   gpio_d6     I2C1_CLK     I2S_DOUT    PWM0    SSP0_DOUT    UART0_RXD    UART1_RXD    CLK_OUT    PDM_CLK     ADC2
PORTD7   gpio_d7     I2C1_DAT     I2S_DIN     PWM1    SSP0_DIN     UART0_TXD    UART1_TXD    ant_ctl[1] PDM_DATA    ADC3

*************************************************************************************************************************************/

#define PORTA0_FUNC_A0              0x00
#define PORTA0_FUNC_I2C0_CLK        0x01
#define PORTA0_FUNC_I2S_CLK         0x02
#define PORTA0_FUNC_PWM0            0x03
#define PORTA0_FUNC_SSP0_CLK        0x04
#define PORTA0_FUNC_UART0_RXD       0x05
#define PORTA0_FUNC_UART1_RXD       0x06
#define PORTA0_FUNC_CLK_OUT         0x07
#define PORTA0_FUNC_PDM_CLK         0x08

#define PORTA1_FUNC_A1              0x00
#define PORTA1_FUNC_I2C0_DAT        0x01
#define PORTA1_FUNC_I2S_FRM         0x02
#define PORTA1_FUNC_PWM1            0x03
#define PORTA1_FUNC_SSP0_CSN        0x04
#define PORTA1_FUNC_UART0_TXD       0x05
#define PORTA1_FUNC_UART1_TXD       0x06
#define PORTA1_FUNC_ANT_CTL0        0x07
#define PORTA1_FUNC_PDM_DAT         0x08

#define PORTA2_FUNC_A2              0x00
#define PORTA2_FUNC_I2C1_CLK        0x01
#define PORTA2_FUNC_I2S_DOUT        0x02
#define PORTA2_FUNC_PWM2            0x03
#define PORTA2_FUNC_SSP0_DOUT       0x04
#define PORTA2_FUNC_UART0_RXD       0x05
#define PORTA2_FUNC_UART1_RXD       0x06
#define PORTA2_FUNC_ANT_CTL0        0x07
#define PORTA2_FUNC_PDM_CLK         0x08

#define PORTA3_FUNC_A3              0x00
#define PORTA3_FUNC_I2C1_DAT        0x01
#define PORTA3_FUNC_I2S_DIN         0x02
#define PORTA3_FUNC_PWM3            0x03
#define PORTA3_FUNC_SSP0_DIN        0x04
#define PORTA3_FUNC_UART0_TXD       0x05
#define PORTA3_FUNC_UART1_TXD       0x06
#define PORTA3_FUNC_ANT_CTL1        0x07
#define PORTA3_FUNC_PDM_DAT         0x08

#define PORTA4_FUNC_A4              0x00
#define PORTA4_FUNC_I2C0_CLK        0x01
#define PORTA4_FUNC_I2S_CLK         0x02
#define PORTA4_FUNC_PWM4            0x03
#define PORTA4_FUNC_SSP0_CLK        0x04
#define PORTA4_FUNC_UART0_RXD       0x05
#define PORTA4_FUNC_UART1_RXD       0x06
#define PORTA4_FUNC_CLK_OUT         0x07
#define PORTA4_FUNC_PDM_CLK         0x08

#define PORTA5_FUNC_A5              0x00
#define PORTA5_FUNC_I2C0_DAT        0x01
#define PORTA5_FUNC_I2S_FRM         0x02
#define PORTA5_FUNC_PWM5            0x03
#define PORTA5_FUNC_SSP0_CSN        0x04
#define PORTA5_FUNC_UART0_TXD       0x05
#define PORTA5_FUNC_UART1_TXD       0x06
#define PORTA5_FUNC_ANT_CTL1        0x07
#define PORTA5_FUNC_PDM_DAT         0x08

#define PORTA6_FUNC_A6              0x00
#define PORTA6_FUNC_I2C1_CLK        0x01
#define PORTA6_FUNC_I2S_DOUT        0x02
#define PORTA6_FUNC_PWM0            0x03
#define PORTA6_FUNC_SSP0_DOUT       0x04
#define PORTA6_FUNC_UART0_RXD       0x05
#define PORTA6_FUNC_UART1_RXD       0x06
#define PORTA6_FUNC_CLK_OUT         0x07
#define PORTA6_FUNC_PDM_CLK         0x08

#define PORTA7_FUNC_A7              0x00
#define PORTA7_FUNC_I2C1_DAT        0x01
#define PORTA7_FUNC_I2S_DIN         0x02
#define PORTA7_FUNC_PWM1            0x03
#define PORTA7_FUNC_SSP0_DIN        0x04
#define PORTA7_FUNC_UART0_TXD       0x05
#define PORTA7_FUNC_UART1_TXD       0x06
#define PORTA7_FUNC_ANT_CTL0        0x07
#define PORTA7_FUNC_PDM_DAT         0x08

#define PORTB0_FUNC_B0              0x00
#define PORTB0_FUNC_I2C0_CLK        0x01
#define PORTB0_FUNC_I2S_CLK         0x02
#define PORTB0_FUNC_PWM0            0x03
#define PORTB0_FUNC_SSP0_CLK        0x04
#define PORTB0_FUNC_UART0_RXD       0x05
#define PORTB0_FUNC_UART1_RXD       0x06
#define PORTB0_FUNC_BLE_TX          0x07
#define PORTB0_FUNC_PDM_CLK         0x08

#define PORTB1_FUNC_B1              0x00
#define PORTB1_FUNC_I2C0_DAT        0x01
#define PORTB1_FUNC_PWM1            0x03
#define PORTB1_FUNC_SSP0_CSN        0x04
#define PORTB1_FUNC_UART0_TXD       0x05
#define PORTB1_FUNC_UART1_TXD       0x06
#define PORTB1_FUNC_BLE_RX          0x07
#define PORTB1_FUNC_PDM_DAT         0x08

#define PORTB2_FUNC_B2              0x00
#define PORTB2_FUNC_I2C1_CLK        0x01
#define PORTB2_FUNC_I2S_DOUT        0x02
#define PORTB2_FUNC_PWM2            0x03
#define PORTB2_FUNC_SSP0_DOUT       0x04
#define PORTB2_FUNC_UART0_RXD       0x05
#define PORTB2_FUNC_UART1_RXD       0x06
#define PORTB2_FUNC_WLAN_TX         0x07
#define PORTB2_FUNC_PDM_CLK         0x08

#define PORTB3_FUNC_B3              0x00
#define PORTB3_FUNC_I2C1_DAT        0x01
#define PORTB3_FUNC_I2S_DIN         0x02
#define PORTB3_FUNC_PWM3            0x03
#define PORTB3_FUNC_SSP0_DIN        0x04
#define PORTB3_FUNC_UART0_TXD       0x05
#define PORTB3_FUNC_UART1_TXD       0x06
#define PORTB3_FUNC_WLAN_RX         0x07
#define PORTB3_FUNC_PDM_DAT         0x08

#define PORTB4_FUNC_B4              0x00
#define PORTB4_FUNC_I2C0_CLK        0x01
#define PORTB4_FUNC_I2S_CLK         0x02
#define PORTB4_FUNC_PWM4            0x03
#define PORTB4_FUNC_SSP0_CLK        0x04
#define PORTB4_FUNC_UART0_RXD       0x05
#define PORTB4_FUNC_UART1_RXD       0x06
#define PORTB4_FUNC_CLK_OUT         0x07
#define PORTB4_FUNC_PDM_CLK         0x08

#define PORTB5_FUNC_B5              0x00
#define PORTB5_FUNC_I2C0_DAT        0x01
#define PORTB5_FUNC_I2S_FRM         0x02
#define PORTB5_FUNC_PWM5            0x03
#define PORTB5_FUNC_SSP0_CSN        0x04
#define PORTB5_FUNC_UART0_TXD       0x05
#define PORTB5_FUNC_UART1_TXD       0x06
#define PORTB5_FUNC_ANT_CTL0        0x07
#define PORTB5_FUNC_PDM_DAT         0x08

#define PORTB6_FUNC_B6              0x00
#define PORTB6_FUNC_I2C1_CLK        0x01
#define PORTB6_FUNC_I2S_DOUT        0x02
#define PORTB6_FUNC_PWM2            0x03
#define PORTB6_FUNC_SSP0_DOUT       0x04
#define PORTB6_FUNC_UART0_RXD       0x05
#define PORTB6_FUNC_UART1_RXD       0x06
#define PORTB6_FUNC_ANT_CTL1        0x07
#define PORTB6_FUNC_PDM_CLK         0x08

#define PORTB7_FUNC_B7              0x00
#define PORTB7_FUNC_I2C1_DAT        0x01
#define PORTB7_FUNC_I2S_DIN         0x02
#define PORTB7_FUNC_PWM3            0x03
#define PORTB7_FUNC_SSP0_DIN        0x04
#define PORTB7_FUNC_UART0_TXD       0x05
#define PORTB7_FUNC_UART1_TXD       0x06
#define PORTB7_FUNC_CLK_OUT         0x07
#define PORTB7_FUNC_PDM_DAT         0x08

#define PORTC0_FUNC_C0              0x00
#define PORTC0_FUNC_I2C0_CLK        0x01
#define PORTC0_FUNC_I2S_CLK         0x02
#define PORTC0_FUNC_PWM0            0x03
#define PORTC0_FUNC_SSP0_CLK         0x04
#define PORTC0_FUNC_UART0_RXD       0x05
#define PORTC0_FUNC_UART1_RXD       0x06
#define PORTC0_FUNC_SWV             0x07
#define PORTC0_FUNC_PDM_CLK         0x08

#define PORTC1_FUNC_C1              0x00
#define PORTC1_FUNC_I2C0_DAT        0x01
#define PORTC1_FUNC_PWM1            0x03
#define PORTC1_FUNC_SSP0_CSN        0x04
#define PORTC1_FUNC_UART0_TXD       0x05
#define PORTC1_FUNC_UART1_TXD       0x06
#define PORTC1_FUNC_SWV             0x07
#define PORTC1_FUNC_PDM_DAT         0x08

#define PORTC2_FUNC_C2              0x00
#define PORTC2_FUNC_I2C1_CLK        0x01
#define PORTC2_FUNC_I2S_DOUT        0x02
#define PORTC2_FUNC_PWM2            0x03
#define PORTC2_FUNC_SSP0_DOUT       0x04
#define PORTC2_FUNC_UART0_RXD       0x05
#define PORTC2_FUNC_UART1_RXD       0x06
#define PORTC2_FUNC_SWV             0x07
#define PORTC2_FUNC_PDM_CLK         0x08

#define PORTC3_FUNC_C3              0x00
#define PORTC3_FUNC_I2C1_DAT        0x01
#define PORTC3_FUNC_I2S_DIN         0x02
#define PORTC3_FUNC_PWM3            0x03
#define PORTC3_FUNC_SSP0_DIN        0x04
#define PORTC3_FUNC_UART0_TXD       0x05
#define PORTC3_FUNC_UART1_TXD       0x06
#define PORTC3_FUNC_SWV             0x07
#define PORTC3_FUNC_PDM_DAT         0x08

#define PORTC4_FUNC_C4              0x00
#define PORTC4_FUNC_I2C0_CLK        0x01
#define PORTC4_FUNC_I2S_CLK         0x02
#define PORTC4_FUNC_PWM4            0x03
#define PORTC4_FUNC_SSP0_CLK        0x04
#define PORTC4_FUNC_UART0_RXD       0x05
#define PORTC4_FUNC_UART1_RXD       0x06
#define PORTC4_FUNC_ANT_CTL1        0x07
#define PORTC4_FUNC_PDM_CLK         0x08

#define PORTC5_FUNC_C5              0x00
#define PORTC5_FUNC_I2C0_DAT        0x01
#define PORTC5_FUNC_I2S_FRM         0x02
#define PORTC5_FUNC_PWM5            0x03
#define PORTC5_FUNC_SSP0_CSN        0x04
#define PORTC5_FUNC_UART0_TXD       0x05
#define PORTC5_FUNC_UART1_TXD       0x06
#define PORTC5_FUNC_SWV             0x07
#define PORTC5_FUNC_PDM_DAT         0x08

#define PORTC6_FUNC_C6              0x00
#define PORTC6_FUNC_I2C1_CLK        0x01
#define PORTC6_FUNC_I2S_DOUT        0x02
#define PORTC6_FUNC_PWM4            0x03
#define PORTC6_FUNC_SSP0_DOUT       0x04
#define PORTC6_FUNC_UART0_RXD       0x05
#define PORTC6_FUNC_UART1_RXD       0x06
#define PORTC6_FUNC_SW_TCK          0x07
#define PORTC6_FUNC_PDM_CLK         0x08

#define PORTC7_FUNC_C7              0x00
#define PORTC7_FUNC_I2C1_DAT        0x01
#define PORTC7_FUNC_I2S_DIN         0x02
#define PORTC7_FUNC_PWM5            0x03
#define PORTC7_FUNC_SSP0_DIN        0x04
#define PORTC7_FUNC_UART0_TXD       0x05
#define PORTC7_FUNC_UART1_TXD       0x06
#define PORTC7_FUNC_SW_DIO          0x07
#define PORTC7_FUNC_PDM_DAT         0x08

#define PORTD0_FUNC_D0              0x00
#define PORTD0_FUNC_I2C0_CLK        0x01
#define PORTD0_FUNC_I2S_CLK         0x02
#define PORTD0_FUNC_PWM0            0x03
#define PORTD0_FUNC_SSP0_CLK        0x04
#define PORTD0_FUNC_UART0_RXD       0x05
#define PORTD0_FUNC_UART1_RXD       0x06
#define PORTD0_FUNC_BLE_TX          0x07
#define PORTD0_FUNC_PDM_CLK         0x08

#define PORTD1_FUNC_D1              0x00
#define PORTD1_FUNC_I2C0_DAT        0x01
#define PORTD1_FUNC_PWM1            0x03
#define PORTD1_FUNC_SSP0_CSN        0x04
#define PORTD1_FUNC_UART0_TXD       0x05
#define PORTD1_FUNC_UART1_TXD       0x06
#define PORTD1_FUNC_BLE_RX          0x07
#define PORTD1_FUNC_PDM_DAT         0x08

#define PORTD2_FUNC_D2              0x00
#define PORTD2_FUNC_I2C1_CLK        0x01
#define PORTD2_FUNC_I2S_DOUT        0x02
#define PORTD2_FUNC_PWM2            0x03
#define PORTD2_FUNC_SSP0_DOUT       0x04
#define PORTD2_FUNC_UART0_RXD       0x05
#define PORTD2_FUNC_UART1_RXD       0x06
#define PORTD2_FUNC_WLAN_TX         0x07
#define PORTD2_FUNC_PDM_CLK         0x08

#define PORTD3_FUNC_D3              0x00
#define PORTD3_FUNC_I2C1_DAT        0x01
#define PORTD3_FUNC_I2S_DIN         0x02
#define PORTD3_FUNC_PWM3            0x03
#define PORTD3_FUNC_SSP0_DIN        0x04
#define PORTD3_FUNC_UART0_TXD       0x05
#define PORTD3_FUNC_UART1_TXD       0x06
#define PORTD3_FUNC_WLAN_RX         0x07
#define PORTD3_FUNC_PDM_DAT         0x08

#define PORTD4_FUNC_D4              0x00
#define PORTD4_FUNC_I2C0_CLK        0x01
#define PORTD4_FUNC_I2S_CLK         0x02
#define PORTD4_FUNC_PWM4            0x03
#define PORTD4_FUNC_SSP0_CLK        0x04
#define PORTD4_FUNC_UART0_RXD       0x05
#define PORTD4_FUNC_UART1_RXD       0x06
#define PORTD4_FUNC_ANT_CTL0        0x07
#define PORTD4_FUNC_PDM_CLK         0x08
#define PORTD4_FUNC_ADC0            0x0c

#define PORTD5_FUNC_D5              0x00
#define PORTD5_FUNC_I2C0_DAT        0x01
#define PORTD5_FUNC_I2S_FRM         0x02
#define PORTD5_FUNC_PWM5            0x03
#define PORTD5_FUNC_SSP0_CSN        0x04
#define PORTD5_FUNC_UART0_TXD       0x05
#define PORTD5_FUNC_UART1_TXD       0x06
#define PORTD5_FUNC_ANT_CTL0        0x07
#define PORTD5_FUNC_PDM_DAT         0x08
#define PORTD5_FUNC_ADC1            0x0c

#define PORTD6_FUNC_D6              0x00
#define PORTD6_FUNC_I2C1_CLK        0x01
#define PORTD6_FUNC_I2S_DOUT        0x02
#define PORTD6_FUNC_PWM0            0x03
#define PORTD6_FUNC_SSP0_DOUT       0x04
#define PORTD6_FUNC_UART0_RXD       0x05
#define PORTD6_FUNC_UART1_RXD       0x06
#define PORTD6_FUNC_CLK_OUT         0x07
#define PORTD6_FUNC_PDM_CLK         0x08
#define PORTD6_FUNC_ADC2            0x0c

#define PORTD7_FUNC_D7              0x00
#define PORTD7_FUNC_I2C1_DAT        0x01
#define PORTD7_FUNC_I2S_DIN         0x02
#define PORTD7_FUNC_PWM1            0x03
#define PORTD7_FUNC_SSP0_DIN        0x04
#define PORTD7_FUNC_UART0_TXD       0x05
#define PORTD7_FUNC_UART1_TXD       0x06
#define PORTD7_FUNC_ANT_CTL1        0x07
#define PORTD7_FUNC_PDM_DAT         0x08
#define PORTD7_FUNC_ADC3            0x0c

enum pmu_gpio_mux_t
{
    PMU_PORT_MUX_GPIO,
    PMU_PORT_MUX_KEYSCAN,
    PMU_PORT_MUX_DIAG,
    PMU_PORT_MUX_PWM,
};

#define GPIO_PA0              (1<<0)
#define GPIO_PA1              (1<<1)
#define GPIO_PA2              (1<<2)
#define GPIO_PA3              (1<<3)
#define GPIO_PA4              (1<<4)
#define GPIO_PA5              (1<<5)
#define GPIO_PA6              (1<<6)
#define GPIO_PA7              (1<<7)

#define GPIO_PB0              (1<<8)
#define GPIO_PB1              (1<<9)
#define GPIO_PB2              (1<<10)
#define GPIO_PB3              (1<<11)
#define GPIO_PB4              (1<<12)
#define GPIO_PB5              (1<<13)
#define GPIO_PB6              (1<<14)
#define GPIO_PB7              (1<<15)

#define GPIO_PC0              (1<<16)
#define GPIO_PC1              (1<<17)
#define GPIO_PC2              (1<<18)
#define GPIO_PC3              (1<<19)
#define GPIO_PC4              (1<<20)
#define GPIO_PC5              (1<<21)
#define GPIO_PC6              (1<<22)
#define GPIO_PC7              (1<<23)

#define GPIO_PD0              (1<<24)
#define GPIO_PD1              (1<<25)
#define GPIO_PD2              (1<<26)
#define GPIO_PD3              (1<<27)
#define GPIO_PD4              (1<<28)
#define GPIO_PD5              (1<<29)
#define GPIO_PD6              (1<<30)
#define GPIO_PD7              ((uint32_t)1<<31)

enum system_port_t
{
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
};

enum system_port_bit_t
{
    GPIO_BIT_0,
    GPIO_BIT_1,
    GPIO_BIT_2,
    GPIO_BIT_3,
    GPIO_BIT_4,
    GPIO_BIT_5,
    GPIO_BIT_6,
    GPIO_BIT_7,
};

#define GPIO_DIR_IN             1
#define GPIO_DIR_OUT            0

#endif // DRIVER_IOMUX_H

