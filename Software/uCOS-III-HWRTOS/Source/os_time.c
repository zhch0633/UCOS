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
*                                           TIME MANAGEMENT
*
* Filename      : os_time.c
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
*                                        DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the
*              specified number of system ticks expires.  This, of course, directly equates to delaying
*              the current task for some time to expire.  No delay will result if the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : dly       Value in 'clock ticks' that the task will either delay for or, the target
*                        match value of the tick counter (OSTickCtr).  Note that specifying 0 means the
*                        task is not to delay.
*
*                        depending on the option argument, the task will wake up when OSTickCtr reaches:
*
*                        OS_OPT_TIME_DLY      : OSTickCtr + dly
*                        OS_OPT_TIME_TIMEOUT  : OSTickCtr + dly
*                        OS_OPT_TIME_MATCH*   : dly
*                        OS_OPT_TIME_PERIODIC*: OSTCBCurPtr.TickCtrPrev + dly
*
*                        * Not supported by the HWOS. Options will result in an error.
*
*              opt       Specifies whether 'dly' represents absolute or relative time; default option
*                        marked with *** :
*
*                    *** OS_OPT_TIME_DLY        specifies a relative time from the current value of
*                                               OSTickCtr.
*                        OS_OPT_TIME_TIMEOUT    same as OS_OPT_TIME_DLY.
*                        OS_OPT_TIME_MATCH*     indicates that 'dly' specifies the absolute value that
*                                               OSTickCtr must reach before the task will be resumed.
*                        OS_OPT_TIME_PERIODIC*  indicates that 'dly' specifies the periodic value that
*                                               OSTickCtr must reach before the task will be resumed.
*
*                        * Not supported by the HWOS. Options will result in an error.
*
*              p_err     Pointer to a variable that will contain an error code from this call.
*
*                        OS_ERR_NONE            the call was successful and the delay occurred.
*                        OS_ERR_OPT_INVALID     if you specified an invalid option for this function.
*                        OS_ERR_SCHED_LOCKED    can't delay when the scheduler is locked.
*                        OS_ERR_TIME_DLY_ISR    if you called this function from an ISR.
*                        OS_ERR_TIME_ZERO_DLY   if you specified a delay of zero.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSTimeDly (OS_TICK   dly,
                 OS_OPT    opt,
                 OS_ERR   *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_TIME_DLY_ISR;
        return;
    }
#endif

    switch (opt) {
        case OS_OPT_TIME_DLY:
        case OS_OPT_TIME_TIMEOUT:
             if (dly == 0u) {                                   /* 0 means no delay!                                    */
                *p_err = OS_ERR_TIME_ZERO_DLY;
                 return;
             }
             break;

        case OS_OPT_TIME_MATCH:
            break;

        case OS_OPT_TIME_PERIODIC:
            *p_err = OS_ERR_HWOS_UNSUPPORTED;
             return;
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }

    CPU_CRITICAL_ENTER();

    if (opt == OS_OPT_TIME_MATCH) {
        dly = dly - OS_HWOS_GET_SYS_TIM();
        if ((dly > OS_TICK_TH_RDY) || (dly == 0u)) {
            *p_err = OS_ERR_TIME_ZERO_DLY;
             CPU_CRITICAL_EXIT();
             return;
        }
    }

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R5(dly);                                        /* Delay.                                               */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SLP_TSK);

    OS_HWOS_CMD_PROC();
    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0xFF86:                                            /* Time out.                                            */
        case 0xFF8D:                                            /* Wait release wup_tsk.                                */
        case 0xFF90:                                            /* Wait release rel_wai.                                */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }

}


