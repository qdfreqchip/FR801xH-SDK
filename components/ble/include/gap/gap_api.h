/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef GAP_API_H
#define GAP_API_H

/*
 * INCLUDES 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * MACROS 
 */

/*
 * CONSTANTS 
 */
/** @defgroup GAP_ADV_MODE_DEFINES
 * @{
 */
#define GAP_ADV_MODE_UNDIRECT   		0x01
#define GAP_ADV_MODE_DIRECT     		0x02
#define GAP_ADV_MODE_NON_CONN_NON_SCAN          0x03
#define GAP_ADV_MODE_NON_CONN_SCAN  	        0x04

/** @defgroup GAP_ADDR_TYPE_DEFINES GAP address type define
 * @{
 */
#define GAP_ADDR_TYPE_PUBLIC                        0x00
#define GAP_ADDR_TYPE_PRIVATE                       0x01
#define GAP_ADDR_TYPE_RANDOM_RESOVABLE              0x02
#define GAP_ADDR_TYPE_RANDOM_NONE_RESOVABLE         0x03

/** @defgroup GAP_ADVCHAN_DEFINES GAP Advertisement Channel Map
 * @{
 */
#define GAP_ADV_CHAN_37  0x01                                                   //!< Advertisement Channel 37
#define GAP_ADV_CHAN_38  0x02                                                   //!< Advertisement Channel 38
#define GAP_ADV_CHAN_39  0x04                                                   //!< Advertisement Channel 39
#define GAP_ADV_CHAN_ALL (GAP_ADV_CHAN_37 | GAP_ADV_CHAN_38 | GAP_ADV_CHAN_39)  //!< All Advertisement Channels Enabled

/** @defgroup GAP_ADV_FILTER_MODE_DEFINES
 * @{
 */
#define GAP_ADV_ALLOW_SCAN_ANY_CON_ANY        0x00
#define GAP_ADV_ALLOW_SCAN_WLST_CON_ANY       0x01
#define GAP_ADV_ALLOW_SCAN_ANY_CON_WLST       0x02
#define GAP_ADV_ALLOW_SCAN_WLST_CON_WLST      0x03

/** @defgroup GAP_ADVTYPE_DEFINES GAP Advertisement Data Types
 * These are the data type identifiers for the data tokens in the advertisement data field.
 * @{
 */
#define GAP_ADTVYPE_FLAGS                       0x01 //!< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES   
#define GAP_ADVTYPE_16BIT_MORE                  0x02 //!< Service: More 16-bit UUIDs available
#define GAP_ADVTYPE_16BIT_COMPLETE              0x03 //!< Service: Complete list of 16-bit UUIDs
#define GAP_ADVTYPE_32BIT_MORE                  0x04 //!< Service: More 32-bit UUIDs available
#define GAP_ADVTYPE_32BIT_COMPLETE              0x05 //!< Service: Complete list of 32-bit UUIDs
#define GAP_ADVTYPE_128BIT_MORE                 0x06 //!< Service: More 128-bit UUIDs available
#define GAP_ADVTYPE_128BIT_COMPLETE             0x07 //!< Service: Complete list of 128-bit UUIDs
#define GAP_ADVTYPE_LOCAL_NAME_SHORT            0x08 //!< Shortened local name
#define GAP_ADVTYPE_LOCAL_NAME_COMPLETE         0x09 //!< Complete local name
#define GAP_ADVTYPE_POWER_LEVEL                 0x0A //!< TX Power Level: 0xXX: -127 to +127 dBm
#define GAP_ADVTYPE_OOB_CLASS_OF_DEVICE         0x0D //!< Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADVTYPE_OOB_SIMPLE_PAIRING_HASHC    0x0E //!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADVTYPE_OOB_SIMPLE_PAIRING_RANDR    0x0F //!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADVTYPE_SM_TK                       0x10 //!< Security Manager TK Value
#define GAP_ADVTYPE_SM_OOB_FLAG                 0x11 //!< Secutiry Manager OOB Flags
#define GAP_ADVTYPE_SLAVE_CONN_INTERVAL_RANGE   0x12 //!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
#define GAP_ADVTYPE_SIGNED_DATA                 0x13 //!< Signed Data field
#define GAP_ADVTYPE_SERVICES_LIST_16BIT         0x14 //!< Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADVTYPE_SERVICES_LIST_128BIT        0x15 //!< Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADVTYPE_SERVICE_DATA                0x16 //!< Service Data - 16-bit UUID
#define GAP_ADVTYPE_PUBLIC_TARGET_ADDR          0x17 //!< Public Target Address
#define GAP_ADVTYPE_RANDOM_TARGET_ADDR          0x18 //!< Random Target Address
#define GAP_ADVTYPE_APPEARANCE                  0x19 //!< Appearance
#define GAP_ADVTYPE_ADV_INTERVAL                0x1A //!< Advertising Interval
#define GAP_ADVTYPE_LE_BD_ADDR                  0x1B //!< LE Bluetooth Device Address
#define GAP_ADVTYPE_LE_ROLE                     0x1C //!< LE Role
#define GAP_ADVTYPE_SIMPLE_PAIRING_HASHC_256    0x1D //!< Simple Pairing Hash C-256
#define GAP_ADVTYPE_SIMPLE_PAIRING_RANDR_256    0x1E //!< Simple Pairing Randomizer R-256
#define GAP_ADVTYPE_SERVICE_DATA_32BIT          0x20 //!< Service Data - 32-bit UUID
#define GAP_ADVTYPE_SERVICE_DATA_128BIT         0x21 //!< Service Data - 128-bit UUID
#define GAP_ADVTYPE_3D_INFO_DATA                0x3D //!< 3D Information Data
#define GAP_ADVTYPE_MANUFACTURER_SPECIFIC       0xFF //!< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data
/** @} End GAP_ADVTYPE_DEFINES */

