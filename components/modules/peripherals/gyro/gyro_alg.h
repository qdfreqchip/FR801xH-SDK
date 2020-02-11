//***************
#ifndef _GYRO_ALG_H_
#define _GYRO_ALG_H_

#include <stdint.h>

#define USER_SKIP_MODE
#define GYRO_IIC_CHL		IIC_CHANNEL_1
#define GYRO_IIC_CLK_PIN	24 // PC6
#define GYRO_IIC_DAT_PIN	25 // PC7
#define GYRO_ADDRESS 		0xDA

#define FOR_GYRO_DRIVER

void gyroscope_init(void);
uint16_t get_skip_num(void);
void clear_sport_num(void);
// gyroscope alg 10ms loop
void gyroscope_loop(void);

// need to write the function entity
void gyroscope_i2c_init(uint8_t chl);
uint32_t get_dt(void); // get the time difference between calls before and after
void delay_ms(uint32_t delayTime);
void I2C_Read_NBytes(uint8_t deviceAddr,uint8_t regAddr,uint8_t readLen,uint8_t *readBuf);
void I2C_Write_NBytes(uint8_t deviceAddr,uint8_t regAddr,uint8_t writeLen,uint8_t *writeBuf);
void I2C_Write_NBytes_imp(uint8_t deviceAddr,uint8_t regAddr,uint8_t writeLen,uint8_t *writeBuf);
// need to write the function entity


void gyro_dev_init(void);

#endif