/*
*********************************************************************************************************
*                                    DELAY TASK FOR SPECIFIED TIME
*
* Description: This function is called to delay execution of the currently running task until some time
*              expires.  This call allows you to specify the delay time in HOURS, MINUTES, SECONDS and
*              MILLISECONDS instead of ticks.
*
* Arguments  : hours     Number of hours that the task will be delayed (max. is 999 if the tick rate is
*                        1000 Hz or less otherwise, a higher value would overflow a 32-bit unsigned counter).
*
*              minutes   Number of minutes (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              seconds   Number of seconds (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              milli     Number of milliseconds (max. 999 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              opt       Time delay bit-field options logically OR'd; default options marked with *** :
*
*                    *** OS_OPT_TIME_DLY        specifies a relative time from the current value of OSTickCtr.
*                        OS_OPT_TIME_TIMEOUT    same as OS_OPT_TIME_DLY.
*                        OS_OPT_TIME_MATCH      indicates that the delay specifies the absolute value that OSTickCtr
*                                               must reach before the task will be resumed.
*                        OS_OPT_TIME_PERIODIC   indicates that the delay specifies the periodic value that OSTickCtr
*                                               must reach before the task will be resumed.
*
*                    *** OS_OPT_TIME_HMSM_STRICT            strictly allow only hours        (0...99)
*                                                                               minutes      (0...59)
*                                                                               seconds      (0...59)
*                                                                               milliseconds (0...999)
*                        OS_OPT_TIME_HMSM_NON_STRICT        allow any value of  hours        (0...999)
*                                                                               minutes      (0...9999)
*                                                                               seconds      (0...65535)
*                                                                               milliseconds (0...4294967295)
*
*              p_err     Pointer to a variable that will receive an error code from this call.
*
*                        OS_ERR_NONE                        If the function returns from the desired delay
*                        OS_ERR_OPT_INVALID                 If you specified an invalid option for 'opt'
*                        OS_ERR_SCHED_LOCKED                Can't delay when the scheduler is locked
*                        OS_ERR_TIME_DLY_ISR                If called from an ISR
*                        OS_ERR_TIME_INVALID_HOURS          If you didn't specify a valid value for 'hours'
*                        OS_ERR_TIME_INVALID_MINUTES        If you didn't specify a valid value for 'minutes'
*                        OS_ERR_TIME_INVALID_SECONDS        If you didn't specify a valid value for 'seconds'
*                        OS_ERR_TIME_INVALID_MILLISECONDS   If you didn't specify a valid value for 'milli'
*                        OS_ERR_TIME_ZERO_DLY               If hours, minutes, seconds and milli are all 0
*
* Returns    : none
*
* Note(s)    : 1) The resolution on the milliseconds depends on the tick rate.  For example, you can't do
*                 a 10 mS delay if the ticker interrupts every 100 mS.  In this case, the delay would be set
*                 to 0.  The actual delay is rounded to the nearest tick.
*
*              2) Although this function allows you to delay a task for many, many hours, it's not
*                 recommended to put a task to sleep for that long.
*********************************************************************************************************
*/

#if (OS_CFG_TIME_DLY_HMSM_EN == DEF_ENABLED)
void  OSTimeDlyHMSM (CPU_INT16U   hours,
                     CPU_INT16U   minutes,
                     CPU_INT16U   seconds,
                     CPU_INT32U   milli,
                     OS_OPT       opt,
                     OS_ERR      *p_err)
{
#if OS_CFG_ARG_CHK_EN > 0u
    CPU_BOOLEAN    opt_invalid;
    CPU_BOOLEAN    opt_non_strict;
#endif
    OS_OPT         opt_time;
    OS_RATE_HZ     tick_rate;
    OS_TICK        ticks;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_TIME_DLY_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }
#endif


    opt_time = opt & OS_OPT_TIME_MASK;                          /* Retrieve time options only.                          */
    switch (opt_time) {
        case OS_OPT_TIME_DLY:
        case OS_OPT_TIME_TIMEOUT:
             if ((milli == 0u) && (seconds == 0u) &&            /* Make sure we didn't specify a 0 delay.               */
                 (minutes == 0u) && (hours == 0u)) {
                *p_err = OS_ERR_TIME_ZERO_DLY;
                 return;
             }
             break;

        case OS_OPT_TIME_MATCH:
        case OS_OPT_TIME_PERIODIC:
            *p_err = OS_ERR_HWOS_UNSUPPORTED;
             return;
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* Validate arguments to be within range.               */
    opt_invalid = DEF_BIT_IS_SET_ANY(opt, ~OS_OPT_TIME_OPTS_MASK);
    if (opt_invalid == DEF_YES) {
       *p_err = OS_ERR_OPT_INVALID;
        return;
    }

    opt_non_strict = DEF_BIT_IS_SET(opt, OS_OPT_TIME_HMSM_NON_STRICT);
    if (opt_non_strict != DEF_YES) {
         if (milli   > (CPU_INT32U)999u) {
            *p_err = OS_ERR_TIME_INVALID_MILLISECONDS;
             return;
         }
         if (seconds > (CPU_INT16U)59u) {
            *p_err = OS_ERR_TIME_INVALID_SECONDS;
             return;
         }
         if (minutes > (CPU_INT16U)59u) {
            *p_err = OS_ERR_TIME_INVALID_MINUTES;
             return;
         }
         if (hours   > (CPU_INT16U)99u) {
            *p_err = OS_ERR_TIME_INVALID_HOURS;
             return;
         }
    } else {
         if (minutes > (CPU_INT16U)9999u) {
            *p_err = OS_ERR_TIME_INVALID_MINUTES;
             return;
         }
         if (hours   > (CPU_INT16U)999u) {
            *p_err = OS_ERR_TIME_INVALID_HOURS;
             return;
         }
    }
#endif


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* Validate arguments to be within range.               */
    opt_invalid = DEF_BIT_IS_SET_ANY(opt, ~OS_OPT_TIME_OPTS_MASK);
    if (opt_invalid == DEF_YES) {
       *p_err = OS_ERR_OPT_INVALID;
        return;
    }

    opt_non_strict = DEF_BIT_IS_SET(opt, OS_OPT_TIME_HMSM_NON_STRICT);
    if (opt_non_strict != DEF_YES) {
         if (milli > 999u) {
            *p_err = OS_ERR_TIME_INVALID_MILLISECONDS;
             return;
         }
         if (seconds > 59u) {
            *p_err = OS_ERR_TIME_INVALID_SECONDS;
             return;
         }
         if (minutes > 59u) {
            *p_err = OS_ERR_TIME_INVALID_MINUTES;
             return;
         }
         if (hours   > 99u) {
            *p_err = OS_ERR_TIME_INVALID_HOURS;
             return;
         }
    } else {
         if (minutes > 9999u) {
            *p_err = OS_ERR_TIME_INVALID_MINUTES;
             return;
         }
         if (hours > 999u) {
            *p_err = OS_ERR_TIME_INVALID_HOURS;
             return;
         }
    }
#endif

                                                                /* Compute the total number of clock ticks required..   */
                                                                /* .. (rounded to the nearest tick).                    */
    tick_rate = OS_CFG_TICK_RATE_HZ;
    ticks     = ((OS_TICK)hours * (OS_TICK)3600u + (OS_TICK)minutes * (OS_TICK)60u + (OS_TICK)seconds) * tick_rate
              + (tick_rate * ((OS_TICK)milli + (OS_TICK)500u / tick_rate)) / (OS_TICK)1000u;

    if (ticks > 0u) {
        CPU_CRITICAL_ENTER();
                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
        OS_HWOS_SET_R5(ticks);                                  /* Delay.                                               */
        OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SLP_TSK);

        OS_HWOS_CMD_PROC();
        CPU_CRITICAL_EXIT();
        OS_HWOS_TASK_SYNC();

        switch(OSTaskRet[OSCtxIDCur].r0) {
            case 0xFF86:                                        /* Time out.                                            */
            case 0xFF8D:                                        /* Wait release wup_tsk.                                */
            case 0xFF90:                                        /* Wait release rel_wai.                                */
                *p_err = OS_ERR_NONE;
                 break;

            case 0xFF80:                                        /* Invalid System Call.                                 */
            default:
                 CPU_SW_EXCEPTION(;);
                 break;
        }
    } else {
       *p_err = OS_ERR_TIME_ZERO_DLY;
    }

}
#endif /* OS_CFG_TIME_DLY_HMSM_EN == DEF_ENABLED */

