/*
 *
 *  Bluetooth low-complexity, subband codec (SBC) library
 *
 *  Copyright (C) 2008-2010  Nokia Corporation
 *  Copyright (C) 2012-2014  Intel Corporation
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2004-2005  Henryk Ploetz <henryk@ploetzli.ch>
 *  Copyright (C) 2005-2006  Brad Midgley <bmidgley@xmission.com>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __SBC_H
#define __SBC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//#include <sys/types.h>

#define EIO                 5      /* I/O error */
#define ENOSPC              28      /* No space left on device */

/* sampling frequency */
#define SBC_FREQ_16000      0x00
#define SBC_FREQ_32000      0x01
#define SBC_FREQ_44100      0x02
#define SBC_FREQ_48000      0x03

/* blocks */
#define SBC_BLK_4       0x00
#define SBC_BLK_8       0x01
#define SBC_BLK_12      0x02
#define SBC_BLK_16      0x03

/* channel mode */
#define SBC_MODE_MONO       0x00
#define SBC_MODE_DUAL_CHANNEL   0x01
#define SBC_MODE_STEREO     0x02
#define SBC_MODE_JOINT_STEREO   0x03

/* allocation method */
#define SBC_AM_LOUDNESS     0x00
#define SBC_AM_SNR      0x01

/* subbands */
#define SBC_SB_4        0x00
#define SBC_SB_8        0x01

struct sbc_struct
{
    uint8_t frequency;
    uint8_t blocks;
    uint8_t subbands;
    //uint8_t mode;
    uint8_t allocation;
    uint8_t bitpool;

    void *priv;
    void *priv_alloc_base;
};

typedef struct sbc_struct sbc_t;

int sbc_init(sbc_t *sbc, void *buffer);
int sbc_reinit(sbc_t *sbc);
int sbc_init_msbc(sbc_t *sbc, void *buffer);

/* Encodes ONE input block into ONE output block */
int sbc_encode(sbc_t *sbc, const void *input, uint32_t input_len,
               void *output, uint32_t output_len, int *written);

/* Returns the output block size in bytes */
uint32_t sbc_get_frame_length(sbc_t *sbc);

/* Returns the input block size in bytes */
uint32_t sbc_get_codesize(sbc_t *sbc);

void sbc_finish(sbc_t *sbc);

#ifdef __cplusplus
}
#endif

#endif /* __SBC_H */
