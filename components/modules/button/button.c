#include <stdint.h>

#include "os_task.h"
#include "os_msg_q.h"
#include "os_timer.h"
#include "user_task.h"
#include "button.h"

#include "driver_pmu.h"

#define BUTTON_IDX_MAX              1

#define BUTTON_SHORT_DURING             0x08        // x10ms
#define BUTTON_LONG_DURING              0x14        // x100ms
#define BUTTON_LONG_LONG_DURING         0x28        // x100ms
#define BUTTON_MULTI_INTERVAL           0x14        // x10ms
#define BUTTON_LONG_PRESSING_INTERVAL   0x1e        // x10ms

uint8_t current_state = BUTTON_WORKING_STATE_IDLE;
uint16_t button_task_id;
os_timer_t button_anti_shake_timer;
os_timer_t button_pressing_timer;
os_timer_t button_state_timer;

/* which io is enabled for button function */
uint32_t button_io_mask = 0;
uint32_t curr_button_before_anti_shake = 0;
uint32_t current_pressed_button = 0;
uint32_t last_saved_button = 0;
uint32_t button_to_be_send = 0;     //for multi click
uint8_t pressed_cnt = 0;           //for multi click

void button_toggle_detected(uint32_t curr_button)
{
    if(button_io_mask != 0) {
        curr_button_before_anti_shake = curr_button & button_io_mask;
        os_timer_start(&button_anti_shake_timer, 10, false);
    }
}

void button_int_isr(uint32_t changed_button)
{
    uint32_t curr_button;
    os_event_t toggle_event;

    curr_button = current_pressed_button ^ changed_button;

    toggle_event.event_id = BUTTON_TOGGLE;
    toggle_event.param = (void *)&curr_button;
    toggle_event.param_len = sizeof(uint32_t);
    os_msg_post(button_task_id, &toggle_event);
}

void button_send_event(uint8_t event, uint32_t button, uint8_t cnt)
{
    os_event_t button_event;
    struct button_msg_t msg;

    msg.button_index = button;
    msg.button_type = event;
    msg.button_cnt = cnt;

    button_event.event_id = USER_EVT_BUTTON;
    button_event.src_task_id = button_task_id;
    button_event.param = (void *)&msg;
    button_event.param_len = sizeof(msg);

    os_msg_post(user_task_id, &button_event);

    pressed_cnt = 0;
}

static void button_idle(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_SINGLE_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
    }
}

static void button_just_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        current_state = BUTTON_WORKING_STATE_IDLE;
        os_timer_stop(&button_state_timer);
        button_send_event(BUTTON_RELEASED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_TIME_OUT)
    {
        current_state = BUTTON_WORKING_STATE_PRESSED;
        os_timer_start(&button_state_timer, (BUTTON_LONG_DURING*10-BUTTON_SHORT_DURING)*10, false);
    }
}

static void button_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        if(0/*__jump_table.button_disable_multi_click*/ & last_saved_button)
        {
            current_state = BUTTON_WORKING_STATE_IDLE;
            button_send_event(BUTTON_SHORT_PRESSED, last_saved_button, 0);
        }
        else
        {
            //TBD，是否开启多击按键
            current_state = BUTTON_WORKING_STATE_WAIT_MULTI;
            button_to_be_send = last_saved_button;
            pressed_cnt++;
            os_timer_start(&button_state_timer, BUTTON_MULTI_INTERVAL*10, false);
        }
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_TIME_OUT)
    {
        current_state = BUTTON_WORKING_STATE_LONG_PRESSED;
        os_timer_start(&button_state_timer, ((BUTTON_LONG_LONG_DURING-BUTTON_LONG_DURING)*10)*10, false);
        os_timer_start(&button_pressing_timer, BUTTON_LONG_PRESSING_INTERVAL*10, false);
        button_send_event(BUTTON_LONG_PRESSED, current_pressed_button, pressed_cnt);
    }
}

