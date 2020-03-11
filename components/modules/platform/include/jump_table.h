#ifndef _JUMP_TABLE_H
#define _JUMP_TABLE_H

#include <stdint.h>

#include "co_math.h"

#define IMAGE_TYPE_CONTROLLER               0x00000000
#define IMAGE_TYPE_HOST                     0x11111111
#define IMAGE_TYPE_FULL                     0x22222222
#define IMAGE_TYPE_APP                      0x33333333

#define APP_BOOT_UART_PC_EN_MAGIC           0x55
#define APP_BOOT_ESCAPE_HS_MAGIC            0xFE

#define JUMP_TABLE_STATIC_KEYS_IRK          (1<<0)
#define JUMP_TABLE_STATIC_KEYS_PRIV_P256    (1<<1)
#define JUMP_TABLE_STATIC_KEYS_PUBL_P256    (1<<2)

#if 0
#define SYSTEM_OPTION_SLEEP_ENABLE          CO_BIT(0)
#define SYSTEM_OPTION_EXT_WAKEUP_ENABLE     CO_BIT(1)
#define SYSTEM_OPTION_PRINT_SEL_MSK         0x0C
#define SYSTEM_OPTION_PRINT_SWO             0x00
#define SYSTEM_OPTION_PRINT_UART1           0x04
#define SYSTEM_OPTION_PRINT_UART2           0x08
#define SYSTEM_OPTION_DEEP_SLEEP_ENABLE     CO_BIT(4)
#define SYSTEM_OPTION_LP_CLK_SEL_MSK        0x60
#define SYSTEM_OPTION_LP_CLK_RC             0x00
#define SYSTEM_OPTION_LP_CLK_32K            0x20
#define SYSTEM_OPTION_LP_CLK_32768          0x40
#define SYSTEM_OPTION_FIX_BAD_CHANNEL       CO_BIT(7)
#define SYSTEM_OPTION_TL_SEL                CO_BIT(8)   //0:uart0, 1:uart1
#define SYSTEM_OPTION_VECTOR_REMAP          CO_BIT(9)
#define SYSTEM_OPTION_WAIT_SLEEP_FLAG       CO_BIT(10)
#else
#define SYSTEM_OPTION_CODED_PHY_500         CO_BIT(0)
#define SYSTEM_OPTION_ACT_MOVE_CFG          CO_BIT(1)
#define SYSTEM_OPTION_SCAN_EXT_ADV          CO_BIT(2)
#define SYSTEM_OPTION_SLEEP_ENABLE          CO_BIT(3)
#define SYSTEM_OPTION_EXT_WAKEUP_ENABLE     CO_BIT(4)
#define SYSTEM_OPTION_LIGHT_SLEEP_ENABLE    CO_BIT(5)
#define SYSTEM_OPTION_PRINT_UART0           0x00
#define SYSTEM_OPTION_PRINT_UART1           CO_BIT(6)
#define SYSTEM_OPTION_PRINT_SWO             CO_BIT(7)
#define SYSTEM_OPTION_PRINT_MASK            (CO_BIT(6)|CO_BIT(7))
#define SYSTEM_OPTION_ENABLE_RTOS           CO_BIT(8)
#define SYSTEM_OPTION_DISABLE_HANDSHAKE     CO_BIT(9)
#define SYSTEM_OPTION_ENABLE_CACHE          CO_BIT(10)
#define SYSTEM_OPTION_ENABLE_QSPI_QMODE     CO_BIT(11)  // 1: 4 lines, 0: 2 lines(default)
#define SYSTEM_OPTION_ENABLE_FLASH_DS       CO_BIT(12)  // 1: enable flash deep sleep mode
#define SYSTEM_OPTION_RTOS_HEAP_SEL         CO_BIT(13)  // 1: KE_MEM_NON_RETENTION, 0: KE_MEM_ENV
#define SYSTEM_OPTION_BOOT_UART_PC_EN       CO_BIT(14)  // 1: use PC6/PC7 for boot up uart, 0: use PA2/PA3
#define SYSTEM_OPTION_ENABLE_HCI_MODE       CO_BIT(15)  // enable uart read start for HCI
#endif

#define JUMP_TABLE_CHECKWORD            0x51525251
#define JUMP_TABLE_CP_CONFIG_CNT        3

/// List of parameters identifiers
enum param_id_t
{
    /// Definition of the tag associated to each parameters
    /// Local Bd Address
    PARAM_BD_ADDRESS                 = 0x01,
    /// Device Name
    PARAM_DEVICE_NAME                = 0x02,
    
    /// SP Private Key 192
    PARAM_SP_PRIVATE_KEY_P192        = 0x13,
    /// SP Public Key 192
    PARAM_SP_PUBLIC_KEY_P192         = 0x14,

    /// SC Private Key (Low Energy)
    PARAM_LE_PRIVATE_KEY_P256        = 0x80,
    /// SC Public Key (Low Energy)
    PARAM_LE_PUBLIC_KEY_P256         = 0x81,
    /// SC Debug: Used Fixed Private Key from NVDS (Low Energy)
    PARAM_LE_DBG_FIXED_P256_KEY      = 0x82,
};

enum param_status_t
{
    /// PARAM status OK
    PARAM_SUCCESS,
    /// 
    PARAM_FAILED,
};

