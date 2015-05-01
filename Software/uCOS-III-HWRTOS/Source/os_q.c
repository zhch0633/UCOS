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
*                                                QEUES
*
* Filename      : os_q.c
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
*                                               CREATE A MESSAGE QUEUE
*
* Description: This function is called by your application to create a message queue.  Message queues MUST
*              be created before they can be used.
*
* Arguments  : p_q         Pointer to the message queue.
*
*              p_name      Pointer to an ASCII string that will be used to name the message queue.
*
*              max_qty     Kept for compatibility with uC/OS-III. Ignored by the HWOS.
*
*              p_err       Pointer to a variable that will contain an error code returned by this function.
*
*                          OS_ERR_NONE                    the call was successful
*                          OS_ERR_CREATE_ISR              can't create from an ISR
*                          OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                          OS_ERR_OBJ_CREATED             if the message queue has already been created
*                          OS_ERR_OBJ_PTR_NULL            if you passed a NULL pointer for 'p_q'
*
* Returns    : none
*********************************************************************************************************
*/

#if (OS_CFG_Q_EN == DEF_ENABLED)
void  OSQCreate (OS_Q        *p_q,
                 CPU_CHAR    *p_name,
                 OS_MSG_QTY   max_qty,
                 OS_ERR      *p_err)

{
    CPU_INT32U     i;
    CPU_INT32U     q_id;
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

    if (p_q == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_q->Type = OS_OBJ_TYPE_Q;                                  /* Mark the data structure as a queue                   */
#endif

    CPU_CRITICAL_ENTER();
    q_id = OS_HWOS_Q_CNT + 1u;
    for (i = 0u; i < OS_HWOS_Q_CNT; i++) {                      /* Search for an available hardware mail box.           */
        if(OSQPtrTbl[i] == DEF_NULL) {
            q_id = i;
            break;
        }
    }

    if (q_id == (OS_HWOS_Q_CNT + 1u)) {                         /* We ran out of hardware mail boxes.                   */
       *p_err = OS_ERR_HWOS_RSRC_NOT_AVAIL;
        CPU_CRITICAL_EXIT();
        return;
    }

    p_q->QID = q_id;
    OSQPtrTbl[q_id] = p_q;


                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(q_id);                                       /* HWOS mail box ID.                                    */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_CRE_MBX);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0x0083:                                            /* Already Created.                                     */
        case 0x0084:                                            /* Invalid Mail Box ID.                                 */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }

}
#endif /* OS_CFG_Q_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                               POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.
*
* Arguments  : p_q           is a pointer to a message queue that must have been created by OSQCreate().
*
*              p_void        is a pointer to the message to send.
*
*              msg_size      specifies the size of the message (in bytes)
*
*              opt           determines the type of POST performed:
*
*                            OS_OPT_POST_ALL*         POST to ALL tasks that are waiting on the queue.  This option
*                                                     can be added to either OS_OPT_POST_FIFO or OS_OPT_POST_LIFO
*                            OS_OPT_POST_FIFO*        POST message to end of queue (FIFO) and wake up a single
*                                                     waiting task.
*                            OS_OPT_POST_LIFO*        POST message to the front of the queue (LIFO) and wake up
*                                                     a single waiting task.
*                            OS_OPT_POST_NO_SCHED*    Do not call the scheduler
*
*                            * Not supported by the HWOS, options silently ignored.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE            The call was successful and the message was sent
*                                OS_ERR_OBJ_PTR_NULL    If 'p_q' is a NULL pointer
*                                OS_ERR_OBJ_TYPE        If the message queue was not initialized
*                                OS_ERR_Q_MAX           If the queue is full
*
* Returns    : None
*********************************************************************************************************
*/

#if (OS_CFG_Q_EN == DEF_ENABLED)
void  OSQPost (OS_Q         *p_q,
               void         *p_void,
               OS_MSG_SIZE   msg_size,
               OS_OPT        opt,
               OS_ERR       *p_err)
{
    CPU_INT32U  ret_val;
    CPU_SR_ALLOC();


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(;);
    }

    if (p_q == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_q->Type != OS_OBJ_TYPE_Q) {                           /* Make sure queue was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_q->QID);                                   /* HWOS Mail Box ID.                                    */
    OS_HWOS_SET_R5((CPU_INT32U)p_void);                         /* Message.                                             */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SND_MBX);

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

        case 0x0084:                                            /* Overflow.                                            */
            *p_err = OS_ERR_Q_FULL;
             return;
             break;

        case 0xFF84:                                            /* Invalid System Call.                                 */
        case 0xFF85:                                            /* Invalid ID.                                          */
        case 0xFF8F:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }
}
#endif /* OS_CFG_Q_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                            PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : p_q           is a pointer to the message queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time specified
*                            by this argument.  If you specify 0, however, your task will wait forever at
*                            the specified queue or, until a message arrives.
*
*              opt           determines whether the user wants to block if the queue is empty or not:
*
*                            OS_OPT_PEND_BLOCKING
*                            OS_OPT_PEND_NON_BLOCKING
*
*              p_msg_size    is a pointer to a variable that will receive the size of the message
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the message
*                            was received, pend aborted or the message queue deleted,  If you pass a NULL
*                            pointer (i.e. (CPU_TS *)0) then you will not get the timestamp.  In other words,
*                            passing a NULL pointer is valid and indicates that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE               The call was successful and your task received a message.
*                            OS_ERR_OBJ_PTR_NULL       if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE           if the message queue was not created
*                            OS_ERR_PEND_ABORT         the pend was aborted
*                            OS_ERR_PEND_ISR           if you called this function from an ISR
*                            OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the queue was not empty
*                            OS_ERR_SCHED_LOCKED       the scheduler is locked
*                            OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                      would lead to a suspension.
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if you received a NULL pointer message or,
*                            if no message was received or,
*                            if 'p_q' is a NULL pointer or,
*                            if you didn't pass a pointer to a queue.
*********************************************************************************************************
*/

