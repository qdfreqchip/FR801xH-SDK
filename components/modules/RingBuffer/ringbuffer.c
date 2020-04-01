#include "ringbuffer.h"
#include "string.h"

/*********************************************************************
 * @fn		app_blockRingBuf_setup
 *
 * @brief	Create a ring Block buffer
 *
 * @param	str         - Pointer to Block loop buffer 
 *          blockbufptr - Buffer
 *          blocknum    - the number of block
 *          blocksize   - size of block
 *
 * @return	None.
 */
void app_blockRingBuf_setup(sApp_BlockRingBuf *str,void *blockbufptr,unsigned char blocknum,unsigned int blocksize)
{
    memset(str,0,sizeof(sApp_BlockRingBuf));
    str->blockbufptr = (uint8_t *)blockbufptr;
    str->blocknum = blocknum;
    str->blocksize = blocksize;
}
/*********************************************************************
 * @fn		app_BlockRingBuf_flush
 *
 * @brief	Zeroing parameters
 *
 * @param	str - Pointer to Block ring buffer 
 *         
 *
 * @return	None.
 */
void app_BlockRingBuf_flush(sApp_BlockRingBuf *str)
{
    str->writeptr = 0;
    str->readptr = 0;
    str->newblockcount = 0;
}

/*********************************************************************
 * @fn		app_BlockRingBuf_is_free
 *
 * @brief	Determine if there is free space
 *
 * @param	str - Pointer to Block ring buffer 
 *         
 *
 * @return	Number of free blocks
 */
uint8_t app_BlockRingBuf_is_free(sApp_BlockRingBuf *ptr_ring_buf)
{
	return ptr_ring_buf->newblockcount;
}
/*********************************************************************
 * @fn		app_BlockRingBuf_free
 *
 * @brief	Free up space for a block
 *
 * @param	str - Pointer to Block ring buffer 
 *         
 *
 * @return	None
 */
void app_BlockRingBuf_free(sApp_BlockRingBuf *ptr_ring_buf)
{
   // uint8_t *readptr; 
    if(ptr_ring_buf->newblockcount)
    {
        
        ptr_ring_buf->newblockcount--;
        ptr_ring_buf->readptr++;
        ptr_ring_buf->readptr %= ptr_ring_buf->blocknum;
    }
    else
    {

    }
}

/*********************************************************************
 * @fn		app_BlockRingBuf_malloc
 *
 * @brief	Determine if there is free space
 *
 * @param	str - Pointer to Block ring buffer 
 *         
 *
 * @return	Address of free blocks or NULL
 */
uint8_t *app_BlockRingBuf_malloc(sApp_BlockRingBuf *ptr_ring_buf)
{
    uint8_t *readptr; 
    if(ptr_ring_buf->newblockcount>0)
    {
        readptr = &ptr_ring_buf->blockbufptr[(ptr_ring_buf->blocksize)*ptr_ring_buf->readptr]; 
        //if(*readptr != 0)
        {
            return readptr;
        }
    }
    return NULL;
}
/*********************************************************************
 * @fn		app_BlockLoopBuf_write
 *
 * @brief	Determine if there is free space
 *
 * @param	RepID        - Temporarily useless 
 *          ptrloopbuf   - Pointer to Block ring buffer 
 *          data         - Data pointer to write
 *          len          - Data length to be written
 *
 * @return	Address of free blocks or NULL
 */
uint8_t app_BlockLoopBuf_write(uint8_t RepID,sApp_BlockRingBuf *ptr_ring_buf,uint8_t const *data, uint32_t len)
{
    uint8_t *writeptr = &ptr_ring_buf->blockbufptr[(ptr_ring_buf->blocksize)*ptr_ring_buf->writeptr];
    if(ptr_ring_buf->newblockcount == ptr_ring_buf->blocknum)//buf is full
    {
        //NRF_LOG_INFO("QueneFull%d",RepID);
        return 1;
    }
    ptr_ring_buf->newblockcount++;
  //  *writeptr++ = RepID;
  //  *writeptr++ = len;
    memcpy(writeptr,data,((len>ptr_ring_buf->blocksize)?ptr_ring_buf->blocksize:len));
    ptr_ring_buf->writeptr++;
    ptr_ring_buf->writeptr %= ptr_ring_buf->blocknum;
    return 0;
}



