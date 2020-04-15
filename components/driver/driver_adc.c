#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "sys_utils.h"

#include "driver_adc.h"
#include "driver_flash.h"
#include "driver_plf.h"
#include "driver_pmu.h"
#include "driver_pmu_regs.h"

#define ADC_FIFO_DEPTH          32
#define ADC_CHANNELS            4

enum adc_trans_mode_t {
    ADC_TRANS_MODE_LOOP,
    ADC_TRANS_MODE_FIXED,
};

struct adc_reg_ctrl_t {
    uint32_t adc_en:1;
    /* fifo should only be used in fixed channel mode */
    uint32_t fifo_en:1;
    /* @ref adc_trans_mode_t */
    uint32_t adc_mode:1;
    /* valid in fixed channel mode to indicate data valid */
    uint32_t data_valid:1;
    /* indicate used channel in fixed mode */
    uint32_t ch_sel:3;
    uint32_t reserved1:1;
    /* each bit represents one channel, used in loop mode */
    uint32_t ch_en:8;
    /* used to clear error interrupt */
    uint32_t int_clr:1;
    uint32_t reserved3:15;
};

struct adc_reg_data_t {
    uint32_t data:10;
    uint32_t reserved:22;
};

struct adc_reg_int_ctl_t {
    uint32_t fifo_full_en:1;
    uint32_t fifo_hfull_en:1;
    uint32_t fifo_empty_en:1;
    uint32_t fifo_hempty_en:1;
    uint32_t error_en:1;
    uint32_t reserved1:3;
    uint32_t fifo_full_status:1;
    uint32_t fifo_hfull_status:1;
    uint32_t fifo_empty_status:1;
    uint32_t fifo_hempty_status:1;
    uint32_t error_status:1;
    uint32_t reserved2:3;
    uint32_t length:4;
    uint32_t reserved3:12;
};

struct adc_regs_t {
    struct adc_reg_ctrl_t ctrl;
    struct adc_reg_data_t data[ADC_CHANNELS];
    uint32_t reserved[8-ADC_CHANNELS];
    uint32_t fifo_data;
    struct adc_reg_int_ctl_t int_ctl;
};

struct adc_env_t {
    enum adc_trans_mode_t mode;
    uint8_t en_channels;
    void (*callback)(uint16_t *, uint32_t);
    uint16_t *buffer;
    uint32_t length;
    uint32_t offset;
};

static volatile struct adc_regs_t *adc_regs = (volatile struct adc_regs_t *)SAR_ADC_BASE;
static struct adc_env_t adc_env;
static uint8_t adc_ref_calib = false;
static uint16_t adc_ref_internal = 1200;
static uint16_t adc_ref_avdd = 2900;

/*********************************************************************
 * @fn      adc_set_sample_clock
 *
 * @brief   set adc sample rate. adc core generate one result per 13 clock.
 *          There are two selections for adc clock source: 12M, 32K
 *
 * @param   clk     - clock selection, @ref adc_sample_clk_t. 
 *       
 * @return  None.
 */
static void adc_set_sample_clock(enum adc_sample_clk_t clk, uint8_t div)
{
    /* set ADC clock dividor */
    ool_write(PMU_REG_CLK_CONFIG, (ool_read(PMU_REG_CLK_CONFIG) & (~(PMU_CLK_ADC_OSCDIV_MSK|PMU_CLK_ADC_RCDIV_MSK)))\
                                        | div);

    /* set ADC clock selection */
    ool_write(PMU_REG_ADC_CTRL1, (ool_read(PMU_REG_ADC_CTRL1) & (~PMU_REG_ADC_CLK_SEL_MSK)) | clk);
    if(clk == ADC_SAMPLE_CLK_24M_DIV13) {
        ool_write(PMU_REG_CLK_CTRL, (ool_read(PMU_REG_CLK_CTRL) & (~(PMU_SARADC_HIGH_CLK|PMU_SARADC_LOW_CLK)))  \
                                        | PMU_SARADC_HIGH_CLK);
    }
    else if(clk == ADC_SAMPLE_CLK_64K_DIV13) {
        ool_write(PMU_REG_CLK_CTRL, (ool_read(PMU_REG_CLK_CTRL) & (~(PMU_SARADC_HIGH_CLK|PMU_SARADC_LOW_CLK)))  \
                                        | PMU_SARADC_LOW_CLK);
    }
}

/*********************************************************************
 * @fn      adc_set_reference
 *
 * @brief   set adc sample reference. The reference should be AVDD(also 
 *          known as IOVDD) or internel reference. The voltage of internal
 *          should be configured to 4 different value. User should choose
 *          proper reference for different input voltage range.
 *
 * @param   ref_sel     - reference selection, @ref adc_reference_t. 
 *          int_ref_cfg - internal reference voltage, @ref adc_internal_ref_t
 *       
 * @return  None.
 */