static void button_wait_multi(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_SINGLE_PRESSED)
    {
        if(current_pressed_button != button_to_be_send)
        {
            if(pressed_cnt > 1)
            {
                button_send_event(BUTTON_MULTI_PRESSED, button_to_be_send, pressed_cnt);
            }
            else
            {
                button_send_event(BUTTON_SHORT_PRESSED, button_to_be_send, pressed_cnt);
            }
            button_send_event(BUTTON_PRESSED, current_pressed_button, pressed_cnt);
        }
        current_state = BUTTON_WORKING_STATE_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
        button_send_event(BUTTON_SHORT_PRESSED, button_to_be_send, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_TIME_OUT)
    {
        current_state = BUTTON_WORKING_STATE_IDLE;
        if(pressed_cnt > 1)
        {
            button_send_event(BUTTON_MULTI_PRESSED, button_to_be_send, pressed_cnt);
        }
        else
        {
            button_send_event(BUTTON_SHORT_PRESSED, button_to_be_send, pressed_cnt);
        }
    }
}

static void button_long_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        current_state = BUTTON_WORKING_STATE_IDLE;
        os_timer_stop(&button_state_timer);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_LONG_RELEASED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
        button_send_event(BUTTON_LONG_RELEASED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_TIME_OUT)
    {
        current_state = BUTTON_WORKING_STATE_LONG_LONG_PRESSED;
        button_send_event(BUTTON_LONG_LONG_PRESSED, current_pressed_button, pressed_cnt);
    }
}

static void button_long_long_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        os_timer_stop(&button_pressing_timer);
        current_state = BUTTON_WORKING_STATE_IDLE;
        button_send_event(BUTTON_LONG_LONG_RELEASED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
        button_send_event(BUTTON_LONG_LONG_RELEASED, last_saved_button, pressed_cnt);
    }
}

static void button_comb_just_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        current_state = BUTTON_WORKING_STATE_IDLE;
        os_timer_stop(&button_state_timer);
        button_send_event(BUTTON_COMB_RELEASED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_SINGLE_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_RELEASED, last_saved_button, pressed_cnt);
        button_send_event(BUTTON_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_TIME_OUT)
    {
        current_state = BUTTON_WORKING_STATE_COMB_PRESSED;
        os_timer_start(&button_state_timer, (BUTTON_LONG_DURING*10-BUTTON_SHORT_DURING)*10, false);
    }
}

static void button_comb_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        current_state = BUTTON_WORKING_STATE_IDLE;
        os_timer_stop(&button_state_timer);
        button_send_event(BUTTON_COMB_SHORT_PRESSED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_SINGLE_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_SHORT_PRESSED, last_saved_button, pressed_cnt);
        button_send_event(BUTTON_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        button_send_event(BUTTON_COMB_SHORT_PRESSED, last_saved_button, pressed_cnt);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_TIME_OUT)
    {
        current_state = BUTTON_WORKING_STATE_COMB_LONG_PRESSED;
        os_timer_start(&button_state_timer, ((BUTTON_LONG_LONG_DURING-BUTTON_LONG_DURING)*10)*10, false);
        os_timer_start(&button_pressing_timer, BUTTON_LONG_PRESSING_INTERVAL*10, false);
        button_send_event(BUTTON_COMB_LONG_PRESSED, current_pressed_button, pressed_cnt);
    }
}

static void button_comb_long_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        current_state = BUTTON_WORKING_STATE_IDLE;
        os_timer_stop(&button_state_timer);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_LONG_RELEASED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_SINGLE_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_LONG_RELEASED, last_saved_button, pressed_cnt);
        button_send_event(BUTTON_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_LONG_RELEASED, last_saved_button, pressed_cnt);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_TIME_OUT)
    {
        current_state = BUTTON_WORKING_STATE_COMB_LONG_LONG_PRESSED;
        button_send_event(BUTTON_COMB_LONG_LONG_PRESSED, current_pressed_button, pressed_cnt);
    }
}

