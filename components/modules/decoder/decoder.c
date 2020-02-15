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
#include "decoder.h"
#include "driver_plf.h"
#include "core_cm3.h"
#include "os_task.h"
#include "os_msg_q.h"
#include "speaker.h"
#include "driver_flash.h"
#include "co_printf.h"
#include <string.h>
#include "os_mem.h"
#include "adpcm_ms.h"
#include "user_task.h"

/*
 * MACROS
 */
#define DEC_DBG FR_DBG_OFF
#define DEC_LOG FR_LOG(DEC_DBG)

/*
 * CONSTANTS 
 */
#define DECODER_STORE_TYPE_RAM      0
#define DECODER_STORE_TYPE_FLASH    1
 
/*
 * TYPEDEFS 
 */

/*
 * GLOBAL VARIABLES 
 */
sbc_store_info_t sbc_sotre_env = {0};
uint8_t *sbc_buff = NULL;
speaker_env_t speaker_env = {0};
bool decoder_hold_flag = false;
struct decoder_env_t decoder_env;
uint8_t stop_flag = 0;
uint8_t decodeTASKState = DECODER_STATE_IDLE;//函数decoder_play_next_frame_handler 中的状态
uint8_t Flash_data_state = true;

/*
 * LOCAL VARIABLES 
 */


/*
 * LOCAL FUNCTIONS
 */

/*
 * EXTERN FUNCTIONS
 */

/*
 * PUBLIC FUNCTIONS
 */


/*********************************************************************
 * @fn		decoder_calc_adpcm_ms_frame_len
 *
 * @brief	Get adpcm frame len
 *
 * @param	header_org     - Pointer to pointer to ADPCM audio data
 *
 * @return	None.
 */

