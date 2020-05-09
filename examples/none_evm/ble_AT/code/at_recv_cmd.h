#ifndef _AT_RECV_CMD_H_
#define _AT_RECV_CMD_H_

#include <stdint.h>

#define AT_MAIN_VER (0)

/*********************************************************************
 * @fn      at_init
 *
 * @brief   Initializate gAT_env elements and assign UART0 pins
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_init(void);

/*********************************************************************
 * @fn      at_store_info_to_flash
 *
 * @brief   Store AT infomations to flash, AT+FLASH cmd will carry out this function
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_store_info_to_flash(void);

/*********************************************************************
 * @fn      at_load_info_from_flash
 *
 * @brief   Load AT infomations from flash, this function should be called at entry: user_entry_after_ble_init
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_load_info_from_flash(void);
/*********************************************************************
 * @fn      at_clr_flash_info
 *
 * @brief   Clear AT infomations from flash, this function is reserved for future uasge
 *			
 *
 * @param   None 
 *       	 
 *
 * @return  None
 */
void at_clr_flash_info(void);

/*********************************************************************
 * @fn      at_clr_uart_buff
 *
 * @brief   Reset uart char receive index to zero.
 *			
 *
 * @param   None
 *       	 
 *
 * @return  None
 */
void at_clr_uart_buff(void);



#endif //_APP_AT_H
