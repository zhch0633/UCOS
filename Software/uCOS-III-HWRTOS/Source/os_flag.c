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
*                                             EVENT FLAGS
*
* Filename      : os_flag.c
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
*                                        CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : p_grp          Pointer to the event flag group to create
*
*              p_name         Name of the event flag group
*
*              flags          Kept for compatibility, returns an error if not 0.
*
*              p_err          Pointer to an error code which will be returned to your application:
*
*                             OS_ERR_NONE                    if the call was successful.
*                             OS_ERR_CREATE_ISR              if you attempted to create an Event Flag from an ISR.
*                             OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                             OS_ERR_OBJ_CREATED             if the event flag group has already been created
*                             OS_ERR_OBJ_PTR_NULL            if 'p_grp' is a NULL pointer
*
* Returns    : none
*********************************************************************************************************
*/
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
void  OSFlagCreate (OS_FLAG_GRP  *p_grp,
                    CPU_CHAR     *p_name,
                    OS_FLAGS      flags,
                    OS_ERR       *p_err)
{
    CPU_INT32U     i;
    CPU_INT32U     grp_id;
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

    if (flags != 0u) {
       *p_err = OS_ERR_HWOS_UNSUPPORTED;
        return;
    }

    if (p_grp == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_grp->Type = OS_OBJ_TYPE_FLAG;                             /* Mark the data structure as a flag                    */
#endif

    CPU_CRITICAL_ENTER();

    grp_id = OS_HWOS_FLAG_CNT + 1u;
    for (i = 0u; i < OS_HWOS_FLAG_CNT; i++) {                   /* Search for an available hardware event group.        */
        if (OSGrpPtrTbl[i] == DEF_NULL) {
            grp_id = i;
            break;
        }
    }

    if (grp_id == (OS_HWOS_FLAG_CNT + 1u)) {                     /* We ran out of hardware semaphores.                   */
       *p_err = OS_ERR_HWOS_RSRC_NOT_AVAIL;
        CPU_CRITICAL_EXIT();
        return;
    }

    p_grp->GrpID = grp_id;
    OSGrpPtrTbl[i] = p_grp;
    RIN32_HWOS->FLG_TBL[p_grp->GrpID] = 0u;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_grp->NamePtr = p_name;
#endif

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(grp_id | DEF_BIT_16);                        /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_CRE_G);

    OS_HWOS_CMD_PROC();

    CPU_CRITICAL_EXIT();
    OS_HWOS_TASK_SYNC();

    switch (OSTaskRet[OSCtxIDCur].r0) {
        case 0x0001:                                            /* Success.                                             */
            *p_err = OS_ERR_NONE;
             break;

        case 0xFF83:                                            /* Already Created.                                     */
        case 0xFF84:                                            /* Invalid System Call.                                 */
        case 0xFF8F:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(;);
             break;
    }
}
#endif /* OS_CFG_FLAG_EN == DEF_ENABLED */


/*
*********************************************************************************************************
*                                     WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag group.  Your
*              application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
*                            The bits you want are specified by setting the corresponding bits in 'flags'.
*                            e.g. if your application wants to wait for bits 0 and 1 then 'flags' would contain 0x03.
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*              opt           specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                            You can specify the 'ONE' of the following arguments:
*
*                            OS_OPT_PEND_FLAG_CLR_ALL   You will wait for ALL bits in 'flags' to be clear (0)
*                            OS_OPT_PEND_FLAG_CLR_ANY   You will wait for ANY bit  in 'flags' to be clear (0)
*                            OS_OPT_PEND_FLAG_SET_ALL   You will wait for ALL bits in 'flags' to be set   (1)
*                            OS_OPT_PEND_FLAG_SET_ANY   You will wait for ANY bit  in 'flags' to be set   (1)
*
*                            You can 'ADD' OS_OPT_PEND_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                      the call.  Example, to wait for any flag in a group AND then clear
*                                      the flags that are present, set 'wait_opt' to:
*
*                                      OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME
*
*                            You can also 'ADD' the type of pend with 'ONE' of the two option:
*
*                            OS_OPT_PEND_NON_BLOCKING   Task will NOT block if flags are not available
*                            OS_OPT_PEND_BLOCKING       Task will     block if flags are not available
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the event flag group was
*                            posted, aborted or the event flag group deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid and
*                            indicates that you don't need the timestamp.
*
*              p_err         is a pointer to an error code and can be:
*
*                            OS_ERR_NONE                The desired bits have been set within the specified 'timeout'
*                            OS_ERR_OBJ_PTR_NULL        If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                            OS_ERR_OPT_INVALID         You didn't specify a proper 'opt' argument.
*                            OS_ERR_PEND_ABORT          The wait on the flag was aborted.
*                            OS_ERR_PEND_ISR            If you tried to PEND from an ISR
*                            OS_ERR_PEND_WOULD_BLOCK    If you specified non-blocking but the flags were not
*                                                       available.
*                            OS_ERR_SCHED_LOCKED        If you called this function when the scheduler is locked
*                            OS_ERR_TIMEOUT             The bit(s) have not been set in the specified 'timeout'.
*
* Returns    : The flags in the event flag group that made the task ready or, 0 if a timeout or an error
*              occurred.
*********************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_TICK       timeout,
                      OS_OPT        opt,
                      CPU_TS       *p_ts,
                      OS_ERR       *p_err)
{
    CPU_INT32U  r6_temp;
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

    if ((opt & OS_OPT_PEND_FLAG_CLR_ALL) || (opt & OS_OPT_PEND_FLAG_CLR_ALL)) {
       *p_err = OS_ERR_HWOS_UNSUPPORTED;
        return (0u);
    }

    if (p_grp == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }

    switch (opt) {                                          /* Validate 'opt'                                         */
        case OS_OPT_PEND_FLAG_CLR_ALL:
        case OS_OPT_PEND_FLAG_CLR_ANY:
        case OS_OPT_PEND_FLAG_SET_ALL:
        case OS_OPT_PEND_FLAG_SET_ANY:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {                      /* Make sure the flag was created                       */
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

    r6_temp  = (opt & OS_OPT_PEND_FLAG_CONSUME) ? DEF_BIT_00 : 0u;
    r6_temp |= (opt & OS_OPT_PEND_FLAG_SET_ALL) ? DEF_BIT_01 : 0u;

    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_grp->GrpID);                               /* HWOS Semaphore ID.                                   */
    OS_HWOS_SET_R5(timeout);                                    /* Timeout.                                             */
    OS_HWOS_SET_R6(r6_temp);                                    /* Pend options.                                        */
    OS_HWOS_SET_R7(flags);                                      /* Flag pattern.                                        */
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_WAI_G);



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
             return (0u);
             break;

        case 0xFF84:                                            /* Wait abort.                                          */
            *p_err = OS_ERR_PEND_ABORT;
             return (0u);
             break;

        case 0xFF92:                                            /* Deleted.                                             */
            *p_err = OS_ERR_OBJ_DEL;
             return (0u);
             break;

        case 0xFF80:                                            /* Invalid System Call.                                 */
        case 0xFF93:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(0u);
             break;
    }

    return (OSTaskRet[OSCtxIDCur].r1);
}


