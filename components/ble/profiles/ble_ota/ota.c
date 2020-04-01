
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "jump_table.h"

#include "os_mem.h"
#include "driver_plf.h"
#include "driver_system.h"
#include "driver_flash.h"
#include "driver_wdt.h"
#include "driver_uart.h"

//#include "qspi.h"
//#include "co_utils.h"
#include "gatt_api.h"
#include "gap_api.h"

#include "sys_utils.h"

#include "ota.h"
#include "ota_service.h"

struct app_otas_status_t
{
    uint8_t read_opcode;
    uint16_t length;
    uint32_t base_addr;
} app_otas_status;
static struct buffed_pkt
{
    uint8_t *buf;
    uint16_t len;   //current length in the buffer
    uint16_t malloced_pkt_num;  //num of pkts
} first_pkt = {0};
uint8_t first_loop = false;
static uint16_t at_data_idx;

extern uint8_t app_boot_get_storage_type(void);
extern void app_boot_save_data(uint32_t dest, uint8_t *src, uint32_t len);
extern void app_boot_load_data(uint8_t *dest, uint32_t src, uint32_t len);
extern void system_set_cache_config(uint8_t value, uint8_t count_10us);

static uint32_t app_otas_get_curr_firmwave_version(void)
{
    struct jump_table_t *jump_table_a = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)  // part B
    {
        struct jump_table_t *jump_table_b = (struct jump_table_t *)(0x01000000 + jump_table_a->image_size);
        return jump_table_b->firmware_version;
    }
    else        // part A
        return jump_table_a->firmware_version;
}
static uint32_t app_otas_get_curr_code_address(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)  // part B
        return jump_table_tmp->image_size;
    else    // part A
        return 0;
}
static uint32_t app_otas_get_storage_address(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)      //partB, then return partA flash Addr
        return 0;
    else
        return jump_table_tmp->image_size;  //partA, then return partB flash Addr
}
static uint32_t app_otas_get_image_size(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    return jump_table_tmp->image_size;
}



__attribute__((section("ram_code"))) static void app_otas_save_data(uint32_t dest, uint8_t *src, uint32_t len)
{
    uint32_t current_remap_address, remap_size;
    current_remap_address = system_regs->remap_virtual_addr;
    remap_size = system_regs->remap_length;

    //GLOBAL_INT_DISABLE();
    *(volatile uint32_t *)0x500a0000 = 0x3c;
    while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x00);
    //if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_CACHE)
    //{
    //    system_set_cache_config(0x60, 10);
    //}
    system_regs->remap_virtual_addr = 0;
    system_regs->remap_length = 0;

    flash_write(dest, len, src);

    system_regs->remap_virtual_addr = current_remap_address;
    system_regs->remap_length = remap_size;
    *(volatile uint32_t *)0x500a0000 = 0x3d;
    while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x02);
    //if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_CACHE)
    //{
    //    system_set_cache_config(0x61, 10);
    //}
    //GLOBAL_INT_RESTORE();
    /*
        uint8_t *buffer = (uint8_t *)ke_malloc(len, KE_MEM_NON_RETENTION);
        flash_read(dest, len, buffer);
        for(uint32_t i = 0; i<len; i++ )
        {
            if( buffer[i] != src[i] )
            {
                co_printf("err check[%d]\r\n",i);
                while(1);
            }
        }
        ke_free((void *)buffer);
    */
}

void ota_clr_buffed_pkt(void)
{
    //current_conidx = 200;
    if(first_pkt.buf != NULL)
    {
        first_loop = true;
        os_free(first_pkt.buf);
        memset(&first_pkt,0x0,sizeof(first_pkt));
    }
}
void ota_init(uint8_t conidx)
{
    app_otas_status.read_opcode = OTA_CMD_NULL;
    first_loop = true;
    at_data_idx = 0;
}
void ota_deinit(uint8_t conidx)
{
    ota_clr_buffed_pkt();
}
void __attribute__((weak)) ota_change_flash_pin(void)
{
    ;
}
void __attribute__((weak)) ota_recover_flash_pin(void)
{
    ;
}

