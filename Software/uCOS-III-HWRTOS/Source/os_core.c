/*
*********************************************************************************************************
*                                           uC/OS-III HW-RTOS
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
*                                                CORE
*
* Filename      : os_core.c
* Version       : V1.00.00
* Programmer(s) : JBL
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  "os.h"


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           INITIALIZATION
*
* Description: This function is used to initialize the internals of uC/OS-III HW-RTOS and MUST be called
*              prior to creating any uC/OS-III-HWOS object and, prior to calling OS_Start().
*
* Arguments  : p_err   Pointer to a variable that will contain an error code returned by this function.
*
                       OS_ERR_NONE    Initialization was successful
*                      Other          Other OS_ERR_xxx depending on the sub-functions called by OSInit().
* Returns    : none
*********************************************************************************************************
*/

void  OSInit (OS_ERR  *p_err)
{
    OS_ERR      os_err;
    CPU_INT32U  i;


    OS_CPU_Init();

    OSCtxIDCur = 64u;

    for (i = 0; i < 64; i++) {
        RIN32_HWOS->CNTX[i].CNTX_TYPE = 0x03;
        RIN32_HWOS->CNTX[i].CNTX_STAT = 0x01;
        RIN32_HWOS->CNTX[i].INIT_ADD = 0u;
        RIN32_HWOS->CNTX[i].PRTY = (0) | (0 << 8u);
        RIN32_HWOS->CNTX[i].INIT_R14 = (CPU_INT32U)0u;
    }

    for (i = 0; i < 128u; i++) {
        RIN32_HWOS->SEM_TBL[i] = 0x00000000;
    }
    for (i = 0; i < 32u; i++) {
        RIN32_HWOS->FLG_TBL[i] = 0x00000000;
    }
    for (i = 0; i < 64u; i++) {
        RIN32_HWOS->MBX_TBL[i] = 0x00000000;
    }
    RIN32_HWOS->HWISR.QINT = 0x00000000;

    RIN32_HWOS->HWISR.HWISR_PNTR[0] = 0x03020100;
    RIN32_HWOS->HWISR.HWISR_PNTR[1] = 0x07060504;
    RIN32_HWOS->HWISR.HWISR_PNTR[2] = 0x0b0a0908;
    RIN32_HWOS->HWISR.HWISR_PNTR[3] = 0x0f0e0d0c;
    RIN32_HWOS->HWISR.HWISR_PNTR[4] = 0x13121110;
    RIN32_HWOS->HWISR.HWISR_PNTR[5] = 0x17161514;
    RIN32_HWOS->HWISR.HWISR_PNTR[6] = 0x1b1a1918;
    RIN32_HWOS->HWISR.HWISR_PNTR[7] = 0x1f1e1d1c;

    OS_IdleTaskInit(&os_err);

    OS_CPU_ExceptStkBase = &OS_CPU_ExceptStk[OS_CFG_ISR_STK_SIZE];

    *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                         START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets uC/OS-III HW-RTOS manages the
*              task that you created.  Before you can call OSStart(), you MUST have called OSInit()
*              and you MUST have created at least one application task.
*
* Argument(s): p_err      Pointer to a variable that will contain an error code returned by this function.
*
*                         OS_ERR_FATAL_RETURN    OS was running and OSStart() returned.
*                         OS_ERR_OS_RUNNING      OS is already running, OSStart() has no effect
*
* Returns    : none
*********************************************************************************************************
*/

void  OSStart (OS_ERR  *p_err)
{
    CPU_SR_ALLOC();


    if (OSRunning == 1u) {
       *p_err = OS_ERR_OS_RUNNING;
        return;
    }

    CPU_CRITICAL_ENTER();
    OSRunning = 1u;
    CPU_CRITICAL_EXIT();

    OS_CPU_Start();

    RIN32_HWOS->CPUIF.CMD = 0x8004;

   *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                      INITIALIZE THE IDLE TASK
*
* Description: This function initializes the idle task
*
* Arguments  : p_err    Pointer to a variable that will contain an error code returned
*                       by this function.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III HW-RTOS and your application MUST NOT call it.
*********************************************************************************************************
*/

void  OS_IdleTaskInit (OS_ERR  *p_err)
{
    OS_ERR  os_err;


    OSTaskCreate(&OSIdleTaskTCB,
                 "Idle Task",
                  OS_IdleTask,
                  DEF_NULL,
                  15u,
                 &OSCfg_IdleTaskStk[0],
                  OS_CFG_IDLE_TASK_STK_SIZE / 10,
                  OS_CFG_IDLE_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                  0u,
                 &os_err);

    *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                              IDLE TASK
*
* Description: This task is internal to uC/OS-III-HWOS and executes whenever no other higher priority
*              tasks executes because they are ALL waiting for event(s) to occur.
*
* Arguments  : p_arg    Argument passed to the task when the task is created.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS HW-RTOS and your application MUST NOT call it.
*********************************************************************************************************
*/

void  OS_IdleTask (void  *p_arg)
{
    while (DEF_ON) {

    }
}
