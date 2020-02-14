#ifndef _DRIVER_PMU_REGS_H
#define _DRIVER_PMU_REGS_H

#include "co_math.h"

#define PMU_REG_ADKEY_ALDO_CTRL     0x01

#define PMU_REG_ALDO_BG_CTRL        0x03

#define PMU_REG_BUCK_CTRL0          0x06

#define PMU_REG_BUCK_CTRL1          0x07

#define PMU_REG_RL_CTRL             0x08

#define PMU_REG_ANA_CLK_EN          0x09

#define PMU_REG_DLDO_CTRL           0x0d

#define PMU_REG_OSC_CAP_CTRL        0x10

#define PMU_REG_OSC32K_OTD_CTRL     0x14

#define PMU_REG_OTD_PKVDDH_CTRL     0x15

#define PMU_REG_PKVDD_CTRL          0x16

#define PMU_REG_PKVDD_CTRL2         0x17

#define PMU_REG_RCOSC_CTRL          0x1d

#define PMU_REG_ADC_CTRL0           0x1e
#define PMU_REG_ADC_BUF_TO_SAM_EN   (CO_BIT(5)) // buffer to sample core enable

#define PMU_REG_ADC_CTRL1           0x1f
#define PMU_REG_ADC_CLK_SEL_MSK     0x03
#define PMU_REG_ADC_DIV_TO_SAM_EN   (CO_BIT(3)) // dividor to sample core enable
#define PMU_REG_ADC_EN              (CO_BIT(4))
#define PMU_REG_ADC_IN_BUF_EN       (CO_BIT(5)) // input buffer enable

#define PMU_REG_ADC_CTRL2           0x20

#define PMU_REG_ADC_CTRL3           0x21
#define PMU_REG_ADC_DIV_EN          (CO_BIT(4)) // input dividor enable

#define PMU_REG_ADC_CTRL4           0x22
#define PMU_REG_ADC_DIV_RES_CTL_MSK 0x0f    /* dividor total resistor value control
                                               BIT0: 12*105
                                               BIT1: 12*415
                                               BIT2: 12*7.25K
                                               BIT3: 12*30K */
#define PMU_REG_ADC_DIV_CTL_MSK     0x30    /* 0:1/4; 1:1/3; 2:1/2; 3:2/3 */
#define PMU_REG_ADC_INT_REF_EN      (CO_BIT(6)) // internal reference enable
#define PMU_REG_ADC_PAD_TO_BUF_EN   (CO_BIT(7)) // pad to buffer renable

#define PMU_REG_ADC_CTRL5           0x23
#define PMU_REG_ADC_PAD_TO_DIV_EN   (CO_BIT(0)) // pad to dividor enable
#define PMU_REG_ADC_PAD_TO_SAM_EN   (CO_BIT(1)) // pad direct to sample core enable
#define PMU_REG_ADC_PAD_EN          (CO_BIT(2)) // sample from pad enable
#define PMU_REG_ADC_PD              (CO_BIT(7)) // ADC module power up

#define PMU_REG_ADC_CTRL6           0x24
#define PMU_REG_ADC_PD_CTL_PO       (CO_BIT(0)) // ADC module power is controlled by PMU_REG_ADC_PD or not
#define PMU_REG_ADC_PWR_EN          (CO_BIT(1)) // ADC module power enable
#define PMU_REG_ADC_PWR_SEL         (CO_BIT(2)) // ADC module AVDD select, 1: ALDO, 0: battery
#define PMU_REG_ADC_REF_SEL_MSK     0x30    /* reference selection
                                               0: internel reference
                                               2: power of ADC module */
#define PMU_REG_ADC_INT_REF_CFG     0xc0    /* internal reference configuration
                                               0:1.2v, 1:1.3v, 2:1.4v, 3:1.5v*/

#define PMU_REG_ADC_CTRL7           0x25

#define PMU_REG_ADC_CTRL8           0x26
#define PMU_REG_ADC_BAT_CTL         (CO_BIT(2))
#define PMU_REG_ADC_BAT_EN          (CO_BIT(3))
#define PMU_REG_ADC_BAT_TO_BUF_EN   (CO_BIT(4))
#define PMU_REG_ADC_BAT_TO_SAM_EN   (CO_BIT(5))
#define PMU_REG_ADC_VBE_TO_BUF_EN   (CO_BIT(6))

#define PMU_REG_PWR_CTRL            0x27

#define PMU_REG_FLASH_POR_CTRL      0x28

