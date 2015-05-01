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
*                                           TASK MANAGEMENT
*
* Filename      : os_task.c
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
************************************************************************************************************************
*                                                CHANGE PRIORITY OF A TASK
*
* Description: This function allows you to change the priority of a task dynamically.  Note that the new
*              priority MUST be available.
*
* Arguments  : p_tcb      is the TCB of the tack to change the priority for
*
*              prio_new   is the new priority
*
*              p_err      is a pointer to an error code returned by this function:
*
*                             OS_ERR_NONE                 is the call was successful
*                             OS_ERR_PRIO_INVALID         if the priority you specify is higher that the maximum allowed
*                                                         (i.e. >= (OS_CFG_PRIO_MAX-1))
*                             OS_ERR_STATE_INVALID        if the task is in an invalid state
*                             OS_ERR_TASK_CHANGE_PRIO_ISR if you tried to change the task's priority from an ISR
************************************************************************************************************************
*/

#if (OS_CFG_TASK_CHANGE_PRIO_EN == DEF_ENABLED)
void  OSTaskChangePrio (OS_TCB   *p_tcb,
                        OS_PRIO   prio_new,
                        OS_ERR   *p_err)
{
    CPU_INT32U  ctx_id;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_TASK_CHANGE_PRIO_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if (prio_new >= OS_HWOS_MAX_PRIO) {
       *p_err = OS_ERR_PRIO_INVALID;
        return;
    }
#endif

    if (p_tcb == DEF_NULL) {
        ctx_id = OSCtxIDCur;
    } else {
        ctx_id = p_tcb->CtxID;
    }

    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(ctx_id);
    OS_HWOS_SET_R5(prio_new);
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_CHG_PRI);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
        case 0x0002:                                            /* Already at new priority.                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }


}
#endif /* OS_CFG_TASK_CHANGE_PRIO_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                            CREATE A TASK
*
* Description: This function is used to have uC/OS-III-HWOS manage the execution of a task.  Tasks can
*              either be created prior to the start of multitasking or by a running task.  A task cannot
*              be created by an ISR.
*
* Arguments  : p_tcb          Pointer to the task's TCB
*
*              p_name         Pointer to an ASCII string to provide a name to the task.
*
*              p_task         Pointer to the task's code
*
*              p_arg          Pointer to an optional data area which can be used to pass parameters to
*                             the task when the task first executes.  Where the task is concerned it
*                             thinks it was invoked and passed the argument 'p_arg' as follows:
*
*                                 void Task (void *p_arg)
*                                 {
*                                     for (;;) {
*                                         Task code;
*                                     }
*                                 }
*
*              prio           Task's priority.  A unique priority MUST be assigned to each task and the
*                             lower the number, the higher the priority.
*
*              p_stk_base     Pointer to the base address of the stack (i.e. low address).
*
*              stk_limit      Argument has no effect. Left for compatibility with uC/OS-III.
*
*              stk_size       Size of the stack in number of elements.  If CPU_STK is set to CPU_INT08U,
*                             'stk_size' corresponds to the number of bytes available.  If CPU_STK is set to
*                             CPU_INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally,
*                             if CPU_STK is set to CPU_INT32U, 'stk_size' contains the number of 32-bit entries
*                             available on the stack.
*
*              q_size         Maximum number of messages that can be sent to the task
*
*              time_quanta    Argument has no effect. Left for compatibility with uC/OS-III.
*
*              p_ext          Argument has no effect. Left for compatibility with uC/OS-III.
*
*              opt            contains additional information (or options) about the behavior of the task.
*                             See OS_OPT_TASK_xxx in OS.H.  Current choices are:
*
*                             OS_OPT_TASK_NONE          No option selected
*                             OS_OPT_TASK_STK_CHK*      Stack checking to be allowed for the task
*                             OS_OPT_TASK_STK_CLR       Clear the stack when the task is created
*                             OS_OPT_TASK_SAVE_FP*      If the CPU has floating-point registers, save them
*                                                       during a context switch.
*                             OS_OPT_TASK_NO_TLS*       If the caller doesn't want or need TLS (Thread Local
*                                                       Storage) support for the task.  If you do not include this
*                                                       option, TLS will be supported by default.
*
*                             * Not supported by the HWOS. Options will be silently ignored.
*
*              p_err          is a pointer to an error code that will be set during this call.  The value pointer
*                             to by 'p_err' can be:
*
*                             OS_ERR_NONE                if the function was successful.
*                             OS_ERR_NAME                if 'p_name' is a NULL pointer
*                             OS_ERR_PRIO_INVALID        if the priority you specify is higher that the maximum
*                                                           allowed (i.e. >= OS_CFG_PRIO_MAX-1) or,
*                                                        if OS_CFG_ISR_POST_DEFERRED_EN is set to 1 and you tried
*                                                           to use priority 0 which is reserved.
*                             OS_ERR_STK_INVALID         if you specified a NULL pointer for 'p_stk_base'
*                             OS_ERR_STK_SIZE_INVALID    if you specified zero for the 'stk_size'
*                             OS_ERR_STK_LIMIT_INVALID   if you specified a 'stk_limit' greater than or equal
*                                                           to 'stk_size'
*                             OS_ERR_TASK_CREATE_ISR     if you tried to create a task from an ISR.
*                             OS_ERR_TASK_INVALID        if you specified a NULL pointer for 'p_task'
*                             OS_ERR_TCB_INVALID         if you specified a NULL pointer for 'p_tcb'
*
* Returns    : none
*********************************************************************************************************
*/

