/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

/*
 * INCLUDES
 */
 
#ifndef _USER_TASK_H
#define _USER_TASK_H

enum user_event_t {
    USER_EVT_AT_COMMAND,
    USER_EVT_BUTTON,
};

enum user_audio_event_t {
    USER_EVT_MIC,			//Mic
    DECODER_EVENT_PREPARE,
    DECODER_EVENT_NEXT_FRAME,
    DECODER_EVENT_STOP,
    DECODER_EVENT_WAIT_END_TO,
};


extern uint16_t user_task_id;
extern uint16_t audio_task_id;


void user_task_init(void);





#endif  // _USER_TASK_H

