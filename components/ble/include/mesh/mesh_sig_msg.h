/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef MESH_SIG_MSG_H
#define MESH_SIG_MSG_H

/*
 * INCLUDES (包含头文件)
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>


/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */
/*
 * TYPEDEFS (类型定义)
 */

#define GEN_ONOFF_SET_UNACK_MSG     0x00000382 
// If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
typedef struct
{
    uint8_t     onoff;      // The target value of the Generic OnOff state
    uint8_t     tid;        // Transaction Identifier
    uint8_t     ttl;        // Transition Time (Optional)
    uint8_t     delay;      // Message execution delay in 5 milliseconds steps
}mesh_gen_onoff_set_unack_t;


/*
 * GLOBAL VARIABLES (全局变量)
 */

/*
 * LOCAL VARIABLES (本地变量)
 */


/*
 * PUBLIC FUNCTIONS (全局函数)
 */





#endif







