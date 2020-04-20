/*
 *
 *  Bluetooth low-complexity, subband codec (SBC) library
 *
 *  Copyright (C) 2008-2010  Nokia Corporation
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2004-2005  Henryk Ploetz <henryk@ploetzli.ch>
 *  Copyright (C) 2005-2008  Brad Midgley <bmidgley@xmission.com>
 *  Copyright (C) 2012-2013  Intel Corporation
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#include "sbc_math.h"
#include "sbc_tables.h"
#include "sbc.h"
#include "sbc_primitives.h"
//#include "os_mem.h"

#define SBC_SYNCWORD	0x9C

#define MSBC_SYNCWORD	0xAD
#define MSBC_BLOCKS	15

/* This structure contains an unpacked SBC frame.
   Yes, there is probably quite some unused space herein */
struct sbc_frame {
	uint8_t frequency;
	uint8_t block_mode;
	uint8_t blocks;
	/*enum {
		MONO		= SBC_MODE_MONO,
		DUAL_CHANNEL	= SBC_MODE_DUAL_CHANNEL,
		STEREO		= SBC_MODE_STEREO,
		JOINT_STEREO	= SBC_MODE_JOINT_STEREO
	} mode;
	uint8_t channels;*/
	enum {
		LOUDNESS	= SBC_AM_LOUDNESS,
		SNR		= SBC_AM_SNR
	} allocation;
	uint8_t subband_mode;
	uint8_t subbands;
	uint8_t bitpool;
	uint16_t codesize;
	uint16_t length;

	/* only the lower 4 bits of every element are to be used */
	uint32_t SBC_ALIGNED scale_factor[8];

	/* raw integer subband samples in the frame */
	int32_t SBC_ALIGNED sb_sample_f[16][8];
};

/*
 * Calculates the CRC-8 of the first len bits in data
 */
