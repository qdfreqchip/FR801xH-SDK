;/*****************************************************************************
; * @file:    startup_MPS_CM3.s
; * @purpose: CMSIS Cortex-M3 Core Device Startup File 
; *           for the ARM 'Microcontroller Prototyping System' 
; * @version: V1.01
; * @date:    19. Aug. 2009
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * Copyright (C) 2008-2009 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M3 
; * processor based microcontrollers.  This file can be freely distributed 
; * within development tools that are supporting such ARM based processors. 
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; *****************************************************************************/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

                import Reset_Handler
                import NMI_Handler
                ;import HardFault_Handler
                import MemManage_Handler
                import BusFault_Handler
                import UsageFault_Handler
                ;import SVC_Handler
                import DebugMon_Handler
                import PendSV_Handler
                import SysTick_Handler
                import rwble_isr_patch
                import timer0_isr_ram
                import timer1_isr_ram					
                import uart0_isr_ram
                import uart1_isr
                ;import pmu_isr_ram
                import i2s_isr_ram
                import exti_isr_ram					
                import pdm_isr



;Stack_Size      EQU     0x280 ;0x00000400 

                AREA    STACK, NOINIT, READWRITE, ALIGN=3

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                
__initial_sp    EQU     0x40004000

                DCD     __initial_sp ;  ?迆3?那??‘o車辰??～?迆MAINo‘那y?迆??那1車?EXMEM℅??aSTACK㏒?米豕os?a??o車㏒???handle thead?∩MSP谷豕?a0x0800xxxx?D米??米(?迄?Y車??∫????∩車D?)
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler_Ram     ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD    	0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler
                    
                DCD     rwble_isr_patch               ; 0
                DCD     timer0_isr_ram                ; 1
                DCD     timer1_isr_ram                ; 2
                DCD     uart0_isr_ram                 ; 3
                DCD     uart1_isr                 ; 4
                DCD     0                         ; 5
                DCD     0                         ; 6
                DCD     0                         ; 7
                DCD     0                         ; 8
                DCD     i2s_isr_ram               ; 9
                DCD     exti_isr_ram              ; 10
                DCD     pmu_isr_ram               ; 11
                DCD     adc_isr                   ; 12
                DCD     pdm_isr				      ; 13
                DCD     0                         ; 14
                DCD     0                         ; 15

                
				AREA    |.text|, CODE, READONLY
				
HardFault_Handler_Ram   PROC
                IMPORT  HardFault_Handler_C
                TST LR, #4			;test bit[2] is 0 ,then exe EQ branch, MSP as sp
                ITE EQ
                MRSEQ R0, MSP
                MRSNE R0, PSP
                B HardFault_Handler_C
                ENDP

				AREA    ram_code, CODE, READONLY	;default as align 4 bytes
pmu_isr_ram   PROC
                IMPORT  pmu_isr_ram_C
                TST LR, #4			;test bit[2] is 0 ,then exe EQ branch, MSP as sp
                ITE EQ
                MRSEQ R0, MSP
                MRSNE R0, PSP
                B pmu_isr_ram_C
                ENDP
	
SVC_Handler     PROC
                IMPORT  prv_call_svc_pc
                IMPORT  vPortSVCHandler
                IMPORT  svc_exception_handler
                ldr     r0, [sp, #0x18]
                ldr     r2, =prv_call_svc_pc
                add     r2, r2, #1
                cmp     r0, r2
                beq     vPortSVCHandler
                ldr	    r1, [sp, #0x14]
                ldr     r2, =svc_exception_handler
                ldr     r2, [r2]
                blx     r2
                str     r0, [sp, #0x18]
                bx      lr
                nop
                ENDP

adc_isr         PROC
                EXPORT  adc_isr           [WEAK]
                B       .
                ENDP
                
				END
					