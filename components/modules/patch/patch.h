#ifndef _PATCH_H
#define _PATCH_H

#include <stdint.h>
#include "rwip.h"

void patch_init(void);
void patch_add_element(void);

uint32_t svc_exception_handler_ram(uint32_t stack);
void low_power_save_entry_imp(uint8_t type);
void low_power_restore_entry_imp(uint8_t type);
void user_entry_before_sleep_imp(void);
void user_entry_after_sleep_imp(void);
void rf_init_app(struct rwip_rf_api *api);

#endif  //_PATCH_H

