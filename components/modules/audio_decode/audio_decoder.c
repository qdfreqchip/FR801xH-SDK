/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
 * INCLUDES
 */
#include <stdint.h>
#include <string.h>
#include "driver_plf.h"

//#include "adpcm.h"
#include "co_list.h"
#include "co_printf.h"
#include "os_task.h"
#include "os_mem.h"
#include "audio_decoder.h"
#include "oi_codec_sbc.h"
#include "driver_uart.h"
#include "sys_utils.h"


#define DEC_ASSERT(v) do { \
    if (!(v)) {             \
        co_printf("%s %d \r\n", __FILE__, __LINE__); \
        while (1) {};   \
    }                   \
} while (0);


enum decoder_event_t
{
    DECODER_EVENT_PREPARE,
    DECODER_EVENT_NEXT_FRAME,
    DECODER_EVENT_STOP,
};

enum decoder_state_t
{
    DECODER_STATE_IDLE,
    DECODER_STATE_BUFFERING,
    DECODER_STATE_PLAYING,
    DECODER_STATE_WAITING_END,
    DECODER_STATE_MAX,
};


static bool decoder_hold_flag = false;
struct decoder_env_t decoder_env;
static enum decoder_state_t decode_task_status = DECODER_STATE_IDLE;
uint16_t task_id_audio_decode = TASK_ID_NONE;
static void(* decoder_end_cb_fn)(void) = NULL;
static void(* decoder_half_buff_fn)(void) = NULL;

uint16_t audio_decoder_calc_sbc_frame_len(struct sbc_header_t *header)
{
    uint8_t m_s = header->c.m_s;
    uint8_t subband = header->c.subband ? 8:4;
    uint8_t block;
    uint8_t bitpool = header->bitpool;

    switch(header->c.block)
    {
        case 0:
            block = 4;
            break;
        case 1:
            block = 8;
            break;
        case 2:
            block = 12;
            break;
        case 3:
            block = 16;
            break;
        default:
            block = 4;
            break;
    }

    switch(m_s)
    {
        case 0:
            return (4+subband/2+(block*bitpool+7)/8);
        case 1:
            return (4+subband+(block*bitpool*2+7)/8);
        case 2:
            return (4+subband+(block*bitpool+7)/8);
        case 3:
            return (4+subband+(subband+block*bitpool+7)/8);
        default:
            return 0;
    }
}

void audio_decoder_start(decoder_buff_t buf_env)
{
    struct decoder_prepare_t *param = (struct decoder_prepare_t *)os_msg_malloc(DECODER_EVENT_PREPARE,task_id_audio_decode,TASK_ID_NONE
                                      ,sizeof(struct decoder_prepare_t) );
    param->data_start = buf_env.start;
    param->data_end = buf_env.end;
    param->store_type = buf_env.type;
    param->tot_data_len = buf_env.tot_data_len;
    param->start_offset = buf_env.start_offset;

    decoder_end_cb_fn = buf_env.end_cb;
    decoder_half_buff_fn = buf_env.half_buff_cb;
    os_msg_send((void *)param);
    decoder_hold_flag = false;
    co_printf("decoder_start msg\r\n");
}
void audio_decoder_start_realtime(uint8_t *data,uint16_t length)
{
    uart_putc_noint(UART1,'i');
    if( decode_task_status == DECODER_STATE_IDLE )
    {
        uint32_t size = (sizeof(OI_CODEC_SBC_DECODER_CONTEXT) + 3) & (~0x03);
        decoder_env.decoder_context = os_malloc(size + 2048*sizeof(uint32_t));
        OI_CODEC_SBC_DecoderReset((OI_CODEC_SBC_DECODER_CONTEXT *)decoder_env.decoder_context,
                                  (OI_UINT32 *)((uint8_t *)decoder_env.decoder_context+size),
                                  2048,
                                  2,
                                  2,
                                  FALSE);
        decoder_env.store_type = DECODER_STORE_TYPE_BLE;
        decoder_env.frame_len = audio_decoder_calc_sbc_frame_len((struct sbc_header_t *)data);
        co_printf("preparing,fram_len:%d\r\n",decoder_env.frame_len);
        co_list_init(&decoder_env.pcm_buffer_list);
        co_list_init(&decoder_env.sbc_buffer_list);
        decoder_env.pcm_buffer_counter = 0;
        decode_task_status = DECODER_STATE_BUFFERING;
    }
    struct decoder_sbc_t *sbc_data = (struct decoder_sbc_t *)os_malloc(sizeof(struct decoder_sbc_t) + length);
    memcpy(&sbc_data->buffer[0], data, length);
    sbc_data->length = length;
    sbc_data->offset = 0;
    co_list_push_back(&decoder_env.sbc_buffer_list,&sbc_data->list);
    audio_decoder_play_next_frame();
}

