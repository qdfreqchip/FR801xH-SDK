/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include <string.h>

#include "gap_api.h"
#include "os_timer.h"
#include "proj_adv_test.h"
#include "flash_usage_config.h"


#if TEST_ADV_MODE_PER_ADV_DIRECT
/*
* DIRECT adv is only for advertising_data, and Periodic adv data is specified by local mac addr and adv sid.
* 1 this example shows that device with local mac addr "\x1F\x19\x07\x09\x17\x20", start a direct periodic adv.
* 2 adv_data is supposed to be received by device with mac addr: "\x1F\x99\x07\x09\x17\x20"
* 3 per_adv_data is with 254*4 total len , and adv_sid value as 9 , and is supposed to be received by device,
*   which started a per_sync action and is listening to this periodic adv.
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_PER_ADV_DIRECT;   //DIRECT adv is only for advertising_data, periodic adv data is specified by local mac addr
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;          //adv interval is 0x40 * 0.625ms
    adv_param.adv_intv_max = 0x40;          //adv interval is 0x40 * 0.625ms
    adv_param.phy_mode = GAP_PHY_1MBPS;      //Aux adv phy is 1M bps

    gap_mac_addr_t peer_mac_addr = {{0x1f,0x99,0x07,0x09,0x17,0x20},0};
    memcpy(&adv_param.peer_mac_addr,&peer_mac_addr,sizeof(gap_mac_addr_t)); //Set peer device mac addr,
    adv_param.peer_mac_addr.addr_type = peer_mac_addr.addr_type;

    adv_param.adv_sid = 9;                  //start adv setting id is 9
    adv_param.per_adv_intv_min = 0x100;     //per_adv interval is 0x100 * 1.25ms
    adv_param.per_adv_intv_max = 0x100;     //per_adv interval is 0x100 * 1.25ms

    gap_set_advertising_param(&adv_param);
    uint8_t adv_data[255]="\xee\x09\x11\x12\x13\x14\x15\x16\x17\x18";       //255 is the MAX len of adv_data
    gap_set_advertising_data(adv_data,sizeof(adv_data) );       //set adv_data

    uint8_t per_adv_data1[254*4]="\xee\x09\x11\x12\x13\x14\x15\x16\x17\x18";
    gap_set_per_adv_data(per_adv_data1,sizeof(per_adv_data1) ); //set per_adv_data

    gap_start_advertising(0);   //start this adv
}

os_timer_t per_sync_stop_timer;
void stop_tim_fn(void *arg)
{
    gap_stop_scan();
    gap_stop_per_sync();
}
void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x99,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_per_sync_param_t per_sync_param;
    per_sync_param.sup_to = 600;                        //supervisor timer is 6s
    per_sync_param.adv_sid = 9;                         //set per adv sid is 0x9.
    per_sync_param.adv_dev_addr.addr_type = 0;          //set device mac addr, which send periodic adv
    memcpy(per_sync_param.adv_dev_addr.addr.addr,"\x1F\x19\x07\x09\x17\x20",MAC_ADDR_LEN);
    gap_start_per_sync(&per_sync_param);                //start per_sync action to listen per_adv

    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 1;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 0;
    gap_start_scan(&scan_param);        //start scan action
    os_timer_init(&per_sync_stop_timer,stop_tim_fn,NULL);
    os_timer_start(&per_sync_stop_timer,15000,0);       //15s, stop scan and per_sync two actions
}
#endif

#if TEST_ADV_MODE_PER_ADV_UNDIRECT
/*
* UNDIRECT adv is only for advertising_data, and Periodic adv data is specified by local mac addr and adv sid.
* 1 this example shows that device with local mac addr "\x1F\x19\x07\x09\x17\x20", start a undirect periodic adv.
* 2 adv_data is supposed to be received by any device
* 3 per_adv_data is with 254*4 total len , and adv_sid value as 9 , and is supposed to be received by device,
*   which started a per_sync action and is listening to this periodic adv.
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_PER_ADV_UNDIRECT;   //DIRECT adv is only for advertising_data, periodic adv data is specified by local mac addr
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;          //adv interval is 0x40 * 0.625ms
    adv_param.adv_intv_max = 0x40;          //adv interval is 0x40 * 0.625ms
    adv_param.phy_mode = GAP_PHY_1MBPS;      //Aux adv phy is 1M bps

    //gap_mac_addr_t peer_mac_addr = {{0x1f,0x99,0x07,0x09,0x17,0x20},0};
    //memcpy(&adv_param.peer_mac_addr,&peer_mac_addr,sizeof(gap_mac_addr_t)); //Set peer device mac addr,
    //adv_param.peer_mac_addr.addr_type = peer_mac_addr.addr_type;

    adv_param.adv_sid = 9;                  //start adv setting id is 9
    adv_param.per_adv_intv_min = 0x100;     //per_adv interval is 0x100 * 1.25ms
    adv_param.per_adv_intv_max = 0x100;     //per_adv interval is 0x100 * 1.25ms

    gap_set_advertising_param(&adv_param);
    uint8_t adv_data[255]="\xee\x09\x11\x12\x13\x14\x15\x16\x17\x18";       //255 is the MAX len of adv_data
    gap_set_advertising_data(adv_data,sizeof(adv_data) );       //set adv_data

    uint8_t per_adv_data1[254*4]="\xee\x09\x11\x12\x13\x14\x15\x16\x17\x18";
    gap_set_per_adv_data(per_adv_data1,sizeof(per_adv_data1) ); //set per_adv_data

    gap_start_advertising(0);   //start this adv
}

os_timer_t per_sync_stop_timer;
void stop_tim_fn(void *arg)
{
    gap_stop_scan();
    gap_stop_per_sync();
}
void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x00,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_per_sync_param_t per_sync_param;
    per_sync_param.sup_to = 600;                        //supervisor timer is 6s
    per_sync_param.adv_sid = 9;                         //set per adv sid is 0x9.
    per_sync_param.adv_dev_addr.addr_type = 0;          //set device mac addr, which send periodic adv
    memcpy(per_sync_param.adv_dev_addr.addr.addr,"\x1F\x19\x07\x09\x17\x20",MAC_ADDR_LEN);
    gap_start_per_sync(&per_sync_param);                //start per_sync action to listen per_adv

    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 1;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 0;
    gap_start_scan(&scan_param);        //start scan action
    os_timer_init(&per_sync_stop_timer,stop_tim_fn,NULL);
    os_timer_start(&per_sync_stop_timer,15000,0);       //15s, stop scan and per_sync two actions
}
#endif

#if TEST_ADV_MODE_EXTEND_NON_CONN_SCAN
/*
* This example shows extended adv, which is non-connect, only for scan.
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_EXTEND_NON_CONN_SCAN;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    adv_param.phy_mode = GAP_PHY_2MBPS;

    gap_set_advertising_param(&adv_param);

    uint8_t rsp_data[254*4-3]="\xee\x09\x11\x12\x13\x14\x15\x16\x17\x18";
    rsp_data[254*4-7] = 3;  //len
    rsp_data[254*4-6] = 6;  //type
    rsp_data[254*4-5] = 5;
    rsp_data[254*4-4] = 4;
    gap_set_advertising_rsp_data(rsp_data,sizeof(rsp_data));

    gap_start_advertising(0);
}

os_timer_t stop_timer;
void stop_tim_fn(void *arg)
{
    gap_stop_scan();
}
void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x00,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 0;
    gap_start_scan(&scan_param);
    os_timer_init(&stop_timer,stop_tim_fn,NULL);
    os_timer_start(&stop_timer,15000,0);       //15s, stop scan and per_sync two actions
}
#endif


#if TEST_ADV_MODE_EXTEND_CONN_UNDIRECT
/*
* This example shows extended adv, which is only for connect, not for scan.
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_EXTEND_CONN_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    adv_param.phy_mode = GAP_PHY_2MBPS;
    adv_param.adv_sid = 0x2;
    gap_set_advertising_param(&adv_param);

    uint8_t adv_data[0x1C];
    uint8_t local_name[] = "8010H_EXT_ADV";
    uint8_t local_name_len = sizeof(local_name);

    uint8_t *pos;
    uint8_t adv_data_len = 0;
    pos = &adv_data[0];
    *pos++ = local_name_len + 1;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, local_name, local_name_len);
    pos += local_name_len;
    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));
    gap_set_advertising_data(adv_data,adv_data_len );

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x00,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn(&mac,0,9,9,0,400);
}
#endif

#if TEST_ADV_MODE_EXTEND_CONN_DIRECT
/*
* This example shows extended adv, which is only for direct_connect, not for scan.
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_EXTEND_CONN_DIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    adv_param.phy_mode = GAP_PHY_2MBPS;
    adv_param.adv_sid = 0x2;
    gap_mac_addr_t peer_mac_addr = {{0x1f,0x99,0x07,0x09,0x17,0x20},0};             //direct peer device mac addr
    memcpy(&adv_param.peer_mac_addr,&peer_mac_addr,sizeof(gap_mac_addr_t));

    gap_set_advertising_param(&adv_param);

    uint8_t adv_data[0x1C];
    uint8_t local_name[] = "8010H_EXT_ADV";
    uint8_t local_name_len = sizeof(local_name);

    uint8_t *pos;
    uint8_t adv_data_len = 0;
    pos = &adv_data[0];
    *pos++ = local_name_len + 1;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, local_name, local_name_len);
    pos += local_name_len;
    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));
    gap_set_advertising_data(adv_data,adv_data_len );

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x99,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn(&mac,0,9,9,0,400);
}
#endif


#if TEST_ADV_MODE_EXTEND_CONN_UNDIRECT_LONGRANGE
/*
* This example shows extended adv on coded phy , which is for connect and scan.
* coded phy is long range
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_EXTEND_CONN_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    adv_param.phy_mode = GAP_PHY_CODED;
    adv_param.adv_sid = 0x2;
    gap_set_advertising_param(&adv_param);

    uint8_t adv_data[0x1C];
    uint8_t local_name[] = "8010H_ADV_LONG";
    uint8_t local_name_len = sizeof(local_name);

    uint8_t *pos;
    uint8_t adv_data_len = 0;
    pos = &adv_data[0];
    *pos++ = local_name_len + 1;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, local_name, local_name_len);
    pos += local_name_len;
    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));
    gap_set_advertising_data(adv_data,adv_data_len );

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
#if 1
    mac_addr_t local_mac = {{0x1F,0x00,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn_long_range(&mac,0,9,9,0,400);
#else
    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 1500;     //15s
    scan_param.phy_mode = GAP_PHY_CODED;
    gap_start_scan(&scan_param);
#endif
}
#endif
#if TEST_ADV_MODE_EXTEND_CONN_DIRECT_LONGRANGE
/*
* This example shows extended adv on LE CODED PHY, which is only for direct_connect, not for scan.
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_EXTEND_CONN_DIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    adv_param.phy_mode = GAP_PHY_CODED;
    adv_param.adv_sid = 0x2;
    gap_mac_addr_t peer_mac_addr = {{0x1f,0x99,0x07,0x09,0x17,0x20},0};             //direct peer device mac addr
    memcpy(&adv_param.peer_mac_addr,&peer_mac_addr,sizeof(gap_mac_addr_t));

    gap_set_advertising_param(&adv_param);

    uint8_t adv_data[0x1C];
    uint8_t local_name[] = "8010H_EXT_ADV";
    uint8_t local_name_len = sizeof(local_name);

    uint8_t *pos;
    uint8_t adv_data_len = 0;
    pos = &adv_data[0];
    *pos++ = local_name_len + 1;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, local_name, local_name_len);
    pos += local_name_len;
    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));
    gap_set_advertising_data(adv_data,adv_data_len );

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x99,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn_long_range(&mac,0,9,9,0,400);
}
#endif
#if TEST_ADV_MODE_EXTEND_NON_CONN_SCAN_LONGRANGE
/*
* This example shows extended adv on LE CODED PHY, which is non-connect, only for scan.
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_EXTEND_NON_CONN_SCAN;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    adv_param.phy_mode = GAP_PHY_CODED;

    gap_set_advertising_param(&adv_param);

    uint8_t rsp_data[254*4-3]="\xee\x09\x11\x12\x13\x14\x15\x16\x17\x18";
    rsp_data[254*4-7] = 3;  //len
    rsp_data[254*4-6] = 6;  //type
    rsp_data[254*4-5] = 5;
    rsp_data[254*4-4] = 4;
    gap_set_advertising_rsp_data(rsp_data,sizeof(rsp_data));

    gap_start_advertising(0);
}

os_timer_t stop_timer;
void stop_tim_fn(void *arg)
{
    gap_stop_scan();
}
void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x00,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 1500;     //15s
    scan_param.phy_mode = GAP_PHY_CODED;
    gap_start_scan(&scan_param);
}
#endif



#if TEST_ADV_MODE_UNDIRECT
/*
* This example shows legacy undirected adv,
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    gap_set_advertising_param(&adv_param);

    uint8_t adv_data[0x1C];
    uint8_t *pos;
    uint8_t adv_data_len = 0;
    pos = &adv_data[0];
    uint8_t manufacturer_value[] = {0x00,0x00};
    *pos++ = sizeof(manufacturer_value) + 1;
    *pos++  = '\xff';
    memcpy(pos, manufacturer_value, sizeof(manufacturer_value));
    pos += sizeof(manufacturer_value);

    uint16_t uuid_value = 0x1812;
    *pos++ = sizeof(uuid_value) + 1;
    *pos++  = '\x03';
    memcpy(pos, (uint8_t *)&uuid_value, sizeof(uuid_value));
    pos += sizeof(uuid_value);
    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));
    gap_set_advertising_data(adv_data,adv_data_len );


    uint8_t scan_rsp_data[0x1F];
    uint8_t scan_rsp_data_len = 0;

    uint8_t local_name[] = "8010H_ADV";
    uint8_t local_name_len = sizeof(local_name);
    pos = &scan_rsp_data[0];
    *pos++ = local_name_len + 1;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, local_name, local_name_len);
    pos += local_name_len;
    scan_rsp_data_len = ((uint32_t)pos - (uint32_t)(&scan_rsp_data[0]));
    gap_set_advertising_rsp_data(scan_rsp_data,scan_rsp_data_len );

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
    gap_scan_param_t scan_param;
    scan_param.scan_mode = GAP_SCAN_MODE_GEN_DISC;
    scan_param.dup_filt_pol = 0;
    scan_param.scan_intv = 32;  //scan event on-going time
    scan_param.scan_window = 20;
    scan_param.duration = 1500;     //15s
    gap_set_link_rssi_report(true);
    gap_start_scan(&scan_param);
}
#endif

#if TEST_ADV_MODE_DIRECT
/*
* This example shows legacy low cycle directed adv,
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_DIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 1600;        //1s
    adv_param.adv_intv_max = 1600;        //1s

    memcpy(&adv_param.peer_mac_addr.addr,"\x1F\x99\x07\x09\x17\x20",MAC_ADDR_LEN);
    adv_param.peer_mac_addr.addr_type = 0;

    gap_set_advertising_param(&adv_param);

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x99,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn(&mac,0,9,9,0,400);
}
#endif

#if TEST_ADV_MODE_HDC_DIRECT
/*
* This example shows legacy high cycle directed adv,
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_HDC_DIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 0x20;
    adv_param.adv_intv_max = 0x20;

    memcpy(&adv_param.peer_mac_addr.addr,"\x1F\x99\x07\x09\x17\x20",MAC_ADDR_LEN);
    adv_param.peer_mac_addr.addr_type = 0;

    gap_set_advertising_param(&adv_param);

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x99,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn(&mac,0,9,9,0,400);
}
#endif


#if TEST_ADV_MODE_UNDIRECT_WHITE_LIST
/*
* This example shows legacy undirected adv and how to use whilte list and ral list for device filter
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    gap_adv_param_t adv_param;

    /************
    * code explain:
    1 get bond info of device idx 0
    2 check if this device is bonded or not
    3 if this device is bond, then set its mac addr to white list, and set its irk to ral list.
    4 set adv filt policy is scan in white list and conn in white list
    This code normally should be placed in entry function: user_entry_after_ble_init()
    *********/
    gap_bond_manager_init(BLE_BONDING_INFO_SAVE_ADDR,BLE_REMOTE_SERVICE_SAVE_ADDR,1,true);
    gap_bond_info_t info;
    gap_bond_manager_get_info(0,&info);
    if(info.bond_flag)
    {
        gap_mac_addr_t mac_set[1];
        memcpy(&mac_set[0],&info.peer_addr,sizeof(gap_mac_addr_t));
        gap_set_wl(&mac_set[0],1);

        gap_ral_t ral_set[1];
        memcpy(&ral_set[0].addr,&info.peer_addr,sizeof(gap_mac_addr_t));
        memcpy(ral_set[0].peer_irk,info.peer_irk,16);
        gap_set_ral(&ral_set[0],1);
        adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_WLST_CON_WLST;
    }
    else
        adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    /**********************/

    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;

    adv_param.adv_intv_min = 0x40;
    adv_param.adv_intv_max = 0x40;
    gap_set_advertising_param(&adv_param);

    uint8_t adv_data[0x1C];
    uint8_t *pos;
    uint8_t adv_data_len = 0;
    pos = &adv_data[0];
    uint8_t manufacturer_value[] = {0x00,0x00};
    *pos++ = sizeof(manufacturer_value) + 1;
    *pos++  = '\xff';
    memcpy(pos, manufacturer_value, sizeof(manufacturer_value));
    pos += sizeof(manufacturer_value);

    uint16_t uuid_value = 0x1812;
    *pos++ = sizeof(uuid_value) + 1;
    *pos++  = '\x03';
    memcpy(pos, (uint8_t *)&uuid_value, sizeof(uuid_value));
    pos += sizeof(uuid_value);
    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));
    gap_set_advertising_data(adv_data,adv_data_len );


    uint8_t scan_rsp_data[0x1F];
    uint8_t scan_rsp_data_len = 0;

    uint8_t local_name[] = "8010H_ADV";
    uint8_t local_name_len = sizeof(local_name);
    pos = &scan_rsp_data[0];
    *pos++ = local_name_len + 1;  //pos len;  (payload + type)
    *pos++  = '\x09';   //pos: type
    memcpy(pos, local_name, local_name_len);
    pos += local_name_len;
    scan_rsp_data_len = ((uint32_t)pos - (uint32_t)(&scan_rsp_data[0]));
    gap_set_advertising_rsp_data(scan_rsp_data,scan_rsp_data_len );

    gap_start_advertising(0);
}

