#ifndef _APP_AT_H
#define _APP_AT_H

#include <stdint.h>

void app_at_cmd_recv_handler(uint8_t *data, uint16_t length);

void app_at_init(void);

#endif //_APP_AT_H