#define PMU_REG_SLEEP_CTRL          0x36
#define PMU_STANDBY_EN              (CO_BIT(0))
#define PMU_SHUTDOWN_EN             (CO_BIT(1))
#define PMU_SHUTDOWN_MODE1_EN       (CO_BIT(2))   // 0: shutdown gpio without power, 1: shutdown gpio with power
#define PMU_OOL_CLK_DIV_MSK         0xF0

#define PMU_REG_CLK_CONFIG          0x37
#define PMU_CLK_ADC_OSCDIV_MSK      0x0F    // 24M clock for SARADC
#define PMU_CLK_ADC_OSCDIV_POS      0
#define PMU_CLK_ADC_RCDIV_MSK       0x30    // 62.5 clock for SARADC
#define PMU_CLK_ADC_RCDIV_POS       4
#define PMU_SYS_CLK_SEL_MSK         0xc0
#define PMU_SYS_CLK_SEL_POS         6
enum PMU_SYS_CLK_T
{
    PMU_SYS_CLK_RC,
    PMU_SYS_CLK_RC_DIV,
    PMU_SYS_CLK_32768,
    PMU_SYS_CLK_32K,
};

#define PMU_REG_ON_OFF_TIMING       0x38
#define PMU_ON_TIMING_MSK           0x0f
#define PMU_ON_TIMING_POS           0
#define PMU_OFF_TIMING_MSK          0xf0
#define PMU_OFF_TIMING_POS          4

#define PMU_REG_OOL_MISC            0x39
#define PMU_ONKEY_TIMING_MSK        0x07
#define PMU_ONKEY_TIMING_POS        0
enum PMU_TONKEY_T
{
    PMU_TONKEY_3S,
    PMU_TONKEY_4S,
    PMU_TONKEY_5S,
    PMU_TONKEY_0_1S,
    PMU_TONKEY_1S,
    PMU_TONKEY_1_5S,
    PMU_TONKEY_2S,
    PMU_TONKEY_0_0_8S,
};
#define PMU_IRQ_TIMING_MSK          0x18
#define PMU_IRQ_TIMING_POS          3
#define PMU_ONKEY_OOL_MODE          (CO_BIT(5))     // on off logic mode: 1--switch, 0--press
enum PMU_ON_OFF_MODE_T
{
    PMU_ON_OFF_MODE_PRESS,
    PMU_ON_OFF_MODE_SWITCH,
};
#define PMU_FIRST_POWER_CHECK_MSK   0xc0
#define PMU_FIRST_POWER_CHECK_POS   6

#define PMU_REG_ISR_ENABLE          0x3a
#define PMU_ISR_BAT_EN              (CO_BIT(0))
#define PMU_ISR_ULV_PO_EN           (CO_BIT(1))     // ultra low voltage power off
#define PMU_ISR_LVD_EN              (CO_BIT(2))
#define PMU_ISR_OTP_EN              (CO_BIT(3))     //�¶ȹ���
#define PMU_ISR_ACOK_EN             (CO_BIT(4))     //Charger insert 
#define PMU_ISR_CALI_EN             (CO_BIT(5))
#define PMU_ISR_ACOFF_EN            (CO_BIT(6))     //Charger remove
#define PMU_ISR_ONKEY_PO_EN         (CO_BIT(7))     //�Ƿ�ʹ��onkeyӲ�������ػ��жϣ��������������ػ��Ļ���һλ�������?
#define PMU_REG_ISR_CLR             0x3b
//#define PMU_REG_ISR_CLR_1           0x3c
#define PMU_ISR_BAT_CLR             CO_BIT(0)
#define PMU_ISR_POWER_OFF_CLR       CO_BIT(1)   // clear ULV_PO and ONKEY_PO
#define PMU_ISR_LVD_CLR             CO_BIT(2)
#define PMU_ISR_OTD_CLR             CO_BIT(3)   //�¶ȹ���
#define PMU_ISR_ACOK_CLR            CO_BIT(4)
#define PMU_ISR_CALI_CLR            CO_BIT(5)
#define PMU_ISR_ACOFF_CLR           CO_BIT(6)
#define PMU_ISR_KEYSCAN_CLR         CO_BIT(7)
#define PMU_ISR_RTC_ALARM_A_CLR     CO_BIT(8)
#define PMU_ISR_RTC_ALARM_B_CLR     CO_BIT(9)
#define PMU_ISR_WDT_CLR             CO_BIT(10)
#define PMU_ISR_ADKEY0_CLR          CO_BIT(11)
#define PMU_ISR_ADKEY1_CLR          CO_BIT(12)
#define PMU_ISR_ONKEY_CLR           CO_BIT(13)
#define PMU_ISR_GPIO_CLR            CO_BIT(14)
#define PMU_ISR_QDEC_CLR            CO_BIT(15)