static void adc_set_reference(enum adc_reference_t ref_sel, enum adc_internal_ref_t int_ref_cfg)
{
    ool_write(PMU_REG_ADC_CTRL6, (ool_read(PMU_REG_ADC_CTRL6) & (~(PMU_REG_ADC_REF_SEL_MSK|PMU_REG_ADC_INT_REF_CFG)))   \
                                        |(ref_sel | int_ref_cfg));

    if(ref_sel == ADC_REFERENCE_INTERNAL) {
        ool_write(PMU_REG_ADC_CTRL4, ool_read(PMU_REG_ADC_CTRL4) | PMU_REG_ADC_INT_REF_EN);
    }
    else if(ref_sel == ADC_REFERENCE_AVDD) {
        ool_write(PMU_REG_ADC_CTRL4, ool_read(PMU_REG_ADC_CTRL4) & (~PMU_REG_ADC_INT_REF_EN));
    }
}

/*********************************************************************
 * @fn      adc_route_config
 *
 * @brief   set adc sample route for different usage. For VBAT sample
 *          there is build-in route: VBAT->1/4 dividor->sample core.
 *          If sample signal is from PAD, different route should be
 *          choosen according to the signal characteristics: if signal
 *          is out range of reference, the signal should be routed to
 *          internal dividor before being sampled; if signal is lack
 *          of drive capability, the signal should be routed to buffer
 *          before being sampled.
 *
 * @param   ref_sel     - reference selection, @ref adc_reference_t. 
 *          int_ref_cfg - internal reference voltage, @ref adc_internal_ref_t
 *       
 * @return  None.
 */
static void adc_route_config(enum adc_trans_source_t src, uint8_t pad_to_sample, uint8_t pad_to_div, uint8_t pad_to_buffer)
{
    if(src == ADC_TRANS_SOURCE_VBAT) {
        ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) & (~PMU_REG_ADC_PAD_EN));
        ool_write(PMU_REG_ADC_CTRL8, ool_read(PMU_REG_ADC_CTRL8) | (PMU_REG_ADC_BAT_EN | PMU_REG_ADC_BAT_TO_SAM_EN));
    }
    else if(src == ADC_TRANS_SOURCE_PAD) {
        ool_write(PMU_REG_ADC_CTRL8, ool_read(PMU_REG_ADC_CTRL8) & (~(PMU_REG_ADC_BAT_EN | PMU_REG_ADC_BAT_TO_SAM_EN)));
        ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) | PMU_REG_ADC_PAD_EN);
        if(pad_to_sample) {
            /* enable pad to sample core */
            ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) | PMU_REG_ADC_PAD_TO_SAM_EN);
        }
        else {
            /* disable pad to sample core */
            ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) & (~PMU_REG_ADC_PAD_TO_SAM_EN));
        }

        if(pad_to_div) {
            /* enable dividor and dividor to sample core */
            ool_write(PMU_REG_ADC_CTRL3, ool_read(PMU_REG_ADC_CTRL3) | PMU_REG_ADC_DIV_EN);
            ool_write(PMU_REG_ADC_CTRL1, ool_read(PMU_REG_ADC_CTRL1) | PMU_REG_ADC_DIV_TO_SAM_EN);
            ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) | PMU_REG_ADC_PAD_TO_DIV_EN);
        }
        else {
            /* disable dividor and dividor to sample core */
            ool_write(PMU_REG_ADC_CTRL3, ool_read(PMU_REG_ADC_CTRL3) & (~PMU_REG_ADC_DIV_EN));
            ool_write(PMU_REG_ADC_CTRL1, ool_read(PMU_REG_ADC_CTRL1) & (~PMU_REG_ADC_DIV_TO_SAM_EN));
            ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) & (~PMU_REG_ADC_PAD_TO_DIV_EN));
        }

        if(pad_to_buffer) {
            /* enable buffer and buffer to sample core */
            ool_write(PMU_REG_ADC_CTRL1, ool_read(PMU_REG_ADC_CTRL1) | PMU_REG_ADC_IN_BUF_EN);
            ool_write(PMU_REG_ADC_CTRL0, ool_read(PMU_REG_ADC_CTRL0) | PMU_REG_ADC_BUF_TO_SAM_EN);
            ool_write(PMU_REG_ADC_CTRL4, ool_read(PMU_REG_ADC_CTRL4) | PMU_REG_ADC_PAD_TO_BUF_EN);
        }
        else {
            /* disable buffer and buffer to sample core */
            ool_write(PMU_REG_ADC_CTRL1, ool_read(PMU_REG_ADC_CTRL1) & (~PMU_REG_ADC_IN_BUF_EN));
            ool_write(PMU_REG_ADC_CTRL0, ool_read(PMU_REG_ADC_CTRL0) & (~PMU_REG_ADC_BUF_TO_SAM_EN));
            ool_write(PMU_REG_ADC_CTRL4, ool_read(PMU_REG_ADC_CTRL4) & (~PMU_REG_ADC_PAD_TO_BUF_EN));
        }
    }
}