enum rtos_entry_type_t
{
    RTOS_ENTRY_TYPE_INIT,
    RTOS_ENTRY_TYPE_STACK_PUSH,
    RTOS_ENTRY_TYPE_STACK_YIELD,
    RTOS_ENTRY_TYPE_WAKEUP_RESTORE,
    RTOS_ENTRY_TYPE_POST_GATT_MSG,
};

struct bd_addr_
{
    ///6-byte array address value
    uint8_t  addr[6];
};

struct jump_table_reserved_t
{
    uint32_t reserved_data;
};

struct jump_table_image_t
{
    uint32_t image_size;
    uint32_t image_type;
};

struct jump_table_middle_t
{
    void *entry;
    //used to init ke_mem, prf_env, ke_task_env, need to be called in rwip_init function
    void (*memory_init_app)(void);
    uint32_t *stack_top_address;
};

struct jump_table_version_t
{
    uint32_t firmware_version;
};

struct jump_table_last_t
{
    uint16_t local_drift;
    uint16_t sleep_algo_dur;    // unit: 312.5us, can be used to wakeup early (enlarge this value)
    uint16_t twext;
    //uint16_t twrm;
    uint16_t twosc;

    /*
     * these parameters will be used several times.
     */
    uint8_t (*param_get)(uint8_t param_id, uint8_t * lengthPtr, void *buf);
    uint8_t (*param_set)(uint8_t param_id, uint8_t length, uint8_t *buf);
    uint32_t system_option;
    uint32_t diag_port;
    uint32_t slp_max_dur;
    uint16_t max_adv_buffer_size;
    uint16_t max_rx_buffer_size;
    uint16_t max_tx_buffer_size;
    uint16_t max_rx_time;
    uint16_t max_tx_time;
    uint16_t lp_cycle_sleep_delay;  // unit: pmu clock cycle, add lp_cycle_wakeup_delay to control minimum sleep time
    uint16_t lp_clk_calib_cnt;
    uint8_t sleep_delay_for_os;     // unit: ms
    uint8_t prf_max;
    uint8_t task_max;
    uint8_t system_clk;
    uint8_t handshake_to;
    uint8_t boot_uart_pc_en_magic;  // when pc6 and pc7 are used for boot up, this byte need to be set to 0x55 for enable double check
    uint8_t initial_qspi_bandrate;

    uint8_t enable_activity_num;
    uint8_t enable_adv_activity_num;
    uint8_t enable_con_num;
    uint8_t em_ble_rx_buf_num;
    uint8_t em_ble_tx_buf_num;

    struct bd_addr_ addr;
    uint32_t checkword;
};

struct jump_table_t
{
    /*
     * these parameters are used only one time.
     */
    uint32_t reserved_data;
    uint32_t image_size;
    uint32_t image_type;
    void *entry;
    //used to init ke_mem, prf_env, ke_task_env, need to be called in rwip_init function
    void (*memory_init_app)(void);
    uint32_t *stack_top_address;
    uint32_t firmware_version;
    uint16_t local_drift;
    uint16_t sleep_algo_dur;    // unit: 312.5us, can be used to wakeup early (enlarge this value)
    uint16_t twext;
    //uint16_t twrm;
    uint16_t twosc;

    /*
     * these parameters will be used several times.
     */
    uint8_t (*param_get)(uint8_t param_id, uint8_t * lengthPtr, void *buf);
    uint8_t (*param_set)(uint8_t param_id, uint8_t length, uint8_t *buf);
    uint32_t system_option;
    uint32_t diag_port;
    uint32_t slp_max_dur;
    uint16_t max_adv_buffer_size;
    uint16_t max_rx_buffer_size;
    uint16_t max_tx_buffer_size;
    uint16_t max_rx_time;
    uint16_t max_tx_time;
    uint16_t lp_cycle_sleep_delay;  // unit: pmu clock cycle, add lp_cycle_wakeup_delay to control minimum sleep time
    uint16_t lp_clk_calib_cnt;
    uint8_t sleep_delay_for_os;     // unit: ms
    uint8_t prf_max;
    uint8_t task_max;
    uint8_t system_clk;
    uint8_t handshake_to;
    uint8_t boot_uart_pc_en_magic;  // when pc6 and pc7 are used for boot up, this byte need to be set to 0x55 for enable double check
    uint8_t initial_qspi_bandrate;

    uint8_t enable_activity_num;
    uint8_t enable_adv_activity_num;
    uint8_t enable_con_num;
    uint8_t em_ble_rx_buf_num;
    uint8_t em_ble_tx_buf_num;

    struct bd_addr_ addr;
    uint32_t checkword;
};

extern struct jump_table_t __jump_table;

/* called by function in controller partition */
extern uint8_t (*gapc_get_conidx)(uint16_t conhdl);
extern void (*appm_init)(void);
extern void (*rwble_hl_init)(void);

extern uint32_t (*svc_exception_handler)(uint32_t svc_pc);
extern void (*low_power_save_entry)(uint8_t);
extern void (*low_power_restore_entry)(uint8_t);
extern void (*user_entry_before_sleep)(void);
extern void (*user_entry_after_sleep)(void);
extern void (*rtos_entry)(uint8_t type, void *arg);

void jump_table_set_static_keys_store_offset(uint32_t offset);
uint8_t jump_table_get_static_keys(uint8_t type, uint8_t *data);
void jump_table_set_static_keys(uint8_t type, uint8_t *data);

#endif //_JUMP_TABLE_H