#if (OS_CFG_Q_EN == DEF_ENABLED)
void  *OSQPend (OS_Q         *p_q,
                OS_TICK       timeout,
                OS_OPT        opt,
                OS_MSG_SIZE  *p_msg_size,
                CPU_TS       *p_ts,
                OS_ERR       *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_PEND_ISR;
        return (DEF_NULL);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(DEF_NULL);
    }

    if (p_q == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (DEF_NULL);
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
    if (p_q->Type != OS_OBJ_TYPE_Q) {                           /* Make sure queue was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (DEF_NULL);
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

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_q->QID);                                   /* HWOS Mail Box ID.                                    */
    OS_HWOS_SET_R5(timeout);                                    /* Timeout.                                             */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_RCV_MBX);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0 & 0x000FFFFu) {
        case 0x0001:                                            /* Success.                                             */
        case 0x0003:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0x0004:                                            /* Would block.                                         */
            *p_err = OS_ERR_PEND_WOULD_BLOCK;
             break;

        case 0xFF86:                                            /* Timeout.                                             */
            *p_err = OS_ERR_TIMEOUT;
             return (DEF_NULL);
             break;

        case 0xFF84:                                            /* Wait abort.                                          */
            *p_err = OS_ERR_PEND_ABORT;
             return (DEF_NULL);
             break;

        case 0xFF94:                                            /* Deleted.                                             */
            *p_err = OS_ERR_OBJ_DEL;
             return (DEF_NULL);
             break;

        case 0xFF8F:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(DEF_NULL);
             break;
    }

    return ((void *)OSTaskRet[OSCtxIDCur].r1);
}
#endif /* OS_CFG_Q_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                               DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : p_q       is a pointer to the message queue you want to delete
*
*              opt       determines delete options as follows:
*
*                        OS_OPT_DEL_NO_PEND          Delete the queue ONLY if no task pending
*                        OS_OPT_DEL_ALWAYS           Deletes the queue even if tasks are waiting.
*                                                        In this case, all the tasks pending will be readied.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                        OS_ERR_NONE                 The call was successful and the queue was deleted
*                        OS_ERR_DEL_ISR              If you tried to delete the queue from an ISR
*                        OS_ERR_OBJ_PTR_NULL         if you pass a NULL pointer for 'p_q'
*                        OS_ERR_OBJ_TYPE             if the message queue was not created
*                        OS_ERR_OPT_INVALID          An invalid option was specified
*                        OS_ERR_TASK_WAITING         One or more tasks were waiting on the queue
*
* Returns    : 0
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence
*                 of the queue MUST check the return code of OSQPend().
*
*              2) Because ALL tasks pending on the queue will be readied, you MUST be careful
*                 in applications where the queue is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the queue.
*********************************************************************************************************
*/

#if ((OS_CFG_Q_EN == DEF_ENABLED) &&  (OS_CFG_Q_DEL_EN == DEF_ENABLED))
OS_OBJ_QTY  OSQDel (OS_Q    *p_q,
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

    if (p_q == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_q->Type != OS_OBJ_TYPE_Q) {                           /* Make sure queue was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif


    CPU_CRITICAL_ENTER();

    OSQPtrTbl[p_q->QID] = DEF_NULL;

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_q->Type = OS_OBJ_TYPE_NONE;
#endif

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_q->QID);                                   /* HWOS Mail Box ID.                                    */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_DEL_MBX);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0 & 0x000FFFFu) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF84:                                            /* Already deleted.                                     */
        case 0xFF85:                                            /* Invalid ID.                                          */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(DEF_NULL);
             break;
    }

    return (0u);
}
#endif /* (OS_CFG_Q_EN == DEF_ENABLED) &&  (OS_CFG_Q_DEL_EN == DEF_ENABLED) */
