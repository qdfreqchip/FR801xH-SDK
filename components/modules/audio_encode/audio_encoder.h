#ifndef  __AUDIO_ENCODE_H
#define  __AUDIO_ENCODE_H

#define ENCODER_MAX_BUFFERING_BLOCK_COUNT       10

enum encode_type
{
    ENCODE_TYPE_ADPCM,
    ENCODE_TYPE_SBC,
};

enum encode_freq
{
    ENC_FREQ_16000 = 0,
    ENC_FREQ_32000 = 1,
    ENC_FREQ_44100 = 2,
    ENC_FREQ_48000 = 3,
};
/*
    bitpool frame_len
    14      36
    21      50
    25      58
    29      66
*/
typedef struct
{
    enum encode_freq freq;
    uint8_t bitpool;
} encode_param_t;

void audio_encoder_init(enum encode_type type);
void audio_encode_start(encode_param_t param);
void audio_encode_stop(void);
void audio_encode_store_pcm_data(uint16_t *data, uint8_t len);


extern uint16_t task_id_audio_encode;


#endif

