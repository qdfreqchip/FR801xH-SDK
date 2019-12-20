#include <stdint.h>

#include "driver_codec.h"
#include "driver_frspim.h"

#define codec_write(addr, data)         frspim_wr(FR_SPI_CODEC_CHAN, addr, 1, (uint32_t)data)
#define codec_read(addr)                (uint8_t)frspim_rd(FR_SPI_CODEC_CHAN, addr, 1)

static void codec_hw_set_freq(uint8_t nValue)
{
    codec_write(0x0b, 0x00);
    codec_write(0x0a, nValue);
    codec_write(0x0b, 0x03);
}

void codec_enable_adc(void)
{
    codec_write(0x15, codec_read(0x15) & 0xE0); //unmute mic
    codec_write(0x12, codec_read(0x12) & 0xBB);
}

void codec_disable_adc(void)
{
    codec_write(0x15, codec_read(0x15) | 0x1F);
    codec_write(0x12, codec_read(0x12) | 0x44);
}

void codec_enable_dac(void)
{
    codec_write(0x14, 0x00);
    codec_write(0x12, codec_read(0x12)&0xC4);
    codec_write(0x13, 0x00);
}

void codec_disable_dac(void)
{
    codec_write(0x14, 0xff);
    codec_write(0x12, codec_read(0x12)|0x3b);
    codec_write(0x13, 0xff);
}

void codec_set_vol(uint8_t value)
{
    codec_write(0x11, value);
}

void codec_init(uint8_t sample_rate)
{
    codec_write(0x15, 0x7F);    //pd all
    codec_write(0x0c, 0x1f);    //mic bias control
    codec_write(0x0d, 0x03);    //LMIX RMIX PD
    codec_write(0x0e, 0x3c);    //ADC control
    codec_write(0x0f, 0xd1);
    codec_write(0x10, 0x20);
    codec_write(0x11, 0x20);
    codec_write(0x12, 0xff);    //pd
    codec_write(0x13, 0xff);    //mute
    codec_write(0x14, 0xff);    //pd
    codec_write(0x15, 0x3f);    //pd all
    codec_write(0x16, 0x00);
    codec_write(0x17, 0x07);
    codec_write(0x18, 0x9c);
    codec_write(0x1a, 0x00);
    codec_write(0x00, 0xf5);
    codec_write(0x25, 0x02);
    codec_write(0x19, 0x1f);    //gain
    
    codec_write(0x09, 0x02);    //SLAVE_I2S
    codec_write(0x40, 0x81);    //CODEC_STEREO
    
    codec_hw_set_freq(sample_rate);
}