void          OSTaskCreate              (OS_TCB                *p_tcb,
                                         CPU_CHAR              *p_name,
                                         OS_TASK_PTR            p_task,
                                         void                  *p_arg,
                                         OS_PRIO                prio,
                                         CPU_STK               *p_stk_base,
                                         CPU_STK_SIZE           stk_limit,
                                         CPU_STK_SIZE           stk_size,
                                         OS_MSG_QTY             q_size,
                                         OS_TICK                time_quanta,
                                         void                  *p_ext,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err)
{
    CPU_INT32U     i;
    CPU_INT32U     ctx_id;
    CPU_STK       *p_sp;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_TASK_CREATE_ISR;
        return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_tcb->Type = OS_OBJ_TYPE_TASK;                             /* Mark the data structure as a semaphore               */
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if (p_tcb == DEF_NULL) {
       *p_err = OS_ERR_TCB_INVALID;
        return;
    }

    if (p_task == DEF_NULL) {
       *p_err = OS_ERR_TASK_INVALID;
        return;
    }

    if (p_stk_base == DEF_NULL) {
       *p_err = OS_ERR_STK_INVALID;
        return;
    }

    if (stk_size < OS_CFG_STK_SIZE_MIN) {
       *p_err = OS_ERR_STK_SIZE_INVALID;
        return;
    }

    if ((prio >= OS_HWOS_MAX_PRIO) && (p_tcb != &OSIdleTaskTCB) ) {
       *p_err = OS_ERR_PRIO_INVALID;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
    ctx_id = OS_HWOS_CNTX_CNT + 1u;
    for (i = 0u; i < OS_HWOS_CNTX_CNT; i++) {
        if (OSTaskStkPtrTbl[i] == DEF_NULL) {
            ctx_id = i;
            break;
        }
    }

    if (ctx_id == (OS_HWOS_CNTX_CNT + 1u)) {                    /* We ran out of hardware tasks.                        */
       *p_err = OS_ERR_HWOS_RSRC_NOT_AVAIL;
        CPU_CRITICAL_EXIT();
        return;
    }

    p_tcb->CtxID = ctx_id;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_tcb->NamePtr = p_name;
#endif

    if ((opt & OS_OPT_TASK_STK_CHK) != 0u) {
        p_sp = p_stk_base;
        for (i = 0u; i < stk_size; i++) {                       /* Stack grows from HIGH to LOW memory                  */
           *p_sp = 0u;                                          /* Clear from bottom of stack and up!                   */
            p_sp++;
        }
    }

    OSTaskStkPtrTbl[i] = OS_CPU_TaskInit(p_task, p_arg, p_stk_base, stk_size);


    if (OSRunning == DEF_NO) {
        RIN32_HWOS->CNTX[ctx_id].CNTX_TYPE = 0x03;
        RIN32_HWOS->CNTX[ctx_id].CNTX_STAT = 0x03;
        RIN32_HWOS->CNTX[ctx_id].INIT_ADD = (CPU_INT32U)p_task;
        RIN32_HWOS->CNTX[ctx_id].PRTY = (prio) | (prio << 8u);
        RIN32_HWOS->CNTX[ctx_id].INIT_R14 = (CPU_INT32U)OSTaskStkPtrTbl[i];
    }

   *p_err = OS_ERR_NONE;

    if (OSRunning == DEF_YES) {
        OS_HWOS_SET_R4(ctx_id);
        OS_HWOS_SET_R5(prio);
        OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_CHG_PRI);

        OS_HWOS_CMD_PROC();

        OS_HWOS_SET_R4(ctx_id);
        OS_HWOS_SET_R5((CPU_INT32U)p_arg);
        OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_STA_TKS);

        OS_HWOS_CMD_PROC();

        CPU_CRITICAL_EXIT();
        OS_HWOS_TASK_SYNC();

        switch (OSTaskRet[OSCtxIDCur].r0) {
            case 0x0001:                                        /* Success.                                             */
                *p_err = OS_ERR_NONE;
                 break;

            case 0xFF80:                                        /* Invalid System Call.                                 */
            case 0xFF81:                                        /* Invalid State.                                       */
            case 0xFF82:                                        /* Invalid System Call.                                 */
            default:
                 CPU_SW_EXCEPTION(;);
                 break;
        }
    }

}


