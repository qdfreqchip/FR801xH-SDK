#include <stdint.h>

#include "os_task.h"
#include "os_msg_q.h"
#include "os_mem.h"

#include "co_printf.h"
#include "user_task.h"
#include "button.h"
#include "driver_gpio.h"
#include "ble_simple_peripheral.h"
#include "lcd.h"
#include "adpcm_ms.h"
#include "speaker.h"
#include "decoder.h"

extern const unsigned char gImage_logo240x240[115200];
extern const unsigned char gImage_erweima240x240[115200];

uint16_t user_task_id;
uint16_t audio_task_id;

/****************************************************
*
按键任务主函数
*
*************************************************/

static int user_task_func(os_event_t *param)
{
    switch(param->event_id)
    {
        case USER_EVT_BUTTON:
            {
                struct button_msg_t *button_msg;
                const char *button_type_str[] = {
                                                    "BUTTON_PRESSED",
                                                    "BUTTON_RELEASED",
                                                    "BUTTON_SHORT_PRESSED",
                                                    "BUTTON_MULTI_PRESSED",
                                                    "BUTTON_LONG_PRESSED",
                                                    "BUTTON_LONG_PRESSING",
                                                    "BUTTON_LONG_RELEASED",
                                                    "BUTTON_LONG_LONG_PRESSED",
                                                    "BUTTON_LONG_LONG_RELEASED",
                                                    "BUTTON_COMB_PRESSED",
                                                    "BUTTON_COMB_RELEASED",
                                                    "BUTTON_COMB_SHORT_PRESSED",
                                                    "BUTTON_COMB_LONG_PRESSED",
                                                    "BUTTON_COMB_LONG_PRESSING",
                                                    "BUTTON_COMB_LONG_RELEASED",
                                                    "BUTTON_COMB_LONG_LONG_PRESSED",
                                                    "BUTTON_COMB_LONG_LONG_RELEASED",
                                                };

                button_msg = (struct button_msg_t *)param->param;
                
                co_printf("KEY 0x%08x, TYPE %s.\r\n", button_msg->button_index, button_type_str[button_msg->button_type]);
				if(button_msg->button_type == BUTTON_SHORT_PRESSED){//短按
						if(button_msg->button_index == GPIO_PD6 ){//Key2
							switch (App_Mode)
							{
							case SPEAKER_FROM_FLASH:
								test_speaker_from_flash();//开始播放音频
								break;
							case PICTURE_UPDATE://刷图片
								co_printf("picture_idx = %d",picture_idx);
								if(picture_idx == 0){
									LCD_DisPIC((uint32_t)gImage_logo240x240);
								}else if(picture_idx == 1){
									LCD_DisPIC((uint32_t)gImage_erweima240x240);
								}else if(picture_idx == 2){
									LCD_Clear_quick(RED);
								}else if(picture_idx == 3){
									LCD_Clear_quick(GREEN);
								}else if(picture_idx == 4){
									LCD_Clear_quick(BLUE);
								}
								picture_idx ++;
								if(picture_idx >= 5)
									picture_idx = 0;
								break;
							}
						}else if(button_msg->button_index == GPIO_PC5){//KEY1  工作模式切换
							if((App_Mode == SPEAKER_FROM_FLASH)){
								test_end_speaker();//停止播放音频
							}
							App_Mode++;
							if(App_Mode >= MODE_MAX){
								App_Mode = PICTURE_UPDATE;
								picture_idx = 0;
							}
							
							lcd_show_logo(App_Mode);//刷新主题显示
						}
					
						
				}else if(button_msg->button_type == BUTTON_LONG_PRESSED){//按键长按
					if(button_msg->button_index == GPIO_PC5 ){
						co_printf("K1 long Pressed\r\n");
						//tft_write_pic_data_to_flash();
					}
				}
            }
            break;
    }

    return EVT_CONSUMED;
}



/****************************************************
*音频任务主函数
*
*************************************************/
static int audio_task_func(os_event_t *param){
	struct decoder_prepare_t *decoder_param = NULL;
	ADPCMContext *context = NULL;
	switch (param->event_id)
	{
#if 0		//Mic 数据编码处理
		case USER_EVT_MIC:
			uint8_t *pcm_data;
			pcm_data = param->param;
			if(encoder_env.reserved_space >= encoder_env.frame_size){
				encode( &adpcm_state
                ,(short *)(pcm_data)
                ,encoder_env.block_size>>1
                ,&encoder_env.out_buffer[encoder_env.encode_write_pos]
              );
			//	os_free(pcm_data);
			  encoder_env.reserved_space -= encoder_env.frame_size;
			
				encoder_frame_out_func(&encoder_env.out_buffer[encoder_env.encode_write_pos],encoder_env.frame_size);
				 encoder_env.reserved_space += encoder_env.frame_size;
			    encoder_env.encode_write_pos += encoder_env.frame_size;
			    if(encoder_env.encode_write_pos >= ENCODER_MAX_BUFFERING_BLOCK_COUNT*encoder_env.frame_size)
			    {
			        encoder_env.encode_write_pos = 0;
			    }
			}
			break;
#endif
		//音频解码准备
		case DECODER_EVENT_PREPARE:			
			
			decoder_param = (struct decoder_prepare_t *)(param->param);

			 decoder_env.decoder_context = os_zalloc(sizeof(ADPCMContext));
            context = (ADPCMContext *)decoder_env.decoder_context;
            context->channel = 1;
            context->block_align = decoder_param->frame_len;
            decoder_env.data_start = decoder_param->data_start;
            decoder_env.data_end = decoder_param->data_end;
            decoder_env.current_pos = decoder_param->data_start + decoder_param->start_offset;
            decoder_env.tot_data_len = decoder_param->tot_data_len;
            decoder_env.store_type = decoder_param->store_type;
            decoder_env.data_processed_len = 0;
            decoder_env.frame_len = decoder_param->frame_len;
            stop_flag = 0;

            co_printf("preparing,fram_len:%d\r\n",decoder_env.frame_len);
            co_list_init(&decoder_env.pcm_buffer_list);
            decoder_env.pcm_buffer_counter = 0;
            decoder_play_next_frame();
         decodeTASKState = DECODER_STATE_BUFFERING;
			
			break;
		//音频解码下一帧数据处理
		 case DECODER_EVENT_NEXT_FRAME:			
			decoder_play_next_frame_handler(&decodeTASKState);
		 	break;
		 
		 //音频解码停止
		case DECODER_EVENT_STOP:				
			    NVIC_DisableIRQ(I2S_IRQn);

			    while(1)
			    {
			        struct co_list_hdr *element = co_list_pop_front(&decoder_env.pcm_buffer_list);
			        if(element == NULL)
			            break;
			        os_free((void *)element);
			    }

			    if(decoder_env.decoder_context != NULL)
			    {
			        os_free((void *)decoder_env.decoder_context);
			        decoder_env.decoder_context = NULL;
			    }

			    //ke_state_set(TASK_DECODER, DECODER_STATE_IDLE);
				decodeTASKState = DECODER_STATE_IDLE;
			    speaker_stop_hw();
			    decoder_end_func();
			    decoder_hold_flag = false;
			break;
	}


return EVT_CONSUMED;
}


void user_task_init(void)
{
    user_task_id = os_task_create(user_task_func);
	audio_task_id = os_task_create(audio_task_func);//创建音频任务
}




