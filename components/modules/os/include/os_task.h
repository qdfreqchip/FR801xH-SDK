/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef OS_TASK_H_
#define OS_TASK_H_

#include <stdint.h>
#include "os_msg_q.h"

#define TASK_ID_FAIL (0xff)

#define TASK_ID_NONE (0xff)

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

/*********************************************************************
 * @fn      os_user_loop_event_set
 *
 * @brief   set a loop event that run inside the while(1),and attention that the prioity is lowest.
 *          If you want to goto sleep,you should clear the event.
 *
 * @param   callback     - the callback function of the loop event. 
 *       
 * @return  None.
 */
void os_user_loop_event_set(void (*callback)(void));

/*********************************************************************
 * @fn      os_user_loop_event_clear
 *
 * @brief   clear the event when you want not to run the loop.
 *
 * @param   None. 
 *       
 * @return  None.
 */
void os_user_loop_event_clear(void);
/**********************************************************************
 ************loop event eg***************
 
void user_loop_callback(void)
{
    static uint16_t count = 0;

    count++;
    if(count > 30000)
    {
    	count = 0;
    	uart_putc_noint(UART1,'t');
    	os_user_loop_event_clear(); // clear event
    }
    //uart_putc_noint(UART1,'t');
}

os_user_loop_event_set(&user_loop_callback); // create a loop event
 */

void os_task_process_saved_msg(uint8_t task_id);

#endif // APP_HT_H_