/*********************************************************************
 * @fn      adc_dividor_config
 *
 * @brief   set adc sample internal dividor configuration. Total resistor
 *          value has 4 choise: 12*105, 12*415, 12*7.25K, 12*32K.
 *
 * @param   div_res     - dividor resistor selection, @ref adc_dividor_total_res_t. 
 *          int_ref_cfg - dividor configuration, @ref adc_dividor_cfg_t
 *       
 * @return  None.
 */
static void adc_dividor_config(enum adc_dividor_total_res_t div_res, enum adc_dividor_cfg_t div_cfg)
{
    ool_write(PMU_REG_ADC_CTRL4, (ool_read(PMU_REG_ADC_CTRL4) & (~(PMU_REG_ADC_DIV_RES_CTL_MSK|PMU_REG_ADC_DIV_CTL_MSK))    \
                                    | (div_cfg | div_res)));
}

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
bool adc_enable(void (*callback)(uint16_t *, uint32_t), uint16_t *buffer, uint32_t length)
{
    if((callback != NULL) && (adc_env.mode != ADC_TRANS_MODE_FIXED)) {
        return false;
    }
    
    /* remove ADC PD and enable ADC core */
    ool_write(PMU_REG_ADC_CTRL6, (ool_read(PMU_REG_ADC_CTRL6) & (~(PMU_REG_ADC_PD_CTL_PO|PMU_REG_ADC_PWR_EN|PMU_REG_ADC_PWR_SEL)))  \
                                    | (PMU_REG_ADC_PWR_EN | PMU_REG_ADC_PWR_SEL));
    ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) & (~PMU_REG_ADC_PD));
    ool_write(PMU_REG_ADC_CTRL1, ool_read(PMU_REG_ADC_CTRL1) | PMU_REG_ADC_EN);
    
    co_delay_100us(1);

    adc_regs->ctrl.adc_en = 1;

    if(adc_env.mode == ADC_TRANS_MODE_FIXED) {
        while(adc_regs->ctrl.data_valid == 0);
    }
    else {
        /* delay for some time to wait data valid */
    }

    if(callback) {
        adc_env.callback = callback;
        adc_env.buffer = buffer;
        adc_env.length = length;
        
        adc_regs->ctrl.fifo_en = 1;
        adc_regs->int_ctl.fifo_hfull_en = 1;
        NVIC_EnableIRQ(ADC_IRQn);
    }

    return true;
}

/*********************************************************************
 * @fn      adc_disable
 *
 * @brief   disable ongoing AD-convert.
 *
 * @param   None.
 *       
 * @return  None.
 */
void adc_disable(void)
{
    NVIC_DisableIRQ(ADC_IRQn);
    
    adc_regs->ctrl.adc_en = 0;
    
    /* disable and power down ADC core */
    ool_write(PMU_REG_ADC_CTRL1, ool_read(PMU_REG_ADC_CTRL1) & (~PMU_REG_ADC_EN));
    ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) | PMU_REG_ADC_PD);
    ool_write(PMU_REG_ADC_CTRL6, ool_read(PMU_REG_ADC_CTRL6) & (~(PMU_REG_ADC_PD_CTL_PO|PMU_REG_ADC_PWR_EN|PMU_REG_ADC_PWR_SEL)));
}

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
void adc_get_result(enum adc_trans_source_t src, uint8_t channels, uint16_t *buffer)
{
#if 1
    if((src == ADC_TRANS_SOURCE_VBAT)
        || (adc_env.mode == ADC_TRANS_MODE_FIXED)) {
        *buffer = adc_regs->data[0].data;
    }
    else if(src == ADC_TRANS_SOURCE_PAD) {
        uint16_t data[ADC_CHANNELS];
        uint8_t first_en_chn, en_chns, prev_en_chan;

        first_en_chn = 0xff;
        en_chns = adc_env.en_channels;

        for(uint8_t i=(ADC_CHANNELS-1); i!=0xff; i--) {
            if(en_chns & (1<<i)) {
                if(first_en_chn == 0xff) {
                    first_en_chn = i;
                }
                else {
                    data[prev_en_chan] = adc_regs->data[i].data;
                    //co_printf("data=0x%04x, chn=%d, src=%d.\r\n", data[prev_en_chan], prev_en_chan, i);
                }
                prev_en_chan = i;
            }
        }
        data[prev_en_chan] = adc_regs->data[first_en_chn].data;
        //co_printf("data=0x%04x, chn=%d, src=%d.\r\n", data[first_en_chn], prev_en_chan, first_en_chn);
        
        for(uint8_t i=0; i<ADC_CHANNELS; i++) {
            if(channels & (1<<i)) {
                *buffer++ = data[i];
            }
        }
    }
#else
    if((src == ADC_TRANS_SOURCE_VBAT)
        || ((channels & (channels-1)) == 0)) {
        *buffer = adc_regs->data[0].data;
    }
#endif
}

