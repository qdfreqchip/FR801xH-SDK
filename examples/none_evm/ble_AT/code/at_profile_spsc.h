#ifndef _AT_PROFILE_SPSC_H_
#define _AT_PROFILE_SPSC_H_


#include <stdint.h>
#include "at_profile_spss.h"

extern at_recv_data_func_t spsc_recv_data_ind_func;
extern uint16_t spsc_client_id;

/*********************************************************************
 * @fn      at_profile_spsc_init
 *
 * @brief   Create AT profile client
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_profile_spsc_init(void);

/*********************************************************************
 * @fn      at_spsc_send_data
 *
 * @brief   function to write date to peer. write without response
 *
 * @param   conidx - link  index
 *       	data   - pointer to data buffer 
 *       	len    - data len
 *
 * @return  None
 */
void at_spsc_send_data(uint8_t conidx, uint8_t *data, uint8_t len);


#endif