#define PMU_REG_RTC_SETTINGS        0x3d
#define PMU_RTC_EN                  CO_BIT(0)
#define PMU_RTC_UPDATE_EN           CO_BIT(1)
#define PMU_RTC_ALARM_A_EN          CO_BIT(2)
#define PMU_RTC_ALARM_B_EN          CO_BIT(3)
#define PMU_RTC_ALMA_INT_EN         CO_BIT(4)
#define PMU_RTC_ALMB_INT_EN         CO_BIT(5)
#define PMU_RTC_ALMA_PO_EN          CO_BIT(6)   //rtc alarm a power on
#define PMU_RTC_ALMB_PO_EN          CO_BIT(7)

#define PMU_REG_RTC_UPDATE_0        0x3e
#define PMU_REG_RTC_UPDATE_1        0x3f
#define PMU_REG_RTC_UPDATE_2        0x40
#define PMU_REG_RTC_UPDATE_3        0x41

#define PMU_REG_RTC_ALMA_VALUE_0    0x42
#define PMU_REG_RTC_ALMA_VALUE_1    0x43
#define PMU_REG_RTC_ALMA_VALUE_2    0x44
#define PMU_REG_RTC_ALMA_VALUE_3    0x45

#define PMU_REG_RTC_ALMB_VALUE_0    0x46
#define PMU_REG_RTC_ALMB_VALUE_1    0x47
#define PMU_REG_RTC_ALMB_VALUE_2    0x48
#define PMU_REG_RTC_ALMB_VALUE_3    0x49

#define PMU_REG_OOL_CFG             0x4a
#define PMU_OOL_SW_OFF_SEL          CO_BIT(0)   // 0: from pmu controller shut down en, 1: reg3c[2]
#define PMU_OOL_HD_OFF_EN           CO_BIT(1)
#define PMU_OOL_SW_OFF_BIT          CO_BIT(2)
#define PMU_IRQ_HD_GATED            CO_BIT(3)
#define PMU_SLP_MODE_1_ENN          CO_BIT(4)
#define PMU_ONKEY_CLEAR             CO_BIT(5)   // avalible in switch mode
#define PMU_ONKEY_POWON_EN          CO_BIT(7)

#define PMU_REG_BT_SLP_CTRL         0x4b
#define PMU_BT_OSC_SLP_EN           (CO_BIT(0))
#define PMU_BT_EXTWKUP_DSB          (CO_BIT(1))
#define PMU_BT_CTRL_PMU_EN          (CO_BIT(2))
#define PMU_BT_WKUP_LP_REG          (CO_BIT(3))
#define PMU_BT_WKUP_CORE_EN         (CO_BIT(4))
#define PMU_BT_WKUP_PMU_EN          (CO_BIT(5))
#define PMU_BT_SLPTIME_RD           (CO_BIT(6))
#define PMU_BT_RETTIME_RD           (CO_BIT(7))

#define PMU_REG_SLP_VAL_0           0x4c
#define PMU_REG_SLP_VAL_1           0x4d
#define PMU_REG_SLP_VAL_2           0x4e
#define PMU_REG_SLP_VAL_3           0x4f

#define PMU_REG_TWEXT_0             0x50
#define PMU_REG_TWEXT_1             0x51
#define PMU_REG_TWOSC_0             0x52
#define PMU_REG_TWOSC_1             0x53

/*
 * 1. control the wakeup source of bt timer
 * 2. GPIO MONITOR enable
 * 3. debounce clock divider configuartion
 */
#define PMU_REG_BT_WKUP_CTRL        0x54
#define PMU_BT_WKUP_IRQ_EN          (CO_BIT(0))     // direct mode
#define PMU_BT_WKUP_OSCMASK         (CO_BIT(1))     // 0: enable protect mode in sleep mode, 1: disable
#define PMU_GPIO_MONITOR_EN         (CO_BIT(2))     // enable gpio status compare with gpio last in reg PMU_REG_PORTA_LAST
#define PMU_DEB_CLK_DIV_POS         3               // base clock is pmu system clock
#define PMU_DEB_CLK_DIV_MSK         0xf8            // freq = pmu_sys_clk / ((N+1)*2)

#define PMU_REG_CALI_CTRL           0x55
#define PMU_CALI_EN                 CO_BIT(0)
#define PMU_CALI_SRC_SEL_POS        1
#define PMU_CALI_SRC_SEL_MASK       0x0e
enum pmu_cali_src_sel_t
{
    PMU_CALI_SRC_LP_RC,     // 62.5K
    PMU_CALI_SRC_RF,        // 2M
    PMU_CALI_SRC_OSC_DIV,   // 31.25K
    PMU_CALI_SRC_EXT_OSC,   // 32768
    PMU_CALI_SRC_LED3,
    PMU_CALI_SRC_D5,
    PMU_CALI_SRC_D6,
    PMU_CALI_SRC_D7,
};

