/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef MESH_API_H
#define MESH_API_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "gap_api.h"

/*
 * MACROS 
 */

/** @defgroup MODEL_VENDOR_MODE, model is from SIG or vendor specific.
 * @{
 */
#define MODEL_TYPE_SIG              0   //!< SIG standard model type.
#define MODEL_TYPE_VENDOR           1   //!< Vender specific model type.

/*
 * CONSTANTS 
 */

/*
 * TYPEDEFS 
 */
// Mesh event type define 
enum mesh_event_type_t 
{
    MESH_EVT_STARTED,                   //!< Mesh started.
    MESH_EVT_STOPPED,                   //!< Mesh stopped.
    MESH_EVT_RESET,                     //!< Received reset command from provisioner.
    MESH_EVT_READY,                     //!< Mesh is ready, can be started.
    MESH_EVT_IN_NETWORK,                //!< The device has been added to the network.
    MESH_EVT_MODEL_APPKEY_BINDED,       //!< Model is binded with an appkey, used in ali mesh provisioning procedure
    MESH_EVT_MODEL_GRPADDR_SUBED,       //!< Model has subscribed to a group addr, used in ali mesh provisioning procedure. 5
    MESH_EVT_PROV_PARAM_REQ,            //!< Received provision parameter request from provisioner.
    MESH_EVT_PROV_AUTH_DATA_REQ,        //!< Received authentication data request from provisioner.
    MESH_EVT_PROV_RESULT,               //!< Received provision result.
    MESH_EVT_UPDATE_IND,                //!< Received information update event, can be keys updated..
    MESH_EVT_RECV_MSG,                  //!< Received a mesh message. 10
    MESH_EVT_COMPO_DATA_REQ,            //!< Received composition data request from provisioner.
    MESH_EVT_ADV_REPORT,                //!< User interface for deal ADV packets received from adv bearer.
};

// Mesh network information update type
enum mesh_update_type_t {
    MESH_UPD_TYPE_NET_KEY_UPDATED,      //!< Network key updated
    MESH_UPD_TYPE_NET_KEY_DELETED,      //!< Network key deleted
    MESH_UPD_TYPE_APP_KEY_UPDATED,      //!< Application key updated
    MESH_UPD_TYPE_APP_KEY_DELETED,      //!< Application key deleted
    MESH_UPD_TYPE_PUBLI_PARAM,          //!< Model publication parameters updated
    MESH_UPD_TYPE_SUBS_LIST,            //!< Model subscription list updated
    MESH_UPD_TYPE_BINDING,              //!< Model/application key binding updated
    MESH_UPD_TYPE_STATE,                //!< State updated
};

// Mesh supported feature type define.
enum mesh_feature_t 
{
    MESH_FEATURE_RELAY      = (1<<0),   //!< Relay mode support.
    MESH_FEATURE_PROXY      = (1<<1),   //!< Proxy mode support.
    MESH_FEATURE_FRIEND     = (1<<2),   //!< Friendly mode support.
    MESH_FEATURE_LOW_POWER  = (1<<3),   //!< Low Power Node mode support.
    MESH_FEATURE_PB_GATT    = (1<<4),   //!< GATT bearer support.
};

// Mesh provision out oob mode.
enum mesh_prov_out_oob
{
    MESH_PROV_OUT_OOB_BLINK        = 0x0001,    //!< Bit[0]: Blink
    MESH_PROV_OUT_OOB_BEEP         = 0x0002,    //!< Bit[1]: Beep
    MESH_PROV_OUT_OOB_VIBRATE      = 0x0004,    //!< Bit[2]: Vibrate
    MESH_PROV_OUT_OOB_NUMERIC      = 0x0008,    //!< Bit[3]: Output Numeric
    MESH_PROV_OUT_OOB_ALPHANUMERIC = 0x0010,    //!< Bit[4]: Output Alphanumeric
                                                //!< Bit[5-15]: Reserved for Future Use
};

/// Input OOB Action field values
enum mesh_prov_in_oob
{
    MESH_PROV_IN_OOB_PUSH         = 0x0001,     //!< Bit[0]: Push
    MESH_PROV_IN_OOB_TWIST        = 0x0002,     //!< Bit[1]: Twist
    MESH_PROV_IN_OOB_NUMERIC      = 0x0004,     //!< Bit[2]: Input Numeric
    MESH_PROV_IN_OOB_ALPHANUMERIC = 0x0008,     //!< Bit[3]: Input Alphanumeric
                                                //!< Bit[4-15]: Reserved for Future Use
};

