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
*                                             SEMAPHORES
*
* Filename      : os_sem.c
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
*                                         CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : p_sem         Pointer to the semaphore to initialize.  Your application is responsible
*                            for allocating storage for the semaphore.
*
*              p_name        Pointer to the name you would like to give the semaphore.
*
*              cnt           Initial value for the semaphore.
*                            If used to share resources, you should initialize to the number of resources
*                            available. If used to signal the occurrence of event(s) then you should
*                            initialize to 0.
*
*              p_err         Pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                    if the call was successful
*                            OS_ERR_CREATE_ISR              if you called this function from an ISR
*                            OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the semaphore after
*                                                             you called OSSafetyCriticalStart().
*                            OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                            OS_ERR_OBJ_CREATED             if the semaphore has already been created
*                            OS_ERR_OBJ_PTR_NULL            if 'p_sem'  is a NULL pointer
*                            OS_ERR_OBJ_TYPE                if 'p_sem' has already been initialized to a
*                                                             different object type
*                            OS_ERR_SEM_OVF                 if 'cnt' is higher than the maximum supported
*                                                           value
*
* Returns    : none
*********************************************************************************************************
*/

#if (OS_CFG_SEM_EN == DEF_ENABLED)
void  OSSemCreate (OS_SEM      *p_sem,
                   CPU_CHAR    *p_name,
                   OS_SEM_CTR   cnt,
                   OS_ERR      *p_err)
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

    if (p_sem == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }

    if (cnt > OS_HWOS_SEM_VAL_MAX) {
       *p_err = OS_ERR_SEM_OVF;
        return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_sem->Type = OS_OBJ_TYPE_SEM;                              /* Mark the data structure as a semaphore               */
#endif

    CPU_CRITICAL_ENTER();
    sem_id = OS_HWOS_SEM_CNT + 1u;
    for (i = 0u; i < OS_HWOS_SEM_CNT; i++) {                    /* Search for an available hardware semaphore.          */
        if(OSSemPtrTbl[i] == DEF_NULL) {
            sem_id = i;
            break;
        }
    }

    if (sem_id == (OS_HWOS_SEM_CNT + 1u)) {                     /* We ran out of hardware semaphores.                   */
       *p_err = OS_ERR_HWOS_RSRC_NOT_AVAIL;
        CPU_CRITICAL_EXIT();
        return;
    }

    p_sem->SemID = sem_id;
    p_sem->Mutex = DEF_NO;
    OSSemPtrTbl[i] = p_sem;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_sem->NamePtr = p_name;
#endif

    RIN32_HWOS->SEM_TBL[sem_id] = DEF_BIT_18 | DEF_BIT_11 | (31 << 6);

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(sem_id | DEF_BIT_16);                        /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_R5(cnt);                                        /* Initial Count.                                       */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_CRE_SEM);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF84:                                            /* Invalid Count.                                       */
            *p_err = OS_ERR_SEM_OVF;
             return;
             break;

        case 0xFF83:                                            /* Already Created.                                     */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        case 0xFF8F:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }
}
#endif /* OS_CFG_SEM_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                          PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : p_sem         Pointer to the semaphore
*
*              timeout       Optional timeout period (in clock ticks).  If non-zero, your task will wait
*                            for the resource up to the amount of time (in 'ticks') specified by this
*                            argument.  If you specify 0, however, your task will wait forever at the
*                            specified semaphore or, until the resource becomes available (or the event occurs).
*
*              opt           Determines whether the user wants to block if the semaphore is
*                            available or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          Pointer to a variable that will receive the timestamp of when the semaphore
*                            was posted or pend aborted or the semaphore deleted.  If you pass a NULL
*                            pointer (i.e. (CPU_TS*)0) then you will not get the timestamp.  In other words,
*                            passing a NULL pointer is valid and indicates that you don't need the timestamp.
*
*              p_err         Pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE               The call was successful and your task owns the resource
*                                                      or, the event you are waiting for occurred.
*                            OS_ERR_OBJ_DEL            If 'p_sem' was deleted
*                            OS_ERR_OBJ_PTR_NULL       If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE           If 'p_sem' is not pointing at a semaphore
*                            OS_ERR_OPT_INVALID        If you specified an invalid value for 'opt'
*                            OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                            OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                      would lead to a suspension.
*                            OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the semaphore was not
*                                                      available.
*                            OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                            OS_ERR_STATUS_INVALID     Pend status is invalid
*                            OS_ERR_TIMEOUT            The semaphore was not received within the specified
*                                                      timeout.
*
*
* Returns    : The current value of the semaphore counter or 0 if not available.
*********************************************************************************************************
*/