/** @defgroup GAP_SCAN_MODE_DEFINES GAP Scan Modes
 * @{
 */
#define GAP_SCAN_MODE_GEN_DISC          0x00  //!< General discovery
#define GAP_SCAN_MODE_OBSERVER          0x02  //!< Observer
/** @} End GAP_SCAN_MODE_DEFINES */


/** @defgroup GAP_SCAN_EVT_TYPE_DEFINES GAP Event Types
 * @{
 */
#define GAP_SCAN_EVT_CONN_UNDIR        0x00  //!< Undirected advertising
#define GAP_SCAN_EVT_CONN_DIR          0x01  //!< Directed advertising
#define GAP_SCAN_EVT_NONCONN_UNDIR     0x02  //!< Non Connectable advertising
#define GAP_SCAN_EVT_SCAN_RSP          0x04  //!< Scan Response


/** @defgroup GAP_PAIRING_MODE_DEFINES GAP Bond Manager Pairing Modes
 * @{
 */
#define GAP_PAIRING_MODE_NO_PAIRING          0x00  //!< Pairing is not allowed
#define GAP_PAIRING_MODE_WAIT_FOR_REQ        0x01  //!< Wait for a pairing request or slave security request
#define GAP_PAIRING_MODE_INITIATE            0x02  //!< Don't wait, initiate a pairing request or slave security request
/** @} End GAP_PAIRING_MODE_DEFINES */

/** @defgroup GAP_IO_CAP_DEFINES GAP Bond Manager I/O Capabilities
 * @{
 */
#define GAP_IO_CAP_DISPLAY_ONLY              0x00  //!< Display Only Device
#define GAP_IO_CAP_DISPLAY_YES_NO            0x01  //!< Display and Yes and No Capable
#define GAP_IO_CAP_KEYBOARD_ONLY             0x02  //!< Keyboard Only
#define GAP_IO_CAP_NO_INPUT_NO_OUTPUT        0x03  //!< No Display or Input Device
#define GAP_IO_CAP_KEYBOARD_DISPLAY          0x04  //!< Both Keyboard and Display Capable
/** @} End GAP_IO_CAP_DEFINES */

/** @defgroup GAP_APPEARANCE_VALUES GAP Appearance Values
 * @{
 */
