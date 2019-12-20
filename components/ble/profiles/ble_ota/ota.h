#ifndef _OTA_H
#define _OTA_H

#include <stdint.h>

#define OTA_HDR_RESULT_LEN          1
#define OTA_HDR_OPCODE_LEN          1
#define OTA_HDR_LENGTH_LEN          2

typedef enum 
{
    OTA_CMD_NVDS_TYPE,
    OTA_CMD_GET_STR_BASE,
    OTA_CMD_READ_FW_VER,    //read firmware version
    OTA_CMD_PAGE_ERASE,
    OTA_CMD_CHIP_ERASE,
    OTA_CMD_WRITE_DATA,
    OTA_CMD_READ_DATA,
    OTA_CMD_WRITE_MEM,
    OTA_CMD_READ_MEM,
    OTA_CMD_REBOOT,
    OTA_CMD_NULL,
}ota_cmd_t;

typedef enum 
{
    OTA_RSP_SUCCESS,
    OTA_RSP_ERROR,
    OTA_RSP_UNKNOWN_CMD,
}ota_rsp_t;

typedef enum 
{
    OTA_NVDS_NONE,
    OTA_NVDS_FLASH,
    OTA_NVDS_EEPROM,
}ota_nvds_type;

__packed struct firmware_version
{
    uint32_t firmware_version;
};

__packed struct storage_baseaddr
{
    uint32_t baseaddr;
};

__packed struct page_erase_rsp
{
    uint32_t base_address;
};

__packed struct write_mem_rsp
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct read_mem_rsp
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct write_data_rsp
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct read_data_rsp
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct app_ota_rsp_hdr_t
{
    uint8_t result;
    uint8_t org_opcode;
    uint16_t length;
    __packed union
    {
        uint8_t nvds_type;
        struct firmware_version version;
        struct storage_baseaddr baseaddr;
        struct page_erase_rsp page_erase;
        struct write_mem_rsp write_mem;
        struct read_mem_rsp read_mem;
        struct write_data_rsp write_data;
        struct read_data_rsp read_data;
    } rsp;
};

__packed struct page_erase_cmd
{
    uint32_t base_address;
};

__packed struct write_mem_cmd
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct read_mem_cmd
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct write_data_cmd
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct read_data_cmd
{
    uint32_t base_address;
    uint16_t length;
};

__packed struct app_ota_cmd_hdr_t
{
    uint8_t opcode;
    uint16_t length;
    __packed union
    {
        struct page_erase_cmd page_erase;
        struct write_mem_cmd write_mem;
        struct read_mem_cmd read_mem;
        struct write_data_cmd write_data;
        struct read_data_cmd read_data;
    } cmd;
};

struct otas_send_rsp
{
    uint8_t conidx;

    uint16_t length;
    uint8_t buffer[];
};


void ota_init(uint8_t conidx);
void ota_deinit(uint8_t conidx);
void app_otas_recv_data(uint8_t conidx,uint8_t *p_data,uint16_t len);
uint16_t app_otas_read_data(uint8_t conidx,uint8_t *p_data);

#endif //__OTA_H



