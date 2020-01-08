#ifndef  __DECODE_H
#define  __DECODE_H

#include "os_timer.h"
#include "co_printf.h"
#include <stdint.h>
#include "co_list.h"

typedef struct
{
    uint32_t start_base;
    uint32_t tot_data_len;
    uint32_t last_offset;
    uint8_t last_page_idx;
} sbc_store_info_t;

typedef struct
{
    uint32_t last_read_offset;
    uint8_t last_read_page_idx;
    uint8_t end_flag;

    uint32_t store_data_len;
    uint16_t sbc_frame_len;
    uint16_t sbc_data_tot_len;
} speaker_env_t;

extern speaker_env_t speaker_env ;

extern sbc_store_info_t sbc_sotre_env;

#define FLASH_PAGE_SIZE (0x1000)
#define USER_FLASH_BASE_ADDR (0x60000)


struct decoder_prepare_t
{
    uint32_t data_start;
    uint32_t data_end;
    uint32_t tot_data_len;
    uint32_t start_offset;
    uint8_t store_type;

    uint16_t frame_len;
};

struct decoder_pcm_t
{
    struct co_list_hdr list;
    uint16_t pcm_size;
    uint16_t pcm_offset;


    uint16_t pcm_data[1];

};

struct decoder_env_t
{
    struct co_list pcm_buffer_list;

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
extern struct decoder_env_t decoder_env;
extern uint8_t stop_flag ;
enum decoder_state_t
{
    DECODER_STATE_IDLE,
    DECODER_STATE_BUFFERING,
    DECODER_STATE_PLAYING,
    DECODER_STATE_WAITING_END,

    DECODER_STATE_MAX,
};

extern uint8_t decodeTASKState;
extern bool decoder_hold_flag;
extern uint8_t *sbc_buff ;
extern uint8_t Flash_data_state;





void decoder_play_next_frame_handler(void *arg);
void decoder_play_next_frame(void);
void test_speaker_from_flash(void);
void decoder_end_func(void);
void test_end_speaker(void);





#endif

