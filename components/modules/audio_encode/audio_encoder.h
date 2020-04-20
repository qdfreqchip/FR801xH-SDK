#ifndef  __AUDIO_ENCODE_H
#define  __AUDIO_ENCODE_H

#define ENCODER_MAX_BUFFERING_BLOCK_COUNT       10


void audio_encoder_init(void);
void audio_encode_start(void);
void audio_encode_stop(void);
void audio_encode_store_pcm_data(uint16_t *data, uint8_t len);


extern uint16_t task_id_audio_encode;


#endif

