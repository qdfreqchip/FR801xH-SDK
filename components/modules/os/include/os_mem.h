/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef OS_MEM_H_
#define OS_MEM_H_

/*
 * INCLUDES 
 */
#include <stdint.h>

/*********************************************************************
 * @fn      os_malloc
 *
 * @brief   malloc a ram space with specfied size
 *
 * @param   size    malloc space
 *
 * @return  None.
 */
void *os_malloc(uint32_t size);

/*********************************************************************
 * @fn      os_zalloc
 *
 * @brief   malloc a ram space with specfied size,ram value is set to 0
 *
 * @param   size    malloc space
            type    -KE_MEM_ENV
                    -KE_MEM_ATT_DB
                    -KE_MEM_KE_MSG
                    -KE_MEM_NON_RETENTION
 *
 * @return  None.
 */
void *os_zalloc(uint32_t size);

/*********************************************************************
 * @fn      os_free
 *
 * @brief   free a malloced ram space
 *
 * @param   ptr    point to ram space,which will be freed.
 *
 * @return  None.
 */
void os_free(void *ptr);
/*********************************************************************
 * @fn      os_get_free_heap_size
 *
 * @brief   get free heap size.
 *
 * @param   None
 *
 * @return  Free heap size.
 */
uint16_t os_get_free_heap_size(void);

/** @functions API for memory leakage debug,
 *   before call these functions, must define USER_MEM_API_ENABLE in your project Preprocessor Symbols.
 * @{
 */
void show_msg_list(void);
void show_ke_malloc(void);
void show_mem_list(void);

#endif // APP_HT_H_