/*********************************************************************
 * @fn      adc_get_ref_voltage
 *
 * @brief   use to calculate absolute value after get ADC sample result.
 *
 * @param   ref - ADC reference setting. @ref adc_reference_t
 *       
 * @return  reference voltage, unit: mv.
 */
uint16_t adc_get_ref_voltage(enum adc_reference_t ref)
{
    if(ref == ADC_REFERENCE_INTERNAL) {
        return adc_ref_internal;
    }
    else if(ref == ADC_REFERENCE_AVDD) {
        return adc_ref_avdd;
    }
    else {
        return 0;
    }
}

/*********************************************************************
 * @fn      adc_init
 *
 * @brief   initiate ADC with parameters stored in cfg.
 *
 * @param   cfg - ADC configurations
 *       
 * @return  None.
 */
void adc_init(struct adc_cfg_t *cfg)
{
    uint8_t channels = cfg->channels;
    uint32_t data[5];

    if(adc_ref_calib == false) {
		adc_ref_calib = true;
        flash_OTP_read(0x1000, 5*sizeof(uint32_t), (void *)data);
		if(data[0] == 0x31303030) {
			adc_ref_internal = ((400*1024)/(data[3] / 32) + (800*1024)/(data[4] / 32)) / 2;
			adc_ref_avdd = ((2400*1024)/(data[1] / 32) + (800*1024)/(data[2] / 32)) / 2;
		}
    }

    memset((void *)&adc_env, 0, sizeof(adc_env));

    if(cfg->src == ADC_TRANS_SOURCE_VBAT) {
        channels = 0x01;
    }
    adc_env.en_channels = channels;

    ool_write(PMU_REG_ADC_CTRL5, ool_read(PMU_REG_ADC_CTRL5) | 0x70);

    if(cfg->clk_sel == ADC_SAMPLE_CLK_64K_DIV13) {
        if((cfg->clk_div & 0x30) == 0 ) {
            cfg->clk_div |= 0x30;
        }
    }
    else {
        if((cfg->clk_div & 0x0f) == 0 ) {
            cfg->clk_div |= 0x0f;
        }
    }
    adc_set_sample_clock(cfg->clk_sel, cfg->clk_div);

    adc_set_reference(cfg->ref_sel, cfg->int_ref_cfg);

    adc_route_config(cfg->src, cfg->route.pad_to_sample, cfg->route.pad_to_div, cfg->route.pad_to_buffer);

    if(cfg->route.pad_to_div) {
        adc_dividor_config(cfg->div_res, cfg->div_cfg);
    }

    /* disable all interrupts */
    *(uint32_t *)&adc_regs->int_ctl = 0;
    
    adc_regs->int_ctl.length = 0x0E;

    /* confirm adc disable and fifo diable */
    adc_regs->ctrl.adc_en = 0;
    adc_regs->ctrl.fifo_en = 0;
    
    if(channels & (channels-1)) {
        /* multi channels */
        adc_regs->ctrl.adc_mode = ADC_TRANS_MODE_LOOP;
        adc_regs->ctrl.ch_en = channels;

        adc_env.mode = ADC_TRANS_MODE_LOOP;
    }
    else {
        /* singal channel */
        uint8_t i;
        
        adc_regs->ctrl.adc_mode = ADC_TRANS_MODE_FIXED;
        
        for(i=0; i<ADC_CHANNELS; i++) {
            if((1<<i) & channels) {
                break;
            }
        }
        adc_regs->ctrl.ch_sel = i;

        adc_env.mode = ADC_TRANS_MODE_FIXED;
    }
}

void adc_isr(void)
{
    uint32_t last_length, read_length;
    uint16_t *buffer;

    buffer = &adc_env.buffer[adc_env.offset];
    last_length = adc_env.length - adc_env.offset;
    if(last_length > (ADC_FIFO_DEPTH>>1)) {
        last_length = ADC_FIFO_DEPTH >> 1;
    }
    
    if(adc_regs->int_ctl.fifo_hfull_status) {
        read_length = last_length;
        while(last_length) {
            *buffer++ = (uint16_t)adc_regs->fifo_data;
            last_length--;
        }
        adc_env.offset += read_length;

        if(adc_env.offset >= adc_env.length) {
            NVIC_DisableIRQ(ADC_IRQn);
            adc_disable();
            adc_env.callback(adc_env.buffer, adc_env.length);
        }
    }
}

