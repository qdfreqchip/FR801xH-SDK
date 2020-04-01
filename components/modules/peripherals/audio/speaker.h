#ifndef _SPEAKER_H
#define _SPEAKER_H
#include <stdint.h>

// <o> Number of Audio  Buffers <2-20>
#define CONFIG_AUDIO_DECODER_BLOCKNUM 30
// <o> Size of Audio  Buffers <2-100>
#define CONFIG_AUDIO_DECODER_BLOCKSIZE 64

void app_demo_audio(void);
void speaker_init(void);
void speaker_start_hw(void);
void speaker_stop_hw(void);
void encoder_frame_out_func(uint8_t *frame_data,uint16_t frame_size);
void PA_init_pins(void);
void Test_Codec_demo(void);
void Test_codec_demo_stop(void);

#endif
