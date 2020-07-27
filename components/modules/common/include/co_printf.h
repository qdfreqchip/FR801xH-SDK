#ifndef _CO_PRINTF_H
#define _CO_PRINTF_H

int co_printf(const char *format, ...);
int co_sprintf(char *out, const char *format, ...);
#define FR_DBG_ON            0x01U
#define FR_DBG_OFF           0x00U
#define FR_LOG(x)   if((x) & FR_DBG_ON) co_printf


#define SEC_CONN_LOG(format,...) do { \
co_printf("[SEC_CONN]:"); \
co_printf(format,##__VA_ARGS__); \
} while(0)

#define SEC_LOG(format,...) do { \
co_printf("[SEC]:"); \
co_printf(format,##__VA_ARGS__); \
} while(0)


#endif