// Mesh provision information.
enum mesh_prov_info
{
    MESH_PROV_INFO_URI_HASH_PRESENT = (1 << 0), //!< URI Hash present or not in the unprovisioned device beacon
};

/// State of the provisioning
enum mesh_prov_state
{
    MESH_PROV_STARTED,      //!< Provisioning started - procedure started by a provisioner 
    MESH_PROV_SUCCEED,      //!< Provisioning succeed
    MESH_PROV_FAILED,       //!< Provisioning failed
};

// Mesh publish message type define
typedef struct
{
    uint8_t     element_idx;    //!< Element index of the received message.
    uint32_t    model_id;       //!< Model ID of the received message.
    uint32_t    opcode;         //!< Mesh message operation code (can be 1, 2 or 3 octet operation code).
    uint16_t    msg_len;        //!< Message length.
    uint8_t     msg[];          //!< Message data.
} mesh_publish_msg_t;

// Mesh response message type define
typedef struct
{
    uint8_t     element_idx;    //!< Element index of the sending message.
    uint8_t     app_key_lid;    //!< App key index of the sending message.
    uint32_t    model_id;       //!< Model ID of the sending message.
    uint32_t    opcode;         //!< Mesh message operation code (can be 1, 2 or 3 octet operation code).
    uint16_t    dst_addr;       //!< Destination address of the sending message.
    uint16_t    msg_len;        //!< Message length.
    uint8_t     msg[];          //!< Message data.
} mesh_rsp_msg_t;

// Mesh receive message type define
typedef struct
{
    uint32_t    model_id;       //!< Model ID of the received message.
    uint32_t    opcode;         //!< Mesh message operation code (can be 1, 2 or 3 octet operation code).
    uint16_t    msg_len;        //!< Message length.
    uint8_t     *p_msg;         //!< Message data.
} mesh_recv_msg_t;

/// Mesh Provisioning state change indication
typedef struct
{
    uint8_t     state;          //!< Provisioning procedure state (@see enum mesh_prov_state) 
    uint16_t    status;         //!< Relevant only for provisioning failed (failed reason) 
} mesh_prov_result_ind_t;

/// Inform reception of a specific mesh message
typedef struct
{
    uint32_t        model_id;           //!< Model id 
    uint8_t         element;            //!< element 
    uint8_t         app_key_lid;        //!< Application Key Local identifier (Required for a response) 
    int8_t          rssi;               //!< Measured RSSI level for the received PDU. 
    uint8_t         not_relayed;        //!< 1 = if message have been received by an immediate peer; 0 = it can have been relayed 
    uint32_t        opcode;             //!< Mesh message operation code (can be 1, 2 or 3 octet operation code) 
    uint16_t        src;                //!< Source address of the message (Required for a response) 
    uint16_t        msg_len;            //!< Message length     
    const uint8_t   *msg;               //!< Message content 
} mesh_model_msg_ind_t;

/// Network key information entry structure if network key is not being updated
typedef struct mesh_netkey
{
    uint8_t  length;            //!< Entry length
    uint8_t  info;              //!< Information
    uint16_t netkey_id;         //!< NetKey ID
    uint8_t  key[16];           //!< Network Key
} mesh_netkey_t;

/// Network key information entry structure if network key is being updated
typedef struct mesh_netkey_upd
{
    uint8_t  length;            //!< Entry length
    uint8_t  info;              //!< Information
    uint16_t netkey_id;         //!< NetKey ID
    uint8_t  key[16];           //!< Network Key
    uint8_t  new_key[16];       //!< New network key
} mesh_netkey_upd_t;

/// Application key information entry structure if application key is not being updated
typedef struct mesh_appkey
{
    uint8_t  length;            //!< Entry length
    uint8_t  info;              //!< Information
    uint16_t netkey_id;         //!< NetKey ID
    uint16_t appkey_id;         //!< AppKey ID
    uint8_t  key[16];           //!< Application Key
} mesh_appkey_t;

/// Application key information entry structure if application key is being updated
typedef struct mesh_appkey_upd
{
    uint8_t  length;                //!< Entry length
    uint8_t  info;                  //!< Information
    uint16_t netkey_id;             //!< NetKey ID
    uint16_t appkey_id;             //!< AppKey ID
    uint8_t  key[16];               //!< Application Key
    uint8_t  new_key[16];           //!< New application key
} mesh_appkey_upd_t;

