/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef GATT_API_H
#define GATT_API_H

/*
* INCLUDES (包含头文件)
*/
#include <stdint.h>

/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */
#define MAX_PEER_SVC_NUM    (10)
#define MAX_CLEINT_ATT_NUM  (50)

#define UUID_SIZE_2         2                   //!< 2 bytes UUID, usually SIG assigned UUID.
#define UUID_SIZE_16        16                  //!< 16 bytes UUID, usually assigned by users. 

/** @defgroup GATT_PROP_BITMAPS_DEFINES GATT Attribute Access Permissions Bit Fields
 * @{
 */
#define GATT_PROP_READ                  0x0001  //!< Attribute is Readable
#define GATT_PROP_WRITE                 0x0002  //!< Attribute is Writable
#define GATT_PROP_AUTHEN_READ           0x0004  //!< Read requires Authentication
#define GATT_PROP_AUTHEN_WRITE          0x0008  //!< Write requires Authentication
#define GATT_PROP_NOTI                  0x0100  //!< Attribute is able to send notification
#define GATT_PROP_INDI                  0x0200  //!< Attribute is able to send indication
#define GATT_PROP_WRITE_REQ             0x0400  //!< Attribute supports write request
#define GATT_PROP_WRITE_CMD             0x0800  //!< Attribute supports write with no response

/** @} End GATT_PERMIT_BITMAPS_DEFINES */

/** @defgroup GATT_OPERATION_NAME GATT operation name define, used with GATTS/C_MSG_CMP_EVT
 * @{
 */
#define GATT_OP_NOTIFY                  0x01    //!< GATT notification operation
#define GATT_OP_INDICA                  0x02    //!< GATT indication operation
#define GATT_OP_PEER_SVC_REGISTERED     0x03    //!< Used with GATTC_CMP_EVT, GATT peer device service registered
#define GATT_OP_WRITE_REQ               0x05    //!< GATT wirte request operation
#define GATT_OP_WRITE_CMD               0x06    //!< GATT wirte command operation, write without response
#define GATT_OP_READ                    0x07    //!< GATT read operation
/** @} End GATT_OPERATION_NAME */


#define GATT_SVC_ID_FAIL                0xff
/*
 * TYPEDEFS (类型定义)
 */

/**
* GATT message events type define 
*/
typedef enum
{
    GATTC_MSG_READ_REQ,         //!< GATT client read request
    GATTC_MSG_WRITE_REQ,        //!< GATT client write request
    GATTC_MSG_ATT_INFO_REQ,     //!< GATT client att information request
    GATTC_MSG_NTF_REQ,          //!< GATT client notification received
    GATTC_MSG_READ_IND,         //!< GATT client read response received
    GATTC_MSG_CMP_EVT,          //!< GATT client message complete event
    GATTC_MSG_LINK_CREATE,
    GATTC_MSG_LINK_LOST,
} gatt_msg_evt_t;

/**
* GATT UUID format.
*/
typedef struct
{
    uint8_t size;               //!< Length of UUID (2 or 16 bytes UUIDs: UUID_SIZE_2 or UUID_SIZE_16). 
    uint8_t p_uuid[16];         //!< Pointer to uuid, could be 2 or 16 bytes array. 
} gatt_uuid_t;

/**
* BLE attribute define format.
*/
typedef struct
{
    gatt_uuid_t     uuid;       //!< Attribute UUID
    uint16_t        prop;       //!< Attribute properties, see @GATT_PROP_BITMAPS_DEFINES
    uint16_t        max_size;   //!< Attribute data maximum size
    uint8_t         *p_data;    //!< Attribute data pointer
} gatt_attribute_t;

/**
* GATT message event operation done structer
*/
typedef struct
{
    uint8_t operation;          //!< GATT request type
    uint8_t status;             //!< Status of the request
    void * arg;                 //!< Parameter pointer
} gatt_op_cmp_t;

/**
* GATT message event data structer.
*/
typedef struct
{
    uint16_t    msg_len;        //!< GATT message length
    void        *p_msg_data;    //!< GATT message data pointer
} gatt_msg_hdl_t;

