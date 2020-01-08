#ifndef _SPEAKER_H
#define _SPEAKER_H
#include <stdint.h>
void app_demo_audio(void);
void speaker_init(void);
void speaker_start_hw(void);
void speaker_stop_hw(void);
void encoder_frame_out_func(uint8_t *frame_data,uint16_t frame_size);
void PA_init_pins(void);

#endif