/// Model publication parameter entry structure if publication address is not a virtual address
typedef struct mesh_publi
{
    uint8_t  length;        //!< Entry length
    uint8_t  info;          //!< Information
    uint16_t element_addr;  //!< Element address
    uint32_t model_id;      //!< Model ID
    uint16_t addr;          //!< Publication address
    uint16_t appkey_id;     //!< AppKey ID
    uint8_t  ttl;           //!< TTL
    uint8_t  period;        //!< Period
    uint8_t  retx_params;   //!< Retransmission parameters
    uint8_t  friend_cred;   //!< Friend credentials
} mesh_publi_t;

/// Model publication parameter entry structure if publication address is a virtual address
typedef struct mesh_publi_virt
{
    uint8_t  length;        //!< Entry length
    uint8_t  info;          //!< Information
    uint16_t element_addr;  //!< Element address
    uint32_t model_id;      //!< Model ID
    uint16_t addr;          //!< Publication address
    uint16_t appkey_id;     //!< AppKey ID
    uint8_t  ttl;           //!< TTL
    uint8_t  period;        //!< Period
    uint8_t  retx_params;   //!< Retransmission parameters
    uint8_t  friend_cred;   //!< Friend credentials
    uint8_t  label_uuid[16];//!< Label UUID
} mesh_publi_virt_t;

/// Model subscription list entry structure
typedef struct mesh_subs
{
    uint8_t  length;                //!< Entry length
    uint8_t  info;                  //!< Information
    uint16_t element_addr;          //!< Element address
    uint32_t model_id;              //!< Model ID
    uint8_t  list[];                //!< List
} mesh_subs_t;

/// Model/Application key binding entry structure
typedef struct mesh_binding
{
    uint8_t  length;                    //!< Entry length
    uint8_t  info;                      //!< Information
    uint16_t element_addr;              //!< Element address
    uint32_t model_id;                  //!< Model ID
    uint16_t appkey_ids[];              //!< List of AppKey IDs
} mesh_binding_t;

/// Configuration update indication message structure
typedef struct mesh_update_ind
{
    uint8_t upd_type;           //!< Update type
    uint8_t length;             //!< Entry length
    uint8_t data[];             //!< Entry value
} mesh_update_ind_t;

// Mesh event structure
typedef struct
{
    enum mesh_event_type_t      type;                   //!< Mesh event type, see @mesh_event_type_t
    union
    {
        mesh_prov_result_ind_t  prov_result;            //!< Provision result, see @mesh_prov_result_ind_t
        mesh_update_ind_t       *update_ind;            //!< Mesh status update indication message.
        mesh_model_msg_ind_t 	model_msg;              //!< Mesh model message.
        uint8_t                 compo_data_req_page;	//!< Mesh composition data request page.
        gap_evt_adv_report_t 	adv_report;             //!< ADV report from adv bearer.
    } param;
} mesh_event_t;

// Mesh model struct.
typedef struct
{
    uint32_t        model_id;                           //!< Model ID of the model to be added.
    uint8_t         model_vendor;                       //!< If it is SIG model or vendor specific model. @MODEL_VENDOR_MODE
    uint8_t         element_idx;                        //!< Element index in the mesh device.
    void (*msg_handler)(const mesh_model_msg_ind_t*);   //!< Model message handler.
} mesh_model_t;

// Gap callback function define for mesh event handling.
typedef void(* mesh_callback_func_t)(mesh_event_t * event);

/*
 * GLOBAL VARIABLES 
 */

/*
 * LOCAL VARIABLES 
 */


/*
 * PUBLIC FUNCTIONS 
 */

/*********************************************************************
 * @fn      mesh_set_cb_func
 *
 * @brief   Mesh callback functions set. Links application layer callbacks to Mesh layer.
 *
 * @param   mesh_evt_cb  - Application layer callback function, defined in application layer.
 *
 * @return  None.
 */
void mesh_set_cb_func(mesh_callback_func_t mesh_evt_cb);

/*********************************************************************
 * @fn      mesh_init
 *
 * @brief   Initialize mesh function in stack.
 *
 * @param   feature -   indicate which features should be enabled.
 *          store_addr  - the flash address used to store mesh link information,
 *                        such as network key, app key, binding info, etc.
 *
 * @return  None.
 */
void mesh_init(enum mesh_feature_t feature, uint32_t store_addr);

/*********************************************************************
 * @fn      mesh_set_runtime
 *
 * @brief   set current time.
 *
 * @param   None.
 *
 * @return  None.
 */
void mesh_set_runtime(void);

/*********************************************************************
 * @fn      mesh_start
 *
 * @brief   start mesh profile once it is initialized.
 *
 * @param   None.
 *
 * @return  None.
 */
void mesh_start(void);

/*********************************************************************
 * @fn      mesh_start
 *
 * @brief   stop mesh profile.
 *
 * @param   None.
 *
 * @return  None.
 */