void audio_decoder_stop(void)
{
    co_printf("decoder_stop msg\r\n");
    struct decoder_stop_t *param = (struct decoder_stop_t *)os_msg_malloc(DECODER_EVENT_STOP, task_id_audio_decode,
                                   TASK_ID_NONE, sizeof(struct decoder_stop_t));
    os_msg_send((void *)param);
}
void audio_decoder_play_next_frame(void)
{
    struct decoder_play_next_frame_t *param = (struct decoder_play_next_frame_t *)os_msg_malloc(DECODER_EVENT_NEXT_FRAME
            , task_id_audio_decode,TASK_ID_NONE, sizeof(struct decoder_play_next_frame_t));
    os_msg_send((void *)param);
}


static void audio_decoder_prepare_handler(struct decoder_prepare_t *param)
{
    switch(decode_task_status)
    {
        case DECODER_STATE_IDLE:
        {
            uint32_t size = (sizeof(OI_CODEC_SBC_DECODER_CONTEXT) + 3) & (~0x03);
            decoder_env.decoder_context = os_malloc(size + 2048*sizeof(uint32_t));
            OI_CODEC_SBC_DecoderReset((OI_CODEC_SBC_DECODER_CONTEXT *)decoder_env.decoder_context,
                                      (OI_UINT32 *)((uint8_t *)decoder_env.decoder_context+size),
                                      2048,
                                      2,
                                      2,
                                      FALSE);
            // TBD, init iis codec
            decoder_env.data_start = param->data_start;
            decoder_env.data_end = param->data_end;
            decoder_env.current_pos = param->data_start + param->start_offset;
            decoder_env.tot_data_len = param->tot_data_len;
            decoder_env.store_type = param->store_type;
            decoder_env.data_processed_len = 0;
            decoder_env.frame_len = audio_decoder_calc_sbc_frame_len((struct sbc_header_t *)decoder_env.data_start);

            co_printf("preparing,fram_len:%d\r\n",decoder_env.frame_len);
            co_list_init(&decoder_env.pcm_buffer_list);

            decoder_env.pcm_buffer_counter = 0;
            audio_decoder_play_next_frame();
            decode_task_status = DECODER_STATE_BUFFERING;
        }
        break;
    }
}
void audio_decoder_update_date_len(uint32_t len)
{
    decoder_env.tot_data_len = len;
}

static void audio_decoder_play_next_frame_handler(struct decoder_play_next_frame_t *param)
{
    uint8_t *buffer;
    uint32_t pcm_len;
    uint32_t streamlen;
    struct decoder_pcm_t *pcm_frame;
    struct decoder_sbc_t *sbc_data = NULL;

    switch(decode_task_status)
    {
        case DECODER_STATE_IDLE:
            break;
        case DECODER_STATE_BUFFERING:
        case DECODER_STATE_PLAYING:

            if(decoder_env.store_type == DECODER_STORE_TYPE_RAM)
                buffer = (uint8_t *)decoder_env.current_pos;
            else if(decoder_env.store_type == DECODER_STORE_TYPE_BLE)
            {
                sbc_data = (struct decoder_sbc_t *)co_list_pick(&decoder_env.sbc_buffer_list);
                if(!sbc_data)
                {
                    uart_putc_noint(UART1,'n');
                    break;
                }
                buffer = &sbc_data->buffer[sbc_data->offset];
            }

            pcm_frame = (struct decoder_pcm_t *)os_malloc(sizeof(struct decoder_pcm_t) + 2*128*2);  // one fame size = 512
            DEC_ASSERT(pcm_frame != NULL);

            pcm_len = 512;
            streamlen = decoder_env.frame_len;
            OI_STATUS rets;

            //co_printf("p:%d\r\n",streamlen);
            if(decoder_hold_flag == false)
            {
                rets = OI_CODEC_SBC_DecodeFrame(decoder_env.decoder_context,
                                                (const OI_BYTE **)&buffer,
                                                (OI_UINT32 *)&streamlen,
                                                (OI_INT16 *)&pcm_frame->pcm_data[0],
                                                (OI_UINT32 *)&pcm_len);
            }
            else
            {
                memset((uint8_t *)&pcm_frame->pcm_data[0],0x0,pcm_len);
                rets = OI_STATUS_SUCCESS;
            }

            if( rets == OI_STATUS_SUCCESS)
            {
                pcm_frame->pcm_size = pcm_len >> 2;
                pcm_frame->pcm_offset = 0;
                GLOBAL_INT_DISABLE();
                co_list_push_back(&decoder_env.pcm_buffer_list,&pcm_frame->list);
                GLOBAL_INT_RESTORE();
                //co_printf("pcmlen=%d,%d\r\n",pcm_len,streamlen);
                decoder_env.pcm_buffer_counter++;

                if(decode_task_status == DECODER_STATE_BUFFERING)
                {
                    if (decoder_env.pcm_buffer_counter > 6)
                    {
                        decode_task_status = DECODER_STATE_PLAYING;
                        NVIC_EnableIRQ(I2S_IRQn);
                    }
                    else
                    {
                        audio_decoder_play_next_frame();
                    }
                }

                if(decoder_hold_flag == false && decoder_env.store_type == DECODER_STORE_TYPE_RAM)
                {
                    decoder_env.current_pos += decoder_env.frame_len;
                    decoder_env.data_processed_len += decoder_env.frame_len;

                    if( (decoder_env.tot_data_len - decoder_env.data_processed_len) < (1024) )
                    {
                        //printf("x");
                        if(decoder_half_buff_fn)
                            decoder_half_buff_fn();
                    }

                    if( (decoder_env.tot_data_len - decoder_env.data_processed_len)  < decoder_env.frame_len )
                    {
                        if(decoder_hold_flag == false)
                        {
                            //printf("y");
                            decode_task_status = DECODER_STATE_WAITING_END;
                        }
                    }
                    if(decoder_env.current_pos >= decoder_env.data_end)
                    {
                        //printf("z");
                        decoder_env.current_pos = decoder_env.data_start;
                    }
                }
                else if (decoder_env.store_type == DECODER_STORE_TYPE_BLE)
                {
                    sbc_data->offset += decoder_env.frame_len;
                    if(sbc_data->offset >= sbc_data->length)
                    {
                        sbc_data = (void *)co_list_pop_front(&decoder_env.sbc_buffer_list);
                        os_free((void *)sbc_data);
                    }
                }
            }
            else
            {
                co_printf("frame dec err,stop\r\n");
                os_free(pcm_frame);
                NVIC_EnableIRQ(I2S_IRQn);
                audio_decoder_stop();
            }
            break;
        case DECODER_STATE_WAITING_END:
            co_printf("STATE_WAITING_END:%d\r\n",decoder_env.pcm_buffer_counter);

            if(decoder_env.pcm_buffer_counter == 0)
                audio_decoder_stop();

            break;
    }
}
static void audio_decoder_stop_handler(struct decoder_stop_t *param)
{
    //co_printf("audio_decoder_stop_handler\r\n");

    NVIC_DisableIRQ(I2S_IRQn);
    while(1)
    {
        struct co_list_hdr *element = co_list_pop_front(&decoder_env.pcm_buffer_list);
        if(element == NULL)
            break;
        os_free((void *)element);
    }
    while(1)
    {
        struct co_list_hdr *element = co_list_pop_front(&decoder_env.sbc_buffer_list);
        if(element == NULL)
            break;
        os_free((void *)element);
    }

    if(decoder_env.decoder_context != NULL)
    {
        os_free((void *)decoder_env.decoder_context);
        decoder_env.decoder_context = NULL;
    }

    decode_task_status = DECODER_STATE_IDLE;

    if(decoder_end_cb_fn)
        decoder_end_cb_fn();
    decoder_hold_flag = false;
}


