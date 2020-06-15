/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
 
#ifndef PROJ_ADV_TEST_H
#define PROJ_ADV_TEST_H


 /*
 * INCLUDES (包含头文件)
 */

/*
 * MACROS (宏定义)
 */
#define TEST_ADV_MODE_UNDIRECT   (0)
#define TEST_ADV_MODE_DIRECT     (0)
#define TEST_ADV_MODE_HDC_DIRECT (0)

#define TEST_ADV_MODE_EXTEND_CONN_UNDIRECT   (0)
#define TEST_ADV_MODE_EXTEND_CONN_DIRECT     (0)
#define TEST_ADV_MODE_EXTEND_NON_CONN_SCAN   (0)
#define TEST_ADV_MODE_EXTEND_CONN_UNDIRECT_LONGRANGE   (0)
#define TEST_ADV_MODE_EXTEND_CONN_DIRECT_LONGRANGE     (0)
#define TEST_ADV_MODE_EXTEND_NON_CONN_SCAN_LONGRANGE   (0)
#define TEST_ADV_MODE_PER_ADV_UNDIRECT       (0)
#define TEST_ADV_MODE_PER_ADV_DIRECT         (0)

#define TEST_ADV_MODE_UNDIRECT_WHITE_LIST         (0)
#define TEST_ADV_MODE_UNDIRECT_DOUBLE_ADV         (1)


#define TEST_BOARD_ADV                  (1)
#define TEST_BOARD_SCAN_OR_CONN         (0)

/*
 * CONSTANTS (常量定义)
 */


/*
 * TYPEDEFS (类型定义)
 */
/*
 * GLOBAL VARIABLES (全局变量)
 */

/*
 * LOCAL VARIABLES (本地变量)
 */

/*
 * LOCAL FUNCTIONS (本地函数)
 */

/*
 * EXTERN FUNCTIONS (外部函数)
 */

/*
 * PUBLIC FUNCTIONS (全局函数)
 */
void start_adv(void);
void start_scan_Or_conn(void);


#endif // end of #ifndef PROJ_ADV_TEST_H