#define GAP_APPEARE_UNKNOWN                     0x0000 //!< Unknown
#define GAP_APPEARE_GENERIC_PHONE               0x0040 //!< Generic Phone
#define GAP_APPEARE_GENERIC_COMPUTER            0x0080 //!< Generic Computer
#define GAP_APPEARE_GENERIC_WATCH               0x00C0 //!< Generic Watch
#define GAP_APPEARE_WATCH_SPORTS                0x00C1 //!< Watch: Sports Watch
#define GAP_APPEARE_GENERIC_CLOCK               0x0100 //!< Generic Clock
#define GAP_APPEARE_GENERIC_DISPLAY             0x0140 //!< Generic Display
#define GAP_APPEARE_GENERIC_RC                  0x0180 //!< Generic Remote Control
#define GAP_APPEARE_GENERIC_EYE_GALSSES         0x01C0 //!< Generic Eye-glasses
#define GAP_APPEARE_GENERIC_TAG                 0x0200 //!< Generic Tag
#define GAP_APPEARE_GENERIC_KEYRING             0x0240 //!< Generic Keyring
#define GAP_APPEARE_GENERIC_MEDIA_PLAYER        0x0280 //!< Generic Media Player
#define GAP_APPEARE_GENERIC_BARCODE_SCANNER     0x02C0 //!< Generic Barcode Scanner
#define GAP_APPEARE_GENERIC_THERMOMETER         0x0300 //!< Generic Thermometer
#define GAP_APPEARE_GENERIC_THERMO_EAR          0x0301 //!< Thermometer: Ear
#define GAP_APPEARE_GENERIC_HR_SENSOR           0x0340 //!< Generic Heart rate Sensor
#define GAP_APPEARE_GENERIC_HRS_BELT            0x0341 //!< Heart Rate Sensor: Heart Rate Belt
#define GAP_APPEARE_GENERIC_BLOOD_PRESSURE      0x0380 //!< Generic Blood Pressure
#define GAP_APPEARE_GENERIC_BP_ARM              0x0381 //!< Blood Pressure: Arm
#define GAP_APPEARE_GENERIC_BP_WRIST            0x0382 //!< Blood Pressure: Wrist
#define GAP_APPEARE_GENERIC_HID                 0x03C0 //!< Generic Human Interface Device (HID)
#define GAP_APPEARE_HID_KEYBOARD                0x03C1 //!< HID Keyboard
#define GAP_APPEARE_HID_MOUSE                   0x03C2 //!< HID Mouse
#define GAP_APPEARE_HID_JOYSTIC                 0x03C3 //!< HID Joystick
#define GAP_APPEARE_HID_GAMEPAD                 0x03C4 //!< HID Gamepad
#define GAP_APPEARE_HID_DIGITIZER_TYABLET       0x03C5 //!< HID Digitizer Tablet
#define GAP_APPEARE_HID_DIGITAL_CARDREADER      0x03C6 //!< HID Card Reader
#define GAP_APPEARE_HID_DIGITAL_PEN             0x03C7 //!< HID Digital Pen
#define GAP_APPEARE_HID_BARCODE_SCANNER         0x03C8 //!< HID Barcode Scanner
/** @} End GAP_APPEARANCE_VALUES */

/*
 * TYPEDEFS 
 */

/** @defgroup GAP_EVT_TYPE_DEFINES for application layer callbacks
 * @{
 */
typedef enum
{
    GAP_EVT_ALL_SVC_ADDED,          //!< All GATT servcie added

    GAP_EVT_SLAVE_CONNECT,          //!< Connected as slave role
    GAP_EVT_MASTER_CONNECT,         //!< Connected as master role
    GAP_EVT_DISCONNECT,             //!< Disconnected
    GAP_EVT_LINK_PARAM_REJECT,      //!< Parameter update rejected
    GAP_EVT_LINK_PARAM_UPDATE,      //!< Parameter update successful
    GAP_EVT_ADV_END,                //!< Advertising ended
    GAP_EVT_SCAN_END,               //!< Scanning ended
    GAP_EVT_ADV_REPORT,             //!< Find a BLE device
    GAP_EVT_CONN_END,               //!< Connecion procedure canceled
    GAP_EVT_PEER_FEATURE,           //!< Got peer device supported features
    GAP_EVT_MTU,                    //!< MTU exchange event
    GAP_EVT_LINK_RSSI,              //!< Got peer device RSSI value

    GAP_SEC_EVT_MASTER_AUTH_REQ,    //!< Authentication request
    GAP_SEC_EVT_MASTER_ENCRYPT,     //!< Encryted as master role
    GAP_SEC_EVT_SLAVE_ENCRYPT,      //!< Enrypted as slave role
} gap_event_type_t;

#define MAC_ADDR_LEN         6
/// Maximal length of the Device Name value
#define LOCAL_NAME_MAX_LEN      (18)

