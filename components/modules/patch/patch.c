#include <stdint.h>

#include "jump_table.h"
#include "low_power.h"
//#include "os_mem.h"

#include "driver_system.h"
#include "driver_uart.h"
#include "driver_pmu.h"
#include "driver_flash.h"

#define PATCH_MAP_BASE_ADDR             0x20002000

struct patch_element_t
{
    uint32_t patch_pc;
    void *replace_function;
};

#ifdef USER_MEM_API_ENABLE
extern void *ke_malloc_user(uint32_t size, uint8_t type);
extern void ke_free_user(void* mem_ptr);
#endif
void pmu_calibration_start(uint8_t type, uint16_t counter);
void pmu_calibration_stop(void);

/*
 * keil debug breakpoint will take place FPB entry at the beginning of patch table with increasing
 * direction, so use patch entry point start at the end of patch table with decreasing direction.
 *
 * TAKE CARE !!!!!!!!!!!!!!!!!!!!!!!!!! the patch breakpoint register[0~5, 8~13] may be cleared by
 * KEIL unexpectly when debugging this IDE.
 */
const struct patch_element_t patch_elements[] =
{
    [15] = {
        .patch_pc = 0x00002b70, // fix bug: from rx_buf_num to tx_buf_num
    },
    [14] = {
        .patch_pc = 0x00002be4, // fix bug: from rx_buf_num to tx_buf_num
    },
    /*
     * (con_par->instant_proc.type == INSTANT_PROC_NO_PROC), this judgement will keep slave latency stopped once
     * receiving connection parameter update, channel update until the new parameters is applied. This processing
     * will make the system working with higher power consumption in deep sleep mode.
     */
    [13] = {
        .patch_pc = 0x00011f4c, // remove (con_par->instant_proc.type == INSTANT_PROC_NO_PROC) in lld_con_sched
    },
    [12] = {
        .patch_pc = 0x00000001, // take place
    },
#ifdef USER_MEM_API_ENABLE
    [11] = {
        .patch_pc = 0x0000a4ce,
        .replace_function = ke_malloc_user,
    },
    [10] = {
        .patch_pc = 0x0000a3a6,
        .replace_function = ke_free_user,
    },
#else
    [11] = {
        .patch_pc = 0x00000001, // take place
    },
    [10] = {
        .patch_pc = 0x00000001, // take place
    },
#endif
    [9] = {
        .patch_pc = 0x00012f30, // disable LLM_CH_MAP_TO temporary
    },
};

__attribute__((aligned(64))) uint32_t patch_map[16] =
{
    0xBF00DF00,		//0
    0xBF00DF01,
    0xBF00DF02,
    0xBF00DF03,
    0xBF00DF04,
    0xBF00DF05,
    0xBF00DF06,
    0xBF00DF07,
    0xBF00DF08,
    0xBF00BF00,
    0xBF00DF0a,		//10
    0xBF00DF0b,
    0x2000b57c, // PUSH {r2-r6,lr}; MOVS r0, #0
    0x6EA2BF00,
    0x22087933,
    0x4B2A0051,
};

__attribute__((section("ram_code"))) void patch_init(void)
{
    uint8_t patch_num = sizeof(patch_elements) / sizeof(patch_elements[0]);
    uint8_t i;

    FPB_SetRemap((uint32_t)&patch_map[0]);

    for(i=patch_num; i!=0; )
    {
        i--;
        if(patch_elements[i].patch_pc)
        {
            FPB_CompSet(patch_elements[i].patch_pc, 0x00, i);
        }
        else
        {
            break;
        }
    }

    FPB_Enable();
}

extern uint32_t record_lr;
__attribute__((section("ram_code"))) uint32_t svc_exception_handler_ram(uint32_t svc_pc,uint32_t lr)
{
    uint8_t patch_num = sizeof(patch_elements) / sizeof(patch_elements[0]);
    uint8_t i;
#ifdef USER_MEM_API_ENABLE
    record_lr = lr;
#endif
    for(i=0; i<patch_num; i++)
    {
        if(svc_pc == patch_elements[i].patch_pc)
        {
            break;
        }
    }

    if(i != patch_num)
    {
        return (uint32_t)(patch_elements[i].replace_function);
    }

    return 0;
}

__attribute__((section("ram_code"))) void low_power_save_entry_imp(uint8_t type)
{
    if(type == LOW_POWER_SAVE_ENTRY_BASEBAND)
    {
        /* set BUCK voltage to higher level */
        ool_write(PMU_REG_BUCK_CTRL1, 0x45);
        /* set DLDO voltage to higher level */
        ool_write(PMU_REG_DLDO_CTRL, 0x62);

        /* set cs pin of internal flash in high level */
        //pmu_gpio_set_dir(GPIO_PORT_B, GPIO_BIT_0, GPIO_DIR_OUT);

        /* power off flash */
        ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) & 0xfd);

        pmu_calibration_stop();
    }
}

