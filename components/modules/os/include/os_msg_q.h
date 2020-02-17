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
 * @brief   Post an event to a task.
 * @param   dst_task_id - Post event to destination task id.
 *          evt - msg, which will be posted
 *
 * @return  None.
 */

void os_msg_post(uint16_t dst_task_id,os_event_t *evt);

#endif // OS_MSG_Q_H