// BD ADDR 
typedef struct 
{
    uint8_t  addr[MAC_ADDR_LEN];           //!< 6-byte array address value
}mac_addr_t;

// GAP BD ADDR strucrue, includes address type
typedef struct
{
    mac_addr_t 	addr;            //!< BD Address of device
    uint8_t 	addr_type;       //!< Address type of the device 0=public/1=private random
}gap_mac_addr_t;

// Connected peer devcie link parameters
typedef struct 
{
    uint8_t     conidx;         //!< Connection index
    mac_addr_t  peer_addr;      //!< BDADDR of peer device
    uint8_t     addr_type;      //!< Peer device address type
    uint16_t    con_interval;   //!< Connection interval
    uint16_t    con_latency;    //!< Slave latency
    uint16_t    sup_to;         //!< Supervision timeout
}conn_peer_param_t;

// Link disconnected event & reason
typedef struct 
{
    uint8_t conidx;             //!< Connection index
    uint8_t reason;             //!< Reason of disconnection
}gap_evt_disconnect_t;

// Link parameter update reject event
typedef struct 
{
    uint8_t conidx;             //!< Connection index
    uint8_t status;             //!< Parameter reject status
}gap_evt_link_param_reject_t;

// Link parameter update success event
typedef struct
{
    uint8_t     conidx;         //!< Connection index
    uint16_t    con_interval;   //!< Connection interval
    uint16_t    con_latency;    //!< Connection latency value
    uint16_t    sup_to;         //!< Supervision timeout
}gap_evt_link_param_update_t;

// Scan result, find remote advertising devide
typedef struct 
{
    uint8_t 		evt_type;		//!< Bit field providing information about the received report (@see GAP_SCAN_EVT_TYPE_DEFINES)
    gap_mac_addr_t 	src_addr;		//!< Target address (in case of a directed advertising report)
    int8_t              tx_pwr;			//!< TX power (in dBm)
    int8_t              rssi;			//!< RSSI (between -127 and +20 dBm)
    uint16_t 		length;			//!< Report length
    uint8_t 		*data;			//!< Report data
}gap_evt_adv_report_t;

// Connected peer device supported features
typedef struct 
{
    uint8_t conidx;                 //!< Connection index
    uint8_t features[8];            //!< Features bitmask
}gap_evt_peer_feature_t;

// Connected peer device MTU size
typedef struct
{
    uint8_t 	conidx;		//!< Connection index
    uint16_t 	value;		//!< MTU size
}gattc_mtu_t;

// Authentication request from master
typedef struct 
{
    uint8_t conidx;                 //!< Connection index
    uint8_t auth;                   //!< Authentication level (@see gap_auth)
}gap_sec_evt_master_auth_req_t;

// GAT event structure
typedef struct
{
    gap_event_type_t                    type;                   //!< GAP event type
    union
    {
        conn_peer_param_t               slave_connect;          //!< Peer slave device connection parameters
        conn_peer_param_t               master_connect;         //!< Peer master device connection parameters
        gap_evt_disconnect_t            disconnect;             //!< Disconnect event
        gap_evt_link_param_reject_t     link_reject;            //!< Parameter reject event
        gap_evt_link_param_update_t     link_update;            //!< Parameter update success event
        uint8_t                         adv_end_status;         //!< Advertising end status
        uint8_t                         scan_end_status;        //!< Scanning end status
        gap_evt_adv_report_t            *adv_rpt;               //!< Scanning results
        uint8_t                         conn_end_reason;        //!< Connection end reason
        gap_evt_peer_feature_t          peer_feature;           //!< Peer device supported features
        gattc_mtu_t                     mtu;                    //!< MTU size
        int8_t                          link_rssi;              //!< Peer device RSSI value

        gap_sec_evt_master_auth_req_t   auth_req;               //!< Master authentication request
        uint8_t                         master_encrypt_conidx;  //!< Connection index of encrypted link, role as master
        uint8_t                         slave_encrypt_conidx;   //!< Connection index of encrypted link, role as slave
    } param;
} gap_event_t;

// Gap callback function define
typedef void(* gap_callback_func_t)(gap_event_t * event);


