/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */
#ifndef _DRIVER_KEYSCAN_H
#define _DRIVER_KEYSCAN_H

#include <stdint.h>

typedef struct
{
    uint8_t row_en;         // pin selection for row, each bits[7:0] map to PD[7:0]
    uint32_t col_en;        // pin selection for column, {PC[3:0], PB[7:0], PA[7:0]}
                            // PC3 and PA0 should not be used at the same time.
    uint8_t row_map_sel;    // reserved
}keyscan_param_t;
/*********************************************************************
 * @fn      keyscan_init
 *
 * @brief   initial and start keyscan module with row and column selection
 *          indicated in parameter.
 *
 * @param   param       - row and column selection
 *
 * @return  None.
 */
void keyscan_init(keyscan_param_t *param);

#endif  // _DRIVER_KEYSCAN_H