uint16_t decoder_calc_adpcm_ms_frame_len(uint8_t **header_org)
{
    uint32_t len;
    uint8_t *header = *header_org;
    uint16_t frame_size;

    if(memcmp(header, "RIFF", 4) == 0)
    {
        header += 12;
        if(memcmp(header, "fmt ", 4) == 0)
        {
            header += 4;
            len = header[0];
            len |= (header[1] << 8);
            len |= (header[2] << 16);
            len |= (header[3] << 24);

            frame_size = header[16];
            frame_size |= (header[17] << 8);
            header += 4;
            header += len;
            while(memcmp(header, "data", 4) != 0)
            {
                header += 4;
                len = header[0];
                len |= (header[1] << 8);
                len |= (header[2] << 16);
                len |= (header[3] << 24);
                header += (len + 4);
            }
            header += 8;
            DEC_LOG("decoder_calc_adpcm_ms_frame_len: %08x, %08x.\r\n", header, *header_org);
            *header_org = header;
            return frame_size;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

/*********************************************************************
 * @fn		read_sbc_from_flash
 *
 * @brief	Get adpcm frame len
 *
 * @param	header_org     - Pointer to pointer to ADPCM audio data
 *
 * @return	None.
 */
int read_sbc_from_flash(uint8_t * sbc_buff, uint32_t read_len)
{
    uint32_t r_len = read_len;
    uint32_t pos = 0;
    while(r_len > 0)
    {
        if( speaker_env.last_read_page_idx < sbc_sotre_env.last_page_idx )
        {
            if( (r_len + speaker_env.last_read_offset) >= FLASH_PAGE_SIZE )
            {
                flash_read(sbc_sotre_env.start_base + speaker_env.last_read_page_idx * FLASH_PAGE_SIZE + speaker_env.last_read_offset
                           ,FLASH_PAGE_SIZE - speaker_env.last_read_offset, sbc_buff + pos);
                r_len -= (FLASH_PAGE_SIZE - speaker_env.last_read_offset);
                pos += (FLASH_PAGE_SIZE - speaker_env.last_read_offset);
                speaker_env.last_read_offset = 0;
                speaker_env.last_read_page_idx++;
            }
            else
            {
                flash_read(sbc_sotre_env.start_base + speaker_env.last_read_page_idx * FLASH_PAGE_SIZE + speaker_env.last_read_offset
                           ,r_len, sbc_buff + pos);
                pos += r_len;
                speaker_env.last_read_offset += r_len;
                r_len = 0;
            }
        }
        else if( speaker_env.last_read_page_idx == sbc_sotre_env.last_page_idx )
        {
            if( speaker_env.last_read_offset >= sbc_sotre_env.last_offset)
            {
                return 0;
            }
            else
            {
                if( (r_len + speaker_env.last_read_offset) > sbc_sotre_env.last_offset  )
                {
                    flash_read(sbc_sotre_env.start_base + speaker_env.last_read_page_idx * FLASH_PAGE_SIZE + speaker_env.last_read_offset
                               ,sbc_sotre_env.last_offset - speaker_env.last_read_offset, sbc_buff + pos);
                    uint32_t no_read_len = ( r_len + speaker_env.last_read_offset - sbc_sotre_env.last_offset );
                    pos += r_len;
                    speaker_env.last_read_offset = sbc_sotre_env.last_offset;
                    r_len = 0;
                    return (read_len - no_read_len);
                }
                else
                {
                    flash_read(sbc_sotre_env.start_base + speaker_env.last_read_page_idx * FLASH_PAGE_SIZE + speaker_env.last_read_offset
                               ,r_len, sbc_buff + pos);
                    pos += r_len;
                    speaker_env.last_read_offset += r_len;
                    r_len = 0;
                }
            }
        }
        else
            return 0;
    }

    return read_len;
}


/*********************************************************************
 * @fn		decoder_start
 *
 * @brief	Start decoding
 *
 * @param	start           - starting address
 *          start           - end address
 *          tot_data_len    - Total length
 *          frame_len       - frame length
 *			start_offset    - Starting offset
 *          type            - type of data (RAM or Flash)
 * @return	None.
 */

void decoder_start(uint32_t start, uint32_t end, uint32_t tot_data_len, uint16_t frame_len, uint32_t start_offset, uint8_t type)
{
    struct decoder_prepare_t param;

    param.data_start = start;
    param.data_end = end;
    param.store_type = type;
    param.tot_data_len = tot_data_len;
    param.start_offset = start_offset;
    param.frame_len = frame_len;
    DEC_LOG("s:%x,e:%x\r\n",start,end);

	os_event_t audio_event;

	audio_event.event_id = DECODER_EVENT_PREPARE;
	audio_event.param = &param;
	audio_event.param_len = sizeof(param);
    os_msg_post(audio_task_id,&audio_event);

    speaker_start_hw();
    decoder_hold_flag = false;
}

/*********************************************************************
 * @fn		decoder_play_next_frame
 *
 * @brief	Release semaphore,Decode the next frame of data
 *
 * @param	None
 *         
 * @return	None.
 */
void decoder_play_next_frame(void)
{
   

	os_event_t audio_event;

	audio_event.event_id = DECODER_EVENT_NEXT_FRAME;
	audio_event.param = NULL;
	audio_event.param_len = 0;
    os_msg_post(audio_task_id,&audio_event);
	DEC_LOG("decoder_play_next_frame\r\n");
}

/*********************************************************************
 * @fn		decoder_stop
 *
 * @brief	Stop decoding
 *
 * @param	None
 *         
 * @return	None.
 */
void decoder_stop(void)
{
    DEC_LOG("decoder_stop\r\n");
    //printf("d_s\r\n");


	os_event_t audio_event;

	audio_event.event_id = DECODER_EVENT_STOP;
	audio_event.param = NULL;
	audio_event.param_len = 0;
   os_msg_post(audio_task_id,&audio_event);
}

/*********************************************************************
 * @fn		test_end_speaker
 *
 * @brief	 stop audio 
 *
 * @param	None
 *         
 * @return	None.
 */
void test_end_speaker(void)
{
    decoder_stop();
    speaker_stop_hw();
}


/*********************************************************************
 * @fn		test_speaker_from_flash
 *
 * @brief	Playing audio from flash
 *
 * @param	None
 *         
 * @return	None.
 */
void test_speaker_from_flash(void)
{

    if( sbc_buff != NULL)
        goto _Exit;
    co_printf("speaker_flash_start\r\n");

    memset((void *)&sbc_sotre_env, 0, sizeof(sbc_sotre_env));
    flash_read(USER_FLASH_BASE_ADDR, sizeof(sbc_sotre_env), (uint8_t *)&sbc_sotre_env);//读取Flash存储的audio信息
    co_printf("%x,%d,%d\r\n",sbc_sotre_env.start_base,sbc_sotre_env.last_page_idx,sbc_sotre_env.last_offset);
    if(sbc_sotre_env.start_base == 0xffffffff)//异常
    {
        memset((void *)&sbc_sotre_env,0x00,sizeof(sbc_sotre_env));
		Flash_data_state = false;//flash没有音频数据
        goto _Exit;
    }
	Flash_data_state = true;//flash中有音频数据
    speaker_init();//speaker 初始化

    sbc_buff = (uint8_t *)os_zalloc(10*1024);//申请10kbuffer


    uint8_t *tmp_buf;

    flash_read(sbc_sotre_env.start_base, 512, sbc_buff);//读取512个字节的数据

    memset((void *)&speaker_env, 0, sizeof(speaker_env));
    tmp_buf = sbc_buff;
    speaker_env.sbc_frame_len = decoder_calc_adpcm_ms_frame_len(&tmp_buf);//获取sbc_frame_len
    //sbc_sotre_env.start_base += (tmp_buf - sbc_buff);
    speaker_env.last_read_offset = (tmp_buf - sbc_buff);
    speaker_env.last_read_page_idx = 0;
    if(speaker_env.sbc_data_tot_len == 0)
        speaker_env.sbc_data_tot_len = (10240 - 10240%speaker_env.sbc_frame_len)&(~0x1);
    speaker_env.store_data_len += read_sbc_from_flash(sbc_buff,speaker_env.sbc_data_tot_len>>1);

    decoder_start((uint32_t)sbc_buff, (uint32_t)sbc_buff + speaker_env.sbc_data_tot_len
                  , speaker_env.store_data_len, speaker_env.sbc_frame_len, 0, DECODER_STORE_TYPE_RAM);

_Exit:
    ;
}

/*********************************************************************
 * @fn		decoder_update_tot_data_len
 *
 * @brief	Update total data length
 *
 * @param	len    -    Latest total data length
 *         
 * @return	None.
 */

void decoder_update_tot_data_len(uint32_t len)
{
    decoder_env.tot_data_len = len;
}

/*********************************************************************
 * @fn		decoder_half_processed
 *
 * @brief	Put new playback data into the cache and update the total audio data length tot_data_len.
 *
 * @param	None
 *		   
 * @return	None.
 */
void decoder_half_processed(void)
{
    if(speaker_env.end_flag)
        goto _Exit;
    else
    {
        uint32_t pos = (speaker_env.store_data_len % speaker_env.sbc_data_tot_len);
        //fputc('z',0);
        uint32_t read_len = read_sbc_from_flash(sbc_buff + pos,speaker_env.sbc_data_tot_len>>1);
        if(read_len > 0)
        {
            speaker_env.store_data_len += read_len;
            decoder_update_tot_data_len( speaker_env.store_data_len );
        }
        else
            speaker_env.end_flag = 1;
    }
_Exit:
    ;
}

/*********************************************************************
 * @fn		decoder_play_next_frame_handler
 *
 * @brief	Processing and decoding the next frame of audio data,
 *
 * @param	arg    -    task state
 *         
 * @return	None.
 */

void  decoder_play_next_frame_handler(void *arg)
{
    uint8_t *buffer;
    uint32_t pcm_len;
    uint32_t streamlen;
    struct decoder_pcm_t *pcm_frame;
	uint8_t *Task_state;
	Task_state = arg;
    // CPU_SR cpu_sr;

    switch(*Task_state)
    {
        case DECODER_STATE_IDLE:
            break;
        case DECODER_STATE_BUFFERING:
        case DECODER_STATE_PLAYING:
#if 1

            if(decoder_env.store_type == DECODER_STORE_TYPE_RAM)
            {
                buffer = (uint8_t *)decoder_env.current_pos;
            }
            else
            {
                // TBD
            }

            pcm_frame = (struct decoder_pcm_t *)os_zalloc(sizeof(struct decoder_pcm_t) + 2*(decoder_env.frame_len+8)*2);
            if(pcm_frame == NULL)
            {
               // return KE_MSG_SAVED;
            }

            pcm_len = 2*(decoder_env.frame_len+8)*2;
            streamlen = decoder_env.frame_len;
            DEC_LOG("playing:%d\r\n",streamlen);
            //printf("p:%d\r\n",streamlen);
            DEC_LOG("sbc_buff[0] = %02x, %02x.\r\n", buffer[0], buffer[1]);

            if(decoder_hold_flag == false)
            {
                adpcm_decode_frame(decoder_env.decoder_context, (short *)&pcm_frame->pcm_data[0], (int *)&pcm_len, buffer, decoder_env.frame_len);
            }
            else
                memset((uint8_t *)&pcm_frame->pcm_data[0],0x0,pcm_len);

            if( 1 )
            {
                pcm_frame->pcm_size = pcm_len >> 1;
                pcm_frame->pcm_offset = 0;
                GLOBAL_INT_DISABLE();
                co_list_push_back(&decoder_env.pcm_buffer_list,&pcm_frame->list);
                GLOBAL_INT_RESTORE();
                DEC_LOG("pcmlen=%d,%d\r\n",pcm_len,streamlen);
                decoder_env.pcm_buffer_counter++;

                if((*Task_state )== DECODER_STATE_BUFFERING)
                {
					if (decoder_env.pcm_buffer_counter > 2)
                    {
                 		decodeTASKState = DECODER_STATE_PLAYING;
                        NVIC_EnableIRQ(I2S_IRQn);//
						
                    }
                    else
                    {
						decoder_play_next_frame();
						
                    }
                }

                if(decoder_hold_flag == false)
                {
                    decoder_env.current_pos += decoder_env.frame_len;
                    decoder_env.data_processed_len += decoder_env.frame_len;
                    if( (decoder_env.tot_data_len - decoder_env.data_processed_len) < (1024) )
                    {
						decoder_half_processed();
                    }

                    if( (decoder_env.tot_data_len - decoder_env.data_processed_len)  < decoder_env.frame_len )
                    {
                        if(decoder_hold_flag == false)
                        {                 
                        	decodeTASKState = DECODER_STATE_WAITING_END;                          
                        }
                    }
                    if(decoder_env.current_pos >= decoder_env.data_end)
                    {
                       decoder_env.current_pos = decoder_env.data_start;
                    }
                }

                if(decoder_env.store_type == DECODER_STORE_TYPE_FLASH)
                {
                    // TBD, free buffer
                }
            }
            else
            {
                os_free(pcm_frame);
                NVIC_EnableIRQ(I2S_IRQn);
                decoder_stop();
            }
#endif
            break;
        case DECODER_STATE_WAITING_END:
            DEC_LOG("STATE_WAITING_END\r\n");
            //if(decoder_env.pcm_buffer_list.first == NULL)
            if(decoder_env.pcm_buffer_counter == 0)
            {
                if(stop_flag == 0)
                {
                    decoder_stop();
                    stop_flag = 1;
                }
            }
            break;
        default:
            break;
    }

}

/*********************************************************************
 * @fn		decoder_end_func
 *
 * @brief	Callback function after decoding，
 *
 * @param	None
 *		   
 * @return	None.
 */
void decoder_end_func(void)
{
    co_printf("E");
    if(sbc_buff!= NULL)
    {
        os_free(sbc_buff);
        sbc_buff = NULL;
    }

}


