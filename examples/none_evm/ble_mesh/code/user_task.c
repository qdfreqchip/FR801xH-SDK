#include <stdint.h>

#include "os_task.h"
#include "os_msg_q.h"

#include "co_printf.h"
#include "user_task.h"
#include "app_at.h"
#include "button.h"

uint16_t user_task_id;

//void app_mesh_dev_reset_ctrl(void);
//void app_auto_update_led_state(uint8_t state);

static int user_task_func(os_event_t *param)
{
    switch(param->event_id)
    {
        case USER_EVT_AT_COMMAND:
            app_at_cmd_recv_handler(param->param, param->param_len);
            break;
        case USER_EVT_BUTTON:
            {
                struct button_msg_t *button_msg;
                const char *button_type_str[] = {
                                                    "BUTTON_PRESSED",
                                                    "BUTTON_RELEASED",
                                                    "BUTTON_SHORT_PRESSED",
                                                    "BUTTON_MULTI_PRESSED",
                                                    "BUTTON_LONG_PRESSED",
                                                    "BUTTON_LONG_PRESSING",
                                                    "BUTTON_LONG_RELEASED",
                                                    "BUTTON_LONG_LONG_PRESSED",
                                                    "BUTTON_LONG_LONG_RELEASED",
                                                    "BUTTON_COMB_PRESSED",
                                                    "BUTTON_COMB_RELEASED",
                                                    "BUTTON_COMB_SHORT_PRESSED",
                                                    "BUTTON_COMB_LONG_PRESSED",
                                                    "BUTTON_COMB_LONG_PRESSING",
                                                    "BUTTON_COMB_LONG_RELEASED",
                                                    "BUTTON_COMB_LONG_LONG_PRESSED",
                                                    "BUTTON_COMB_LONG_LONG_RELEASED",
                                                };

                button_msg = (struct button_msg_t *)param->param;
                
                co_printf("KEY 0x%08x, TYPE %s.\r\n", button_msg->button_index, button_type_str[button_msg->button_type]);
#if 0
                if(button_msg->button_type == BUTTON_SHORT_PRESSED )
                {
                    static uint8_t led_state = 0;
                    led_state = 1-led_state;
                    app_auto_update_led_state(led_state);
                }
#endif                
            }
            break;
    }

    return EVT_CONSUMED;
}

void user_task_init(void)
{
    user_task_id = os_task_create(user_task_func);
}