/*
*********************************************************************************************************
*                                         RESUME A DELAYED TASK
*
* Description: This function is used resume a task that has been delayed through a call to either
*              OSTimeDly() or OSTimeDlyHMSM().  Note that you cannot call this function to resume a
*              task that is waiting for an event with timeout.
*
* Arguments  : p_tcb    Pointer to the TCB of the task to resume.
*
*              p_err    Pointer to a variable that will receive an error code.
*
*                       OS_ERR_NONE                  Task has been resumed
*                       OS_ERR_STATE_INVALID         Task is in an invalid state
*                       OS_ERR_TIME_DLY_RESUME_ISR   If called from an ISR
*                       OS_ERR_TIME_NOT_DLY          Task is not waiting for time to expire
*                       OS_ERR_TASK_SUSPENDED        Task cannot be resumed, it was suspended by OSTaskSuspend()
*
* Note(s)    : none
*********************************************************************************************************
*/

#if (OS_CFG_TIME_DLY_RESUME_EN == DEF_ENABLED)
void  OSTimeDlyResume (OS_TCB  *p_tcb,
                       OS_ERR  *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_TIME_DLY_RESUME_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if ((p_tcb == DEF_NULL) || (p_tcb->CtxID == OSCtxIDCur)) {
       *p_err = OS_ERR_TASK_NOT_DLY;                            /* Can't resume self.                                   */
        return;
    }
#endif

    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_tcb->CtxID);                               /* Context id of the task to resume.                    */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SLP_TSK);

    OS_HWOS_CMD_PROC();
    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch(OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF8B:
        case 0xFF8C:
        case 0xFF8E:
            *p_err = OS_ERR_TASK_NOT_SUSPENDED;
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }
}
#endif /* OS_CFG_TIME_DLY_RESUME_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                       GET CURRENT SYSTEM TIME
*
* Description: This function is used by your application to obtain the current value of the counter
*              which keeps track of the number of clock ticks.
*
* Arguments  : p_err    Pointer to a variable that will receive an error code
*
*                           OS_ERR_NONE           If the call was successful
*
* Returns    : The current system time
*********************************************************************************************************
*/

OS_TICK  OSTimeGet (OS_ERR  *p_err)
{
    OS_TICK  ticks;


    ticks = OS_HWOS_GET_SYS_TIM();
   *p_err = OS_ERR_NONE;

    return (ticks);
}


/*
*********************************************************************************************************
*                                          SET SYSTEM CLOCK
*
* Description: This function sets the counter which keeps track of the number of clock ticks.
*
* Arguments  : ticks    Desired tick value
*
*              p_err    Pointer to a variable that will receive an error code
*
*                       OS_ERR_NONE           If the call was successful
*
* Returns    : none
*********************************************************************************************************
*/

void  OSTimeSet (OS_TICK   ticks,
                 OS_ERR   *p_err)
{
    OS_HWOS_SET_SYS_TIM(ticks);
   *p_err = OS_ERR_NONE;
}
