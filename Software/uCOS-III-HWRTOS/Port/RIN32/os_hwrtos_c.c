/*
*********************************************************************************************************
*                                           uC/OS-III HWOS
*
*                         (c) Copyright 2004-2014; Micrium, Inc.; Weston, FL
*
*                  All rights reserved.  Protected by international copyright laws.
*
*                  uC/OS-III-HWOS is provided in source form to registered licensees ONLY.  It is
*                  illegal to distribute this source code to any third party unless you receive
*                  written permission by an authorized Micrium representative.  Knowledge of
*                  the source code may NOT be used to develop a similar product.
*
*                  Please help us continue to provide the Embedded community with the finest
*                  software available.  Your honesty is greatly appreciated.
*
*                  You can find our product's user manual, API reference, release notes and
*                  more information at: https://doc.micrium.com
*
*                  You can contact us at: www.micrium.com
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                              HWOS PORT
*
* Filename      : os_hwos_c.c
* Version       : V1.00.00.00
* Programmer(s) : JBL
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  "../../Source/os.h"


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       OS CPU INITIALIZATION
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OS_CPU_Init (void)
{
    CPU_IntSrcEn(92u);
    CPU_IntSrcPrioSet(92u, 14u);
    CPU_IntSrcPrioSet(14u, 15u);
}


/*
*********************************************************************************************************
*                                       INITIALIZE A TASK'S STACK
*
* Description: This function is called by OS_Task_Create() or OSTaskCreateExt() to initialize the stack
*              frame of the task being created. This function is highly processor specific.
*
* Arguments  : p_task       Pointer to the task entry point address.
*
*              p_arg        Pointer to a user supplied data area that will be passed to the task
*                               when the task first executes.
*
*              p_stk_base   Pointer to the base address of the stack.
*
*              stk_size     Size of the stack, in number of CPU_STK elements.
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : 1) Interrupts are enabled when task starts executing.
*
*              2) All tasks run in SVC mode.
*********************************************************************************************************
*/

CPU_STK  *OS_CPU_TaskInit (OS_TASK_PTR    p_task,
                           void          *p_arg,
                           CPU_STK       *p_stk_base,
                           CPU_STK_SIZE   stk_size)
{
    CPU_STK       *p_stk;

                                                                /* -------------------- STACK INIT -------------------- */
    p_stk = &p_stk_base[stk_size];                              /* Load stack pointer.                                  */
                                                                /* Registers stacked as if auto-saved on exception.     */
    *--p_stk = (CPU_STK)0x01000000u;                            /* xPSR                                                 */
    *--p_stk = (CPU_STK)p_task | 0x01;                          /* Entry Point                                          */
    *--p_stk = (CPU_STK)0u;                                     /* R14 (LR)                                             */
    *--p_stk = (CPU_STK)0x12121212u;                            /* R12                                                  */
    *--p_stk = (CPU_STK)0x03030303u;                            /* R3                                                   */
    *--p_stk = (CPU_STK)0x02020202u;                            /* R2                                                   */
    *--p_stk = (CPU_STK)0u;                                     /* R1                                                   */
    *--p_stk = (CPU_STK)p_arg;                                  /* R0 : argument                                        */
                                                                /* Remaining registers saved on process stack.          */
    *--p_stk = (CPU_STK)0x11111111u;                            /* R11                                                  */
    *--p_stk = (CPU_STK)0x10101010u;                            /* R10                                                  */
    *--p_stk = (CPU_STK)0x09090909u;                            /* R9                                                   */
    *--p_stk = (CPU_STK)0x08080808u;                            /* R8                                                   */
    *--p_stk = (CPU_STK)0x07070707u;                            /* R7                                                   */
    *--p_stk = (CPU_STK)0x06060606u;                            /* R6                                                   */
    *--p_stk = (CPU_STK)0x05050505u;                            /* R5                                                   */
    *--p_stk = (CPU_STK)0x04040404u;                            /* R4                                                   */

    return (p_stk);
}


/*
*********************************************************************************************************
*                                         INITIALIZE SYS TICK
*
* Description: Initialize the SysTick.
*
* Arguments  : cnts         Number of SysTick counts between two OS tick interrupts.
*
* Note(s)    : 1) This function MUST be called after OSStart() & after processor initialization.
*********************************************************************************************************
*/

void  OS_CPU_SysTickInit (CPU_INT32U  cnts)
{
    RIN32_HWOS->CMN.WT_TO_PRESCL = cnts;
}
