#include <stdint.h>
#include <stdbool.h>

#include "co_printf.h"
#include "sha256.h"

#include "mesh_api.h"

#define MESH_INFO_STORE_ADDR            0x30000

/*
 * this is an ali mesh key sample: 0000009c,78da076b60cb,ee7751e0dad7483eb1c7391310b4a951
 * these information should be read from flash in actual product.
 */
static const uint8_t ali_mesh_key_bdaddr[] = {0xcb, 0x60, 0x6b, 0x07, 0xda, 0x78};
static const uint8_t ali_mesh_key_pid[] = {0x9c, 0x00, 0x00, 0x00};
static const uint8_t ali_mesh_key_secret[] = {0x51, 0xa9, 0xb4, 0x10, 0x13, 0x39, 0xc7, 0xb1, 0x3e, 0x48, 0xd7, 0xda, 0xe0, 0x51, 0x77, 0xee};

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
    uint8_t tmp_data[16];
    
    switch(event->type) {
        case MESH_EVT_READY:
            mesh_start();
            break;
        case MESH_EVT_PROV_PARAM_REQ:
            tmp_data[0] = 0xa8;
            tmp_data[1] = 0x01;
            tmp_data[2] = 0x7c;
            memcpy(&tmp_data[3], ali_mesh_key_pid, 4);
            memcpy(&tmp_data[7], ali_mesh_key_bdaddr, 6);
            tmp_data[13] = 0x00;
            tmp_data[14] = 0x00;
            tmp_data[15] = 0x00;
            mesh_send_prov_param_rsp((uint8_t *)tmp_data, 0xd97478b3, 0, 0, 0, 0, 0, 0, 0, 1, 0);
            break;
        case MESH_EVT_PROV_AUTH_DATA_REQ:
            sha256_gen_auth_value((BYTE *)ali_mesh_key_pid, (BYTE *)ali_mesh_key_bdaddr, (BYTE *)ali_mesh_key_secret, tmp_data);
            mesh_send_prov_auth_data_rsp(true, 16, (uint8_t *)tmp_data);
            break;
        case MESH_EVT_RESET:
            co_printf("removed from network by provisoner.\r\n");
            break;
        case MESH_EVT_ADV_REPORT:
            {
                #if 0
                gap_evt_adv_report_t *report = &(event->param.adv_report);
                co_printf("recv adv from: %02x-%02x-%02x-%02x-%02x-%02x\r\n", report->src_addr.addr.addr[5],
                                                                                report->src_addr.addr.addr[4],
                                                                                report->src_addr.addr.addr[3],
                                                                                report->src_addr.addr.addr[2],
                                                                                report->src_addr.addr.addr[1],
                                                                                report->src_addr.addr.addr[0]);
                for(uint16_t i=0; i<report->length; i++) {
                    co_printf("%02x ", report->data[i]);
                }
                co_printf("\r\n");
                #endif
            }
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

void ali_mesh_get_addr(uint8_t *addr)

{
    memcpy(addr, ali_mesh_key_bdaddr, 6);
}

/*********************************************************************
 * @fn      ali_mesh_send_user_adv_packet
 *
 * @brief   this is an example to show how to send data defined by user, 
 *          this function should not be recall until event GAP_EVT_ADV_END 
 *          is received.
 *
 * @param   duration    - how many 10ms advertisng will last.
 *          adv_data    - advertising data pointer
 *          adv_len     - advertising data length
 *
 * @return  None.
 */
void ali_mesh_send_user_adv_packet(uint8_t duration, uint8_t *adv_data, uint8_t adv_len)
{
    gap_adv_param_t adv_param;

    adv_param.adv_mode = GAP_ADV_MODE_NON_CONN_NON_SCAN;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PRIVATE;
    adv_param.adv_intv_min = 32;
    adv_param.adv_intv_max = 32;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    gap_set_advertising_param(&adv_param);
    gap_set_advertising_data(adv_data, adv_len);
    gap_start_advertising(duration);
}

