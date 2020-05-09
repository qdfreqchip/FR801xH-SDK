#ifndef _AT_CMD_TASK_H_
#define _AT_CMD_TASK_H_


#include <stdint.h>

#include "driver_pmu.h"
#include "driver_uart.h"
#include "sys_utils.h"
#include "gap_api.h"


#define ADV_REPORT_NUM  10
#define BLE_CONNECTION_MAX (20)


#define ADV_LED_INIT    
#define ADV_LED_ON      pmu_set_led2_value(1)
#define ADV_LED_OFF     pmu_set_led2_value(0)

#define LINK_LED_INIT   do{ \
        pmu_set_port_mux(GPIO_PORT_D,GPIO_BIT_7,PMU_PORT_MUX_GPIO);    \
        pmu_set_gpio_value(GPIO_PORT_D, BIT(7), 0);   \
        pmu_set_pin_dir(GPIO_PORT_D,BIT(7), GPIO_DIR_OUT);     \
        pmu_set_pin_pull(GPIO_PORT_D, BIT(7), false);    \
        pmu_set_pin_to_PMU(GPIO_PORT_D,BIT(7));     \
    } while(0)
#define LINK_LED_ON     pmu_set_gpio_value(GPIO_PORT_D, BIT(7), 1)
#define LINK_LED_OFF    pmu_set_gpio_value(GPIO_PORT_D, BIT(7), 0)



enum
{
    AT_RECV_CMD,
    AT_RECV_TRANSPARENT_DATA,
    AT_TRANSPARENT_START_TIMER,
    AT_RECV_UPGRADE_DATA,
    RECV_CMD_MAX,
};


struct recv_cmd_t
{
    uint16_t recv_length;
    uint8_t recv_data[];
};

struct at_ctrl
{
    bool adv_ongoing;   //record adv status
    bool scan_ongoing;  //record scan status
    bool initialization_ongoing;    //record init status
    bool upgrade_start;
    bool async_evt_on_going;  //async event is on going, must wait util it end
    bool transparent_start;  //transparent flag
    bool one_slot_send_start;  //
    uint8_t curr_adv_int;  //current adv interval
    uint8_t transparent_conidx;  //
    uint32_t one_slot_send_len;  //
    uint16_t scan_duration; //uint: 10ms
};

#define IDLE_ROLE    (0)
#define SLAVE_ROLE   (1<<0)
#define MASTER_ROLE  (1<<1)
typedef struct at_defualt_info
{
    uint8_t role;    //0 = idle; 1 = salve; 2 = master;
    bool auto_transparent;   
    uint8_t rf_power;
    uint8_t adv_int;
    bool auto_sleep;
    uint8_t encryption_link;    //0 = idle; 'S' = salve enc; 'M' = master enc, 'B' = master bond;
} default_info_t;

struct at_conn_peer_param
{
    uint8_t encryption;
    uint8_t link_mode;
    conn_peer_param_t conn_param;
};
struct at_adv_report
{
    uint8_t        evt_type;
    uint8_t        adv_addr_type;
    mac_addr_t     adv_addr;
    uint8_t        data_len;
    uint8_t        data[0x1F];
    ///RSSI value for advertising packet (in dBm, between -127 and +20 dBm)
    int8_t         rssi;
};

struct at_buff_env
{
    uint16_t flash_page_idx;
    uint16_t flash_write_cnt;
    default_info_t default_info;
    uart_param_t uart_param;
    struct at_conn_peer_param master_peer_param;
    struct at_conn_peer_param peer_param[BLE_CONNECTION_MAX];
    struct at_adv_report adv_rpt[ADV_REPORT_NUM];
};

extern struct at_ctrl gAT_ctrl_env;
extern struct at_buff_env gAT_buff_env;
extern const uint8_t rf_power_arr[6];

/*********************************************************************
 * @fn      find_uart_idx_from_baudrate
 *
 * @brief   Misc function, tranfer actual baudrate value to uart_baud_map array index. 
 *			
 *
 * @param   baudrate - Actual baudrate value  
 *       	 
 *
 * @return  Array index in the uart_baud_map array
 */
uint8_t find_uart_idx_from_baudrate(uint32_t baudrate);


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
 * @fn      at_cb_adv_end
 *
 * @brief   at event call back function, handle after adv action end.
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_cb_adv_end(void *arg);

/*********************************************************************
 * @fn      at_recv_cmd_handler
 *
 * @brief   Handle at commands , this function is called in at_task when a whole AT cmd is detected
 *			
 *
 * @param   param - pointer to at command data buffer
 *       	
 *
 * @return  None
 */
void at_recv_cmd_handler(struct recv_cmd_t *param);

/*********************************************************************
 * @fn      at_send_rsp
 *
 * @brief   Common function for at command execution result response sending 
 *			
 *
 * @param   str - AT command execution result string. 
 *       	 
 *
 * @return  None
 */
void at_send_rsp(char *str);

/*********************************************************************
 * @fn      atuo_transparent_set
 *
 * @brief   Set flag and preparate to enter into transparent mode
 *			
 *
 * @param   param - pointer to at command data buffer
 *       	
 *
 * @return  None
 */
void atuo_transparent_set(void);

/*********************************************************************
 * @fn      atuo_transparent_clr
 *
 * @brief   Clear transparent mode flag and clear related profile data receive function. 
 *			
 *
 * @param   None
 *       	
 *
 * @return  None
 */
void atuo_transparent_clr(void);

/*********************************************************************
 * @fn      at_start_connecting
 *
 * @brief   Start a active connection opertaion
 *			
 *
 * @param   arg - reseved
 *       	
 *
 * @return  None
 */
void at_start_connecting(void *arg);

/*********************************************************************
 * @fn      at_start_advertising
 *
 * @brief   Start an advertising action 
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_start_advertising(void *arg);

/*********************************************************************
 * @fn      at_init_gap_cb_func
 *
 * @brief   initialization setting for AT event : AT_GAP_CB_ADV_END & AT_GAP_CB_DISCONNECT
 *			
 *
 * @param   arg - reserved
 *       	
 *
 * @return  None
 */
void at_init_gap_cb_func(void);

/*********************************************************************
 * @fn      at_spss_recv_data_ind_func
 *
 * @brief   This function will send data which is received from AT service profile to UART0.
 *			
 *
 * @param   value  - point to data buffer received from AT service profile  
 *       	length - data buffer length
 *
 * @return   None
 */
void at_spss_recv_data_ind_func(uint8_t *value, uint16_t length);

/*********************************************************************
 * @fn      at_spsc_recv_data_ind_func
 *
 * @brief   This function will send data which is received from AT client profile to UART0.
 *			
 *
 * @param   value  - point to data buffer received from AT service profile  
 *       	length - data buffer length
 *
 * @return   None
 */
void at_spsc_recv_data_ind_func(uint8_t *value, uint16_t length);
/*********************************************************************
 * @fn      at_init_adv_rsp_parameter
 *
 * @brief   Set advertising data, include manufacturer_value,HID_SERVICE_UUID, 
 *			and set adv configration parameters.
 *
 * @param   None
 *       	
 *
 * @return  None
 */
void at_init_advertising_parameter(void);


#endif
