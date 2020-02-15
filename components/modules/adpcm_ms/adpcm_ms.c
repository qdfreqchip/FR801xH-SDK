/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
 
/*
 * INCLUDES
 */
#include "adpcm_ms.h"


/*
 * MACROS
 */

/*
 * CONSTANTS 
 */
static int AdaptationTable[] =
{
    230, 230, 230, 230, 307, 409, 512, 614,
    768, 614, 512, 409, 307, 230, 230, 230
};

static int AdaptCoeff1[] =
{
    256, 512, 0, 192, 240, 460, 392
};

static int AdaptCoeff2[] =
{
    0, -256, 0, 64, 0, -208, -232
};

/*********************************************************************
 * @fn		adpcm_ms_expand_nibble
 *
 * @brief	.
 *
 * @param	c       -
 *          nibble  -
 *
 * @return	short   -.
 */
short adpcm_ms_expand_nibble(ADPCMChannelStatus *c, char nibble)
{
    int predictor;

    predictor = (((c->sample1) * (c->coeff1)) + ((c->sample2) * (c->coeff2))) / 256;
    predictor += (signed)((nibble & 0x08)?(nibble - 0x10):(nibble)) * c->idelta;

    c->sample2 = c->sample1;
    c->sample1 = av_clip_int16(predictor);
    c->idelta = (AdaptationTable[(int)nibble] * c->idelta) >> 8;
    if (c->idelta < 16) c->idelta = 16;

    return c->sample1;
}


/*********************************************************************
 * @fn		adpcm_decode_frame
 *
 * @brief	.
 *
 * @param	c           -
 *          pcm_buf     -
 *          data_size   -
 *          adpcm_buf   -
 *          buf_size    -
 *
 * @return	short       -.
 */
int adpcm_decode_frame(ADPCMContext *c,
                       short *pcm_buf, int *data_size,
                       uint8_t *adpcm_buf, int buf_size)
{

    int n;
    int block_predictor[2];
    short *samples;//**out
    uint8_t *src ;//**in
    int st ;
    if (!buf_size)
        return 0;

    if(*data_size/4 < buf_size + 8)  //?????
    {
        printf("reeorrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr...\n") ;
        return -1;
    }

    st = c->channel == 2 ;
    samples = pcm_buf;
    *data_size= 0;
    src = adpcm_buf;

    if (c->block_align != 0 && buf_size > c->block_align)
        buf_size = c->block_align;
    n = buf_size - 7 * c->channel;
    if (n < 0)
        return -1;
    block_predictor[0] = av_clip(*src++, 0, 7);
    block_predictor[1] = 0;
    if(st)
        block_predictor[1] = av_clip(*src++, 0, 7) ;
    c->status[0].idelta = (int16_t)((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
    src+=2;
    if(st)
    {

        c->status[1].idelta = (int16_t)((*src & 0xFF) | ((src[1] << 8) & 0xFF00)) ;
        src+=2 ;
    }
    c->status[0].coeff1 = AdaptCoeff1[block_predictor[0]];
    c->status[0].coeff2 = AdaptCoeff2[block_predictor[0]];
    c->status[1].coeff1 = AdaptCoeff1[block_predictor[1]];
    c->status[1].coeff2 = AdaptCoeff2[block_predictor[1]];

    c->status[0].sample1 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
    src+=2;
    if (st) c->status[1].sample1 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
    if (st) src+=2;
    c->status[0].sample2 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
    src+=2;
    if (st) c->status[1].sample2 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
    if (st) src+=2;

    *samples++ = c->status[0].sample1;
    if (st) *samples++ = c->status[1].sample1;

    *samples++ = c->status[0].sample2;
    if (st) *samples++ = c->status[1].sample2;

    for(; n>0; n--)
    {
        *samples++ = adpcm_ms_expand_nibble(&c->status[0], (src[0] >> 4) & 0x0F);
        *samples++ = adpcm_ms_expand_nibble(&c->status[st], src[0] & 0x0F);
        src ++;
    }
    *data_size = (uint8_t *)samples - (uint8_t *)pcm_buf;
    return src - adpcm_buf;
}