void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x99,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn(&mac,0,9,9,0,400);
}
#endif

#if TEST_ADV_MODE_UNDIRECT_DOUBLE_ADV
/*
* This example shows case: simultaneously two legacy undirected adv keep running. 
*/
void start_adv(void)
{
    mac_addr_t local_mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

//adv1
    gap_adv_param_t adv_param;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;

    adv_param.adv_intv_min = 80;
    adv_param.adv_intv_max = 80;
    gap_set_advertising_param(&adv_param);

    uint8_t adv_data[]="\x03\x03\x12\x18";
    uint8_t rsp_data[]="\x09\x08\x46\x52\x38\x30\x31\x30\x48\x00";
    gap_set_advertising_data(adv_data,sizeof(adv_data) -1);
    gap_set_advertising_rsp_data(rsp_data,sizeof(rsp_data) -1);
    gap_start_advertising(0);

//adv2
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;

    adv_param.adv_intv_min = 100;
    adv_param.adv_intv_max = 100;
    gap_set_advertising_param1(&adv_param);

    uint8_t adv_data1[]="\x03\x03\x02\x18";
    uint8_t rsp_data1[]="\x09\x08\x49\x52\x38\x30\x31\x30\x48\x00";
    gap_set_advertising_data1(adv_data1,sizeof(adv_data1) -1);
    gap_set_advertising_rsp_data1(rsp_data1,sizeof(rsp_data1) -1);
    gap_start_advertising1(0);
}

void start_scan_Or_conn(void)
{
    mac_addr_t local_mac = {{0x1F,0x99,0x07,0x09,0x17,0x20}};
    gap_address_set(&local_mac);

    mac_addr_t mac = {{0x1F,0x19,0x07,0x09,0x17,0x20}};
    gap_start_conn(&mac,0,9,9,0,400);
}
#endif


