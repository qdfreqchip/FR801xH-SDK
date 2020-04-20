#ifndef _ADPCM_H
#define _ADPCM_H


#include <stdint.h>

typedef unsigned int uint;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

struct CodecState
{
	int valprev;
	int index;
};

void decode(struct CodecState* state, u8* input, int numSamples, s16* output);
void encode(struct CodecState* state, s16* input, int numSamples, u8* output);


#endif