/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by
*              specifying a NULL pointer for 'p_tcb'.  The deleted task is returned to the dormant state
*              and can be re-activated by creating the deleted task again.
*
* Arguments  : p_tcb      TCB of the tack to delete
*
*              p_err      Pointer to an error code returned by this function:
*
*                         OS_ERR_NONE              if the call is successful
*                         OS_ERR_STATE_INVALID     if the state of the task is invalid
*                         OS_ERR_TASK_DEL_IDLE     if you attempted to delete uC/OS-III's idle task
*                         OS_ERR_TASK_DEL_INVALID  if you attempted to delete uC/OS-III's ISR handler task
*                         OS_ERR_TASK_DEL_ISR      if you tried to delete a task from an ISR
*
* Returns    : none
*********************************************************************************************************
*/
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
void  OSTaskDel (OS_TCB  *p_tcb,
                 OS_ERR  *p_err)
{
    CPU_INT32U     ctx_id;
    CPU_BOOLEAN    self;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed from an ISR                              */
       *p_err = OS_ERR_TASK_DEL_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }
#endif

    if (p_tcb == &OSIdleTaskTCB) {                              /* Not allowed to delete the idle task                  */
       *p_err = OS_ERR_TASK_DEL_IDLE;
        return;
    }

    CPU_CRITICAL_ENTER();

    if (p_tcb == DEF_NULL) {
        ctx_id = OSCtxIDCur;
        self = DEF_YES;
    } else {
        ctx_id = p_tcb->CtxID;
        if(p_tcb->CtxID == OSCtxIDCur) {
        	self = DEF_YES;
        } else {
        	self = DEF_NO;
        }
    }

    switch (OS_HWOS_GET_CNTX_STAT(ctx_id)) {
        case OS_HWOS_CNTX_STAT_WAIT:
        case OS_HWOS_CNTS_STAT_RDY:
        case OS_HWOS_CNTX_STAT_RUN:
             break;

        case OS_HWOS_CNTX_STAT_IDLE:
        case OS_HWOS_CNTX_STAT_STOP:
            *p_err = OS_ERR_STATE_INVALID;                      /* Can't delete a stopped or deleted task.              */
             CPU_CRITICAL_EXIT();
             return;
             break;

        default:
             CPU_SW_EXCEPTION(;);                               /* Corrupted state.                                     */
             break;
    }


    OSTaskStkPtrTbl[ctx_id] = DEF_NULL;                         /* Clear the stack entry.                               */

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_tcb->Type = OS_OBJ_TYPE_NONE;
#endif

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(ctx_id);
    if (self == DEF_YES) {
    	OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_EXT_TSK);
    } else {
    	OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_TER_TSK);
    }

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF88:                                            /* Already deleted.                                     */
        case 0xFF81:                                            /* Invalid State.                                       */
        case 0xFF8A:                                            /* Invalid System Call.                                 */
        case 0xFF89:                                            /* Invalid System Call.                                 */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }

}
#endif


