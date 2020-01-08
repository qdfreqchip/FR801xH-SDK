#ifdef __CAPB18_001_H
#define __CAPB18_001_H
void CAPB18_I2C_init(void);
uint8_t CAPB18_COFF_GET(void);
uint8_t demo_CAPB18_APP(void);

uint8_t CAPB18_data_get(float *temperature,float *air_press);


#endif
