/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

extern uint32_t system_get_pclk(void);

#define configUSE_PREEMPTION		        1
#define configUSE_IDLE_HOOK			        0
#define configUSE_TICK_HOOK			        0
#define configCPU_CLOCK_HZ			        ( system_get_pclk() )	
#define configTICK_RATE_HZ			        ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		        ( 5 )
#define configMINIMAL_STACK_SIZE	        ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE		        ( ( size_t ) ( 17 * 1024 ) )
#define configMAX_TASK_NAME_LEN		        ( 16 )
#define configUSE_TRACE_FACILITY	        0
#define configUSE_16_BIT_TICKS		        0
#define configIDLE_SHOULD_YIELD		        1

//TBD, to be removed
//#define configASSERT(x)                     while((uint32_t)(x) == 0)

/* Timer definitions */
#define configUSE_TIMERS                    1
#define configTIMER_TASK_PRIORITY           ( 2 )
#define configTIMER_QUEUE_LENGTH            ( 10 )
#define configTIMER_TASK_STACK_DEPTH        ( 256 )

/* mutexes definitions */
#define configUSE_MUTEXES                   1
#define configUSE_RECURSIVE_MUTEXES         1

/* semaphores definitions */
#define configUSE_COUNTING_SEMAPHORES       1

/* sleep mode definitions */
#define configUSE_TICKLESS_IDLE             1
// #define portSUPPRESS_TICKS_AND_SLEEP(x)
//#define configPRE_SLEEP_PROCESSING(x)       (x = 0)
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   10

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		        0
#define configMAX_CO_ROUTINE_PRIORITIES     ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		        1
#define INCLUDE_uxTaskPriorityGet		        1
#define INCLUDE_vTaskDelete				        1
#define INCLUDE_vTaskCleanUpResources	        0
#define INCLUDE_vTaskSuspend			        1
#define INCLUDE_vTaskDelayUntil			        1
#define INCLUDE_vTaskDelay				        1
#define INCLUDE_xTaskGetCurrentTaskHandle       1

/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY 		0xFF
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	0x20 /* equivalent to 0xb0, or priority 11. */

#define xPortPendSVHandler          PendSV_Handler
#define xPortSysTickHandler         SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

