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
*                                                HW ISR
*
* Filename      : os_hwisr.c
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
*                                       SET A SEMAPHORE POST HW ISR
*
* Description: Configure an hardware ISR to perform a semaphore post.
*
* Arguments  : p_sem         Pointer to the semaphore.
*
*              qint_id       QINT signal to configure (0-31).
*
*              opt           Determine whether to enable the HW ISR before returning. HW ISR are enabled
*                            by default.
*
*                            OS_OPT_QINT_ENABLED
*                            OS_OPT_QINT_DISABLED
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                  The call was successful.
*                            OS_ERR_CREATE_ISR            Can't configure an HW-ISR from an ISR.
*                            OS_ERR_OBJ_PTR_NULL          p_sem is Null.
*                            OS_ERR_HWOS_QINT_ID_INVALID  Invalid QUINT signal id.
*                            OS_ERR_HWOS_INT_ID_INVALID   Invalid interrupt id.
*                            OS_ERR_OPT_INVALID           Invalid options.
*                            OS_ERR_OBJ_TYPE              p_sem does not point to a semaphore.
*                            OS_ERR_HWOS_QINT_CFG         QINT signal id is already configured.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSHWISRSemPostSet (OS_SEM       *p_sem,
                         OS_QINT_ID    qint_id,
                         OS_INT_ID     int_id,
                         OS_OPT        opt,
                         OS_ERR       *p_err)
{
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

    if (p_sem == DEF_NULL) {                                    /* Validate p_sem.                                      */
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }

    if (qint_id > OS_HWOS_HWISR_CNT) {                          /* Validate QINT ID.                                     */
       *p_err = OS_ERR_HWOS_QINT_ID_INVALID;                    /* Validate ISR ID.                                      */
        return;
    }

    if (int_id > 255u) {
       *p_err = OS_ERR_HWOS_INT_ID_INVALID;
        return;
    }

    switch (opt) {                                              /* Validate 'opt'                                       */
        case OS_OPT_QINT_ENABLED:
        case OS_OPT_QINT_DISABLED:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {                       /* Check the object type.                               */
       *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();

    if (RIN32_HWOS->HWISR.HWISP_OPR[qint_id] != 0u &&
        RIN32_HWOS->QINTSEL[qint_id]         != 0u) {           /* QINT must be cleared.                                */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }

    if ((RIN32_HWOS->HWISR.QINT & (1u << qint_id)) != 0u) {     /* QINT must be disabled.                               */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }


    RIN32_HWOS->QINTSEL[qint_id] = int_id | 0x80;               /* Configure assigned ISR.                              */

                                                                /* Configure QINT operation.                            */
    RIN32_HWOS->HWISR.HWISP_OPR[0]  = (OS_HWOS_HWISR_TYPE_SIG_SEM << 24) |
                                      (p_sem->SemID)                     |
                                      (7 << 8);

    if ((opt & OS_OPT_QINT_DISABLED) == 0u) {
        RIN32_HWOS->HWISR.QINT |= 1u << qint_id;                /* Enable QINT.                                         */
    }

    CPU_CRITICAL_EXIT();

   *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                         SET A FLAG POST HW ISR
*
* Description: Configure an hardware ISR to perform a flag post.
*
* Arguments  : p_grp         Pointer to the flag.
*
*              flags         Flags to set.
*
*              qint_id       QINT signal to configure (0-31).
*
*              opt           Determine whether to enable the HW ISR before returning. HW ISR are enabled
*                            by default.
*
*                            OS_OPT_QINT_ENABLED
*                            OS_OPT_QINT_DISABLED
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                  The call was successful.
*                            OS_ERR_CREATE_ISR            Can't configure an HW-ISR from an ISR.
*                            OS_ERR_OBJ_PTR_NULL          p_grp is NULL.
*                            OS_ERR_HWOS_QINT_ID_INVALID  Invalid QUINT signal id.
*                            OS_ERR_HWOS_INT_ID_INVALID   Invalid interrupt id.
*                            OS_ERR_OPT_INVALID           Invalid options.
*                            OS_ERR_OBJ_TYPE              p_sem does not point to a semaphore.
*                            OS_ERR_HWOS_QINT_CFG         QINT signal id is already configured.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSHWISRFlagPostSet (OS_FLAG_GRP  *p_grp,
                          OS_FLAGS      flags,
                          OS_QINT_ID    qint_id,
                          OS_INT_ID     int_id,
                          OS_OPT        opt,
                          OS_ERR       *p_err)
{
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

    if (p_grp == DEF_NULL) {                                    /* Validate p_grp.                                       */
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }

    if (qint_id > OS_HWOS_HWISR_CNT) {                          /* Validate QINT ID.                                     */
       *p_err = OS_ERR_HWOS_QINT_ID_INVALID;                    /* Validate ISR ID.                                      */
        return;
    }

    if (int_id > 255u) {
       *p_err = OS_ERR_HWOS_INT_ID_INVALID;
        return;
    }

    switch (opt) {                                              /* Validate 'opt'                                       */
        case OS_OPT_QINT_ENABLED:
        case OS_OPT_QINT_DISABLED:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {                      /* Check the object type.                               */
       *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();

    if (RIN32_HWOS->HWISR.HWISP_OPR[qint_id] != 0u &&
        RIN32_HWOS->QINTSEL[qint_id]         != 0u) {           /* QINT must be cleared.                                */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }

    if ((RIN32_HWOS->HWISR.QINT & (1u << qint_id)) != 0u) {     /* QINT must be disabled.                               */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }


    RIN32_HWOS->QINTSEL[qint_id] = int_id | 0x80;               /* Configure assigned ISR.                              */

                                                                /* Configure QINT operation.                            */
    RIN32_HWOS->HWISR.HWISP_OPR[0]  = (OS_HWOS_HWISR_TYPE_SET_FLAG << 24) |
                                      (p_grp->GrpID)                      |
                                      (flags << 8);

    if ((opt & OS_OPT_QINT_DISABLED) == 0u) {
        RIN32_HWOS->HWISR.QINT |= 1u << qint_id;                /* Enable QINT.                                         */
    }

    CPU_CRITICAL_EXIT();

   *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                         SET A TASK RESUME HW ISR
*
* Description: Configure an hardware ISR to perform a task resume.
*
* Arguments  : p_tcb         Pointer to the task.
*
*              qint_id       QINT signal to configure (0-31).
*
*              opt           Determine whether to enable the HW ISR before returning. HW ISR are enabled
*                            by default.
*
*                            OS_OPT_QINT_ENABLED
*                            OS_OPT_QINT_DISABLED
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                  The call was successful.
*                            OS_ERR_CREATE_ISR            Can't configure an HW-ISR from an ISR.
*                            OS_ERR_OBJ_PTR_NULL          p_tcb is NULL.
*                            OS_ERR_HWOS_QINT_ID_INVALID  Invalid QUINT signal id.
*                            OS_ERR_HWOS_INT_ID_INVALID   Invalid interrupt id.
*                            OS_ERR_OPT_INVALID           Invalid options.
*                            OS_ERR_OBJ_TYPE              p_sem does not point to a semaphore.
*                            OS_ERR_HWOS_QINT_CFG         QINT signal id is already configured.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSHWISRTaskResumeSet (OS_TCB       *p_tcb,
                            OS_QINT_ID    qint_id,
                            OS_INT_ID     int_id,
                            OS_OPT        opt,
                            OS_ERR       *p_err)
{
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

    if (p_tcb == DEF_NULL) {                                    /* Validate p_tcb.                                      */
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }

    if (qint_id > OS_HWOS_HWISR_CNT) {                          /* Validate QINT ID.                                     */
       *p_err = OS_ERR_HWOS_QINT_ID_INVALID;                    /* Validate ISR ID.                                      */
        return;
    }

    if (int_id > 255u) {
       *p_err = OS_ERR_HWOS_INT_ID_INVALID;
        return;
    }

    switch (opt) {                                              /* Validate 'opt'                                       */
        case OS_OPT_QINT_ENABLED:
        case OS_OPT_QINT_DISABLED:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }
#endif


#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_tcb->Type != OS_OBJ_TYPE_TASK) {                      /* Check the object type.                               */
       *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();

    if (RIN32_HWOS->HWISR.HWISP_OPR[qint_id] != 0u &&
        RIN32_HWOS->QINTSEL[qint_id]         != 0u) {           /* QINT must be cleared.                                */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }

    if ((RIN32_HWOS->HWISR.QINT & (1u << qint_id)) != 0u) {     /* QINT must be disabled.                               */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }


    RIN32_HWOS->QINTSEL[qint_id] = int_id | 0x80;               /* Configure assigned ISR.                              */

                                                                /* Configure QINT operation.                            */
    RIN32_HWOS->HWISR.HWISP_OPR[0]  = (OS_HWOS_HWISR_TYPE_WUP_TSK << 24) |
                                      (p_tcb->CtxID)                     |
                                      (7 << 8);

    if ((opt & OS_OPT_QINT_DISABLED) == 0u) {
        RIN32_HWOS->HWISR.QINT |= 1u << qint_id;                /* Enable QINT.                                         */
    }

    CPU_CRITICAL_EXIT();

   *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                         ENABLED AN HW ISR
*
* Description: Enable an hardware ISR.
*
* Arguments  : qint_id       QINT signal to enable (0-31).
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                  The call was successful.
*                            OS_ERR_CREATE_ISR            Can't configure an HW-ISR from an ISR.
*                            OS_ERR_HWOS_QINT_ID_INVALID  Invalid QUINT signal id.
*                            OS_ERR_HWOS_QINT_CFG         QINT signal id is not configured.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSHWISREn (OS_QINT_ID   qint_id,
                 OS_ERR      *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_CREATE_ISR;
        return;
    }
#endif


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (qint_id > OS_HWOS_HWISR_CNT) {                          /* Validate QINT ID.                                    */
       *p_err = OS_ERR_HWOS_QINT_ID_INVALID;                    /* Validate ISR ID.                                     */
        return;
    }
#endif


    CPU_CRITICAL_ENTER();

    if (RIN32_HWOS->HWISR.HWISP_OPR[qint_id] == 0u) {           /* QINT must be configured.                             */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }

    if ((RIN32_HWOS->HWISR.QINT & (1u << qint_id)) != 0u) {     /* QINT must be disabled.                               */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }

    RIN32_HWOS->HWISR.QINT |= 1u << qint_id;                    /* Enable QINT.                                         */

    CPU_CRITICAL_EXIT();

    *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                         DISABLE AN HW ISR
*
* Description: Disable an hardware ISR.
*
* Arguments  : qint_id       QINT signal to disable (0-31).
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                  The call was successful.
*                            OS_ERR_CREATE_ISR            Can't configure an HW-ISR from an ISR.
*                            OS_ERR_HWOS_QINT_ID_INVALID  Invalid QUINT signal id.
*                            OS_ERR_HWOS_QINT_CFG         QINT signal id is not configured or already
*                                                         disabled.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSHWISRDis (OS_QINT_ID   qint_id,
                  OS_ERR      *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_CREATE_ISR;
        return;
    }
#endif


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (qint_id > OS_HWOS_HWISR_CNT) {                          /* Validate QINT ID.                                    */
       *p_err = OS_ERR_HWOS_QINT_ID_INVALID;                    /* Validate ISR ID.                                     */
        return;
    }
#endif

    CPU_CRITICAL_ENTER();

    if (RIN32_HWOS->HWISR.HWISP_OPR[qint_id] == 0u) {           /* QINT must be configured.                             */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }

    if ((RIN32_HWOS->HWISR.QINT & (1u << qint_id)) == 0u) {     /* QINT must be enabled.                                */
       *p_err = OS_ERR_HWOS_QINT_CFG;
        CPU_CRITICAL_EXIT();
        return;
    }

    RIN32_HWOS->HWISR.QINT &= ~(1u << qint_id);                 /* Enable QINT.                                         */

    CPU_CRITICAL_EXIT();

   *p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                         CLEAR AN HW ISR
*
* Description: Unconfigure an hardware ISR.
*
* Arguments  : qint_id       QINT signal to clear (0-31).
*
*              p_err         is a pointer to a variable that will contain an error code returned by this
*                            function.
*
*                            OS_ERR_NONE                  The call was successful.
*                            OS_ERR_CREATE_ISR            Can't configure an HW-ISR from an ISR.
*                            OS_ERR_HWOS_QINT_ID_INVALID  Invalid QUINT signal id.
*
* Returns    : none
*********************************************************************************************************
*/

void  OSHWISRClr (OS_QINT_ID   qint_id,
                  OS_ERR      *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_CREATE_ISR;
        return;
    }
#endif


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)                          /* ---------------- VALIDATE ARGUMENTS ---------------- */
    if (qint_id > OS_HWOS_HWISR_CNT) {                          /* Validate QINT ID.                                    */
       *p_err = OS_ERR_HWOS_QINT_ID_INVALID;                    /* Validate ISR ID.                                     */
        return;
    }
#endif

    CPU_CRITICAL_ENTER();

    if ((RIN32_HWOS->HWISR.QINT & (1u << qint_id)) != 0u) {     /* Disable QINT if enabled.                             */
        RIN32_HWOS->HWISR.QINT &= ~(1u << qint_id);
    }

    RIN32_HWOS->QINTSEL[qint_id] = 0u;                          /* Clear assigned ISR.                                  */

    RIN32_HWOS->HWISR.HWISP_OPR[0]  = 0u;                       /* Clear QINT operation.                                */


    CPU_CRITICAL_EXIT();

   *p_err = OS_ERR_NONE;
}