void mesh_stop(void);

/*********************************************************************
 * @fn      mesh_model_bind_appkey
 *
 * @brief   binding model with indicate app key
 *
 * @param   model_id    - the operated model id
 *          element     - which element this model belongs to
 *          app_key_index - which key is this model binded with
 *
 * @return  None.
 */
void mesh_model_bind_appkey(uint32_t model_id, uint8_t element, uint8_t app_key_index);

/*********************************************************************
 * @fn      mesh_model_sub_group_addr
 *
 * @brief   model subscripts indicated group address
 *
 * @param   model_id    - the operated model id
 *          element     - which element this model belongs to
 *          group_addr  - which group address does this model subscript to
 *
 * @return  None.
 */
void mesh_model_sub_group_addr(uint32_t model_id, uint8_t element, uint16_t group_addr);

/*********************************************************************
 * @fn      mesh_add_model
 *
 * @brief   Add mesh models.
 *
 * @param   mesh_model_t     - Address of mesh models array to be added
 *
 * @return  None.
 */
void mesh_add_model(const mesh_model_t *p_model);

/*********************************************************************
 * @fn      mesh_get_remote_param
 *
 * @brief   Get the necessaire param for send rsp msg.
 *
 * @param   src -- The src id about the gateway device.
 *
 * @param   app_key_lid -- App key index of the sending message.
 * 
 * @return  None.
 */
void mesh_get_remote_param(uint16_t * src,uint8_t * app_key_lid);

/*********************************************************************
 * @fn      mesh_publish_msg
 *
 * @brief   Publish a message in mesh network.
 *
 * @param   mesh_publish_msg_t     - Address of message to be published.
 *
 * @return  None.
 */
void mesh_publish_msg(mesh_publish_msg_t *p_publish_msg);

/*********************************************************************
 * @fn      mesh_send_rsp
 *
 * @brief   Send a response message in mesh network.
 *
 * @param   p_rsp_msg   - Address of message to be send.
 *
 * @return  None.
 */
void mesh_send_rsp(mesh_rsp_msg_t *p_rsp_msg);

/*********************************************************************
 * @fn      mesh_send_prov_param_rsp
 *
 * @brief   response to provision parameter request.
 *
 * @param   uuid        - device uuid.
 *          uri_hash    -
 *          oob_info    - 
 *          pub_key_oob - Public key OOB information available
 *          static_oob  - Static OOB information available
 *          out_oob_size - Maximum size of Output OOB supported
 *          in_oob_size - Maximum size in octets of Input OOB supported
 *          out_oob_action - Supported Output OOB Actions (@see enum mesh_prov_out_oob)
 *          in_oob_action - Supported Input OOB Actions (@see enum mesh_prov_in_oob)
 *          nb_elt      - number of element
 *          info        - Bit field providing additional information (@see enum mesh_prov_info) 
 *
 * @return  None.
 */
void mesh_send_prov_param_rsp(uint8_t *uuid, uint32_t uri_hash, uint16_t oob_info, uint8_t pub_key_oob,
                                        uint8_t static_oob, uint8_t out_oob_size, uint8_t in_oob_size,
                                        uint16_t out_oob_action, uint16_t in_oob_action, uint8_t nb_elt, uint8_t info);

/*********************************************************************
 * @fn      mesh_send_prov_auth_data_rsp
 *
 * @brief   response to provision authentication data request.
 *
 * @param   accept      - accept or not.
 *          auth_size   - length of authentication data
 *          auth_data   - pointer to authentication data memory
 *
 * @return  None.
 */
void mesh_send_prov_auth_data_rsp(uint8_t accept, uint8_t auth_size, uint8_t *auth_data);

/*********************************************************************
 * @fn      mesh_send_compo_data_rsp
 *
 * @brief   response to composition data request.
 *
 * @param   page    - which page response data belongs to.
 *          data    - data pointer
 *          length  - data length
 *
 * @return  None.
 */
void mesh_send_compo_data_rsp(uint8_t page, uint8_t *data, uint8_t length);

/*********************************************************************
 * @fn      mesh_info_store_into_flash
 *
 * @brief   store mesh network information into flash. For avoid program flash
 *          too frequently, app level should call this function with 2 or more
 *          second delay after receiving MESH_EVT_UPDATE_IND meassge.
 *
 * @param   None.
 *
 * @return  None.
 */
void mesh_info_store_into_flash(void);

/*********************************************************************
 * @fn      mesh_info_clear
 *
 * @brief   used to clear mesh network information stored in flash.
 *
 * @param   None.
 *
 * @return  None.
 */
void mesh_info_clear(void);

#endif

