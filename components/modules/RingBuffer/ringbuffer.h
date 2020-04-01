#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H
#include "os_timer.h"
#include "co_printf.h"
#include <stdint.h>


typedef struct 
{
    unsigned char *blockbufptr;
    unsigned char writeptr;
    unsigned char readptr;
    volatile unsigned char newblockcount;
    unsigned char blocknum;
    unsigned int blocksize;
}sApp_BlockRingBuf;

uint8_t app_BlockLoopBuf_write(uint8_t RepID,sApp_BlockRingBuf *ptrloopbuf,uint8_t const *data, uint32_t len);
uint8_t *app_BlockRingBuf_malloc(sApp_BlockRingBuf *ptrloopbuf);
uint8_t app_BlockRingBuf_is_free(sApp_BlockRingBuf *ptrloopbuf);
void app_BlockRingBuf_free(sApp_BlockRingBuf *ptrloopbuf);
void app_BlockRingBuf_flush(sApp_BlockRingBuf *str);
void app_blockRingBuf_setup(sApp_BlockRingBuf *str,void *blockbufptr,unsigned char blocknum,unsigned int blocksize);






#endif   