/*
*********************************************************************************************************
*                                           SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if 'p_tcb'
*              is a NULL pointer or the pointer to the TCB of the calling task.
*
* Arguments  : p_tcb    Pointer to the TCB to suspend.
*                       If p_tcb is a NULL pointer then, suspend the current task.
*
*              p_err    Pointer to a variable that will receive an error code from this function.
*
*                       OS_ERR_NONE                      if the requested task is suspended
*                       OS_ERR_SCHED_LOCKED              you can't suspend the current task is the scheduler is
*                                                        locked
*                       OS_ERR_TASK_SUSPEND_ISR          if you called this function from an ISR
*                       OS_ERR_TASK_SUSPEND_IDLE         if you attempted to suspend the idle task which is not
*                                                        allowed.
*                       OS_ERR_TASK_SUSPEND_INT_HANDLER  if you attempted to suspend the idle task which is not
*                                                        allowed.
*
* Note(s)    : 1) You should use this function with great care.  If you suspend a task that is waiting
*                 for an event (i.e. a message, a semaphore, a queue ...) you will prevent this task from
*                 running when the event arrives.
*********************************************************************************************************
*/

void  OSTaskSuspend (OS_TCB  *p_tcb,
                     OS_ERR  *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed from an ISR                              */
       *p_err = OS_ERR_TASK_SUSPEND_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if ((p_tcb != DEF_NULL) && (p_tcb->CtxID != OSCtxIDCur)) {
       *p_err = OS_ERR_HWOS_UNSUPPORTED;                        /* The HWOS can't suspend another task.                 */
        return;
    }
#endif

    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R5(0xFFFFFFFF);
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SLP_TSK);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0xFF8D:                                            /* Wait release wup_tsk.                                */
        case 0xFF90:                                            /* Wait release rel_wai.                                */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        case 0xFF86:                                            /* Time out.                                            */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }
}


/*
*********************************************************************************************************
*                                               RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call
*              that will remove an explicit task suspension.
*
* Arguments  : p_tcb      Pointer to the task's OS_TCB to resume
*
*              p_err      Pointer to a variable that will contain an error code returned by this function
*
*                         OS_ERR_NONE                  if the requested task is resumed
*                         OS_ERR_STATE_INVALID         if the task is in an invalid state
*                         OS_ERR_TASK_RESUME_ISR       if you called this function from an ISR
*                         OS_ERR_TASK_RESUME_SELF      You cannot resume 'self'
*                         OS_ERR_TASK_NOT_SUSPENDED    if the task to resume has not been suspended
*
* Returns    : none
*********************************************************************************************************
*/

void  OSTaskResume (OS_TCB  *p_tcb,
                    OS_ERR  *p_err)
{
    CPU_INT32U  ret_val;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed from an ISR                              */
       *p_err = OS_ERR_TASK_RESUME_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if ((p_tcb == DEF_NULL) || (p_tcb->CtxID == OSCtxIDCur)) {
       *p_err = OS_ERR_TASK_RESUME_SELF;                        /* Can't resume self.                                   */
        return;
    }
#endif

    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */

    OS_HWOS_SET_R4(p_tcb->CtxID);                               /* Context id of the task to resume.                    */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_WUP_TSK);

    OS_HWOS_CMD_PROC_ISR(ret_val);

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    if (ret_val == 0u) {
        ret_val = OSTaskRet[OSCtxIDCur].r0;
    }

    switch (ret_val) {
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