// Gap advertising parameters
typedef struct
{
    uint8_t     	adv_mode;               //!< Advertising mode, connectable/none-connectable, see @ GAP_ADV_MODE_DEFINES
    uint8_t     	adv_addr_type;          //!< see @ GAP_ADDR_TYPE_DEFINES
    gap_mac_addr_t 	peer_mac_addr;          //!< peer mac addr,used for direction adv
    uint8_t     	phy_mode;               //!< reseverd
    uint16_t    	adv_intv_min;           //!< Minimum advertising interval, (in unit of 625us). Must be greater than 20ms
    uint16_t    	adv_intv_max;           //!< Maximum advertising interval, (in unit of 625us). Must be greater than 20ms
    uint8_t     	adv_chnl_map;           //!< Advertising channal map, 37, 38, 39, see @ GAP_ADVCHAN_DEFINES
    uint8_t     	adv_filt_policy;        //!< Advertising filter policy, see @ GAP_ADV_FILTER_MODE_DEFINES
} gap_adv_param_t;

// Gap scan parameters
typedef struct
{
    uint8_t     scan_mode;           //!< scan mode, see @ GAP_SCAN_MODE_DEFINES
    uint8_t     dup_filt_pol;        //!< scan duplicated pkt filter enbale, 0, donot filter; 1 filter duplicated pkt
    uint16_t    scan_intv;           //!< scan interval, (in unit of 625us). range [4,16384]
    uint16_t    scan_window;         //!< scan window, (in unit of 625us). must <= scan_intv, range [4,16384]
    uint16_t    duration;            //!< Scan duration (in unit of 10ms). 0 means that the controller will scan continuously until host stop it
} gap_scan_param_t;

// Gap security parameters
typedef struct
{
    bool     mitm;              //!< Man In The Middle mode enalbe/disable
    bool     ble_secure_conn;   //!< BLE Secure Simple Pairing, also called Secure Connection mode.
    uint8_t  io_cap;            //!< IO capbilities, see @ GAP_IO_CAP_DEFINES
    uint8_t  pair_init_mode;    //!< If initialize pairing procesure or not, see @ GAP_PAIRING_MODE_DEFINES
    bool     bond_auth;         //!< Bond_auth enable/disable,if true, then will distribute encryption key,and will check this key_req when bonding. 
    uint32_t password;          //!< Password.
} gap_security_param_t;


/*
 * GLOBAL VARIABLES 
 */

/*
 * LOCAL VARIABLES 
 */

/*
 * LOCAL FUNCTIONS 
 */

/*
 * EXTERN FUNCTIONS 
 */

/*
 * PUBLIC FUNCTIONS 
 */


/*********************************************************************
 * @fn      gap_set_cb_func
 *
 * @brief   GAP callback functions set. Links application layer callbacks to GAP layer.
 *
 * @param   gap_evt     - GAP events trigger the callbacks. See @ enum gap_event.
 *
 *          gap_evt_cb  - Application layer callback function, defined in application layer.
 *
 * @return  None.
 */
void gap_set_cb_func(gap_callback_func_t gap_evt_cb);


/*********************************************************************
 * @fn      gap_set_advertising_param
 *
 * @brief   Set ble advertising parameters.
 *
 * @param   p_adv_param    - pointer to adv configuration parameters.
 *
 * @return  None.
 */
void gap_set_advertising_param(gap_adv_param_t *p_adv_param);

/*********************************************************************
 * @fn      gap_set_advertising_data
 *
 * @brief   Set ble advertising data.
 *
 * @param   p_adv_data    - pointer to adv data buffer.
 *          adv_data_len  - adv data len.
 *
 * @return  None.
 */
void gap_set_advertising_data(uint8_t *p_adv_data, uint8_t adv_data_len);
/*********************************************************************
 * @fn      gap_set_advertising_param
 *
 * @brief   Set ble advertising parameters.
 *
 * @param   p_rsp_data    - pointer to scan response data buffer.
 *          rsp_data_len  - scan response data len.
 *
 * @return  None.
 */
void gap_set_advertising_rsp_data(uint8_t *p_rsp_data, uint8_t rsp_data_len);

/*********************************************************************
 * @fn      gap_start_advertising
 *
 * @brief   Start advertising.
 *
 * @param   duration    - Advertising duration (in unit of 10ms). 0 means that advertising continues
 *
 * @return  None.
 */
void gap_start_advertising(uint16_t duration);

/*********************************************************************
* @fn      gap_stop_advertising
*
* @brief   Stop advertising.
*
* @param   None.
*
* @return  None.
*/
void gap_stop_advertising(void);

