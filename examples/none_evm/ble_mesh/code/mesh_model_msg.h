/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef _MESH_MODEL_MSG_H
#define _MESH_MODEL_MSG_H

/*
 * INCLUDES
 */

/*
 * CONSTANTS
 */
#define MESH_MODEL_ID_ONOFF                 0x1000
#define MESH_MODEL_ID_LIGHTNESS             0x1300
#define MESH_MODEL_ID_LIGHTCRL              0x1303
#define MESH_MODEL_ID_HSL                   0x1307
#define MESH_MODEL_ID_VENDOR_ALI            0x01A80000

#define MESH_ALI_GROUP_ADDR_LED             0xc000
#define MESH_ALI_GROUP_ADDR_FUN             0xc007
#define MESH_ALI_SECOND_GROUP_ADDR          0xcfff

#define MESH_ALI_PUBLISH_ADDR               0xf000

#define MESH_GEN_ONOFF_SET                  0x00000282
#define MESH_GEN_ONOFF_SET_UNACK            0x00000382
#define MESH_GEN_ONOFF_STATUS               0x00000482

#define MESH_LIGHTNESS_SET                  0x00004c82
#define MESH_LIGHTNESS_SET_UNACK            0x00004d82
#define MESH_LIGHTNESS_STATUS               0x00004e82

#define MESH_HSL_SET                        0x00007682
#define MESH_HSL_SET_UNACK                  0x00007782
#define MESH_HSL_STATUS                     0x00007882

#define MESH_TEMPERATURE_SET                0x00005e82
#define MESH_TEMPERATURE_SET_UNACK          0x00005f82
#define MESH_TEMPERATURE_STATUS             0x00006082

#define MESH_VENDOR_OLD_SET                 0x0001A8c1
#define MESH_VENDOR_OLD_SET_UNACK           0x0001A8c2
#define MESH_VENDOR_OLD_STATUS              0x0001A8c3

#define MESH_VENDOR_SET                     0x0001A8d1
#define MESH_VENDOR_SET_UNACK               0x0001A8d2
#define MESH_VENDOR_STATUS                  0x0001A8d3
#define MESH_VENDOR_INDICATION              0x0001A8d4
#define MESH_VENDOR_CONFIRMATION            0x0001A8d5

#define MESH_EVENT_UPDATA_ID                0xF009
#define MESH_EVENT_DEV_RST                  0x23

#define MESH_LIGHTNESS_STATUS_MIN_SIZE      2
#define MESH_LIGHTNESS_STATUS_FULL_SIZE     5

/*
 * TYPEDEFS
 */
/** @defgroup BLE_MESH_MESSAGE_DEFINES for application layer use
 * @{
 */

// Generic OnOff Set
struct mesh_gen_onoff_model_set_t
{
    uint8_t onoff;
    uint8_t tid;
    uint8_t ttl;
    uint8_t delay;
};

// General OnOff Status
struct mesh_gen_onoff_model_status_t
{
    uint8_t present_onoff;
    uint8_t target_onoff;
    uint8_t remain;
} __attribute__((packed)) ;

// Light Lightness Set
struct mesh_lightness_model_set_t
{
    uint16_t level;
    uint8_t tid;
    uint8_t ttl;
    uint8_t delay;
};

// Light Lightness Status
struct mesh_lightness_model_status_t
{
    uint16_t current_level;
    uint16_t target_level;
    uint8_t remain;
} __attribute__((packed)) ;

// Light HSL Set
struct mesh_hsl_model_set_t
{
    uint16_t lightness;
    uint16_t hue;
    uint16_t hsl_saturation;
    uint8_t tid;
    uint8_t transition_time;
    uint8_t delay;
} __attribute__((packed)) ;

// Light HSL Status
struct mesh_hsl_model_status_t
{
    uint16_t hsl_lightness;
    uint16_t hsl_hue;
    uint16_t hsl_saturation;
    uint8_t remain;
} __attribute__((packed));

// Light CTL Set
struct mesh_CTL_model_set_t
{
    uint16_t lightness;
    uint16_t temperature;
    uint16_t delta_UV;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
};

// Light CTL Status
struct mesh_CTL_model_status_t
{
    uint16_t current_lightness;
    uint16_t current_temperature;
    uint16_t target_lightness;
    uint16_t target_temperature;
    uint8_t remain;
} __attribute__((packed)) ;

// Vendor Model Set
struct mesh_vendor_model_set_t
{
    uint8_t tid;
    uint8_t num_of_value;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Status
struct mesh_vendor_model_status_t
{
    uint8_t tid;
    uint8_t num_of_value;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Set new
struct mesh_vendor_model_set_new_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Status new
struct mesh_vendor_model_status_new_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Indication
struct mesh_vendor_model_indication_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

#endif  // _MESH_MODEL_MSG_H

