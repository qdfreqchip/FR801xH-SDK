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
#include "speaker.h"
#include "driver_codec.h"
#include "driver_i2s.h"
#include "os_timer.h"
#include "co_printf.h"
#include "driver_plf.h"
#include "core_cm3.h"
#include <stdio.h>
#include <string.h>
#include "driver_pmu.h"
#include "decoder.h"
#include "os_mem.h"
#include "driver_system.h"
#include "driver_gpio.h"
#include "ringbuffer.h"
#include "ble_simple_peripheral.h"

/*
 * MACROS
 */
#define PA_ENABLE      gpio_porta_write(gpio_porta_read() | (1<<GPIO_BIT_1) )//gpio_set_pin_value(GPIO_PORT_A, GPIO_BIT_1, 1)
#define PA_DISABLE     gpio_porta_write(gpio_porta_read() & ~(1<<GPIO_BIT_1) )//gpio_set_pin_value(GPIO_PORT_A, GPIO_BIT_1, 0)

/*
 * CONSTANTS 
 */

/*
 * TYPEDEFS 
 */

/*
 * GLOBAL VARIABLES 
 */
volatile uint8_t audio_data[I2S_FIFO_DEPTH*2];
uint8_t audio_data_len=0;
uint8_t audio_data_update =0;
uint8_t TimerCnt=0;
sApp_BlockRingBuf app_audio_data_BlockRingBuf;
static uint8_t app_audio_data_blockBuf[CONFIG_AUDIO_DECODER_BLOCKNUM][CONFIG_AUDIO_DECODER_BLOCKSIZE];//

/*
 * LOCAL VARIABLES 
 */
static volatile struct i2s_reg_t *i2s_reg = (struct i2s_reg_t *)I2S_BASE;

/*
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * @fn		speaker_init
 *
 * @brief	Initialize the speaker
 *
 * @param	None
 *
 * @return	None.
 */
void speaker_init(void)
{
	pmu_codec_power_enable();
	audio_speaker_codec_init();	

	i2s_init(I2S_DIR_TX,8000,1);	
	NVIC_SetPriority(I2S_IRQn, 2);	//Setting the I2S interrupt priority
	co_printf("speaker_init\r\n");
	PA_init_pins();					//Initialize PA enable pin
}

/*********************************************************************
 * @fn		speaker_start_hw
 *
 * @brief	Start outputting audio
 *
 * @param	None
 *
 * @return	None.
 */
void speaker_start_hw(void)
{
	pmu_codec_power_enable();               
	i2s_start();                            
    codec_enable_dac();                     
	co_printf("speaker_start_hw\r\n");
	PA_ENABLE;								

}

/*********************************************************************
 * @fn		speaker_stop_hw
 *
 * @brief	Stop outputting audio
 *
 * @param	None
 *
 * @return	None.
 */
void speaker_stop_hw(void)
{
	pmu_codec_power_disable();    
	PA_DISABLE;						
	codec_disable_dac();				
    i2s_stop();							
	co_printf("speaker_stop_hw\r\n");
}

/*********************************************************************
 * @fn		PA_init_pins
 *
 * @brief	Initialize PA enable pin
 *
 * @param	None
 *
 * @return	None.
 */
void PA_init_pins(void)
{
	system_set_port_mux(GPIO_PORT_A, GPIO_BIT_1, PORTA1_FUNC_A1);
	system_set_port_pull(GPIO_PA1, true);
	gpio_set_dir(GPIO_PORT_A, GPIO_BIT_1, GPIO_DIR_OUT);
}

/*********************************************************************
 * @fn		i2s_isr_ram
 *
 * @brief	I2S interrupt handler
 *
 * @param	None
 *
 * @return	None.
 */