#define PMU_REG_RST_CTRL            0x56
#define PMU_RST_EXT_PIN_EN          CO_BIT(0)   // enable ext pin reset pmu
#define PMU_RST_WDT_EN              CO_BIT(1)   // enable watchdog reset pmu
#define PMU_RST_EXT_PIN_DCORE_EN    CO_BIT(2)   // enable ext pin reset dcore
#define PMU_RST_SLP_TIMER           CO_BIT(3)   // reset bt sleep timer
#define PMU_RST_RTC                 CO_BIT(4)   // reset rtc
#define PMU_RST_KEYSCAN             CO_BIT(5)   // reset keyscan
#define PMU_RST_PWM                 CO_BIT(6)   // reset pwm
#define PMU_RST_CALIBRATION         CO_BIT(7)   // reset calibration

#define PMU_REG_CLK_CTRL            0x57
#define PMU_SARADC_HIGH_CLK         CO_BIT(0)
#define PMU_SARADC_LOW_CLK          CO_BIT(1)
#define PMU_KEYSCAN_CLK_EN          CO_BIT(2)
#define PMU_PTC_CLK_EN              CO_BIT(3)
#define PMU_CALI_CLK_EN             CO_BIT(4)
#define PMU_RTC_CLK_EN              CO_BIT(5)
#define PMU_BT_TIMER_CLK_EN         CO_BIT(6)
#define PMU_DEB_CLK_EN              CO_BIT(7)

#define PMU_REG_PORTA_SEL           0x58
#define PMU_REG_PORTB_SEL           0x59
#define PMU_REG_PORTC_SEL           0x5a
#define PMU_REG_PORTD_SEL           0x5b

#define PMU_REG_GPIO_PDVDD_ON       0x5c
#define PMU_REG_GPIO_PKVDDH_OFF     0x5d

#define PMU_REG_PORTA_OEN           0x60
#define PMU_REG_PORTB_OEN           0x61
#define PMU_REG_PORTC_OEN           0x62
#define PMU_REG_PORTD_OEN           0x63
#define PMU_REG_PORTA_PUL           0x64
#define PMU_REG_PORTB_PUL           0x65
#define PMU_REG_PORTC_PUL           0x66
#define PMU_REG_PORTD_PUL           0x67

#define PMU_REG_LVD_BAT_DEB_CFG     0x68
#define PMU_QDEC_AUTO_RST           CO_BIT(0)
#define PMU_BAT_DEB_SEL             CO_BIT(6)   // 1: new, 0: old(0xb5)
#define PMU_LVD_DEB_SEL             CO_BIT(7)   // 1: new, 0: old(0xb5)

#define PMU_REG_PORTA_TRIG_MASK     0x69
#define PMU_REG_PORTB_TRIG_MASK     0x6a
#define PMU_REG_PORTC_TRIG_MASK     0x6b
#define PMU_REG_PORTD_TRIG_MASK     0x6c

#define PMU_REG_ACOK_DEB_NEW_CFG    0x6d
#define PMU_ACOK_DEB_NEW_LEN_POS    0
#define PMU_ACOK_DEB_NEW_LEN_MSK    0x3f        // {N, 4'h9}
#define PMU_ACOK_DEB_NEW_SEL        CO_BIT(6)   // 1: new, 0: old(0xb5)

#define PMU_REG_CALI_CNT_L          0x6e
#define PMU_REG_CALI_CNT_H          0x6f

#define PMU_REG_ISR_STATUS          0x70
#define PMU_REG_ISR_STATUS_2        0x71
#define PMU_ISR_BAT_STATE           CO_BIT(0)
#define PMU_ISR_POWER_OFF_STATE     CO_BIT(1)     // for ulv_po and onkey_po
#define PMU_ISR_LVD_STATE           CO_BIT(2)
#define PMU_ISR_OTD_STATE           CO_BIT(3)
#define PMU_ISR_ACOK_STATE          CO_BIT(4)
#define PMU_ISR_CALI_STATE          CO_BIT(5)
#define PMU_ISR_ACOFF_STATE         CO_BIT(6)
#define PMU_ISR_KEYSCAN_STATE       CO_BIT(7)
#define PMU_ISR_ALARM_A_STATE       CO_BIT(8)
#define PMU_ISR_ALARM_B_STATE       CO_BIT(9)
#define PMU_ISR_WDT_STATE           CO_BIT(10)
#define PMU_ISR_ADKEY0_STATE        CO_BIT(11)
#define PMU_ISR_ADKEY1_STATE        CO_BIT(12)
#define PMU_ISR_ONKEY_STATE         CO_BIT(13)
#define PMU_ISR_GPIO_STATE          CO_BIT(14)
#define PMU_ISR_QDEC_STATE          CO_BIT(15)

