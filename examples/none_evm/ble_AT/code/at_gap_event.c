/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include <string.h>

#include "co_printf.h"
#include "os_timer.h"
#include "gap_api.h"
#include "gatt_api.h"
#include "sys_utils.h"
#include "at_profile_spsc.h"
#include "at_profile_spss.h"
#include "at_gap_event.h"
#include "at_cmd_task.h"
#include "at_recv_cmd.h"

#include "ble_hl_error.h"
#include "driver_system.h"
#define LINK_INTERVAL_MIN (24)      //(6)

//Current link interval to be updated
static uint16_t current_con_interval = LINK_INTERVAL_MIN;

//Timer for delayed updation operation of link parameters
static os_timer_t update_param_timer;

//Variable to store at event callback functions
static at_cb_func_t at_cb_func[AT_GAP_CB_MAX] = {0};

static void at_master_connected(conn_peer_param_t *param);
static void at_master_encrypted(uint8_t conidx);
static void at_cb_slave_connected(conn_peer_param_t *param);
static void at_slave_encrypted(uint8_t conidx);
static void at_all_svc_added(void);

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
void at_con_param_update(uint8_t conidx,uint16_t latency)
{
    conn_peer_param_t *con_param = gap_get_latest_conn_parameter();

    if( con_param->con_latency != latency || con_param->con_interval > current_con_interval || con_param->sup_to != 400 )
    {
        gap_conn_param_update(conidx, current_con_interval,current_con_interval,latency,400);
    }
}

/*********************************************************************
 * @fn      param_timer_func
 *
 * @brief   execute function of timer update_param_timer
 *
 * @param   arg  - timer function input parameter  
 *       
 *
 * @return  None.
 */
static void param_timer_func(void *arg)
{
    if( gAT_buff_env.default_info.auto_sleep == true)
        at_con_param_update((uint8_t)arg,15);
    else
        at_con_param_update((uint8_t)arg,0);
}

/*********************************************************************
 * @fn      at_cb_param_ind
 *
 * @brief   at event call back function, indicate link parameter is updated
 *
 * @param   ind - indicate link parameter which is updated 
 *       
 *
 * @return  None.
 */
static void at_cb_param_ind(gap_evt_link_param_update_t *ind)
{
    co_printf("con[%d]:%d\r\n",ind->conidx,ind->con_interval);

    if(ind->con_interval > LINK_INTERVAL_MIN)
    {
        if( gAT_buff_env.default_info.auto_sleep == true)
            at_con_param_update(ind->conidx,15);
        else
            at_con_param_update(ind->conidx,0);
    }
}

/*********************************************************************
 * @fn      at_cb_param_rejected
 *
 * @brief   at event call back function, indicate link parameter is rejected
 *
 * @param   cs - parameter contain conidx and status 
 *       
 *
 * @return  None.
 */
