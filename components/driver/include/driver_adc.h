#ifndef _DRIVER_ADC_H
#define _DRIVER_ADC_H

#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
                          USAGE SAMPLE
    1. get vbat value
        struct adc_cfg_t cfg;
        uint16_t result;

        memset((void*)&cfg, 0, sizeof(cfg));
        cfg.src = ADC_TRANS_SOURCE_VBAT;
        cfg.ref_sel = ADC_REFERENCE_INTERNAL;
        cfg.int_ref_cfg = ADC_INTERNAL_REF_1_2;
        adc_init(&cfg);
        adc_enable(NULL, NULL, 0);

        adc_get_result(ADC_TRANS_SOURCE_VBAT, 0, &result);
        // vbat_vol = (result * 4800) / 1024 mV.

    2. sample voltage from PAD
        struct adc_cfg_t cfg;
        uint16_t result;
        
        system_set_port_mux(GPIO_PORT_D, GPIO_BIT_4, PORTD4_FUNC_ADC0);
        
        memset((void*)&cfg, 0, sizeof(cfg));
        cfg.src = ADC_TRANS_SOURCE_PAD;
        cfg.ref_sel = ADC_REFERENCE_AVDD;
        cfg.channels = ascii_strn2val((const char *)&data[0], 16, 2);
        cfg.route.pad_to_sample = 1;
        adc_init(&cfg);
        adc_enable(NULL, NULL, 0);

        adc_get_result(ADC_TRANS_SOURCE_VBAT, 0x01, &result);
 */

enum adc_reference_t {
    ADC_REFERENCE_INTERNAL = 0x00,
    ADC_REFERENCE_AVDD = 0x20,
};

enum adc_internal_ref_t {
    ADC_INTERNAL_REF_1_2 = 0x00,
    ADC_INTERNAL_REF_1_3 = 0x40,
    ADC_INTERNAL_REF_1_4 = 0x80,
    ADC_INTERNAL_REF_1_5 = 0xc0,
};

enum adc_dividor_total_res_t {
    ADC_DIV_TOT_RES_105 = 0x01,
    ADC_DIV_TOT_RES_415 = 0x02,
    ADC_DIV_TOT_RES_7_25K = 0x04,
    ADC_DIV_TOT_RES_30K = 0x08,
};

enum adc_dividor_cfg_t {
    ADC_DIV_CFG_1_4 = 0x00,
    ADC_DIV_CFG_1_3 = 0x10,
    ADC_DIV_CFG_1_2 = 0x20,
    ADC_DIV_CFG_2_3 = 0x30,
};

enum adc_trans_source_t {
    ADC_TRANS_SOURCE_VBAT,
    ADC_TRANS_SOURCE_PAD,
};

enum adc_trans_mode_t {
    ADC_TRANS_MODE_LOOP,
    ADC_TRANS_MODE_FIXED,
};

struct adc_cfg_t {
    enum adc_trans_source_t src;

    /* this field is used if src==ADC_TRANS_SOURCE_PAD */
    struct adc_cfg_route_t {
        /* only one bit can be set at the same time */
        uint8_t pad_to_sample:1;
        uint8_t pad_to_div:1;
        uint8_t pad_to_buffer:1;
        uint8_t reserved:5;
    } route;

    /* the following two parameters are used to select reference */
    enum adc_reference_t ref_sel;
    enum adc_internal_ref_t int_ref_cfg;

    /* the following two parameters are available when pad_to_div=1 */
    enum adc_dividor_total_res_t div_res;
    enum adc_dividor_cfg_t div_cfg;

    /* each bit of lower 4-bits represent one channel */
    uint8_t channels;
};

/*********************************************************************
 * @fn      adc_enable
 *
 * @brief   after adc is initiated, call this function to start AD-convert.
 *          If callback is set a none NULL value, the ADC will work in
 *          interrupt mode. And after indicated length of samples are fetched,
 *          callback will be called to return all these values stored in buffer.
 *          This way can be used in over-sample application.
 *          If callback is NULL, user should call adc_get_result to fetch
 *          value manually.
 *
 * @param   callback    - callback function after all samples have been fetched. 
 *          buffer      - memory pointer to save sample values.
 *          length      - how many samples are needed.
 *       
 * @return  callback should only be set in FIXED channel sample mode, the return
 *          value is false if these configurations mismatch.
 */
bool adc_enable(void (*callback)(uint16_t *, uint32_t), uint16_t *buffer, uint32_t length);

/*********************************************************************
 * @fn      adc_disable
 *
 * @brief   disable ongoing AD-convert.
 *
 * @param   None.
 *       
 * @return  None.
 */
void adc_disable(void);

/*********************************************************************
 * @fn      adc_get_result
 *
 * @brief   get latest converted value, src and channels should match with
 *          configurations set when calling adc_init.
 *
 * @param   src     - @ref adc_trans_source_t.
 *          channels- value of indicated channels are needed.
 *          buffer  - the sample results will be stored in sequence.
 *       
 * @return  None.
 */
void adc_get_result(enum adc_trans_source_t src, uint8_t channels, uint16_t *buffer);

/*********************************************************************
 * @fn      adc_init
 *
 * @brief   initiate ADC with parameters stored in cfg.
 *          !!!!!!!!!!!!!!!!!!!!!!!!
 *          currently the adc module only supports enable one channel at the same time.
 *
 * @param   cfg - ADC configurations
 *       
 * @return  None.
 */
void adc_init(struct adc_cfg_t *cfg);

#endif  // _DRIVER_ADC_H