#define PMU_REG_QDEC_MULTI_LEN      0x76

#define PMU_REG_ANA_RAW_STATUS      0x77
#define PMU_BAT_RAW_STATUS          CO_BIT(0)
#define PMU_BVDDA_GT_PO_RAW_STATUS  CO_BIT(1)
#define PMU_ULV_RAW_STATUS          CO_BIT(2)
#define PMU_OTD_RAW_STATUS          CO_BIT(3)
#define PMU_ACOK_RAW_STATUS         CO_BIT(4)
#define PMU_ONKEY_RAW_STATUS        CO_BIT(5)
#define PMU_ADKEY0_ON_RAW_STATUS    CO_BIT(6)
#define PMU_ADKEY1_ON_RAW_STATUS    CO_BIT(7)

#define PMU_REG_QDEC_CTRL           0x78
#define PMU_QDEC_FIRST_EDGE_DIS     CO_BIT(0)   // 1: disable first edge check
#define PMU_QDEC_SEC_EDGE_DIS       CO_BIT(1)   // 1: disable second edge check  
enum pmu_qdec_irq_type
{
    PMU_ISR_QDEC_SINGLE_EN = CO_BIT(2),
    PMU_ISR_QDEC_MULTI_EN = CO_BIT(3),
};
#define PMU_QDEC_DEB_LA_EN          CO_BIT(4)
#define PMU_QDEC_DEB_LB_EN          CO_BIT(5)
#define PMU_QDEC_DEB_LC_EN          CO_BIT(6)
#define PMU_QDEC_LC_RST_CNT_EN      CO_BIT(7)
#define PMU_ISR_QDEC_MASK           0x0C

#define PMU_REG_DEB_CFG             0x79    // debounce config
#define PMU_DEB_ONKEY_POS           0       // actual value = {N, 4'h9}
#define PMU_DEB_ONKEY_MSK           0x0f
#define PMU_DEB_ADKEY_POS           4       // actual value = {N, 4'h9}
#define PMU_DEB_ADKEY_MSK           0xf0

#define PMU_REG_ISR_ENABLE_2        0x7a
#define PMU_ISR_ONKEY_LOW_EN        CO_BIT(0)
#define PMU_ISR_ONKEY_HIGH_EN       CO_BIT(1)
#define PMU_ISR_ADKEY0_LOW_EN       CO_BIT(2)
#define PMU_ISR_ADKEY0_HIGH_EN      CO_BIT(3)
#define PMU_ISR_ADKEY1_LOW_EN       CO_BIT(4)
#define PMU_ISR_ADKEY1_HIGH_EN      CO_BIT(5)

#define PMU_REG_QDEC_DEB_LEN        0x7b    // unit: PMU_REG_BT_WKUP_CTRL[3:7]

#define PMU_REG_CALI_RESULT_0       0x7c
#define PMU_REG_CALI_RESULT_1       0x7d
#define PMU_REG_CALI_RESULT_2       0x7e
#define PMU_REG_CALI_RESULT_3       0x7f

/*
  COL --> OUTPUT
  COL[3:0] => PORTA[7:4]
  COL[7:4] => PORTB[7:4]
*/
#define PMU_REG_KEY_COL_SEL             0x80

/*
  ROW --> INPUT
  ROW[7:0]   => PORTC[7:0]
  ROW[15:8]  => PORTD[7:0]
  ROW[19:16] => PORTA[3:0]
*/
#define PMU_REG_KEY_ROW_SEL0            0x81
#define PMU_REG_KEY_ROW_SEL1            0x82
#define PMU_REG_KEY_ROW_SEL2            0x83

/*
  BIT[7:4] --> during clk counter for each column scan
  BIT[3:0] --> during clk between contiguous column scan
*/
#define PMU_REG_KEY_SCAN_LEN            0x84

/*
  BIT7 --> enable key scan
  BIT[3:0] --> generate interrupt after N times scan
*/
#define PMU_REG_KEY_SCAN_FNC            0x85
#define PMU_REG_KEY_SCAN_VAL            0x0f
#define PMU_REG_KEY_SCAN_H              CO_BIT(4)
#define PMU_REG_KEY_SCAN_MAP            CO_BIT(5)
#define PMU_REG_KEY_SCAN_EN             CO_BIT(7)