/*
*********************************************************************************************************
*                                                POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to set or clear are
*              specified by a 'bit mask'.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*
*                            OS_OPT_POST_FLAG_SET       set
*                            OS_OPT_POST_FLAG_CLR       cleared
*
*                            you can also 'add' OS_OPT_POST_NO_SCHED to prevent the scheduler from being called.
*
*              p_err         is a pointer to an error code and can be:
*
*                            OS_ERR_NONE                The call was successful
*                            OS_ERR_OBJ_PTR_NULL        You passed a NULL pointer
*                            OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                            OS_ERR_OPT_INVALID         You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*********************************************************************************************************
*/

OS_FLAGS  OSFlagPost (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_OPT        opt,
                      OS_ERR       *p_err)
{
    CPU_INT32U  ret_val;
    OS_FLAGS    flags_val;
    CPU_SR_ALLOC();


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (p_err == DEF_NULL) {                                    /* Validate err ptr.                                    */
        CPU_SW_EXCEPTION(0u);
    }

    if (p_grp == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }

    switch (opt) {                                              /* Validate 'opt'                                       */
        case OS_OPT_POST_FLAG_SET:
        case OS_OPT_POST_FLAG_CLR:
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {                      /* Make sure the flag was created                       */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif


    CPU_CRITICAL_ENTER();

                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_grp->GrpID);                               /* HWOS Group ID.                                       */
    OS_HWOS_SET_R5(flags);                                      /* Flag pattern.                                        */

    if (opt & OS_OPT_POST_FLAG_CLR) {
        OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_CLR_G);
    } else {
        OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_SET_G);
    }

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

        case 0xFF84:                                            /* Invalid Semaphore.                                   */
        case 0xFF85:                                            /* Invalid System Call.                                 */
        case 0xFF87:                                            /* Invalid System Call.                                 */
        case 0xFF80:                                            /* Invalid System Call.                                 */
        default:
             CPU_SW_EXCEPTION(0u);
             break;
    }

    flags_val = (RIN32_HWOS->FLG_TBL[p_grp->GrpID] >> 1u) & 0xFFFF;

    return (flags_val);
}


/*
*********************************************************************************************************
*                                     DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag group.
*
* Arguments  : p_grp     Pointer to the desired event flag group.
*
*              opt       Determines delete options as follows:
*
*                        OS_OPT_DEL_NO_PEND*          Deletes the event flag group ONLY if no task pending
*                        OS_OPT_DEL_ALWAYS*           Deletes the event flag group even if tasks are waiting.
*                                                     In this case, all the tasks pending will be readied.
*
*                      * Both options ignored by the HWOS, the semaphore will always be deleted.
*
*              p_err     Pointer to an error code that can contain one of the following values:
*
*                        OS_ERR_NONE                  The call was successful and the event flag group was deleted
*                        OS_ERR_DEL_ISR               If you attempted to delete the event flag group from an ISR
*                        OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                        OS_ERR_OBJ_TYPE              If you didn't pass a pointer to an event flag group
*                        OS_ERR_OPT_INVALID           An invalid option was specified
*
* Returns    : 0u, kept for compatibility.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the event flag
*                 group MUST check the return code of OSFlagPost and OSFlagPend().
*********************************************************************************************************
*/


#if ((OS_CFG_FLAG_EN == DEF_ENABLED) && (OS_CFG_FLAG_DEL_EN == DEF_ENABLED))
OS_OBJ_QTY  OSFlagDel (OS_FLAG_GRP  *p_grp,
                       OS_OPT        opt,
                       OS_ERR       *p_err)
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

    if (p_grp == DEF_NULL) {
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {                      /* Make sure the flag was created                       */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif


    CPU_CRITICAL_ENTER();

    OSGrpPtrTbl[p_grp->GrpID] = DEF_NULL;                       /* Clear the group entry.                               */

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_grp->Type = OS_OBJ_TYPE_NONE;
#endif


                                                                /* ---------------- BUILD HWOS COMMAND ---------------- */
    OS_HWOS_SET_R4(p_grp->GrpID);
    OS_HWOS_SET_SYSC(OS_HWOS_TSFNC_DEL_G);

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
#endif /* (OS_CFG_FLAG_EN == DEF_ENABLED) && (OS_CFG_FLAG_DEL_EN == DEF_ENABLED) */
