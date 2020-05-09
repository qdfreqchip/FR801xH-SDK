#ifndef _AT_GAP_EVENT_H_
#define _AT_GAP_EVENT_H_

#include <stdint.h>
#include "gap_api.h"

enum at_cb_func_idx
{
    AT_GAP_CB_ADV_END,
    AT_GAP_CB_SCAN_END,
    AT_GAP_CB_ADV_RPT,
    AT_GAP_CB_CONN_END,
    AT_GAP_CB_DISCONNECT,
    AT_GAP_CB_MAX,
};
typedef void (*at_cb_func_t)(void *arg);

extern uint16_t current_con_interval;

/*********************************************************************
 * @fn      at_set_gap_cb_func
 *
 * @brief   Fucntion to set at event call back function
 *
 * @param   func_idx - at event idx 
 *       	func 	 - at event call back function 
 *
 * @return  None.
 */
void at_set_gap_cb_func(enum at_cb_func_idx func_idx,at_cb_func_t func);

/*********************************************************************
 * @fn      at_slave_encrypted
 *
 * @brief   at event call back function, handle after link is lost
 *
 * @param   arg - point to buff of gap_evt_disconnect_t struct type
 *       
 *
 * @return  None.
 */
void at_cb_disconnected(void *arg);

/*********************************************************************
 * @fn      at_con_param_update
 *
 * @brief   function to update link parameters.
 *
 * @param   conidx  - indicate which link idx will do patameter updation 
 *       	latency - indicate latency for patameter updation operation 
 *
 * @return  None.
 */
void at_con_param_update(uint8_t conidx,uint16_t latency);

/*********************************************************************
 * @fn      proj_ble_gap_evt_func
 *
 * @brief   Gap event handler function.
 *
 * @param   event - gap event to be posted
 *       
 *
 * @return  None.
 */
void proj_ble_gap_evt_func(gap_event_t *event);

#endif //_AT_GAP_EVENT_H_