#ifdef CFG_FPGA_TEST        //for FPGA
__attribute__((section("ram_code"))) void low_power_restore_entry_imp(uint8_t type)
{
    if(type == LOW_POWER_RESTORE_ENTRY_BEGINNING)
    {
        SCB->VTOR = 0x20000000; //set exception vector offset to RAM space
    }
    else if(type == LOW_POWER_RESTORE_ENTRY_BASEBAND)
    {
#ifndef KEEP_CACHE_SRAM_RETENTION
        // manul enable the cache and invalidating the SRAM
        *(volatile uint32_t *)0x500a0000 = 0x38;
        *(volatile uint32_t *)0x500a0000 = 0x3c;
        while(((*(volatile uint32_t *)0x500a0004) & 0x10) == 0);
        *(volatile uint32_t *)0x500a0000 = 0x3e;
        while((*(volatile uint32_t *)0x500a0000) & 0x02);
        *(volatile uint32_t *)0x500a0000 = 0x3d;
        while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x02);
#else
        // manul enable the cache without invalidating the SRAM
        *(volatile uint32_t *)0x500a0000 = 0x38;
        *(volatile uint32_t *)0x500a0000 = 0x3c;
        while(((*(volatile uint32_t *)0x500a0004) & 0x10) == 0);
        //*(volatile uint32_t *)0x500a0000 = 0x1e;
        //while((*(volatile uint32_t *)0x500a0000) & 0x02);
        *(volatile uint32_t *)0x500a0000 = 0x3d;
        while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x02);
#endif
    }
    else if(type == LOW_POWER_RESTORE_ENTRY_DRIVER)
    {
        NVIC_EnableIRQ(PMU_IRQn);
        system_set_port_mux(GPIO_PORT_D, GPIO_BIT_6, PORTD6_FUNC_UART1_RXD);
        system_set_port_mux(GPIO_PORT_D, GPIO_BIT_7, PORTD7_FUNC_UART1_TXD);
        uart_init(UART1, BAUD_RATE_115200);
        NVIC_EnableIRQ(UART1_IRQn);

        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART0_RXD);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART0_TXD);
        uart_init(UART0, BAUD_RATE_115200);
        NVIC_EnableIRQ(UART0_IRQn);
    }
}
#else       //for chip
__attribute__((section("ram_code"))) void low_power_restore_entry_imp(uint8_t type)
{
    if(type == LOW_POWER_RESTORE_ENTRY_BEGINNING)
    {
        SCB->VTOR = 0x20000000; //set exception vector offset to RAM space

        /* set BUCK voltage to min */
        ool_write(PMU_REG_BUCK_CTRL1, 0x05);
        /* set DLDO voltage to min */
        ool_write(PMU_REG_DLDO_CTRL, 0x42);

        /* power on flash */
        ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) | 0x02);
    }
    else if(type == LOW_POWER_RESTORE_ENTRY_BASEBAND)
    {
#ifndef KEEP_CACHE_SRAM_RETENTION
        // manul enable the cache and invalidating the SRAM
        *(volatile uint32_t *)0x500a0000 = 0x38;
        *(volatile uint32_t *)0x500a0000 = 0x3c;
        while(((*(volatile uint32_t *)0x500a0004) & 0x10) == 0);
        *(volatile uint32_t *)0x500a0000 = 0x3e;
        while((*(volatile uint32_t *)0x500a0000) & 0x02);
        *(volatile uint32_t *)0x500a0000 = 0x3d;
        while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x02);
#else
        // manul enable the cache without invalidating the SRAM
        *(volatile uint32_t *)0x500a0000 = 0x38;
        *(volatile uint32_t *)0x500a0000 = 0x3c;
        while(((*(volatile uint32_t *)0x500a0004) & 0x10) == 0);
        //*(volatile uint32_t *)0x500a0000 = 0x1e;
        //while((*(volatile uint32_t *)0x500a0000) & 0x02);
        *(volatile uint32_t *)0x500a0000 = 0x3d;
        while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x02);
#endif
    }
    else if(type == LOW_POWER_RESTORE_ENTRY_DRIVER)
    {
        /* handle the cs control to QSPI controller */
        //pmu_gpio_set_dir(GPIO_PORT_B, GPIO_BIT_0, GPIO_DIR_IN);
        pmu_calibration_start(PMU_CALI_SRC_LP_RC, __jump_table.lp_clk_calib_cnt);

        patch_init();
    }
}
#endif

__attribute__((section("ram_code"))) __attribute__((weak)) void user_entry_before_sleep_imp(void)
{
    /* turn off flash power */
    //ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) & 0xfd);

    //uart_putc_noint_no_wait(UART0, 's');
}

__attribute__((section("ram_code"))) __attribute__((weak)) void user_entry_after_sleep_imp(void)
{
    /* turn on flash power */
    //ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) | 0x02);

    //uart_putc_noint_no_wait(UART0, 'w');
    //pmu_calibration_start(PMU_CALI_SRC_LP_RC, __jump_table.lp_clk_calib_cnt);
}

