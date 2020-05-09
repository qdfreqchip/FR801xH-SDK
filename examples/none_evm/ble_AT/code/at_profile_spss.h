#ifndef _AT_PROFILE_SPSS_H_
#define _AT_PROFILE_SPSS_H_


#include <stdint.h>
typedef void (*at_recv_data_func_t)(uint8_t *value, uint16_t length);


extern at_recv_data_func_t spss_recv_data_ind_func;
extern uint16_t spss_svc_id;
extern uint8_t spss_uuids[64];

/*********************************************************************
 * @fn      at_profile_spss_init
 *
 * @brief   Create AT profile service
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_profile_spss_init(void);

/*********************************************************************
 * @fn      at_spss_send_data
 *
 * @brief   function to notification date to peer.
 *
 * @param   conidx - link  index
 *       	data   - pointer to data buffer 
 *       	len    - data len
 *
 * @return  None
 */
void at_spss_send_data(uint8_t conidx, uint8_t *data, uint8_t len);


#endif