#if (OS_CFG_SEM_EN == DEF_ENABLED)
OS_SEM_CTR  OSSemPend (OS_SEM   *p_sem,
                       OS_TICK   timeout,
                       OS_OPT    opt,
                       CPU_TS   *p_ts,
                       OS_ERR   *p_err)
{
    CPU_INT32U sem_cnt;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_PEND_ISR;
        return (0u);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(0u);
    }

    if (p_sem == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }

    switch (opt) {                                              /* Validate 'opt'                                       */
        case OS_OPT_PEND_BLOCKING:
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {                       /* Make sure the semaphore was created                  */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
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
    OS_HWOS_SET_R4(p_sem->SemID);                               /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_R5(timeout);                                    /* Timeout.                                             */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_WAI_SEM);

    OS_HWOS_CMD_PROC();

    sem_cnt = OS_HWOS_GET_SEM_CNT(p_sem->SemID);
    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch(OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
        case 0x0003:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0x0004:                                            /* Would block.                                         */
            *p_err = OS_ERR_PEND_WOULD_BLOCK;
             break;

        case 0xFF86:                                            /* Timeout.                                             */
            *p_err = OS_ERR_TIMEOUT;
             return (0u);
             break;

        case 0xFF84:                                            /* Wait abort.                                          */
            *p_err = OS_ERR_PEND_ABORT;
             return (0u);
             break;

        case 0xFF93:                                            /* Deleted.                                             */
            *p_err = OS_ERR_OBJ_DEL;
             return (0u);
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        case 0xFF90:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(0u);
             break;
    }

    return (sem_cnt);
}
#endif /* OS_CFG_SEM_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : p_sem    Pointer to the semaphore
*
*              opt      Determines the type of POST performed:
*
*                        OS_OPT_POST_1         POST and ready only the highest priority task waiting on semaphore
*                                              (if tasks are waiting).
*                        OS_OPT_POST_ALL*      POST to ALL tasks that are waiting on the semaphore
*
*                        OS_OPT_POST_NO_SCHED+ Do not call the scheduler
*
*                        Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*                        * Not supported by the HWOS, option will return an error.
*                        + Not supported by the HWOS, option silently ignored.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                       OS_ERR_NONE          The call was successful and the semaphore was signaled.
*                       OS_ERR_OBJ_PTR_NULL  If 'p_sem' is a NULL pointer.
*                       OS_ERR_OBJ_TYPE      If 'p_sem' is not pointing at a semaphore.
*                       OS_ERR_SEM_OVF       If the post would cause the semaphore count to overflow.
*
* Returns    : The current value of the semaphore counter or 0 upon error.
*********************************************************************************************************
*/

#if (OS_CFG_SEM_EN == DEF_ENABLED)
OS_SEM_CTR  OSSemPost (OS_SEM  *p_sem,
                       OS_OPT   opt,
                       OS_ERR  *p_err)
{
    CPU_INT32U  sem_cnt;
    CPU_INT32U  ret_val;
    CPU_SR_ALLOC();


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(0u);
    }

    if (p_sem == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }

    switch (opt) {                                              /* Validate 'opt'                                       */
        case OS_OPT_POST_1:
        case OS_OPT_POST_ALL:
        case OS_OPT_POST_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return (0u);
    }

    if ((opt & OS_OPT_POST_ALL) != 0u) {
       *p_err = OS_ERR_HWOS_UNSUPPORTED;
        return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {                       /* Make sure the semaphore was created                  */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif


    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_sem->SemID);                               /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SIG_SEM);

    OS_HWOS_CMD_PROC_ISR(ret_val);

    sem_cnt = OS_HWOS_GET_SEM_CNT(p_sem->SemID);
    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    if(ret_val == 0u) {
        ret_val = OSTaskRet[OSCtxIDCur].r0;
    }

    switch (ret_val) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF8F:                                            /* Overflow.                                            */
            *p_err = OS_ERR_SEM_OVF;
             return (0u);
             break;

        case 0xFF84:                                            /* Invalid Semaphore.                                   */
        case 0xFF85:                                            /* Invalid System Call.                                 */
        case 0xFF87:                                            /* Invalid System Call.                                 */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(0u);
             break;
    }

    return (sem_cnt);
}
#endif /* OS_CFG_SEM_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                                  DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore to delete
*
*              opt           determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND*         Delete semaphore ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS*          Deletes the semaphore even if tasks are waiting.
*                                                        In this case, all the tasks pending will be readied.
*
*                            * Both options ignored by the HWOS, the semaphore will always be deleted.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                 The call was successful and the semaphore was deleted
*                            OS_ERR_DEL_ISR              If you attempted to delete the semaphore from an ISR
*                            OS_ERR_OBJ_PTR_NULL         If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE             If 'p_sem' is not pointing at a semaphore
*                            OS_ERR_OPT_INVALID          An invalid option was specified
*                            OS_ERR_TASK_WAITING         One or more tasks were waiting on the semaphore
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the semaphore
*                 MUST check the return code of OSSemPend().
*********************************************************************************************************
*/

#if (OS_CFG_SEM_DEL_EN == DEF_ENABLED)
OS_OBJ_QTY  OSSemDel (OS_SEM  *p_sem,
                      OS_OPT   opt,
                      OS_ERR  *p_err)
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
        CPU_SW_EXCEPTION(0u);
    }

    if (p_sem == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {                       /* Make sure the semaphore was created                  */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif


    CPU_CRITICAL_ENTER();

    OSSemPtrTbl[p_sem->SemID] = DEF_NULL;                       /* Clear the sem entry.                                 */

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_sem->Type = OS_OBJ_TYPE_NONE;
#endif

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_sem->SemID);
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
#endif /* OS_CFG_SEM_DEL_EN == DEF_ENABLED */
