#include <stdbool.h>

#include "driver_system.h"
#include "driver_pmu.h"
#include "driver_gpio.h"
#include "driver_pmu_pwm.h"

void pmu_pwm_start(enum system_port_t port, enum system_port_bit_t bit,bool repeat_flag,bool reverse_flag)
{
    switch( (port * 8 + bit)%4 )
    {
        case 0:
            //ool_write(PMU_REG_PTC0_CTRL, PMU_PTC_CNT_RESET );
            //co_delay_100us(1);
            //ool_write(PMU_REG_PTC0_CTRL, 0 );
            //fputc('A',0);
            ool_write(PMU_REG_PTC0_CTRL, PMU_PTC_EN|(reverse_flag<<4)|((!repeat_flag)<<2)|PMU_PTC_OE );
            //co_printf("A:%x\r\n",ool_read(PMU_REG_PTC0_CTRL) );
            break;
        case 1:
            //ool_write(PMU_REG_PTC1_CTRL, PMU_PTC_CNT_RESET );
            //co_delay_100us(1);
            //ool_write(PMU_REG_PTC1_CTRL, 0 );
            //fputc('B',0);
            ool_write(PMU_REG_PTC1_CTRL, PMU_PTC_EN|(reverse_flag<<4)|((!repeat_flag)<<2)|PMU_PTC_OE );
            //co_printf("B:%x\r\n",ool_read(PMU_REG_PTC1_CTRL));
            break;
        case 2:
            //ool_write(PMU_REG_PTC2_CTRL, PMU_PTC_CNT_RESET );
            //co_delay_100us(1);
            //ool_write(PMU_REG_PTC2_CTRL, 0 );
            //fputc('C',0);
            ool_write(PMU_REG_PTC2_CTRL, PMU_PTC_EN|(reverse_flag<<4)|((!repeat_flag)<<2)|PMU_PTC_OE );
            //co_printf("C:%x\r\n",ool_read(PMU_REG_PTC2_CTRL) );
            break;
    }
}
void pmu_pwm_stop(enum system_port_t port, enum system_port_bit_t bit)
{
    switch( (port * 8 + bit)%4 )
    {
        case 0:
            ool_write(PMU_REG_PTC0_CTRL, PMU_PTC_CNT_RESET );
            break;
        case 1:
            ool_write(PMU_REG_PTC1_CTRL, PMU_PTC_CNT_RESET );
            break;
        case 2:
            ool_write(PMU_REG_PTC2_CTRL, PMU_PTC_CNT_RESET );
            break;
    }
}

void pmu_pwm_set_param(enum system_port_t port, enum system_port_bit_t bit,uint16_t high_count,uint16_t low_count)
{
    switch( (port * 8 + bit)%4 )
    {
        case 0:
            ool_write16(PMU_REG_PTC0_HRC_0,low_count);
            ool_write16(PMU_REG_PTC0_LRC_0,high_count);
            //co_printf("a:%x,%x\r\n",ool_read16(PMU_REG_PTC0_HRC_0),ool_read16(PMU_REG_PTC0_LRC_0) );
            break;
        case 1:
            ool_write16(PMU_REG_PTC1_HRC_0,low_count);
            ool_write16(PMU_REG_PTC1_LRC_0,high_count);
            //co_printf("b:%x,%x\r\n",ool_read16(PMU_REG_PTC1_HRC_0),ool_read16(PMU_REG_PTC1_LRC_0) );
            break;
        case 2:
            ool_write16(PMU_REG_PTC2_HRC_0,low_count);
            ool_write16(PMU_REG_PTC2_LRC_0,high_count);
            //co_printf("c:%x,%x\r\n",ool_read16(PMU_REG_PTC2_HRC_0),ool_read16(PMU_REG_PTC2_LRC_0) );
            break;
    }
}

void pmu_pwm_init(void)
{
    ool_write(PMU_REG_RST_CTRL, ool_read(PMU_REG_RST_CTRL) | PMU_RST_PWM);
    ool_write(PMU_REG_CLK_CTRL, ool_read(PMU_REG_CLK_CTRL) | PMU_PTC_CLK_EN);
}

