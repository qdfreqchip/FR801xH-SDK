/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_CODEC_H
#define _DRIVER_CODEC_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdint.h>

/*
 * MACROS (宏定义)
 */
/** @defgroup CODEC_SAMPLE_RATE_DEFINES
 * @{
 */
#define CODEC_SAMPLE_RATE_48000         0x44
#define CODEC_SAMPLE_RATE_44100         0xBB
#define CODEC_SAMPLE_RATE_24000         0x33
#define CODEC_SAMPLE_RATE_16000         0x22
#define CODEC_SAMPLE_RATE_8000          0x00

/*
 * PUBLIC FUNCTIONS (全局函数)
 */

/*********************************************************************
 * @fn      codec_enable_adc
 *
 * @brief   Enable the adc (aka MIC input) channel of codec.
 *
 * @param   None.
 *
 * @return  None.
 */
void codec_enable_adc(void);

/*********************************************************************
 * @fn      codec_disable_adc
 *
 * @brief   Disable the adc (aka MIC input) channel of codec.
 *
 * @param   None.
 *
 * @return  None.
 */
void codec_disable_adc(void);

/*********************************************************************
 * @fn      codec_enable_dac
 *
 * @brief   Enable the dac (aka speaker output) channel of codec.
 *
 * @param   None.
 *
 * @return  None.
 */
void codec_enable_dac(void);

/*********************************************************************
 * @fn      codec_disable_dac
 *
 * @brief   Disable the dac (aka speaker output) channel of codec.
 *
 * @param   None.
 *
 * @return  None.
 */
void codec_disable_dac(void);

/*********************************************************************
 * @fn      codec_set_vol
 *
 * @brief   set the volumn of speaker.
 *
 * @param   value   - volumn target, should be set from 0x00(min) to 0x3f(max).
 *
 * @return  None.
 */
void codec_set_vol(uint8_t value);

/*********************************************************************
 * @fn      codec_init
 *
 * @brief   init the codec module.
 *
 * @param   sample_rate - which sample rate should the codec work at, such as
 *                        CODEC_SAMPLE_RATE_8000.
 *
 * @return  None.
 */
void codec_init(uint8_t sample_rate);

void audio_speaker_codec_init(void);
void speaker_codec_init(uint8_t sample_rate);


#endif  // _DRIVER_CODEC_H

