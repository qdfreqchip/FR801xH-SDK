#include <stdint.h>
#include <stdbool.h>

#include "co_printf.h"

#include "mesh_api.h"

#define MESH_INFO_STORE_ADDR            0x30000

static const mesh_model_t light_models[] = 
{
    [0] = {
        .model_id = 0x1000,
        .model_vendor = false,
        .element_idx = 0,
    },
    [1] = {
        .model_id = 0x1300,
        .model_vendor = false,
        .element_idx = 0,
    },
};

static void mesh_callback_func(mesh_event_t * event)
{
    static const uint8_t uuid[] = {0xa8, 0x01, 0x7c, 0x9c, 0x00, 0x00, 0x00, 0xcb, 0x60, 0x6b, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00};
    static const uint8_t auth_data[] = {0x0a, 0x0d, 0x0c, 0xab, 0xa2, 0x90, 0xac, 0x28, 0x24, 0x8e, 0x52, 0xc6, 0xd5, 0x7b, 0xb2, 0x9d};
    
    co_printf("mesh event type is %d.\r\n", event->type);
    switch(event->type) {
        case MESH_EVT_READY:
            mesh_start();
            break;
        case MESH_EVT_PROV_PARAM_REQ:
            mesh_send_prov_param_rsp((uint8_t *)uuid, 0xd97478b3, 0, 0, 0, 0, 0, 0, 0, 1, 0);
            break;
        case MESH_EVT_PROV_AUTH_DATA_REQ:
            mesh_send_prov_auth_data_rsp(true, 16, (uint8_t *)auth_data);
            break;
        default:
            break;
    }
}

void ali_mesh_led_init(void)
{
    mesh_set_cb_func(mesh_callback_func);
    
    mesh_init(MESH_FEATURE_RELAY
                | MESH_FEATURE_PROXY
                | MESH_FEATURE_PB_GATT, MESH_INFO_STORE_ADDR);
    
    mesh_add_model(&light_models[0]);
    mesh_add_model(&light_models[1]);
}