/**
* GATT message event structure.
*/
typedef struct
{
    gatt_msg_evt_t  msg_evt;    //!< The event that message come with
    uint8_t         conn_idx;   //!< Connection index
    uint8_t         svc_id;     //!< service id of this message
    uint16_t        att_idx;    //!< Attribute index of in the service table
    union
    {
        gatt_msg_hdl_t  msg;    //!< GATT message, length, data pointer
        gatt_op_cmp_t   op;     //!< GATT operation, read, write, notification, indication
    } param;
} gatt_msg_t;



/*********************************************************************
 * @fn      gatt_msg_handler
 *
 * @brief   User application handles read request in this callback.
 *
 * @param   p_read  - the pointer to read buffer. NOTE: It's just a pointer from lower layer, please create the buffer in application layer.
 *          len     - the pointer to the length of read buffer. Application to assign it.
 *          att_idx - offset of the attribute value.
 *
 * @return  Data length.
 */
typedef uint16_t (*gatt_msg_handler_t)(gatt_msg_t *p_msg);

/**
* Profile service define format.
*/
typedef struct
{
    const gatt_attribute_t  *p_att_tb;          //!< Service's attributes table to add to system attribute database. 
    uint8_t                 att_nb;             //!< Service's attributes number. 
    gatt_msg_handler_t      gatt_msg_handler;   //!< Read request callback function. 
} gatt_service_t;

/**
* Profile client define format.
*/
typedef struct
{
    const gatt_uuid_t       *p_att_tb;          //!< Service UUID
    uint8_t                 att_nb;             //!< Service's attributes number. 
    gatt_msg_handler_t      gatt_msg_handler;   //!< Read request callback function. 
} gatt_client_t;



/**
* BLE client write format.
*/
typedef struct
{
    uint8_t     conidx;         //!< Connection index
    uint8_t     client_id;      //!< Service ID among all services in current system. 
    uint8_t     att_idx;        //!< Attribute id number in its service attribute table. 
    uint8_t     *p_data;        //!< Data pointer to be written
    uint16_t     data_len;       //!< Data length to be written
} gatt_client_write_t;
/**
* BLE client read format.
*/
typedef struct
{
    uint8_t     conidx;         //!< Connection index
    uint8_t     client_id;      //!< Service ID among all services in current system. 
    uint8_t     att_idx;        //!< Attribute id number in its service attribute table. 
} gatt_client_read_t;

/**
* BLE client enable ntf format.
*/
typedef struct
{
    uint8_t     conidx;         //!< Connection index
    uint8_t     client_id;      //!< Service ID among all services in current system. 
    uint8_t     att_idx;        //!< Attribute id number in its service attribute table. 
} gatt_client_enable_ntf_t;


/**
* BLE notification format.
*/
typedef struct
{
    uint8_t     conidx;         //!< Connection index
    uint8_t     svc_id;         //!< Service ID among all services in current system. 
    uint8_t     att_idx;        //!< Attribute id number in its service attribute table. 
    uint8_t     *p_data;        //!< Data to be sent as notification.
    uint16_t     data_len;       //!< Data length
} gatt_ntf_t;

/**
* BLE indication format.
*/
typedef struct
{
    uint8_t     conidx;         //!< Connection index
    uint8_t     svc_id;         //!< Service ID among all services in current system. 
    uint8_t     att_idx;        //!< Attribute id number in its service attribute table. 
    uint8_t     *p_data;        //!< Data to be sent as indication.
    uint16_t     data_len;       //!< Data length
} gatt_ind_t;


/*
 * GLOBAL VARIABLES (全局变量)
 */

/*
 * LOCAL VARIABLES (本地变量)
 */

/*
 * LOCAL FUNCTIONS (本地函数)
 */

/*
 * EXTERN FUNCTIONS (外部函数)
 */

/*
 * PUBLIC FUNCTIONS (全局函数)
 */

/** @function group ble peripheral device APIs (ble外设相关的API)
 * @{
 */

/*********************************************************************
 * @fn      gatt_add_service
 *
 * @brief   Addding a services & characteristics into gatt database.
 *
 * @param   p_service - service data to be added.
 *
 * @return  Assigned svc_id.
 */
uint8_t gatt_add_service(gatt_service_t *p_service);

/*********************************************************************
 * @fn      gatt_add_client
 *
 * @brief   Addding a GATT client in the system.
 *
 * @param   p_client - client information.
 *
 * @return  Assigned client_id.
 */