static void at_cb_param_rejected(gap_evt_link_param_reject_t *cs)
{
    if(cs->status == GAP_ERR_COMMAND_DISALLOWED)
        goto _exit;
    if(cs->status != GAP_ERR_REJECTED && cs->status != LL_ERR_UNACCEPTABLE_CONN_INT
       && cs->status != GAP_ERR_INVALID_PARAM)
        os_timer_start(&update_param_timer,3000,0);
    else
    {
        if(current_con_interval < 15)
        {
            current_con_interval += 3;
            co_printf("r_u:%d\r\n",current_con_interval);
            os_timer_start(&update_param_timer,3000,0);
        }
        else
        {
            os_timer_stop(&update_param_timer);
        }
    }
_exit:
    ;
}


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
void at_set_gap_cb_func(enum at_cb_func_idx func_idx,at_cb_func_t func)
{
    at_cb_func[func_idx] = func;
}

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
void proj_ble_gap_evt_func(gap_event_t *event)
{
    switch(event->type)
    {
        case GAP_EVT_ADV_END:
        {
            co_printf("adv_end,status:0x%02x\r\n",event->param.adv_end.status);
            gAT_ctrl_env.adv_ongoing = false;

            if(at_cb_func[AT_GAP_CB_ADV_END]!=NULL)
                at_cb_func[AT_GAP_CB_ADV_END](NULL);
        }
        break;
        case GAP_EVT_SCAN_END:
        {
            co_printf("scan_end,status:0x%02x\r\n",event->param.scan_end_status);
            gap_set_link_rssi_report(false);
            gAT_ctrl_env.scan_ongoing = false;

            if(at_cb_func[AT_GAP_CB_SCAN_END]!=NULL)
                at_cb_func[AT_GAP_CB_SCAN_END](NULL);
        }
        break;
        case GAP_EVT_ADV_REPORT:
        {
            co_printf("addr_type:%d,adv_rpt,rssi:%d,evt_type:%d\r\n",event->param.adv_rpt->src_addr.addr_type,
                      event->param.adv_rpt->rssi,event->param.adv_rpt->evt_type);
            /*co_printf("mac:");
            show_reg(event->param.adv_rpt->src_addr.addr.addr,6,1);
            co_printf("content:");
            show_reg(event->param.adv_rpt->data,event->param.adv_rpt->length,1);
            co_printf("\r\n");*/
            if(at_cb_func[AT_GAP_CB_ADV_RPT]!=NULL)
                at_cb_func[AT_GAP_CB_ADV_RPT](event->param.adv_rpt);
        }
        break;

        case GAP_EVT_ALL_SVC_ADDED:
        {
            co_printf("all svc added\r\n");
            at_all_svc_added();
        }
        break;

        case GAP_EVT_MASTER_CONNECT:
        {
            at_master_connected(&(event->param.master_connect));
        }
        break;

        case GAP_EVT_SLAVE_CONNECT:
        {
            at_cb_slave_connected(&(event->param.slave_connect));
        }
        break;

        case GAP_EVT_DISCONNECT:
        {
            if(at_cb_func[AT_GAP_CB_DISCONNECT]!=NULL)
                at_cb_func[AT_GAP_CB_DISCONNECT](&(event->param.disconnect));
        }
        break;

        case GAP_EVT_LINK_PARAM_REJECT:
            co_printf("Link[%d]param reject,status:0x%02x\r\n",event->param.link_reject.conidx,event->param.link_reject.status);
            at_cb_param_rejected(&(event->param.link_reject));
            break;

        case GAP_EVT_LINK_PARAM_UPDATE:
            co_printf("Link[%d]param update,interval:%d,latency:%d,timeout:%d\r\n",event->param.link_update.conidx
                      ,event->param.link_update.con_interval,event->param.link_update.con_latency,event->param.link_update.sup_to);
            at_cb_param_ind(&(event->param.link_update));
            break;

        case GAP_EVT_CONN_END:
        {
            co_printf("conn_end,reason:0x%02x\r\n",event->param.conn_end_reason);
            gAT_ctrl_env.initialization_ongoing = false;

            if(at_cb_func[AT_GAP_CB_CONN_END]!=NULL)
                at_cb_func[AT_GAP_CB_CONN_END](NULL);
        }
        break;

        case GAP_EVT_PEER_FEATURE:
            co_printf("peer[%d] feats ind\r\n",event->param.peer_feature.conidx);
            show_reg((uint8_t *)&(event->param.peer_feature.features),8,1);
            break;

        case GAP_EVT_MTU:
            co_printf("mtu update,conidx=%d,mtu=%d\r\n",event->param.mtu.conidx,event->param.mtu.value);
            break;
        case GAP_EVT_LINK_RSSI:
            co_printf("link rssi %d\r\n",event->param.link_rssi);
            break;
        case GAP_SEC_EVT_MASTER_AUTH_REQ:
            co_printf("link[%d],recv auth req:0x%02x\r\n",event->param.auth_req.conidx,event->param.auth_req.auth);
            break;
        case GAP_SEC_EVT_MASTER_ENCRYPT:
            at_master_encrypted(event->param.master_encrypt_conidx);
            break;
        case GAP_SEC_EVT_SLAVE_ENCRYPT:
            co_printf("slave[%d]_encrypted\r\n",event->param.slave_encrypt_conidx);
            at_slave_encrypted(event->param.slave_encrypt_conidx);
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      at_cb_slave_connected
 *
 * @brief   at event call back function, handle after slave role link is established
 *
 * @param   param - link parameters 
 *       
 *
 * @return  None.
 */
void at_cb_slave_connected(conn_peer_param_t *param)
{
    memcpy(&(gAT_buff_env.peer_param[param->conidx].conn_param),param,sizeof(conn_peer_param_t));
    gAT_buff_env.peer_param[param->conidx].link_mode = SLAVE_ROLE;  //slave
    gAT_buff_env.peer_param[param->conidx].encryption = false;  //slave

    if(gAT_ctrl_env.transparent_start == false)
    {
        uint8_t at_rsp[30];
        sprintf((char *)at_rsp,"+CONN:%d\r\nOK",param->conidx);
        at_send_rsp((char *)at_rsp);
    }
    gap_security_req(param->conidx);

    current_con_interval = LINK_INTERVAL_MIN;
    os_timer_init( &update_param_timer,param_timer_func,(void *)(param->conidx));
    os_timer_start(&update_param_timer,1500,0);
    LINK_LED_ON;
    if(gap_get_connect_num()==1)
        atuo_transparent_set();
    else
        atuo_transparent_clr();
}

/*********************************************************************
 * @fn      at_slave_encrypted
 *
 * @brief   at event call back function, handle after slave role link is encrypted
 *
 * @param   conidx - link index 
 *       
 *
 * @return  None.
 */
void at_slave_encrypted(uint8_t conidx)
{
    gAT_buff_env.peer_param[conidx].encryption = true;  //slave
}


/*********************************************************************
 * @fn      at_master_connected
 *
 * @brief   at event call back function, handle after master role link is established
 *
 * @param   param - link parameters 
 *       
 *
 * @return  None.
 */
void at_master_connected(conn_peer_param_t *param)
{
    memcpy(&(gAT_buff_env.peer_param[param->conidx].conn_param),param,sizeof(conn_peer_param_t));
    gAT_buff_env.peer_param[param->conidx].link_mode = MASTER_ROLE;  //master
    gAT_buff_env.peer_param[param->conidx].encryption = false;

    memcpy(&(gAT_buff_env.master_peer_param.conn_param),param,sizeof(conn_peer_param_t));
    gAT_buff_env.master_peer_param.link_mode = MASTER_ROLE;  //master

    co_printf("at_master_connected\r\n");

    if(gAT_ctrl_env.transparent_start == false)         //if(gAT_ctrl_env.async_evt_on_going)
    {
        uint8_t at_rsp[30];
        sprintf((char *)at_rsp,"+CONN:%d\r\nOK",param->conidx);
        at_send_rsp((char *)at_rsp);
        gAT_ctrl_env.async_evt_on_going = false;
    }

    if(gAT_buff_env.default_info.encryption_link == 'M'
       || gAT_buff_env.default_info.encryption_link == 'B')
    {
        if (gap_security_get_bond_status() && gAT_buff_env.default_info.encryption_link == 'M')
            gap_security_enc_req(param->conidx);
        else
            gap_security_pairing_req(param->conidx);
    }
    else
    {
        gatt_discovery_all_peer_svc(spsc_client_id,param->conidx);
        LINK_LED_ON;
        if(gap_get_connect_num() == 1)
            atuo_transparent_set();
        else
            atuo_transparent_clr();
    }
}

/*********************************************************************
 * @fn      at_slave_encrypted
 *
 * @brief   at event call back function, handle after master role link is encrypted
 *
 * @param   conidx - link index 
 *       
 *
 * @return  None.
 */
void at_master_encrypted(uint8_t conidx)
{
    co_printf("at_master_encrypted[%d]\r\n",conidx);
    gAT_buff_env.peer_param[conidx].encryption = true;

    gatt_discovery_all_peer_svc(spsc_client_id,conidx);
    LINK_LED_ON;
    if(gap_get_connect_num() == 1)
        atuo_transparent_set();
    else
        atuo_transparent_clr();
}

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
void at_cb_disconnected(void *arg)
{
    gap_evt_disconnect_t *param = (gap_evt_disconnect_t *)arg;
    co_printf("at_disconnect[%d]\r\n",param->conidx);

    if(gap_get_connect_num()==0)
    {
        LINK_LED_OFF;
        atuo_transparent_clr();
    }

    if( (gAT_ctrl_env.async_evt_on_going)
        || (gAT_ctrl_env.async_evt_on_going == false && gAT_ctrl_env.transparent_start == false)    //passive disconnect,no transparant, then report to host
      )
    {
        uint8_t at_rsp[30];
        sprintf((char *)at_rsp,"+DISCONN:%d\r\nOK",param->conidx);
        at_send_rsp((char *)at_rsp);
        gAT_ctrl_env.async_evt_on_going = false;
    }

// one slot send, but link loss
    if(gAT_ctrl_env.one_slot_send_start && gAT_ctrl_env.one_slot_send_len > 0)
    {
        if(gap_get_connect_status(gAT_ctrl_env.transparent_conidx)==false)
        {
            at_clr_uart_buff();
            gAT_ctrl_env.one_slot_send_start = false;
            gAT_ctrl_env.one_slot_send_len = 0;
            uint8_t at_rsp[] = "SEND FAIL";
            at_send_rsp((char *)at_rsp);
        }
    }
}


/*********************************************************************
 * @fn      at_slave_encrypted
 *
 * @brief   at event call back function, handle after all profile is created
 *
 * @param   None
 *       
 *
 * @return  None.
 */
void at_all_svc_added(void)
{
    if(gAT_buff_env.default_info.role & MASTER_ROLE)
        at_start_connecting(NULL);
    if(gAT_buff_env.default_info.role & SLAVE_ROLE)
        at_start_advertising(NULL);
    if( gAT_buff_env.default_info.auto_sleep)
        system_sleep_enable();
    else
        system_sleep_disable();
}