static void button_comb_long_long_pressed(uint8_t event)
{
    if(event == BUTTON_WORKING_EVENT_RELEASED)
    {
        current_state = BUTTON_WORKING_STATE_IDLE;
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_LONG_LONG_RELEASED, last_saved_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_SINGLE_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_LONG_LONG_RELEASED, last_saved_button, pressed_cnt);
        button_send_event(BUTTON_PRESSED, current_pressed_button, pressed_cnt);
    }
    else if(event == BUTTON_WORKING_EVENT_COMB_PRESSED)
    {
        current_state = BUTTON_WORKING_STATE_COMB_JUST_PRESSED;
        os_timer_start(&button_state_timer, BUTTON_SHORT_DURING*10, false);
        os_timer_stop(&button_pressing_timer);
        button_send_event(BUTTON_COMB_LONG_LONG_RELEASED, last_saved_button, pressed_cnt);
        button_send_event(BUTTON_COMB_PRESSED, current_pressed_button, pressed_cnt);
    }
}

void (*const button_statemachines[BUTTON_WORKING_STATE_MAX])(uint8_t) =
{
    button_idle,
    button_just_pressed,
    button_pressed,
    button_wait_multi,
    button_long_pressed,
    button_long_long_pressed,
    button_comb_just_pressed,
    button_comb_pressed,
    button_comb_long_pressed,
    button_comb_long_long_pressed,
};

//one or more button is released or pressed
static int button_toggle_handler(uint32_t curr_button)
{
    enum button_working_event_t event;

    current_pressed_button = curr_button;

    if(last_saved_button != current_pressed_button)
    {
        if(current_pressed_button == 0)
        {
            event = BUTTON_WORKING_EVENT_RELEASED;
        }
        else
        {
            if((current_pressed_button & (current_pressed_button-1)) == 0)
            {
                event = BUTTON_WORKING_EVENT_SINGLE_PRESSED;
            }
            else
            {
                event = BUTTON_WORKING_EVENT_COMB_PRESSED;
            }
        }

        button_statemachines[current_state](event);

        last_saved_button = current_pressed_button;
    }

    return EVT_CONSUMED;
}

static void button_timeout_handler(void *param)
{
    button_statemachines[current_state](BUTTON_WORKING_EVENT_TIME_OUT);
}

static void button_pressing_timeout_handler(void *param)
{
    enum button_type_t event;
    if((current_pressed_button & (current_pressed_button - 1)) == 0)
    {
        event = BUTTON_LONG_PRESSING;
    }
    else
    {
        event = BUTTON_COMB_LONG_PRESSING;
    }

    button_send_event(event, current_pressed_button, pressed_cnt);

    os_timer_start(&button_pressing_timer, BUTTON_LONG_PRESSING_INTERVAL*10, false);
}

static void button_anti_shake_timeout_handler(void *param)
{
    uint32_t curr_button;
    os_event_t toggle_event;

    curr_button = ool_read32(PMU_REG_GPIOA_V);
    curr_button &= button_io_mask;

    if(curr_button == curr_button_before_anti_shake)
    {
        curr_button ^= button_io_mask;
        toggle_event.event_id = BUTTON_TOGGLE;
        toggle_event.param = (void *)&curr_button;
        toggle_event.param_len = sizeof(uint32_t);
        os_msg_post(button_task_id, &toggle_event);
    }
}

static int button_task_func(os_event_t *event)
{
    switch(event->event_id)
    {
        case BUTTON_TOGGLE:
            button_toggle_handler(*(uint32_t *)event->param);
            break;
    }

    return EVT_CONSUMED;
}

void button_init(uint32_t enable_io)
{
    button_io_mask = enable_io;
    
    button_task_id = os_task_create(button_task_func);
    os_timer_init(&button_anti_shake_timer, button_anti_shake_timeout_handler, NULL);
    os_timer_init(&button_pressing_timer, button_pressing_timeout_handler, NULL);
    os_timer_init(&button_state_timer, button_timeout_handler, NULL);
}