#define PMU_REG_WAKEUP_SRC              0x86
#define PMU_WAKEUP_SRC_B0               CO_BIT(0)   // this value should keep 1
#define PMU_WAKEUP_SRC_B1               CO_BIT(1)   // 1: wakeup signal route to pmu_ctrl, 0: to bt timer
#define PMU_ONKEY_WAKEUP_EN             CO_BIT(2)
#define PMU_WAKEUP_PROTECT_DIS          CO_BIT(3)
#define PMU_GPIO_WAKEUP_EN              CO_BIT(4)
#define PMU_GPIO_WAKEUP_SRC_CLEAR       CO_BIT(5)
#define PMU_IRQ_WAKEUP_EN               CO_BIT(6)
#define PMU_REG_WAKEUP_SRC_B7           CO_BIT(7)   // this value should keep 0

#define PMU_REG_MEM_ISO_EN_CTRL         0x88
#define PMU_REG_MEM_RET_CTRL            0x89

#define PMU_REG_WKUP_PWO_DLY            0x8b    // osc_en to digital release reset delay, N * pmu_sys_clock

#define PMU_REG_WKUP_PMUFSM_CHG_DLY     0x8d    // wakeup from sleep to normal delay, N * pmu_sys_clock

#define PMU_REG_ISO_CTRL                0x8f

// PTC control and cfg
#define PMU_REG_PTC0_HRC_0              0x90        // unit: rc clock
#define PMU_REG_PTC0_HRC_1              0x91
#define PMU_REG_PTC0_LRC_0              0x92
#define PMU_REG_PTC0_LRC_1              0x93
#define PMU_REG_PTC1_HRC_0              0x94
#define PMU_REG_PTC1_HRC_1              0x95
#define PMU_REG_PTC1_LRC_0              0x96
#define PMU_REG_PTC1_LRC_1              0x97
#define PMU_REG_PTC2_HRC_0              0x98
#define PMU_REG_PTC2_HRC_1              0x99
#define PMU_REG_PTC2_LRC_0              0x9a
#define PMU_REG_PTC2_LRC_1              0x9b
//#define PMU_REG_PTC3_HRC_0              0x9c
//#define PMU_REG_PTC3_HRC_1              0x9d
//#define PMU_REG_PTC3_LRC_0              0x9e
//#define PMU_REG_PTC3_LRC_1              0x9f

#define PMU_REG_SYSTEM_STATUS           0x9F //4f
#define PMU_SYS_PO_MAGIC                0x5A
#define PMU_SYS_WK_MAGIC                0xC3

#define PMU_REG_PORTA_LAST              0xa0
#define PMU_REG_PORTB_LAST              0xa1
#define PMU_REG_PORTC_LAST              0xa2
#define PMU_REG_PORTD_LAST              0xa3

#define PMU_REG_PTC0_CTRL               0xa4
#define PMU_REG_PTC1_CTRL               0xa5
#define PMU_REG_PTC2_CTRL               0xa6
#define PMU_REG_PTC3_CTRL               0xa7
#define PMU_PTC_EN                      CO_BIT(0)
#define PMU_PTC_OE                      CO_BIT(1)
#define PMU_PTC_SINGLE_MODE             CO_BIT(2)
#define PMU_PTC_CNT_RESET               CO_BIT(3)
#define PMU_PTC_OUTPUT_REVERT           CO_BIT(4)

#define PMU_REG_LED_PORT_DBG            0xa7
#define PMU_POATA_LED                   CO_BIT(0)
#define PMU_POATB_LED                   CO_BIT(1)
#define PMU_POATC_LED                   CO_BIT(2)
#define PMU_POATD_LED                   CO_BIT(3)
#define PMU_DBG                         CO_BIT(7)

#define PMU_REG_PORTA_MUX_L             0xa8
#define PMU_REG_PORTA_MUX_H             0xa9
#define PMU_PORT_MUX_MSK                0x3
#define PMU_PORT_MUX_LEN                2

#define PMU_REG_DIAG_SEL                0xaa    //[7:4] PORT_CD; [3:0] PORT_AB; 

#define PMU_REG_GPIOA_V                 0xac
#define PMU_REG_GPIOB_V                 0xad
#define PMU_REG_GPIOC_V                 0xae
#define PMU_REG_GPIOD_V                 0xaf

#define PMU_REG_WTD_LEN_0               0xb0
#define PMU_REG_WTD_LEN_1               0xb1
#define PMU_REG_WTD_LEN_2               0xb2

