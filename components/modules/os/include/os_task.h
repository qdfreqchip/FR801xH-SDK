/**
 ****************************************************************************************
 *
 * @file app_ht.h
 *
 * @brief Health Thermometer Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef OS_TASK_H_
#define OS_TASK_H_

#include <stdint.h>
#include "os_msg_q.h"

#define TASK_ID_FAIL (0xff)
enum os_event_hdl_result
{
    EVT_CONSUMED = 0, ///< consumed, msg and ext are freed by the kernel
    EVT_NO_FREE,      ///< consumed, nothing is freed by the kernel
    EVT_SAVED,        ///< not consumed, will be pushed in the saved queue
};

typedef int (*os_task_func_t)(os_event_t *param);


void os_task_delete(uint8_t task_id);


/*********************************************************************
 * @fn      osal_task_create
 *
 * @brief   Create an os task. ???????????. ?????????????????????event process????
 * @param   task_func     - Event process function for the task.
 *			
 *
 * @return    0xff - Task create failure; others - Assigned task ID, 0~50
 */
uint16_t os_task_create(os_task_func_t task_func);



#endif // APP_HT_H_
