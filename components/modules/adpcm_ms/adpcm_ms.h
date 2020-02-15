/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
 
#ifndef ADPCM_MS_H
#define ADPCM_MS_H

/*
 * INCLUDES
 */
#include <stdint.h>
#include <stdio.h>

/*
 * MACROS
 */

/*
 * CONSTANTS 
 */
#define BLKSIZE 1024

/*
 * TYPEDEFS 
 */
typedef struct ADPCMChannelStatus
{
    int predictor;

    short int step_index;
    int step;
    /* for encoding */
    int prev_sample;

    /* MS version */
    short sample1;
    short sample2;
    int coeff1;
    int coeff2;
    int idelta;
} ADPCMChannelStatus;

typedef struct ADPCMContext
{
    int channel; /* for stereo MOVs, decode left, then decode right, then tell it's decoded */
    int block_align ;
    ADPCMChannelStatus status[2];
} ADPCMContext;

/*
 * GLOBAL VARIABLES 
 */

/*
 * LOCAL VARIABLES 
 */

/*
 * LOCAL FUNCTIONS
 */

static inline int av_clip(int a, int amin, int amax)
{
    if (a < amin)      return amin;
    else if (a > amax) return amax;
    else               return a;
}

static inline int16_t av_clip_int16(int a)
{
    if ((a+32768) & ~65535) return (a>>31) ^ 32767;
    else                    return a;
}

short adpcm_ms_expand_nibble(ADPCMChannelStatus *c, char nibble) ;
int adpcm_decode_frame(ADPCMContext *c,
                       short *pcm_buf, int *data_size,
                       uint8_t *adpcm_buf, int buf_size) ;

//short adpcm_ms_expand_nibble(ADPCMChannelStatu1s *c, char nibble) ;
//int adpcm_decode_frame(ADPCMContext *c, void *data, int *data_size,uint8_t *buf, int buf_size) ;

#endif