int audio_decode_task(os_event_t *param)
{
    switch(param->event_id)
    {
        case DECODER_EVENT_PREPARE:
            audio_decoder_prepare_handler( (struct decoder_prepare_t *)(param->param) );
            break;
        case DECODER_EVENT_NEXT_FRAME:
            audio_decoder_play_next_frame_handler( (struct decoder_play_next_frame_t *)(param->param) );
            break;
        case DECODER_EVENT_STOP:
            audio_decoder_stop_handler( (struct decoder_stop_t *)(param->param) );
            break;
    }
    return EVT_CONSUMED;
}
void audio_decoder_init(void)
{
    task_id_audio_decode = os_task_create(audio_decode_task);
    decode_task_status = DECODER_STATE_IDLE;
    decoder_env.decoder_context = NULL;
}



#include "driver_uart.h"
#include "sys_utils.h"

uint16_t audio_sw_decoder_init(uint8_t *buff_start)
{
    uint32_t size = (sizeof(OI_CODEC_SBC_DECODER_CONTEXT) + 3) & (~0x03);
    decoder_env.decoder_context = os_malloc(size + 2048*sizeof(uint32_t));
    OI_CODEC_SBC_DecoderReset((OI_CODEC_SBC_DECODER_CONTEXT *)decoder_env.decoder_context,
                              (OI_UINT32 *)((uint8_t *)decoder_env.decoder_context+size),
                              2048,
                              2,
                              2,
                              FALSE);
    decoder_env.data_start = (uint32_t)buff_start;
    decoder_env.frame_len = audio_decoder_calc_sbc_frame_len((struct sbc_header_t *)decoder_env.data_start);

    //co_printf("preparing,fram_len:%d\r\n",decoder_env.frame_len);
    return decoder_env.frame_len;
}
uint8_t audio_sw_decoder_buff(uint8_t *frame_buffer,uint16_t frame_len,uint8_t *out_buffer)
{
    uint32_t pcm_len = 512;
    OI_STATUS rets = OI_CODEC_SBC_DecodeFrame(decoder_env.decoder_context,
                     (const OI_BYTE **)&frame_buffer,
                     (OI_UINT32 *)&frame_len,
                     (OI_INT16 *)out_buffer,
                     (OI_UINT32 *)&pcm_len);
    if( rets == OI_STATUS_SUCCESS)
        return 0;
    else
        return 1;
}
void audio_sw_decoder_deinit(void)
{
    if(decoder_env.decoder_context != NULL)
    {
        os_free((void *)decoder_env.decoder_context);
        decoder_env.decoder_context = NULL;
    }
}