/*********************************************************************
* @fn      gap_start_scan
*
* @brief   Start scanning devices.
*
* @param   p_scan_param - scan paramters.
*
* @return  None.
*/
void gap_start_scan(gap_scan_param_t *p_scan_param);

/*********************************************************************
* @fn      gap_stop_scan
*
* @brief   Stop scanning devices.
*
* @param   None.
*
* @return  None.
*/
void gap_stop_scan(void);

/*********************************************************************
* @fn       gap_start_conn
*
* @brief    Start connecting.
*
* @param    addr        - peer device mac addr.
*           addr_type   - peer device mac addr type.
*           min_itvl    - minimum connection inteval. uint: 1.25ms
*           max_itvl    - maximum connection inteval. uint: 1.25ms
*           slv_latency - number of slave latency.
*           timeout     - supervision timeout of the link. uint: 10ms
*
* @return   None.
*/
void gap_start_conn(mac_addr_t *addr, uint8_t addr_type, uint16_t min_itvl, uint16_t max_itvl, uint16_t slv_latency, uint16_t timeout);

/*********************************************************************
* @fn      gap_stop_conn
*
* @brief   Stop connecting procedure, cancel it.
*
* @param   None.
*
* @return  None.
*/
void gap_stop_conn(void);

/*********************************************************************
* @fn      gap_disconnect_req
*
* @brief   Disconnect a peer device.
*
* @param   conidx   - connection index of the connection to be disconnected.
*
* @return  None.
*/
void gap_disconnect_req(uint8_t conidx);

/*********************************************************************
 * @fn      gap_address_get
 *
 * @brief   Fetch BLE mac address of the chip.
 *
 * @param   addr    - addr pointer.
 *
 * @return  None.
 */
void gap_address_get(mac_addr_t *addr);

/*********************************************************************
 * @fn      gap_address_set
 *
 * @brief   Configure BLE mac address.
 *
 * @param   mac     - 6 bytes MAC address
 *
 * @return  None.
 */
void gap_address_set(mac_addr_t *addr);

/*********************************************************************
 * @fn      gap_get_connect_status
 *
 * @brief   Get the connection status with a peer device.
 *
 * @param   conidx  - connection index with the peer device.
 *
 * @return  the connection status.
 */
bool gap_get_connect_status(uint8_t conidx);

/*********************************************************************
 * @fn      gap_get_encryption_status
 *
 * @brief   Get the connection encryption status with a peer device.
 *
 * @param   conidx  - connection index with the peer device.
 *
 * @return  the connection encryption status.
 */
bool gap_get_encryption_status(uint8_t conidx);

/*********************************************************************
 * @fn      gap_set_dev_name
 *
 * @brief   Set the local device name.
 *
 * @param   p_name  - name to be set.
 *          len     - length of the name string.
 *
 * @return  none.
 */
void gap_set_dev_name(uint8_t *p_name,uint8_t len);

/*********************************************************************
 * @fn      gap_get_dev_name
 *
 * @brief   Get the local device name.
 *
 * @param   p_name  - buff buff to load local name.
 *
 * @return  length of the local name .
 */
uint8_t gap_get_dev_name(uint8_t* p_name);

/*********************************************************************
 * @fn      gap_set_dev_appearance
 *
 * @brief   Set the local device apperance.
 *
 * @param   appearance  - device apperance, see @ GAP_APPEARANCE_VALUES
 *
 * @return  none.
 */
void gap_set_dev_appearance(uint16_t appearance);
/*********************************************************************
 * @fn      gap_get_dev_appearance
 *
 * @brief   Get the local device apperance.
 *
 * @param   None
 *
 * @return  device apperance, see @ GAP_APPEARANCE_VALUES
 */
uint16_t gap_get_dev_appearance(void);

/*********************************************************************
 * @fn      gap_get_connect_num
 *
 * @brief   Get the current active connections number.
 *
 * @param   none.
 *
 * @return  number of active connections.
 */
uint8_t gap_get_connect_num(void);

/*********************************************************************
 * @fn      gap_set_link_rssi_report
 *
 * @brief   Used to enable or disable lower layer report Link RSSI in real time.
 *          User layer should reimplement a function to receive rssi value.
 *          This function will be called once a new rssi is generated after
 *          report is enabled. This function sample is:
 *          __attribute__((section("ram_code"))) void gap_rssi_ind(int8_t rssi, uint8_t conidx)
 *          {
 *              co_printf("rssi: link=%d, rssi=%d.\r\n", conidx, rssi);
 *          }
 *
 * @param   enable  - connection index.
 *
 * @return  None.
 */
