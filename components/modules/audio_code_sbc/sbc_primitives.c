/*
 *
 *  Bluetooth low-complexity, subband codec (SBC) library
 *
 *  Copyright (C) 2008-2010  Nokia Corporation
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2004-2005  Henryk Ploetz <henryk@ploetzli.ch>
 *  Copyright (C) 2005-2006  Brad Midgley <bmidgley@xmission.com>
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

#include <stdint.h>
#include <limits.h>
#include <string.h>
#include "sbc.h"
#include "sbc_math.h"
#include "sbc_tables.h"

#include "sbc_primitives.h"

/*
 * A reference C code of analysis filter with SIMD-friendly tables
 * reordering and code layout. This code can be used to develop platform
 * specific SIMD optimizations. Also it may be used as some kind of test
 * for compiler autovectorization capabilities (who knows, if the compiler
 * is very good at this stuff, hand optimized assembly may be not strictly
 * needed for some platform).
 *
 * Note: It is also possible to make a simple variant of analysis filter,
 * which needs only a single constants table without taking care about
 * even/odd cases. This simple variant of filter can be implemented without
 * input data permutation. The only thing that would be lost is the
 * possibility to use pairwise SIMD multiplications. But for some simple
 * CPU cores without SIMD extensions it can be useful. If anybody is
 * interested in implementing such variant of a filter, sourcecode from
 * bluez versions 4.26/4.27 can be used as a reference and the history of
 * the changes in git repository done around that time may be worth checking.
 */

