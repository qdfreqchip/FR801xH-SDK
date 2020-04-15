/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef OS_MSG_Q_H
#define OS_MSG_Q_H

#include <stdint.h>

/**
* OS event definition.
*/
typedef struct _os_event_
{
    uint16_t event_id;
    uint16_t src_task_id;
    void *param;
    uint16_t param_len;
} os_event_t;

/*********************************************************************
 * @fn      os_msg_post
 *
 * @brief   Post an event to a task. this function will remalloc a message if evt->len £¡= 0
 * @param   dst_task_id - Post event to destination task id.
 *          evt - msg, which will be posted
 *
 * @return  None.
 */
void os_msg_post(uint16_t dst_task_id,os_event_t *evt);
/*********************************************************************
 * @fn      os_msg_malloc
 *
 * @brief   Malloc space for a msg.
 * @param   evt_id - msg id.
 *          dst_task_id - destination task id the msg will be posted to
 *          src_task_id - soruce task id, which post the msg
 *          len - msg total len
 *
 * @return  Pointer to the malloced msg space.
 */
void *os_msg_malloc(uint16_t evt_id,uint16_t dst_task_id,uint16_t src_task_id,uint16_t len);
/*********************************************************************
 * @fn      os_msg_send
 *
 * @brief   Post a msg which is malloced by function: os_msg_malloc.
 * @param   msg - message will be posted.
 *
 * @return  None.
 */
void os_msg_send(void *msg);
/*********************************************************************
 * @fn      os_msg_free
 *
 * @brief   Free a msg which is malloced by function: os_msg_malloc.
 * @param   msg - message will be released.
 *
 * @return  None.
 */
void os_msg_free(void *msg);

#endif // OS_MSG_Q_H