static const uint8_t crc_table[256] = {
	0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
	0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
	0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
	0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
	0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
	0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
	0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
	0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
	0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
	0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
	0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
	0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
	0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
	0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
	0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
	0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
	0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
	0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
	0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
	0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
	0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
	0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
	0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
	0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
	0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
	0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
	0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
	0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
	0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
	0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
	0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
	0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

static uint8_t sbc_crc8(const uint8_t *data, size_t len)
{
	uint8_t crc = 0x0f;
	size_t i;
	uint8_t octet;

	for (i = 0; i < len / 8; i++)
		crc = crc_table[crc ^ data[i]];

	octet = data[i];
	for (i = 0; i < len % 8; i++) {
		char bit = ((octet ^ crc) & 0x80) >> 7;

		crc = ((crc & 0x7f) << 1) ^ (bit ? 0x1d : 0);

		octet = octet << 1;
	}

	return crc;
}

/*
 * Code straight from the spec to calculate the bits array
 * Takes a pointer to the frame in question, a pointer to the bits array and
 * the sampling frequency (as 2 bit integer)
 */
static SBC_ALWAYS_INLINE void sbc_calculate_bits_internal(
		const struct sbc_frame *frame, int bits[8], int subbands)
{
	uint8_t sf = frame->frequency;

	int bitneed[8], loudness, max_bitneed, bitcount, slicecount, bitslice;
	int sb;

	max_bitneed = 0;
	if (frame->allocation == SNR) {
		for (sb = 0; sb < subbands; sb++) {
			bitneed[sb] = frame->scale_factor[sb];
			if (bitneed[sb] > max_bitneed)
				max_bitneed = bitneed[sb];
		}
	} else {
		for (sb = 0; sb < subbands; sb++) {
			if (frame->scale_factor[sb] == 0)
				bitneed[sb] = -5;
			else {
				if (subbands == 4)
					loudness = frame->scale_factor[sb] - sbc_offset4[sf][sb];
				else
					loudness = frame->scale_factor[sb] - sbc_offset8[sf][sb];
				if (loudness > 0)
					bitneed[sb] = loudness / 2;
				else
					bitneed[sb] = loudness;
			}
			if (bitneed[sb] > max_bitneed)
				max_bitneed = bitneed[sb];
		}
	}

	bitcount = 0;
	slicecount = 0;
	bitslice = max_bitneed + 1;
	do {
		bitslice--;
		bitcount += slicecount;
		slicecount = 0;
		for (sb = 0; sb < subbands; sb++) {
			if ((bitneed[sb] > bitslice + 1) && (bitneed[sb] < bitslice + 16))
				slicecount++;
			else if (bitneed[sb] == bitslice + 1)
				slicecount += 2;
		}
	} while (bitcount + slicecount < frame->bitpool);

	if (bitcount + slicecount == frame->bitpool) {
		bitcount += slicecount;
		bitslice--;
	}

	for (sb = 0; sb < subbands; sb++) {
		if (bitneed[sb] < bitslice + 2)
			bits[sb] = 0;
		else {
			bits[sb] = bitneed[sb] - bitslice;
			if (bits[sb] > 16)
				bits[sb] = 16;
		}
	}

	for (sb = 0; bitcount < frame->bitpool &&
					sb < subbands; sb++) {
		if ((bits[sb] >= 2) && (bits[sb] < 16)) {
			bits[sb]++;
			bitcount++;
		} else if ((bitneed[sb] == bitslice + 1) && (frame->bitpool > bitcount + 1)) {
			bits[sb] = 2;
			bitcount += 2;
		}
	}

	for (sb = 0; bitcount < frame->bitpool &&
					sb < subbands; sb++) {
		if (bits[sb] < 16) {
			bits[sb]++;
			bitcount++;
		}
	}
}

static void sbc_calculate_bits(const struct sbc_frame *frame, int bits[8])
{
	if (frame->subbands == 4)
		sbc_calculate_bits_internal(frame, bits, 4);
	else
		sbc_calculate_bits_internal(frame, bits, 8);
}

static int sbc_analyze_audio(struct sbc_encoder_state *state,
						struct sbc_frame *frame)
{
	int blk;
	int16_t *x;

	switch (frame->subbands) {
	case 4:
		x = &state->X[state->position - 4 *
				state->increment + frame->blocks * 4];
		for (blk = 0; blk < frame->blocks;
					blk += state->increment) {
			state->sbc_analyze_4s(
				state, x,
				frame->sb_sample_f[blk],
				frame->sb_sample_f[blk + 1] -
				frame->sb_sample_f[blk]);
			x -= 4 * state->increment;
		}
		return frame->blocks * 4;

	case 8:
		x = &state->X[state->position - 8 *
				state->increment + frame->blocks * 8];
		for (blk = 0; blk < frame->blocks;
					blk += state->increment) {
			state->sbc_analyze_8s(
				state, x,
				frame->sb_sample_f[blk],
				frame->sb_sample_f[blk + 1] -
				frame->sb_sample_f[blk]);
			x -= 8 * state->increment;
		}
		return frame->blocks * 8;

	default:
		return -EIO;
	}
}

/* Supplementary bitstream writing macros for 'sbc_pack_frame' */

#define PUT_BITS(data_ptr, bits_cache, bits_count, v, n)		\
	do {								\
		bits_cache = (v) | (bits_cache << (n));			\
		bits_count += (n);					\
		if (bits_count >= 16) {					\
			bits_count -= 8;				\
			*data_ptr++ = (uint8_t)				\
				(bits_cache >> bits_count);		\
			bits_count -= 8;				\
			*data_ptr++ = (uint8_t)				\
				(bits_cache >> bits_count);		\
		}							\
	} while (0)

#define FLUSH_BITS(data_ptr, bits_cache, bits_count)			\
	do {								\
		while (bits_count >= 8) {				\
			bits_count -= 8;				\
			*data_ptr++ = (uint8_t)				\
				(bits_cache >> bits_count);		\
		}							\
		if (bits_count > 0)					\
			*data_ptr++ = (uint8_t)				\
				(bits_cache << (8 - bits_count));	\
	} while (0)

/*
 * Packs the SBC frame from frame into the memory at data. At most len
 * bytes will be used, should more memory be needed an appropriate
 * error code will be returned. Returns the length of the packed frame
 * on success or a negative value on error.
 *
 * The error codes are:
 * -1 Not enough memory reserved
 * -2 Unsupported sampling rate
 * -3 Unsupported number of blocks
 * -4 Unsupported number of subbands
 * -5 Bitpool value out of bounds
 * -99 not implemented
 */

static SBC_ALWAYS_INLINE int sbc_pack_frame_internal(uint8_t *data,
					struct sbc_frame *frame, int frame_subbands, int frame_channels)
{
	/* Bitstream writer starts from the fourth byte */
	uint8_t *data_ptr = data + 4;
	uint32_t bits_cache = 0;
	uint32_t bits_count = 0;

	/* Will copy the header parts for CRC-8 calculation here */
	uint8_t crc_header[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int crc_pos = 0;

	uint32_t audio_sample;

	int sb, blk;	/* channel, subband, block and bit counters */
	int bits[8];		/* bits distribution */
	uint32_t levels[8];	/* levels are derived from that */
	uint32_t sb_sample_delta[8];

	/* Can't fill in crc yet */

	crc_header[0] = data[1];
	crc_header[1] = data[2];
	crc_pos = 16;

	for (sb = 0; sb < frame_subbands; sb++) {
		PUT_BITS(data_ptr, bits_cache, bits_count,
			frame->scale_factor[sb] & 0x0F, 4);
		crc_header[crc_pos >> 3] <<= 4;
		crc_header[crc_pos >> 3] |= frame->scale_factor[sb] & 0x0F;
		crc_pos += 4;
	}

	/* align the last crc byte */
	if (crc_pos % 8)
		crc_header[crc_pos >> 3] <<= 8 - (crc_pos % 8);

	data[3] = sbc_crc8(crc_header, crc_pos);

	sbc_calculate_bits(frame, bits);

	for (sb = 0; sb < frame_subbands; sb++) {
		levels[sb] = ((1 << bits[sb]) - 1) <<
			(32 - (frame->scale_factor[sb] +
				SCALE_OUT_BITS + 2));
		sb_sample_delta[sb] = (uint32_t) 1 <<
			(frame->scale_factor[sb] +
				SCALE_OUT_BITS + 1);
	}

	for (blk = 0; blk < frame->blocks; blk++) {
		for (sb = 0; sb < frame_subbands; sb++) {

			if (bits[sb] == 0)
				continue;

			audio_sample = ((uint64_t) levels[sb] *
				(sb_sample_delta[sb] +
				frame->sb_sample_f[blk][sb])) >> 32;

			PUT_BITS(data_ptr, bits_cache, bits_count,
				audio_sample, bits[sb]);
		}
	}

	FLUSH_BITS(data_ptr, bits_cache, bits_count);

	return data_ptr - data;
}

static int sbc_pack_frame(uint8_t *data, struct sbc_frame *frame)
{
	int frame_subbands = 4;

	data[0] = SBC_SYNCWORD;

	data[1] = (frame->frequency & 0x03) << 6;
	data[1] |= (frame->block_mode & 0x03) << 4;
	data[1] |= (0x00/*SBC_MODE_MONO*/ & 0x03) << 2;
	data[1] |= (frame->allocation & 0x01) << 1;

	data[2] = frame->bitpool;

	if (frame->subbands != 4)
		frame_subbands = 8;

	if (frame->bitpool > frame_subbands << 4)
		return -5;


	if (frame->subbands == 4) {
		return sbc_pack_frame_internal(
			data, frame, 4, 1);
	} else {
		data[1] |= 0x01;
		return sbc_pack_frame_internal(
			data, frame, 8, 1);
	}
}

static int msbc_pack_frame(uint8_t *data, struct sbc_frame *frame)
{
	data[0] = MSBC_SYNCWORD;
	data[1] = 0;
	data[2] = 0;

	return sbc_pack_frame_internal(data, frame, 8, 1);
}

static void sbc_encoder_init(bool msbc, struct sbc_encoder_state *state,
						const struct sbc_frame *frame)
{
	memset(&state->X, 0, sizeof(state->X));
	state->position = (SBC_X_BUFFER_SIZE - frame->subbands * 9) & ~7;
	if (msbc)
		state->increment = 1;
	else
		state->increment = 4;

	sbc_init_primitives(state);
}

struct sbc_priv {
	bool init;
	bool msbc;
	struct SBC_ALIGNED sbc_frame frame;
	struct SBC_ALIGNED sbc_encoder_state enc_state;

	int (*pack_frame)(uint8_t *data, struct sbc_frame *frame);
};

static void sbc_set_defaults(sbc_t *sbc)
{
	struct sbc_priv *priv = sbc->priv;

	if (priv->msbc) {
		priv->pack_frame = msbc_pack_frame;
	} else {
		priv->pack_frame = sbc_pack_frame;
	}

	sbc->frequency = SBC_FREQ_44100;
	sbc->subbands = SBC_SB_8;
	sbc->blocks = SBC_BLK_16;
	sbc->bitpool = 32;  //2~250
}

int sbc_init(sbc_t *sbc, void *buffer)
{
	if (!sbc)
		return -EIO;

	memset(sbc, 0, sizeof(sbc_t));

	//sbc->priv_alloc_base = ke_malloc(sizeof(struct sbc_priv) + SBC_ALIGN_MASK, KE_MEM_ENV);
	//if (!sbc->priv_alloc_base)
	//	return -ENOMEM;
	sbc->priv_alloc_base = buffer;

	sbc->priv = (void *) (((uintptr_t) sbc->priv_alloc_base +
			SBC_ALIGN_MASK) & ~((uintptr_t) SBC_ALIGN_MASK));

	memset(sbc->priv, 0, sizeof(struct sbc_priv));

	sbc_set_defaults(sbc);

	return 0;
}

int sbc_init_msbc(sbc_t *sbc, void *buffer)
{
	struct sbc_priv *priv;

	if (!sbc)
		return -EIO;

	memset(sbc, 0, sizeof(sbc_t));

	sbc->priv_alloc_base = buffer;

	sbc->priv = (void *) (((uintptr_t) sbc->priv_alloc_base +
			SBC_ALIGN_MASK) & ~((uintptr_t) SBC_ALIGN_MASK));

	memset(sbc->priv, 0, sizeof(struct sbc_priv));

	priv = sbc->priv;
	priv->msbc = true;

	sbc_set_defaults(sbc);

	sbc->frequency = SBC_FREQ_16000;
	sbc->blocks = MSBC_BLOCKS;
	sbc->subbands = SBC_SB_8;
	sbc->allocation = SBC_AM_LOUDNESS;
	sbc->bitpool = 26;

	return 0;
}

int sbc_encode(sbc_t *sbc, const void *input, uint32_t input_len,
			void *output, uint32_t output_len, int *written)
{
	struct sbc_priv *priv;
	int samples;
	int framelen;
	int (*sbc_enc_process_input)(int position,
			const uint8_t *pcm, int16_t X[SBC_X_BUFFER_SIZE],
			int nsamples);

	if (!sbc || !input)
		return -EIO;

	priv = sbc->priv;

	if (written)
		*written = 0;

	if (!priv->init) {
		priv->frame.frequency = sbc->frequency;
		priv->frame.allocation = sbc->allocation;
		priv->frame.subband_mode = sbc->subbands;
		priv->frame.subbands = sbc->subbands ? 8 : 4;
		priv->frame.block_mode = sbc->blocks;
		if (priv->msbc)
			priv->frame.blocks = MSBC_BLOCKS;
		else
			priv->frame.blocks = 4 + (sbc->blocks * 4);
		priv->frame.bitpool = sbc->bitpool;
		priv->frame.codesize = sbc_get_codesize(sbc);
		priv->frame.length = sbc_get_frame_length(sbc);

		sbc_encoder_init(priv->msbc, &priv->enc_state, &priv->frame);
		priv->init = true;
	} else if (priv->frame.bitpool != sbc->bitpool) {
		priv->frame.length = sbc_get_frame_length(sbc);
		priv->frame.bitpool = sbc->bitpool;
	}

	/* input must be large enough to encode a complete frame */
	if (input_len < priv->frame.codesize)
		return 0;

	/* output must be large enough to receive the encoded frame */
	if (!output || output_len < priv->frame.length)
		return -ENOSPC;

	/* Select the needed input data processing function and call it */
	if (priv->frame.subbands == 8) {
		sbc_enc_process_input =
			priv->enc_state.sbc_enc_process_input_8s_le;
	} else {
		sbc_enc_process_input =
			priv->enc_state.sbc_enc_process_input_4s_le;
	}

	priv->enc_state.position = sbc_enc_process_input(
		priv->enc_state.position, (const uint8_t *) input,
		priv->enc_state.X, priv->frame.subbands * priv->frame.blocks);

	samples = sbc_analyze_audio(&priv->enc_state, &priv->frame);

	priv->enc_state.sbc_calc_scalefactors(
		priv->frame.sb_sample_f, priv->frame.scale_factor,
		priv->frame.blocks, priv->frame.subbands);
	framelen = priv->pack_frame(output, &priv->frame);

	if (written)
		*written = framelen;

	return samples * 2;
}

void sbc_finish(sbc_t *sbc)
{
	if (!sbc)
		return;

	//ke_free(sbc->priv_alloc_base);

	memset(sbc, 0, sizeof(sbc_t));
}

size_t sbc_get_frame_length(sbc_t *sbc)
{
	int ret;
	uint8_t subbands, blocks, bitpool;
	struct sbc_priv *priv;

	priv = sbc->priv;
	if (priv->init && priv->frame.bitpool == sbc->bitpool)
		return priv->frame.length;

	subbands = sbc->subbands ? 8 : 4;
	if (priv->msbc)
		blocks = MSBC_BLOCKS;
	else
		blocks = 4 + (sbc->blocks * 4);
    
	bitpool = sbc->bitpool;

	ret = 4 + (4 * subbands) / 8;

    ret += ((blocks * bitpool) + 7) / 8;

	return ret;
}

size_t sbc_get_codesize(sbc_t *sbc)
{
	uint16_t subbands, blocks;
	struct sbc_priv *priv;

	priv = sbc->priv;
	if (!priv->init) {
		subbands = sbc->subbands ? 8 : 4;
		if (priv->msbc)
			blocks = MSBC_BLOCKS;
		else
			blocks = 4 + (sbc->blocks * 4);
	} else {
		subbands = priv->frame.subbands;
		blocks = priv->frame.blocks;
	}

	return subbands * blocks * 2;
}

int sbc_reinit(sbc_t *sbc)
{
	struct sbc_priv *priv;

	if (!sbc || !sbc->priv)
		return -EIO;

	priv = sbc->priv;

	if (priv->init)
		memset(sbc->priv, 0, sizeof(struct sbc_priv));

	sbc_set_defaults(sbc);

	return 0;
}