#define PMU_REG_WTD_CTRL                0xb3
#define PMU_WTD_EN                      CO_BIT(0)
#define PMU_WTD_IRQ_EN                  CO_BIT(1)
#define PMU_OOL_MISC_1_POS              4
#define PMU_OOL_MISC_1_MSK              0xf0

/* bit[7:4] oen, bit[3:0] out_value */
#define PMU_REG_LED_CTRL                0xb4

#define PMU_REG_OOL_DEB_CFG             0xb5
#define PMU_LVD_DEB_LEN_POS             0
#define PMU_LVD_DEB_LEN_MSK             0x03
enum pmu_lvd_deb_len_t      // N * pmu_sys_clk_perirod
{
    PMU_LVD_DEB_16000,
    PMU_LVD_DEB_32000,
    PMU_LVD_DEB_64000,
    PMU_LVD_DEB_128000,
};
#define PMU_ACOK_DEB_LEN_POS            2
#define PMU_ACOK_DEB_LEN_MSK            0x0c
enum pmu_acok_deb_len_t      // N * pmu_sys_clk_perirod
{
    PMU_ACOK_DEB_3200,
    PMU_ACOK_DEB_6400,
    PMU_ACOK_DEB_12800,
    PMU_ACOK_DEB_25600,
};
#define PMU_BAT_FULL_DEB_LEN_POS        4
#define PMU_BAT_FULL_DEB_LEN_MSK        0x30
enum pmu_bat_full_deb_len_t      // N * pmu_sys_clk_perirod
{
    PMU_BAT_FULL_DEB_16000,
    PMU_BAT_FULL_DEB_32000,
    PMU_BAT_FULL_DEB_64000,
    PMU_BAT_FULL_DEB_128000,
};
#define PMU_ACOK_DEB_EN                 CO_BIT(6)
#define PMU_BAT_FULL_DEB_EN             CO_BIT(7)

#define PMU_REG_PWR_OPTION              0xb6
#define PMU_PWR_BUCK_MODE               CO_BIT(0)   // 0: ldo mode, 1: buck mode
#define PMU_PWR_SWITCH_BUCK_MODE_EN     CO_BIT(4)
#define PMU_DEEP_SLEEP_STATE_OPTION     CO_BIT(6)
#define PMU_DEEP_SLEEP_ON_OPTION        CO_BIT(7)

#define PMU_REG_QDEC_CNTA_VALUE         0xb8
#define PMU_REG_QDEC_CNTB_VALUE         0xb9

#define PMU_REG_QDEC_PIN_MUX            0xb8
#define PMU_QDEC_LA_PIN_MUX_POS         0
#define PMU_QDEC_LA_PIN_MUX_MSK         0x0f
#define PMU_QDEC_LB_PIN_MUX_POS         4
#define PMU_QDEC_LB_PIN_MUX_MSK         0xf0
enum pmu_qdec_la_pin_t {
    PMU_QDEC_LA_PIN_PA0 = 1,
    PMU_QDEC_LA_PIN_PA2,
    PMU_QDEC_LA_PIN_PA4,
    PMU_QDEC_LA_PIN_PA6,
    PMU_QDEC_LA_PIN_PB0,
    PMU_QDEC_LA_PIN_PB2,
    PMU_QDEC_LA_PIN_PB4,
    PMU_QDEC_LA_PIN_PB6,
    PMU_QDEC_LA_PIN_PC0,
    PMU_QDEC_LA_PIN_PC2,
    PMU_QDEC_LA_PIN_PC4,
    PMU_QDEC_LA_PIN_PC6,
    PMU_QDEC_LA_PIN_PD0,
    PMU_QDEC_LA_PIN_PD2,
    PMU_QDEC_LA_PIN_PD4,
};

enum pmu_qdec_lb_pin_t {
    PMU_QDEC_LB_PIN_PA1 = 1,
    PMU_QDEC_LB_PIN_PA3,
    PMU_QDEC_LB_PIN_PA5,
    PMU_QDEC_LB_PIN_PA7,
    PMU_QDEC_LB_PIN_PB1,
    PMU_QDEC_LB_PIN_PB3,
    PMU_QDEC_LB_PIN_PB5,
    PMU_QDEC_LB_PIN_PB7,
    PMU_QDEC_LB_PIN_PC1,
    PMU_QDEC_LB_PIN_PC3,
    PMU_QDEC_LB_PIN_PC5,
    PMU_QDEC_LB_PIN_PC7,
    PMU_QDEC_LB_PIN_PD1,
    PMU_QDEC_LB_PIN_PD3,
    PMU_QDEC_LB_PIN_PD5,
};

