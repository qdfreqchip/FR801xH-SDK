#include <stdint.h>

#include "driver_efuse.h"
#include "driver_plf.h"

#include "co_math.h"

/*
 * TYPEDEFS (类型定义)
 */
struct efuse_reg_t {    
	uint32_t ctrl;    
	uint32_t d0;    
	uint32_t d1;    
	uint32_t d2;    
	uint32_t len_reg;
};

void efuse_write(uint32_t data0,uint32_t data1,uint32_t data2)
{
    volatile struct efuse_reg_t * const efuse_reg = (volatile struct efuse_reg_t *)(EFUSE_BASE);
    
    //ool_write(0x40,0x40);//读写efuse需将aldo电压降低到2.5V  
    efuse_reg->len_reg = 0x2814; 
    efuse_reg->d0 = data0;
    efuse_reg->d1 = data1;
    efuse_reg->d2 = data2;   
	
	efuse_reg->ctrl = 0x05;            
	while(((efuse_reg->ctrl)&CO_BIT(2)));            
	while(!((efuse_reg->ctrl)&CO_BIT(0)));    
    //efuse_reg->d0 = data0;
    //efuse_reg->d1 = data1;
    //efuse_reg->d2 = data2;   
}

void efuse_read(uint32_t *data0,uint32_t *data1,uint32_t *data2)
{
    volatile struct efuse_reg_t * const efuse_reg = (volatile struct efuse_reg_t *)(EFUSE_BASE);
    
	efuse_reg->ctrl = 0x03;
	while(((efuse_reg->ctrl)&CO_BIT(1)));            
	while(!((efuse_reg->ctrl)&CO_BIT(0)));    
	efuse_reg->len_reg = 0x2814; 
	*data0 = efuse_reg->d0;    
	*data1 = efuse_reg->d1;    
	*data2 = efuse_reg->d2;
}

