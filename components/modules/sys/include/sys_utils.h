#ifndef _SYS_UTILS_H
#define _SYS_UTILS_H

#include <stdint.h>

#include "co_printf.h"
#include "compiler.h"

#define BIT(x) (1<<(x))

#define ROUND(x,y)  ( (x%y)?(x/y+1):(x/y) )

#define SET_FEILD(reg,field,pos,value) \
    (reg & ~(field<<pos) | (value<<pos))

#define MIN(x,y) ( (x<y)?(x):(y) )

#define MAX(x,y) ( (x>y)?(x):(y) )

#define TO_BIG_EDN_16(x) ( (x&0xff)<<8 | (x&0xff00)>>8 )

#define TO_LITTLE_EDN_16(x) ( (x&0xff)<<8 | (x&0xff00)>>8 )

__STATIC __INLINE void show_reg(uint8_t *data,uint32_t len,uint8_t dbg_on)
{
    uint32_t i=0;
    if(len == 0 || (dbg_on==0)) return;
    for(; i<len; i++)
    {
        co_printf("0x%02X ",data[i]);
    }
    co_printf("\r\n");
}

void co_delay_100us(uint32_t num);

/*
 * num should not be greater than 8_000_000
 */
void co_delay_10us(uint32_t num);
void cpu_delay_100us(uint32_t num);
uint8_t hex4bit_to_char(const uint8_t hex_value);
uint8_t hex4bit_to_caps_char(const uint8_t hex_value);

void hex_arr_to_str(const uint8_t hex_arr[],uint8_t arr_len,uint8_t *str);
void str_to_hex_arr(const uint8_t *str, uint8_t hex_arr[],uint8_t arr_len);
void val_to_str(const uint32_t int_value,uint8_t *str);

char char_to_val(const char c);
int str_to_val( const char str[], char base, char n);
int ascii_strn2val( const char str[], char base, char n);

void system_prevent_sleep_clear(void);
void system_prevent_sleep_set(void);
#endif  //_SYS_UTILS_H



