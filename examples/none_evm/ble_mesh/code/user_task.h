#ifndef _USER_TASK_H
#define _USER_TASK_H

enum user_event_t {
    USER_EVT_AT_COMMAND,
    USER_EVT_BUTTON,
};

extern uint16_t user_task_id;

void user_task_init(void);

#endif  // _USER_TASK_H