__attribute__((section("ram_code"))) void i2s_isr_ram(void)
{
	uint8_t i;
    if(App_Mode == CODEC_TEST){
		uint16_t i2s_adc_data[I2S_FIFO_DEPTH/2];
	    if((i2s_reg->status.rx_half_full)) {//codec_ADC
				//co_printf("M");
				uint8_t LoopBufWrite =0;
				for(uint32_t i=0; i<(I2S_FIFO_DEPTH/2); i++) {
						 i2s_adc_data[i] = i2s_reg->data;
						// uart_putc_noint_no_wait(UART1_BASE,i2s_adc_data[i] );
					}
				LoopBufWrite = app_BlockLoopBuf_write(0, 
											&app_audio_data_BlockRingBuf, 
											(uint8_t const *)i2s_adc_data, 
											I2S_FIFO_DEPTH);
				if(LoopBufWrite == 1){//buffer ย๚มห
				co_printf("FL");
				}else{
				//	co_printf("I");
				}
	    }

		 if((i2s_reg->status.tx_half_empty)){
		 	uint16_t *tx_data;
			//co_printf("O\r\n");
			tx_data = (uint16_t*)app_BlockRingBuf_malloc(&app_audio_data_BlockRingBuf);
			if(tx_data != NULL){
				for (i=0; i<(I2S_FIFO_DEPTH/2); i++)
	                {
	                    uint32_t tmp_data = *tx_data++;
	                    tmp_data &= 0xFFFF;

						i2s_reg->data = tmp_data;
					//	uart_putc_noint_no_wait(UART1_BASE,tmp_data);
	                }
				app_BlockRingBuf_free(&app_audio_data_BlockRingBuf); 
			}else{
				//co_printf("E");
				for(i=0; i<(I2S_FIFO_DEPTH/2); i++)
	            {
					i2s_reg->data = 0;

	            }
			}
		 }
	}else{
		uint32_t last = 0;
	    if((i2s_reg->status.tx_half_empty)&&(i2s_reg->mask.tx_half_empty))//codec_DAC
	    {
	        
#define I2S_FIFO_DEPTH      64
	        struct co_list_hdr *element;
	        struct decoder_pcm_t *pcm;
	        uint16_t *tx_data;
		        if(co_list_is_empty(&decoder_env.pcm_buffer_list))
		        {
		            co_printf("F");
		            for(i=0; i<(I2S_FIFO_DEPTH/2); i++)
		            {
						i2s_reg->data = 0;

		            }
		        }
		        else
		        {
		            element = decoder_env.pcm_buffer_list.first;

		            pcm = (struct decoder_pcm_t *)element;
		            tx_data = (uint16_t *)&pcm->pcm_data[pcm->pcm_offset];
		            last = pcm->pcm_size - pcm->pcm_offset;
		            if(last > (I2S_FIFO_DEPTH/2))
		            {
		              //  co_printf("X");
		                for (i=0; i<(I2S_FIFO_DEPTH/2); i++)
		                {
		                    uint32_t tmp_data = *tx_data++;
		                    tmp_data &= 0xFFFF;

							i2s_reg->data = tmp_data;
		                }

		                pcm->pcm_offset += (I2S_FIFO_DEPTH/2);
		            }
		            else
		            {
		               
		                for (i=0; i<last; i++)
		                {
		                    uint32_t tmp_data = *tx_data++;
		                    tmp_data &= 0xFFFF;
							i2s_reg->data = tmp_data;
		                }
		                co_list_pop_front(&decoder_env.pcm_buffer_list);
		                os_free((void *)pcm);
		                decoder_env.pcm_buffer_counter--;
		                decoder_play_next_frame();

		                while(!co_list_is_empty(&decoder_env.pcm_buffer_list))
		                {
		                    element = decoder_env.pcm_buffer_list.first;

		                    pcm = (struct decoder_pcm_t *)element;
		                    tx_data = (uint16_t *)&pcm->pcm_data[0];
		                    last = pcm->pcm_size - pcm->pcm_offset;
		                    if((last + i) > (I2S_FIFO_DEPTH/2))
		                    {
		                        pcm->pcm_offset = (I2S_FIFO_DEPTH/2) - i;
		                       for(; i<(I2S_FIFO_DEPTH/2); i++)
		                        {
		                            uint32_t tmp_data = *tx_data++;
		                            tmp_data &= 0xFFFF;                      
									i2s_reg->data = tmp_data;
		                        }
		                        break;
		                    }
		                    else
		                    {
		                        last += i;
		                        for(; i<last; i++)
		                        {
		                            uint32_t tmp_data = *tx_data++;
		                            tmp_data &= 0xFFFF;
		                            //REG_PL_WR(I2S_REG_DATA, tmp_data);
									i2s_reg->data = tmp_data;
		                        }
		                        co_list_pop_front(&decoder_env.pcm_buffer_list);
		                        os_free((void *)pcm);
		                        decoder_env.pcm_buffer_counter--;
		                        decoder_play_next_frame();
		                    }
		                }

		            }
		        }
			
	    }
	}
}

/*********************************************************************
 * @fn		Test_Codec_demo
 *
 * @brief	Codec input and output test, speak into the microphone, the speaker can play audio
 *
 * @param	None
 *
 * @return	None.
 */
void Test_Codec_demo(void)
{
	co_printf("Test_Codec_demo\r\n");
	//App_Mode = CODEX_TEST;
	app_blockRingBuf_setup(&app_audio_data_BlockRingBuf,
                            app_audio_data_blockBuf,
                            CONFIG_AUDIO_DECODER_BLOCKNUM,
                            CONFIG_AUDIO_DECODER_BLOCKSIZE);
	PA_init_pins();					//Initialize PA enable pin
	app_BlockRingBuf_flush(&app_audio_data_BlockRingBuf);
	pmu_codec_power_enable();
	codec_init(CODEC_SAMPLE_RATE_8000);
//	audio_speaker_codec_init();
	
	i2s_init(I2S_DIR_TX,8000,1);//speaker
	codec_enable_dac();
	i2s_init(I2S_DIR_RX,8000,1);//Mic 
	codec_enable_adc();
	PA_ENABLE;
	NVIC_SetPriority(I2S_IRQn, 2);
	 i2s_start();
    NVIC_EnableIRQ(I2S_IRQn);
	

}
/*********************************************************************
 * @fn		Test_codec_demo_stop
 *
 * @brief	Stop codec test
 *
 * @param	None
 *
 * @return	None.
 */
void Test_codec_demo_stop(void)
{
	co_printf("Test_codec_demo_stop\r\n");
	NVIC_DisableIRQ(I2S_IRQn);
	         
	PA_DISABLE;						
	codec_disable_dac();				
    i2s_stop();							
	co_printf("speaker_stop_hw\r\n");
	codec_disable_adc();
	pmu_codec_power_disable(); 
}


