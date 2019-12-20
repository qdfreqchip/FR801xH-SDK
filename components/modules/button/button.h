#ifndef _BUTTON_H
#define _BUTTON_H

#include <stdint.h>

enum button_event_t {
    BUTTON_TOGGLE,
    BUTTON_PRESSED_EVENT,
    BUTTON_TIMER_TO_TIMER,
    BUTTON_PRESSING_TO_TIMER,
    BUTTON_ANTI_SHAKE_TO_TIMER,
};

enum button_type_t {
    BUTTON_PRESSED,
    BUTTON_RELEASED,
    BUTTON_SHORT_PRESSED,
    BUTTON_MULTI_PRESSED,
    BUTTON_LONG_PRESSED,
    BUTTON_LONG_PRESSING,
    BUTTON_LONG_RELEASED,
    BUTTON_LONG_LONG_PRESSED,
    BUTTON_LONG_LONG_RELEASED,
    BUTTON_COMB_PRESSED,
    BUTTON_COMB_RELEASED,
    BUTTON_COMB_SHORT_PRESSED,
    BUTTON_COMB_LONG_PRESSED,
    BUTTON_COMB_LONG_PRESSING,
    BUTTON_COMB_LONG_RELEASED,
    BUTTON_COMB_LONG_LONG_PRESSED,
    BUTTON_COMB_LONG_LONG_RELEASED,
};

enum button_working_state_t {
    BUTTON_WORKING_STATE_IDLE,
    BUTTON_WORKING_STATE_JUST_PRESSED,
    BUTTON_WORKING_STATE_PRESSED,
    BUTTON_WORKING_STATE_WAIT_MULTI,
    BUTTON_WORKING_STATE_LONG_PRESSED,
    BUTTON_WORKING_STATE_LONG_LONG_PRESSED,
    BUTTON_WORKING_STATE_COMB_JUST_PRESSED,
    BUTTON_WORKING_STATE_COMB_PRESSED,
    BUTTON_WORKING_STATE_COMB_LONG_PRESSED,
    BUTTON_WORKING_STATE_COMB_LONG_LONG_PRESSED,
    BUTTON_WORKING_STATE_MAX,
};

enum button_working_event_t {
    BUTTON_WORKING_EVENT_RELEASED,
    BUTTON_WORKING_EVENT_SINGLE_PRESSED,
    BUTTON_WORKING_EVENT_COMB_PRESSED,
    BUTTON_WORKING_EVENT_TIME_OUT,
};

struct button_toggle_param_t {
    uint32_t curr_button;
    uint32_t timestamp;
};

struct button_msg_t {
    uint32_t button_index;
    uint8_t button_type;
    uint8_t button_cnt; //only for multi click
};

void button_toggle_detected(uint32_t curr_button);

void button_int_isr(uint32_t changed_button);

void button_init(uint32_t enable_io);

#endif  //_BUTTON_H

