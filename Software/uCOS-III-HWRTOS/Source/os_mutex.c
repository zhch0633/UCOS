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
*                                               MUTEXES
*
* Filename      : os_mutex.c
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
*                                           CREATE A MUTEX
*
* Description: This function creates a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to initialize.  Your application is responsible
*                            for allocating storage for the mutex.
*
*              p_name        is a pointer to the name you would like to give the mutex.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                    if the call was successful
*                            OS_ERR_CREATE_ISR              if you called this function from an ISR
*                            OS_ERR_NAME                    if 'p_name'  is a NULL pointer
*                            OS_ERR_OBJ_CREATED             if the mutex has already been created
*                            OS_ERR_OBJ_PTR_NULL            if 'p_mutex' is a NULL pointer
*
* Returns    : none
*********************************************************************************************************
*/

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
void  OSMutexCreate (OS_MUTEX  *p_mutex,
                     CPU_CHAR  *p_name,
                     OS_ERR    *p_err)
{
    CPU_INT32U     i;
    CPU_INT32U     sem_id;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_CREATE_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if (p_mutex == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_mutex->Type = OS_OBJ_TYPE_MUTEX;                          /* Mark the data structure as a semaphore               */
#endif

    CPU_CRITICAL_ENTER();
    sem_id = OS_HWOS_SEM_CNT + 1u;
    for (i = 0u; i < OS_HWOS_SEM_CNT; i++) {                    /* Search for an available hardware semaphore.          */
        if (OSSemPtrTbl[i] == DEF_NULL) {
            sem_id = i;
            break;
        }
    }

    if (sem_id == (OS_HWOS_SEM_CNT + 1u)) {                     /* We ran out of hardware semaphores.                   */
       *p_err = OS_ERR_HWOS_RSRC_NOT_AVAIL;
        CPU_CRITICAL_EXIT();
        return;
    }

    p_mutex->SemID = sem_id;
    p_mutex->Mutex = DEF_YES;
    OSSemPtrTbl[i] = (OS_SEM *)p_mutex;

    RIN32_HWOS->SEM_TBL[sem_id] = DEF_BIT_18 | 0x00000040;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_mutex->NamePtr = p_name;
#endif

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(sem_id | DEF_BIT_16);                        /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_R5(1u);
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_CRE_SEM);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF83:                                            /* Already Created.                                     */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        case 0xFF8F:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }
}
#endif /* OS_CFG_MUTEX_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                                    PEND ON MUTEX
*
* Description: This function waits for a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task
*                            will wait for the resource up to the amount of time (in 'ticks') specified by
*                            this argument.  If you specify 0, however, your task will wait forever at the
*                            specified mutex or, until the resource becomes available.
*
*              opt           determines whether the user wants to block if the mutex is not available or not:
*
*                            OS_OPT_PEND_BLOCKING
*                            OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the mutex was posted or
*                            pend aborted or the mutex deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you
*                            will not get the timestamp.  In other words, passing a NULL pointer is valid and indicates
*                            that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE               The call was successful and your task owns the resource
*                            OS_ERR_MUTEX_OWNER        If calling task already owns the mutex
*                            OS_ERR_OBJ_DEL            If 'p_mutex' was deleted
*                            OS_ERR_OBJ_PTR_NULL       If 'p_mutex' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE           If 'p_mutex' is not pointing at a mutex
*                            OS_ERR_OPT_INVALID        If you didn't specify a valid option
*                            OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                            OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                      would lead to a suspension.
*                            OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the mutex was not
*                                                      available.
*                            OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                            OS_ERR_STATE_INVALID      If the task is in an invalid state
*                            OS_ERR_STATUS_INVALID     If the pend status has an invalid value
*                            OS_ERR_TIMEOUT            The mutex was not received within the specified timeout.
*
* Returns    : none
*********************************************************************************************************
*/
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
void  OSMutexPend (OS_MUTEX  *p_mutex,
                   OS_TICK    timeout,
                   OS_OPT     opt,
                   CPU_TS    *p_ts,
                   OS_ERR    *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_PEND_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if (p_mutex == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }

    switch (opt) {                                              /* Validate 'opt'                                       */
        case OS_OPT_PEND_BLOCKING:
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) {                   /* Make sure mutex was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif

    if (timeout == 0u) {                                        /* Adjust timeout value for the HWOS.                   */
        timeout = 0xFFFFFFFFu;
    } else if (timeout == 0xFFFFFFFFu) {
        timeout -= 1u;
    }

    if (opt & OS_OPT_PEND_NON_BLOCKING) {
        timeout = 0u;
    }

    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_mutex->SemID);                             /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_R5(timeout);                                    /* Timeout.                                             */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_WAI_SEM);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
        case 0x0003:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0x0004:                                            /* Would block.                                         */
            *p_err = OS_ERR_PEND_WOULD_BLOCK;
             break;

        case 0xFF86:                                            /* Timeout.                                             */
            *p_err = OS_ERR_TIMEOUT;
             return;
             break;

        case 0xFF84:                                            /* Wait abort.                                          */
            *p_err = OS_ERR_PEND_ABORT;
             return;
             break;

        case 0xFF93:                                            /* Deleted.                                             */
            *p_err = OS_ERR_OBJ_DEL;
             return;
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }

    return;
}
#endif /* OS_CFG_MUTEX_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                                   POST TO A MUTEX
*
* Description: This function signals a mutex
*
* Arguments  : p_mutex  is a pointer to the mutex
*
*              opt      is an option you can specify to alter the behavior of the post.  The choices are:
*
*                       OS_OPT_POST_NONE        No special option selected
*                       OS_OPT_POST_NO_SCHED    If you don't want the scheduler to be called after the post.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                       OS_ERR_NONE             The call was successful and the mutex was signaled.
*                       OS_ERR_MUTEX_NESTING    Mutex owner nested its use of the mutex
*                       OS_ERR_MUTEX_NOT_OWNER  If the task posting is not the Mutex owner
*                       OS_ERR_OBJ_PTR_NULL     If 'p_mutex' is a NULL pointer.
*                       OS_ERR_OBJ_TYPE         If 'p_mutex' is not pointing at a mutex
*                       OS_ERR_POST_ISR         If you attempted to post from an ISR
*
* Returns    : none
*********************************************************************************************************
*/

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
void  OSMutexPost (OS_MUTEX  *p_mutex,
                   OS_OPT     opt,
                   OS_ERR    *p_err)
{
    CPU_INT32U  ret_val;
    CPU_SR_ALLOC();


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if (p_mutex == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }

    if ((opt & OS_OPT_POST_ALL) != 0u) {
       *p_err = OS_ERR_HWOS_UNSUPPORTED;
        return;
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) {                   /* Make sure mutex was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_mutex->SemID);                             /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SIG_SEM);

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

        case 0xFF87:                                            /* Not owner.                                           */
        case 0xFF8F:                                            /* Not owner (No owner).                                */
            *p_err = OS_ERR_MUTEX_NOT_OWNER;
             return;
             break;

        case 0xFF84:                                            /* Invalid Semaphore.                                   */
        case 0xFF85:                                            /* Invalid System Call.                                 */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }

    return;
}
#endif /* OS_CFG_MUTEX_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                                   DELETE A MUTEX
*
* Description: This function deletes a mutex and readies all tasks pending on the mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to delete
*
*              opt           determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND          Delete mutex ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS           Deletes the mutex even if tasks are waiting.
*                                                        In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                 The call was successful and the mutex was deleted
*                            OS_ERR_DEL_ISR              If you attempted to delete the mutex from an ISR
*                            OS_ERR_OBJ_PTR_NULL         If 'p_mutex' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE             If 'p_mutex' is not pointing to a mutex
*                            OS_ERR_OPT_INVALID          An invalid option was specified
*                            OS_ERR_STATE_INVALID        Task is in an invalid state
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the mutex MUST
*                 check the return code of OSMutexPend().
*
*              2) Because ALL tasks pending on the mutex will be readied, you MUST be careful in applications where the
*                 mutex is used for mutual exclusion because the resource(s) will no longer be guarded by the mutex.
*********************************************************************************************************
*/

#if ((OS_CFG_MUTEX_EN == DEF_ENABLED) && (OS_CFG_MUTEX_DEL_EN == DEF_ENABLED))
OS_OBJ_QTY  OSMutexDel (OS_MUTEX  *p_mutex,
                        OS_OPT     opt,
                        OS_ERR    *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed from an ISR                              */
       *p_err = OS_ERR_DEL_ISR;
        return (0u);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if (p_mutex == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) {                   /* Make sure mutex was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif


    CPU_CRITICAL_ENTER();

    OSSemPtrTbl[p_mutex->SemID] = DEF_NULL;                     /* Clear the sem entry.                                 */

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_mutex->Type = OS_OBJ_TYPE_NONE;
#endif

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_mutex->SemID);
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_DEL_SEM);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF85:                                            /* Deleted.                                             */
            *p_err = OS_ERR_OBJ_TYPE;
             return (0u);
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        case 0xFF84:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(0u);
             break;
    }

    return (0u);
}
#endif /* (OS_CFG_MUTEX_EN == DEF_ENABLED) && (OS_CFG_MUTEX_DEL_EN == DEF_ENABLED) */