static __asm void sbc_analyze_four_simd(const int16_t *in, int32_t *out, const int16_t *consts)
{
    PRESERVE8

    push {r3-r9, lr}

    mov r5, #0x8000
    mov r6, #0x8000
    mov r7, #0x8000
    mov r8, #0x8000

    movs r9, #0

a1_4_loop
    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r5, r3, r4, r5
    adds r9, r9, #2
    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r5, r3, r4, r5
    adds r9, r9, #2

    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r6, r3, r4, r6
    adds r9, r9, #2
    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r6, r3, r4, r6
    adds r9, r9, #2

    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r7, r3, r4, r7
    adds r9, r9, #2
    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r7, r3, r4, r7
    adds r9, r9, #2

    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r8, r3, r4, r8
    adds r9, r9, #2
    ldrsh r3, [r0, r9]
    ldrsh r4, [r2, r9]
    mla r8, r3, r4, r8
    adds r9, r9, #2

    movs r3, #80
    cmp r9, r3
    bne a1_4_loop

    asrs r5, r5, #16
    asrs r6, r6, #16
    asrs r7, r7, #16
    asrs r8, r8, #16

    adds r2, r2, r9
    ldrsh r0, [r2, #0]
    mul r3, r0, r5
    ldrsh r0, [r2, #2]
    mla r3, r0, r6, r3
    ldrsh r0, [r2, #16]
    mla r3, r0, r7, r3
    ldrsh r0, [r2, #18]
    mla r3, r0, r8, r3
    str r3, [r1]

    adds r2, r2, #4
    ldrsh r0, [r2, #0]
    mul r3, r0, r5
    ldrsh r0, [r2, #2]
    mla r3, r0, r6, r3
    ldrsh r0, [r2, #16]
    mla r3, r0, r7, r3
    ldrsh r0, [r2, #18]
    mla r3, r0, r8, r3
    str r3, [r1, #4]

    adds r2, r2, #4
    ldrsh r0, [r2, #0]
    mul r3, r0, r5
    ldrsh r0, [r2, #2]
    mla r3, r0, r6, r3
    ldrsh r0, [r2, #16]
    mla r3, r0, r7, r3
    ldrsh r0, [r2, #18]
    mla r3, r0, r8, r3
    str r3, [r1, #8]

    adds r2, r2, #4
    ldrsh r0, [r2, #0]
    mul r3, r0, r5
    ldrsh r0, [r2, #2]
    mla r3, r0, r6, r3
    ldrsh r0, [r2, #16]
    mla r3, r0, r7, r3
    ldrsh r0, [r2, #18]
    mla r3, r0, r8, r3
    str r3, [r1, #12]

    pop {r3-r9, pc}
}

static __asm void sbc_analyze_eight_simd(const int16_t *in, int32_t *out, const int16_t *consts)
{
    PRESERVE8

    push {r3-r12, lr}
    push {r1}

    mov r5, #0x8000
    mov r6, #0x8000
    mov r7, #0x8000
    mov r8, #0x8000
    mov r9, #0x8000
    mov r10, #0x8000
    mov r11, #0x8000
    mov r12, #0x8000

    movs r1, #0

a1_8_loop
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r5, r3, r4, r5
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r5, r3, r4, r5
    adds r1, r1, #2

    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r6, r3, r4, r6
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r6, r3, r4, r6
    adds r1, r1, #2

    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r7, r3, r4, r7
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r7, r3, r4, r7
    adds r1, r1, #2

    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r8, r3, r4, r8
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r8, r3, r4, r8
    adds r1, r1, #2
    
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r9, r3, r4, r9
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r9, r3, r4, r9
    adds r1, r1, #2

    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r10, r3, r4, r10
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r10, r3, r4, r10
    adds r1, r1, #2

    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r11, r3, r4, r11
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r11, r3, r4, r11
    adds r1, r1, #2

    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r12, r3, r4, r12
    adds r1, r1, #2
    ldrsh r3, [r0, r1]
    ldrsh r4, [r2, r1]
    mla r12, r3, r4, r12
    adds r1, r1, #2

    movs r3, #160
    cmp r1, r3
    bne a1_8_loop

    sub sp, sp, #16
    mov r3, sp
    
    asrs r5, r5, #16
    strh r5, [r3]
    asrs r6, r6, #16
    strh r6, [r3, #2]
    asrs r7, r7, #16
    strh r7, [r3, #4]
    asrs r8, r8, #16
    strh r8, [r3, #6]
    asrs r9, r9, #16
    strh r9, [r3, #8]
    asrs r10, r10, #16
    strh r10, [r3, #10]
    asrs r11, r11, #16
    strh r11, [r3, #12]
    asrs r12, r12, #16
    strh r12, [r3, #14]

    movs r5, #0
    movs r6, #0
    movs r7, #0
    movs r8, #0
    movs r9, #0
    movs r10, #0
    movs r11, #0
    movs r12, #0

a1_8_loop_2
    ldrsh r0, [r3]
    ldrsh r4, [r3, #2]
    
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r5, r0, r3, r5
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r5, r4, r3, r5

    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r6, r0, r3, r6
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r6, r4, r3, r6

    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r7, r0, r3, r7
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r7, r4, r3, r7

    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r8, r0, r3, r8
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r8, r4, r3, r8

    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r9, r0, r3, r9
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r9, r4, r3, r9

    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r10, r0, r3, r10
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r10, r4, r3, r10

    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r11, r0, r3, r11
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r11, r4, r3, r11

    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r12, r0, r3, r12
    ldrsh r3, [r2, r1]
    adds r1, r1, #2
    mla r12, r4, r3, r12

    add sp, sp, #4
    mov r3, sp
    mov r0, #288
    cmp r1, r0
    blt a1_8_loop_2

    pop {r1}
    stmia r1!, {r5-r12}

    pop {r3-r12, pc}
}

static inline void sbc_analyze_4b_4s_simd(struct sbc_encoder_state *state,
		int16_t *x, int32_t *out, int out_stride)
{
	/* Analyze blocks */
	sbc_analyze_four_simd(x + 12, out, analysis_consts_fixed4_simd_odd);
	out += out_stride;
	sbc_analyze_four_simd(x + 8, out, analysis_consts_fixed4_simd_even);
	out += out_stride;
	sbc_analyze_four_simd(x + 4, out, analysis_consts_fixed4_simd_odd);
	out += out_stride;
	sbc_analyze_four_simd(x + 0, out, analysis_consts_fixed4_simd_even);
}

static inline void sbc_analyze_4b_8s_simd(struct sbc_encoder_state *state,
		int16_t *x, int32_t *out, int out_stride)
{
	/* Analyze blocks */
	sbc_analyze_eight_simd(x + 24, out, analysis_consts_fixed8_simd_odd);
	out += out_stride;
	sbc_analyze_eight_simd(x + 16, out, analysis_consts_fixed8_simd_even);
	out += out_stride;
	sbc_analyze_eight_simd(x + 8, out, analysis_consts_fixed8_simd_odd);
	out += out_stride;
	sbc_analyze_eight_simd(x + 0, out, analysis_consts_fixed8_simd_even);
}

static inline void sbc_analyze_1b_8s_simd_even(struct sbc_encoder_state *state,
		int16_t *x, int32_t *out, int out_stride);

static inline void sbc_analyze_1b_8s_simd_odd(struct sbc_encoder_state *state,
		int16_t *x, int32_t *out, int out_stride)
{
	sbc_analyze_eight_simd(x, out, analysis_consts_fixed8_simd_odd);
	state->sbc_analyze_8s = sbc_analyze_1b_8s_simd_even;
}

static inline void sbc_analyze_1b_8s_simd_even(struct sbc_encoder_state *state,
		int16_t *x, int32_t *out, int out_stride)
{
	sbc_analyze_eight_simd(x, out, analysis_consts_fixed8_simd_even);
	state->sbc_analyze_8s = sbc_analyze_1b_8s_simd_odd;
}

/*
 * Internal helper functions for input data processing. In order to get
 * optimal performance, it is important to have "nsamples", "nchannels"
 * and "big_endian" arguments used with this inline function as compile
 * time constants.
 */

static SBC_ALWAYS_INLINE int sbc_encoder_process_input_s4_internal(
	int position,
	const uint8_t *pcm, int16_t X[SBC_X_BUFFER_SIZE],
	int nsamples)
{
    const uint16_t *pcm_t = (const uint16_t *)pcm;
    
	/* handle X buffer wraparound */
	if (position < nsamples) {
		memcpy(&X[SBC_X_BUFFER_SIZE - 40], &X[position],
						36 * sizeof(int16_t));
		position = SBC_X_BUFFER_SIZE - 40;
	}

	#define PCM(i)  pcm_t[i]

	/* copy/permutate audio samples */
	while ((nsamples -= 8) >= 0) {
		position -= 8;
		int16_t *x = &X[position];
		x[0]  = PCM(7);
		x[1]  = PCM(3);
		x[2]  = PCM(6);
		x[3]  = PCM(4);
		x[4]  = PCM(0);
		x[5]  = PCM(2);
		x[6]  = PCM(1);
		x[7]  = PCM(5);
		pcm_t += 8;
	}
	#undef PCM

	return position;
}

static SBC_ALWAYS_INLINE int sbc_encoder_process_input_s8_internal(
	int position,
	const uint8_t *pcm, int16_t X[SBC_X_BUFFER_SIZE],
	int nsamples)
{
    const uint16_t *pcm_t = (const uint16_t *)pcm;
    
	/* handle X buffer wraparound */
	if (position < nsamples) {
		memcpy(&X[SBC_X_BUFFER_SIZE - 72], &X[position],
						72 * sizeof(int16_t));
		position = SBC_X_BUFFER_SIZE - 72;
	}

	#define PCM(i)  pcm_t[i]

	if (position % 16 == 8) {
		position -= 8;
		nsamples -= 8;
		int16_t *x = &X[position];
		x[0]  = PCM(7);
		x[2]  = PCM(6);
		x[3]  = PCM(0);
		x[4]  = PCM(5);
		x[5]  = PCM(1);
		x[6]  = PCM(4);
		x[7]  = PCM(2);
		x[8]  = PCM(3);

		pcm_t += 8;
	}

	/* copy/permutate audio samples */
	while (nsamples >= 16) {
		position -= 16;
		int16_t *x = &X[position];
		x[0]  = PCM(15);
		x[1]  = PCM(7);
		x[2]  = PCM(14);
		x[3]  = PCM(8);
		x[4]  = PCM(13);
		x[5]  = PCM(9);
		x[6]  = PCM(12);
		x[7]  = PCM(10);
		x[8]  = PCM(11);
		x[9]  = PCM(3);
		x[10] = PCM(6);
		x[11] = PCM(0);
		x[12] = PCM(5);
		x[13] = PCM(1);
		x[14] = PCM(4);
		x[15] = PCM(2);
		pcm_t += 16;
		nsamples -= 16;
	}

	if (nsamples == 8) {
		position -= 8;
		int16_t *x = &X[position];
		x[-7] = PCM(7);
		x[1]  = PCM(3);
		x[2]  = PCM(6);
		x[3]  = PCM(0);
		x[4]  = PCM(5);
		x[5]  = PCM(1);
		x[6]  = PCM(4);
		x[7]  = PCM(2);
	}
	#undef PCM

	return position;
}

/*
 * Input data processing functions. The data is endian converted if needed,
 * channels are deintrleaved and audio samples are reordered for use in
 * SIMD-friendly analysis filter function. The results are put into "X"
 * array, getting appended to the previous data (or it is better to say
 * prepended, as the buffer is filled from top to bottom). Old data is
 * discarded when neededed, but availability of (10 * nrof_subbands)
 * contiguous samples is always guaranteed for the input to the analysis
 * filter. This is achieved by copying a sufficient part of old data
 * to the top of the buffer on buffer wraparound.
 */

static int sbc_enc_process_input_4s_le(int position,
		const uint8_t *pcm, int16_t X[SBC_X_BUFFER_SIZE],
		int nsamples)
{
	return sbc_encoder_process_input_s4_internal(
		position, pcm, X, nsamples);
}

static int sbc_enc_process_input_8s_le(int position,
		const uint8_t *pcm, int16_t X[SBC_X_BUFFER_SIZE],
		int nsamples)
{
	return sbc_encoder_process_input_s8_internal(
		position, pcm, X, nsamples);
}

/* Supplementary function to count the number of leading zeros */

static inline int sbc_clz(uint32_t x)
{
#ifdef __GNUC__
	return __builtin_clz(x);
#else
	/* TODO: this should be replaced with something better if good
	 * performance is wanted when using compilers other than gcc */
	int cnt = 0;
	while (x) {
		cnt++;
		x >>= 1;
	}
	return 32 - cnt;
#endif
}

static void sbc_calc_scalefactors(
	int32_t sb_sample_f[16][8],
	uint32_t scale_factor[8],
	int blocks, int subbands)
{
	int sb, blk;
    
	for (sb = 0; sb < subbands; sb++) {
		uint32_t x = 1 << SCALE_OUT_BITS;
		for (blk = 0; blk < blocks; blk++) {
			int32_t tmp = fabs(sb_sample_f[blk][sb]);
			if (tmp != 0)
				x |= tmp - 1;
		}
		scale_factor[sb] = (31 - SCALE_OUT_BITS) -
			sbc_clz(x);
	}
}

/*
 * Detect CPU features and setup function pointers
 */
void sbc_init_primitives(struct sbc_encoder_state *state)
{
	/* Default implementation for analyze functions */
	state->sbc_analyze_4s = sbc_analyze_4b_4s_simd;
	if (state->increment == 1)
		state->sbc_analyze_8s = sbc_analyze_1b_8s_simd_odd;
	else
		state->sbc_analyze_8s = sbc_analyze_4b_8s_simd;

	/* Default implementation for input reordering / deinterleaving */
	state->sbc_enc_process_input_4s_le = sbc_enc_process_input_4s_le;
	state->sbc_enc_process_input_8s_le = sbc_enc_process_input_8s_le;

	/* Default implementation for scale factors calculation */
	state->sbc_calc_scalefactors = sbc_calc_scalefactors;

	/* ARM optimizations */
#ifdef SBC_BUILD_WITH_ARMV6_SUPPORT
	sbc_init_primitives_armv6(state);
#endif
}