uint8_t gatt_add_client(gatt_client_t *p_client);

/*********************************************************************
 * @fn      gatt_change_svc_uuid
 *
 * @brief   Change service UUID after service is added.
 *
 * @param   svc_id      - profile svc_id.
 *          att_idx     - idx in profile service attribute table
 *          new_uuid    - new uuid to be set
 *          uuid_len    - new uuid len.
 *
 * @return  None.
 */
void gatt_change_svc_uuid(uint8_t svc_id,uint8_t att_idx,uint8_t *new_uuid,uint8_t uuid_len);

/*********************************************************************
 * @fn      gatt_change_client_uuid
 *
 * @brief   Change client UUID.
 *
 * @param   client_id   - client information.
 *          att_idx     -
 *          new_uuid    -
 *          uuid_len    -
 *
 * @return  None.
 */
void gatt_change_client_uuid(uint8_t client_id,uint8_t att_idx,uint8_t *new_uuid,uint8_t uuid_len);

/*********************************************************************
 * @fn      gatt_discovery_all_peer_svc
 *
 * @brief   Discover peer device service.
 *
 * @param   client_id  - client information.
 *          conidx     -
 *
 * @return  None.
 */
void gatt_discovery_all_peer_svc(uint8_t client_id,uint8_t conidx);

/*********************************************************************
 * @fn      gatt_discovery_peer_svc
 *
 * @brief   Discover peer device service.
 *
 * @param   client_id   - client information.
 *          conidx      -
 *          uuid_len    -
 *          group_uuid  -
 *
 * @return  None.
 */
void gatt_discovery_peer_svc(uint8_t client_id, uint8_t conidx, uint8_t uuid_len, uint8_t *group_uuid);

/*********************************************************************
 * @fn      gatt_client_write_req
 *
 * @brief   Write request, with response.
 *
 * @param   write_att   - write operation parameter.
 *
 * @return  None.
 */
void gatt_client_write_req(gatt_client_write_t write_att);

/*********************************************************************
 * @fn      gatt_client_write_cmd
 *
 * @brief   Write request, without response.
 *
 * @param   gatt_client_write_t   - client information.
 *
 * @return  None.
 */
void gatt_client_write_cmd(gatt_client_write_t write_att);

/*********************************************************************
 * @fn      gatt_client_enable_ntf
 *
 * @brief   Enable notification.
 *
 * @param   gatt_client_enable_ntf_t   - client information.
 *
 * @return  None.
 */
void gatt_client_enable_ntf(gatt_client_enable_ntf_t ntf_enable_att);

/*********************************************************************
 * @fn      gatt_client_read
 *
 * @brief   Read request.
 *
 * @param   gatt_client_read_t   - client information.
 *
 * @return  None.
 */
void gatt_client_read(gatt_client_read_t read_att);

/*********************************************************************
 * @fn      gatt_notification
 *
 * @brief   Sending notification.
 *          发送notification.
 *
 * @param   ntf_att - in which service and which attribute the notification will be sent
 *                    This parameter contains servcie ID and attribute ID
 *                    to indicate exact which attribute will be used to send notification.
 *          p_data - notification data to be sent.
 *          data_len - length of notification data.
 *
 * @return  None.
 */
void gatt_notification(gatt_ntf_t ntf_att);

/*********************************************************************
 * @fn      gatt_indication
 *
 * @brief   Sending indication.
 *
 * @param   ind_att - in which service and which attribute the indication will be sent
 *                    This parameter contains servcie ID and attribute ID
 *                    to indicate exact which attribute will be used to send indication.
 *          p_data - indication data to be sent.
 *          data_len - length of indication data.
 *
 * @return  None.
 */
void gatt_indication(gatt_ind_t ind_att);

/*********************************************************************
 * @fn      gatt_mtu_exchange_req
 *
 * @brief   Change maximum MTU.
 *
 * @param   conidx - Connection index.
 *
 * @return  None.
 */
void gatt_mtu_exchange_req(uint8_t conidx);
/****************************************************************************************
 * @brief Gets the negotiated MTU. This function gets the negotiated MTU.
 *
 * @param[in] conidx   Link idx.
 *
 * @return MTU negotiated
 */
uint16_t gatt_get_mtu(uint8_t conidx);


#endif // end of #ifndef GATT_API_H