#define PMU_REG_QDEC_LC_PIN_MUX         0xb9
#define PMU_QDEC_LC_PIN_MUX_POS         0
#define PMU_QDEC_LC_PIN_MUX_MSK         0x0f
enum pmu_qdec_lc_pin_t {
    PMU_QDEC_LC_PIN_PA2 = 1,
    PMU_QDEC_LC_PIN_PA5,
    PMU_QDEC_LC_PIN_PA7,
    PMU_QDEC_LC_PIN_PB2,
    PMU_QDEC_LC_PIN_PB5,
    PMU_QDEC_LC_PIN_PB7,
    PMU_QDEC_LC_PIN_PC2,
    PMU_QDEC_LC_PIN_PC5,
    PMU_QDEC_LC_PIN_PC7,
    PMU_QDEC_LC_PIN_PB0,
    PMU_QDEC_LC_PIN_PB4,
    PMU_QDEC_LC_PIN_PB6,
    PMU_QDEC_LC_PIN_PD5,
    PMU_QDEC_LC_PIN_PD6,
    PMU_QDEC_LC_PIN_PD7,
};

#define PMU_REG_NVDS_REGS_0             0xba
#define PMU_REG_NVDS_REGS_1             0xbb
#define PMU_REG_NVDS_REGS_2             0xbc
#define PMU_REG_NVDS_REGS_3             0xbd
#define PMU_REG_NVDS_REGS_4             0xbe
#define PMU_REG_NVDS_REGS_5             0xbf

#define PMU_REG_PORTB_MUX_L             0xc0
#define PMU_REG_PORTB_MUX_H             0xc1
#define PMU_REG_PORTC_MUX_L             0xc2
#define PMU_REG_PORTC_MUX_H             0xc3
#define PMU_REG_PORTD_MUX_L             0xc4
#define PMU_REG_PORTD_MUX_H             0xc5

#define PMU_REG_KEYSCAN_CTRL            0xc6
#define PMU_KEYSCAN_EN                  CO_BIT(0)
#define PMU_KEYSCAN_LP_EN               CO_BIT(1)
#define PMU_KEYSCAN_MAP                 CO_BIT(2)   // 0: ROW[3:0]=PD[7:4], 1: ROW[3:0]=PC[7:4]
#define PMU_KEYSCAN_IRQ_EN              CO_BIT(3)

#define PMU_REG_KEYSCAN_GLICTH_CFG      0xc7
#define PMU_KEYSCAN_GLICTH_WIDTH_MSK    0x0f
#define PMU_KEYSCAN_GLICTH_WIDTH_POS    0
#define PMU_KEYSCAN_GLICTH_EN           CO_BIT(4)
#define PMU_GHOST_KEY_DETECT_EN         CO_BIT(5)
#define PMU_GHOST_KEY_MASK_0            CO_BIT(6)   // fix to 1
#define PMU_GHOST_KEY_MASK_1            CO_BIT(7)   // fix to 1

#define PMU_REG_KEYSCAN_ROW_SEL         0xc8        /* (pc4:7 or pd4:7) pd0:3 */
#define PMU_REG_KEYSCAN_COL_SEL_1       0xc9        /* pa0:7 */
#define PMU_REG_KEYSCAN_COL_SEL_2       0xca        /* pb0:7 */
#define PMU_REG_KEYSCAN_COL_SEL_3       0xc6        /* pc0:3 */

#define PMU_REG_KEYSCAN_INTERVAL        0xcb    // freq = RC clock / 4

#define PMU_REG_KEYSCAN_STATUS_0        0xcc
#define PMU_REG_KEYSCAN_STATUS_1        0xd0
#define PMU_REG_KEYSCAN_STATUS_2        0xd4
#define PMU_REG_KEYSCAN_STATUS_3        0xd8
#define PMU_REG_KEYSCAN_STATUS_4        0xdc

/*
NOTICE:
pmu_reg [0xcc ~ 0xe0].
can't use write after read.
read is keyscan_status
*/
#define PMU_REG_BT_TIMER_WU_IRQ_PROTECT 0xcc    // unit: pmu_sys_clk, start N clock to check IRQ after enter deep sleep.

#define PMU_REG_CALI_CNT_RSV_1          0xcd    // for extern clock calibration usage, just debug

#define PMU_REG_BAT_DEB_LEN             0xce    // {N, 5'd0} * deb_clock
#define PMU_REG_LVD_DEB_LEN             0xcf    // {N, 5'd0} * deb_clock

#define PMU_REG_BUCK_OFF_DLY            0xd9

#define PMU_REG_PWO_SLP_DLY             0xdc    // sleep start to sleep active delay, N * pmu_sys_clock


#endif  // _DRIVER_PMU_REGS_H

