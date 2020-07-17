#ifndef  __AUDIO_DECODE_H
#define  __AUDIO_DECODE_H
#include <stdint.h>

#include "co_list.h"

#define CFG_DEC_SBC

#define DECODER_STORE_TYPE_RAM      0
#define DECODER_STORE_TYPE_FLASH    1
#define DECODER_STORE_TYPE_BLE      2

struct decoder_stop_t
{
    uint8_t null;
};

struct decoder_play_next_frame_t
{
    uint8_t null;
};

struct decoder_prepare_t
{
    uint32_t data_start;
    uint32_t data_end;
    uint32_t tot_data_len;
    uint32_t start_offset;
    uint8_t store_type;
#ifdef CFG_DEC_ADPCM_MS
    uint16_t frame_len;
#endif
};

struct decoder_env_t
{
    struct co_list pcm_buffer_list;
    struct co_list sbc_buffer_list;

    void * decoder_context;
    uint32_t data_start;
    uint32_t data_end;
    uint32_t tot_data_len;
    uint32_t data_processed_len;

    uint32_t current_pos;
    uint16_t frame_len;
    uint8_t store_type;
    uint8_t pcm_buffer_counter;
};

struct decoder_pcm_t
{
    struct co_list_hdr list;
    uint16_t pcm_size;
    uint16_t pcm_offset;
#ifdef CFG_DEC_SBC
    uint32_t pcm_data[1];
#endif
#ifdef CFG_DEC_ADPCM_MS
    uint16_t pcm_data[1];
#endif
};

struct decoder_sbc_t
{
    struct co_list_hdr list;
    uint16_t length;
    uint16_t offset;
    uint8_t buffer[1];
};

__packed struct sbc_header_t
{
    uint8_t reserved;
    __packed struct
    {
        uint8_t subband:1;
        uint8_t reserved:1;
        uint8_t m_s:2;
        uint8_t block:2;
        uint8_t reserved2:2;
    } c;
    uint8_t bitpool;
};

extern struct decoder_env_t decoder_env;
extern uint16_t task_id_audio_decode;
typedef struct
{
    uint32_t start;
    uint32_t end;
    uint32_t tot_data_len;
    uint32_t start_offset;
    uint8_t type;
    void(* half_buff_cb)(void);
    void(* end_cb)(void);
} decoder_buff_t;
void audio_decoder_play_next_frame(void);


void audio_decoder_init(void);
void audio_decoder_start(decoder_buff_t buf_env);
void audio_decoder_update_date_len(uint32_t len);
void audio_decoder_start_realtime(uint8_t *buff,uint16_t size);


uint16_t audio_sw_decoder_init(uint8_t *buff_start);
uint8_t audio_sw_decoder_buff(uint8_t *frame_buffer,uint16_t frame_len,uint8_t *out_buffer);
void audio_sw_decoder_deinit(void);


#endif