void app_otas_recv_data(uint8_t conidx,uint8_t *p_data,uint16_t len)
{
    struct app_ota_cmd_hdr_t *cmd_hdr = (struct app_ota_cmd_hdr_t *)p_data;
    struct app_ota_rsp_hdr_t *rsp_hdr;
    uint16_t rsp_data_len = (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN+OTA_HDR_RESULT_LEN);

    if(first_loop)
    {
        first_loop = false;
        gap_conn_param_update(conidx, 6, 6, 0, 500);
        gatt_mtu_exchange_req(conidx);
    }
    co_printf("app_otas_recv_data[%d]: %d, %d. %d\r\n",at_data_idx, gatt_get_mtu(conidx), len, cmd_hdr->cmd.write_data.length);
    show_reg(p_data,sizeof(struct app_ota_cmd_hdr_t),1);
    at_data_idx++;
    ota_change_flash_pin();
    wdt_feed();

    switch(cmd_hdr->opcode)
    {
        case OTA_CMD_NVDS_TYPE:
            rsp_data_len += 1;
            break;
        case OTA_CMD_GET_STR_BASE:
            at_data_idx = 0;
            ota_clr_buffed_pkt();
            rsp_data_len += sizeof(struct storage_baseaddr);
            break;
        case OTA_CMD_READ_FW_VER:
            rsp_data_len += sizeof(struct firmware_version);
            break;
        case OTA_CMD_PAGE_ERASE:
            rsp_data_len += sizeof(struct page_erase_rsp);
            break;
        case OTA_CMD_WRITE_DATA:
            rsp_data_len += sizeof(struct write_data_rsp);
            break;
        case OTA_CMD_READ_DATA:
            rsp_data_len += sizeof(struct read_data_rsp) + cmd_hdr->cmd.read_data.length;
            if(rsp_data_len > OTAS_NOTIFY_DATA_SIZE)
            {
                // 数据太长，不能通过notify返回，通知client采用read方式获取
                rsp_data_len = sizeof(struct read_data_rsp) + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN+OTA_HDR_RESULT_LEN);
                app_otas_status.read_opcode = OTA_CMD_READ_DATA;
                app_otas_status.length = cmd_hdr->cmd.read_data.length;
                app_otas_status.base_addr = cmd_hdr->cmd.read_data.base_address;
            }
            break;
        case OTA_CMD_WRITE_MEM:
            rsp_data_len += sizeof(struct write_mem_rsp);
            break;
        case OTA_CMD_READ_MEM:
            rsp_data_len += sizeof(struct read_mem_rsp) + cmd_hdr->cmd.read_mem.length;
            if(rsp_data_len > OTAS_NOTIFY_DATA_SIZE)
            {
                // 数据太长，不能通过notify返回，通知client采用read方式获取
                rsp_data_len = sizeof(struct read_data_rsp) + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN+OTA_HDR_RESULT_LEN);
                app_otas_status.read_opcode = OTA_CMD_READ_MEM;
                app_otas_status.length = cmd_hdr->cmd.read_data.length;
                app_otas_status.base_addr = cmd_hdr->cmd.read_data.base_address;
            }
            else
            {
                app_otas_status.read_opcode = OTA_CMD_NULL;
            }
            break;
    }

    struct otas_send_rsp *req = os_malloc(sizeof(struct otas_send_rsp) + rsp_data_len);
    uint16_t base_length;

    req->conidx = conidx;
    req->length = rsp_data_len;
    rsp_hdr = (struct app_ota_rsp_hdr_t *)&req->buffer[0];
    rsp_hdr->result = OTA_RSP_SUCCESS;
    rsp_hdr->org_opcode = cmd_hdr->opcode;
    rsp_hdr->length = rsp_data_len - (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN+OTA_HDR_RESULT_LEN);

    switch(cmd_hdr->opcode)
    {
        case OTA_CMD_NVDS_TYPE:
            rsp_hdr->rsp.nvds_type = app_boot_get_storage_type() | 0x10;    // 0x10 is used to identify FR8010H
            break;
        case OTA_CMD_GET_STR_BASE:
            rsp_hdr->rsp.baseaddr.baseaddr = app_otas_get_storage_address();
            break;
        case OTA_CMD_READ_FW_VER:
            rsp_hdr->rsp.version.firmware_version = __jump_table.firmware_version;
            break;
        case OTA_CMD_PAGE_ERASE:
        {
            rsp_hdr->rsp.page_erase.base_address = cmd_hdr->cmd.page_erase.base_address;
            uint32_t new_bin_base = app_otas_get_storage_address();
#if 1
            ///co_printf("cur_code_addr:%x\r\n",new_bin_base);
            if( app_otas_get_curr_code_address() == 0 )
            {
                if(rsp_hdr->rsp.page_erase.base_address < app_otas_get_image_size())
                {
                    gap_disconnect_req(conidx);
                    break;
                }
            }
            else
            {
                if(rsp_hdr->rsp.page_erase.base_address >= app_otas_get_image_size())
                {
                    gap_disconnect_req(conidx);
                    break;
                }
            }
#endif
            if(rsp_hdr->rsp.page_erase.base_address == new_bin_base)
            {
                for(uint16_t offset = 256; offset < 4096; offset += 256)
                    flash_page_erase(offset + new_bin_base);
            }
            else
                flash_erase(rsp_hdr->rsp.page_erase.base_address, 0x1000);
        }
        break;
        case OTA_CMD_CHIP_ERASE:
            break;
        case OTA_CMD_WRITE_DATA:
        {
            rsp_hdr->rsp.write_data.base_address = cmd_hdr->cmd.write_data.base_address;
            rsp_hdr->rsp.write_data.length = cmd_hdr->cmd.write_data.length;
//write user data.
            if(rsp_hdr->rsp.write_data.base_address >= (app_otas_get_image_size()*2))
            {
                app_otas_save_data(rsp_hdr->rsp.write_data.base_address,
                                   p_data + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN)+sizeof(struct write_data_cmd),
                                   rsp_hdr->rsp.write_data.length);
                break;
            }

            uint32_t new_bin_base = app_otas_get_storage_address();
            if( rsp_hdr->rsp.write_data.base_address == new_bin_base )
            {
                if(first_pkt.buf == NULL)
                {
                    first_pkt.malloced_pkt_num = ROUND(256,rsp_hdr->rsp.write_data.length);
                    first_pkt.buf = os_malloc(rsp_hdr->rsp.write_data.length * first_pkt.malloced_pkt_num);
                    uint8_t * tmp = p_data + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN)+sizeof(struct write_data_cmd);
                    first_pkt.len = rsp_hdr->rsp.write_data.length;
                    memcpy(first_pkt.buf,tmp,first_pkt.len);
                }
            }
            else
            {
                if( rsp_hdr->rsp.write_data.base_address <= (new_bin_base + rsp_hdr->rsp.write_data.length *(first_pkt.malloced_pkt_num-1)) )
                {
                    if(first_pkt.buf != NULL)
                    {
                        uint8_t * tmp = p_data + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN)+sizeof(struct write_data_cmd);
                        memcpy(first_pkt.buf + first_pkt.len,tmp,rsp_hdr->rsp.write_data.length);
                        first_pkt.len += rsp_hdr->rsp.write_data.length;
                    }
                }
                else
                    app_otas_save_data(rsp_hdr->rsp.write_data.base_address,
                                       p_data + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN)+sizeof(struct write_data_cmd),
                                       rsp_hdr->rsp.write_data.length);
                //change firmware version in buffed pkt.
                if(first_pkt.len >= rsp_hdr->rsp.write_data.length * first_pkt.malloced_pkt_num)
                {
                    uint32_t firmware_offset = (uint32_t)&((struct jump_table_t *)0x01000000)->firmware_version- 0x01000000;
                    if( *(uint32_t *)((uint32_t)first_pkt.buf + firmware_offset) <= app_otas_get_curr_firmwave_version() )
                    {
                        uint32_t new_bin_ver = app_otas_get_curr_firmwave_version() + 1;
                        co_printf("old_ver:%08X\r\n",*(uint32_t *)((uint32_t)first_pkt.buf + firmware_offset));
                        co_printf("new_ver:%08X\r\n",new_bin_ver);
                        //checksum_minus = new_bin_ver - *(uint32_t *)((uint32_t)first_pkt.buf + firmware_offset);
                        *(uint32_t *)((uint32_t)first_pkt.buf + firmware_offset) = new_bin_ver;
                    }
                    //write data from 256 ~ rsp_hdr->rsp.write_data.length * first_pkt.malloced_pkt_num
                    app_otas_save_data(new_bin_base + 256,first_pkt.buf + 256,first_pkt.len - 256);
                }
            }
        }
        break;
        case OTA_CMD_READ_DATA:
            rsp_hdr->rsp.read_data.base_address = cmd_hdr->cmd.read_data.base_address;
            rsp_hdr->rsp.read_data.length = cmd_hdr->cmd.read_data.length;
            base_length = sizeof(struct read_data_rsp) + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN+OTA_HDR_RESULT_LEN);
            if(rsp_data_len != base_length)
            {
                app_boot_load_data((uint8_t*)rsp_hdr+base_length,
                                   rsp_hdr->rsp.read_data.base_address,
                                   rsp_hdr->rsp.read_data.length);
            }
            break;
        case OTA_CMD_WRITE_MEM:
            rsp_hdr->rsp.write_mem.base_address = cmd_hdr->cmd.write_mem.base_address;
            rsp_hdr->rsp.write_mem.length = cmd_hdr->cmd.write_mem.length;
            memcpy((void *)rsp_hdr->rsp.write_mem.base_address,
                   p_data + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN)+sizeof(struct write_data_cmd),
                   rsp_hdr->rsp.write_mem.length);
            break;
        case OTA_CMD_READ_MEM:
            rsp_hdr->rsp.read_mem.base_address = cmd_hdr->cmd.read_mem.base_address;
            rsp_hdr->rsp.read_mem.length = cmd_hdr->cmd.read_mem.length;
            base_length = sizeof(struct read_mem_rsp) + (OTA_HDR_OPCODE_LEN+OTA_HDR_LENGTH_LEN+OTA_HDR_RESULT_LEN);
            if(rsp_data_len != base_length)
            {
                memcpy((uint8_t*)rsp_hdr+base_length,
                       (void *)rsp_hdr->rsp.read_mem.base_address,
                       rsp_hdr->rsp.read_data.length);
            }
            break;
        case OTA_CMD_REBOOT:
            if(first_pkt.buf != NULL)
            {
                uint32_t new_bin_base = app_otas_get_storage_address();
                flash_page_erase(new_bin_base);
                app_otas_save_data(new_bin_base,first_pkt.buf,256);
            }
            uart_finish_transfers(UART1_BASE);
            ota_clr_buffed_pkt();
            NVIC_SystemReset();
            break;
        default:
            rsp_hdr->result = OTA_RSP_UNKNOWN_CMD;
            break;
    }

    ota_gatt_report_notify(conidx,req->buffer,req->length);
    ota_recover_flash_pin();
}



uint16_t app_otas_read_data(uint8_t conidx,uint8_t *p_data)
{
    uint16_t length;
    switch(app_otas_status.read_opcode)
    {
        case OTA_CMD_READ_DATA:
            app_boot_load_data(p_data,app_otas_status.base_addr,app_otas_status.length);
            length = app_otas_status.length;
            break;
        case OTA_CMD_READ_MEM:
            memcpy(p_data, (uint8_t *)app_otas_status.base_addr, app_otas_status.length);
            length = app_otas_status.length;
            break;
        default:
            length = 0;
            break;
    }
    app_otas_status.read_opcode = OTA_CMD_NULL;
    return length;
}