void gap_set_link_rssi_report(bool enable);

/**********************************************************************
 * @fn      gap_conn_param_update
 *
 * @brief   Send connection parameters update request from ble peripheral device.
 *
 * @param   conidx                  - connection index of the connection.
 *
 *          min_intv                - minimum connection interval. unit: 1.25ms
 *
 *          max_intv                - maximum connection interval, in normal case can be set same as min_intv. unit: 1.25ms
 *
 *          slave_latency           - slave latency, number of connection events that slave would like to skip.
 *
 *          supervision_timeout     - timeout before connection is dropped. unit: 10ms
 *
 * @return  None.
 */
void gap_conn_param_update(uint8_t conidx, uint16_t min_intv, uint16_t max_intv, uint16_t slave_latency, uint16_t supervision_timeout);

/*********************************************************************
 * @fn      gap_bond_manager_init
 *
 * @brief   Initialize bonding manager. For bonding features when security is needed.
 *
 * @param   flash_addr      - Flash page addr where peer device bond information is stored, 
 *                            should be integer multiple of 0x1000
 *          svc_flash_addr  - Flash page addr where peer device services information is stored, 
 *                            should be integer multiple of 0x1000
 *          max_dev_num     - Max supported number of peer devices
 *          enable          - Enable bit of bond manager fucntion. True -Enalbe; False-Disable
 *
 * @return  None.
 */
void gap_bond_manager_init(uint32_t flash_addr,uint32_t svc_flash_addr,uint8_t max_dev_num,bool enable);

/*********************************************************************
 * @fn      gap_bond_manager_delete_all
 *
 * @brief   Erase all bond information.
 *
 * @param   None.
 *
 * @return  None.
 */
void gap_bond_manager_delete_all(void);

/*********************************************************************
 * @fn      gap_bond_manager_delete
 *
 * @brief   Erase a bond information.
 *
 * @param   mac_addr    - bond device BD_ADDR to be deleted.
 *          addr_type   - address type of the devcie to be deleted.
 *
 * @return  None.
 */
void gap_bond_manager_delete(uint8_t *mac_addr, uint8_t addr_type);

/*********************************************************************
 * @fn      gap_security_param_init
 *
 * @brief   Initialize security paramters, MITM, IO Capbilities, bond.
 *
 * @param   sec_param    - Security parameters, see @ gap_security_param_t.
 *
 * @return  None.
 */
void gap_security_param_init(gap_security_param_t *sec_param);

/*********************************************************************
 * @fn      gap_security_send_pairing_password
 *
 * @brief   Send pairing password.
 *
 * @param   conidx      - connection index.
 *          password    - password.
 *
 * @return  None.
 */
void gap_security_send_pairing_password(uint32_t conidx,uint32_t password);

/*********************************************************************
 * @fn      gap_security_pairing_req
 *
 * @brief   Send pairing request.
 *
 * @param   conidx  - connection index.
 *
 * @return  None.
 */
void gap_security_pairing_req(uint8_t conidx);

/*********************************************************************
 * @fn      gap_security_enc_req
 *
 * @brief   Send encrypt request.
 *
 * @param   conidx  - connection index.
 *
 * @return  None.
 */
void gap_security_enc_req(uint8_t conidx);

/*********************************************************************
 * @fn      gap_security_get_bond_status
 *
 * @brief   Get bond status of a connection.
 *
 * @param   None.
 *
 * @return  Bond status.
 */
bool gap_security_get_bond_status(void);

/*********************************************************************
 * @fn      gap_security_get_bond_status
 *
 * @brief   Get bond status of a connection.
 *
 * @param   None.
 *
 * @return  Bond status.
 */
void gap_security_req(uint8_t conidx);

/**********************************************************************
 * @fn      gap_get_latest_conn_parameter
 *
 * @brief   Get the latest connection parameters.
 *
 * @param   None
 *
 * @return  Point to buff of the lastest conn parameter.
 */
conn_peer_param_t *gap_get_latest_conn_parameter(void);

#endif // end of #ifndef GAP_API_H

