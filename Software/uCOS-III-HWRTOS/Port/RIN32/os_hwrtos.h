
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
*                                             HWOS HEADER
*
* Filename      : os_hwos.h
* Version       : V1.00.00.00
* Programmer(s) : JBL
*********************************************************************************************************
*/


#ifndef   OS_HWOS_H
#define   OS_HWOS_H

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <intrinsics.h>

#include "os_hwrtos_reg.h"


/*
*********************************************************************************************************
*                                            HWOS CONSTANTS
*********************************************************************************************************
*/

#define   OS_HWOS_CNTX_CNT    64u
#define   OS_HWOS_SEM_CNT     128u
#define   OS_HWOS_SEM_VAL_MAX 31u
#define   OS_HWOS_FLAG_CNT    64u
#define   OS_HWOS_Q_CNT       64u
#define   OS_HWOS_MAX_PRIO    15u
#define   OS_HWOS_CNTX_NONE   64u
#define   OS_HWOS_HWISR_CNT   32u


/*
*********************************************************************************************************
*                                            RESPONSE MASKS
*********************************************************************************************************
*/

#define   OS_HWOS_RESP_MASK 0x0000FFFFu
#define   OS_HWOS_CTX_MASK  0x00FF0000u
#define   OS_HWOS_CTX_REQ   0x80000000u


/*
*********************************************************************************************************
*                                            CONTEXT STATUS
*********************************************************************************************************
*/

#define  OS_HWOS_CNTX_STAT_IDLE 0x00u
#define  OS_HWOS_CNTX_STAT_STOP 0x01u
#define  OS_HWOS_CNTX_STAT_WAIT 0x02u
#define  OS_HWOS_CNTS_STAT_RDY  0x03u
#define  OS_HWOS_CNTX_STAT_RUN  0x05u


/*
*********************************************************************************************************
*                                             HWISR TYPES
*********************************************************************************************************
*/

#define  OS_HWOS_HWISR_TYPE_SET_FLAG 1u
#define  OS_HWOS_HWISR_TYPE_SIG_SEM  2u
#define  OS_HWOS_HWISR_TYPE_REL_WAI  3u
#define  OS_HWOS_HWISR_TYPE_WUP_TSK  4u


/*
*********************************************************************************************************
*                                              COMMANDS
*********************************************************************************************************
*/

#define  OS_HWOS_TSFNC_EXT_TSK  0x4000u
#define  OS_HWOS_TSFNC_STA_TKS  0x4001u
#define  OS_HWOS_TSFNC_TER_TSK  0x4013u
#define  OS_HWOS_TSFNC_CHG_PRI  0x400Eu

#define  OS_HWOS_TSFNC_CRE_G    0x4002u
#define  OS_HWOS_TSFNC_DEL_G    0x4003u
#define  OS_HWOS_TSFNC_WAI_G    0x4009u
#define  OS_HWOS_TSFNC_POL_G    0x400Au
#define  OS_HWOS_TSFNC_SET_G    0x400Bu
#define  OS_HWOS_TSFNC_CLR_G    0x400Du

#define  OS_HWOS_TSFNC_CRE_SEM  0x4004u
#define  OS_HWOS_TSFNC_DEL_SEM  0x4005u
#define  OS_HWOS_TSFNC_WAI_SEM  0x4006u
#define  OS_HWOS_TSFNC_SIG_SEM  0x4007u

#define  OS_HWOS_TSFNC_CRE_MBX  0x401Au
#define  OS_HWOS_TSFNC_DEL_MBX  0x401Bu
#define  OS_HWOS_TSFNC_SND_MBX  0x401Cu
#define  OS_HWOS_TSFNC_RCV_MBX  0x401Du

#define  OS_HWOS_TSFNC_LOC_CPU  0x400Fu
#define  OS_HWOS_TSFNC_UNL_CPU  0x4010u
#define  OS_HWOS_TSFNC_DIS_DSP  0x4011u
#define  OS_HWOS_TSFNC_ENA_DSP  0x4012u
#define  OS_HWOS_TSFNC_GET_TID  0x4019u
#define  OS_HWOS_TSFNC_ROT_RDQ  0x401Eu

#define  OS_HWOS_TSFNC_REL_WAI  0x4014u
#define  OS_HWOS_TSFNC_SLP_TSK  0x4015u
#define  OS_HWOS_TSFNC_WUP_TSK  0x4016u

#define  OS_HWOS_TSFNC_SET_TIM  0x4017u
#define  OS_HWOS_TSFNC_GET_TIM  0x4018u


/*
*********************************************************************************************************
*                                         HWOS ACCESS MACROS
*********************************************************************************************************
*/

#define  OS_HWOS_SET_R4(val) do { RIN32_HWOS->CPUIF.R4 = (val); } while (0)
#define  OS_HWOS_SET_R5(val) do { RIN32_HWOS->CPUIF.R5 = (val); } while (0)
#define  OS_HWOS_SET_R6(val) do { RIN32_HWOS->CPUIF.R6 = (val); } while (0)
#define  OS_HWOS_SET_R7(val) do { RIN32_HWOS->CPUIF.R7 = (val); } while (0)
#define  OS_HWOS_SET_SYSC(val) do { RIN32_HWOS->CPUIF.SYSC = (val); } while (0)

#define  OS_HWOS_GET_R0() RIN32_HWOS->CPUIF.R0

#define  OS_HWOS_GET_CNTX_STAT(cntx) RIN32_HWOS->CNTX[(cntx)].CNTX_STAT

#define  OS_HWOS_GET_SEM_CNT(sem) ((RIN32_HWOS->SEM_TBL[sem] & 0x3Eu) >> 1u)

#define  OS_HWOS_GET_SYS_TIM() RIN32_HWOS->CMN.SYS_TIM
#define  OS_HWOS_SET_SYS_TIM(time) do { RIN32_HWOS->CMN.SYS_TIM = time; } while (0)


#define  OS_HWOS_TASK_SW() do { *((CPU_REG32 *)0xE000ED04) = 0x10000000;} while (0)
#define  OS_HWOS_TASK_SYNC() do { __ISB(); } while (0)


/*
*********************************************************************************************************
*                                      COMMAND PROCESSING MACROS
*********************************************************************************************************
*/

#define  OS_HWOS_CMD_PROC()   do { \
    CPU_INT32U   r0; \
    CPU_INT32U   r1; \
    CPU_INT32U   new_ctx_id; \
 \
    while(DEF_TRUE) { \
 \
        r0 = RIN32_HWOS->CPUIF.R0; \
        if(r0 & 0x20000000u) { \
            break; \
        } \
    } \
    r1 = RIN32_HWOS->CPUIF.R1; \
     \
    if(r0 & OS_HWOS_CTX_REQ) { \
        new_ctx_id = (r0 & OS_HWOS_CTX_MASK) >> 16u; \
        OSHWOSPendCtxSw = new_ctx_id; \
        if((r0 & OS_HWOS_RESP_MASK) != 0u) { \
            OSTaskRet[new_ctx_id].r0 = (r0 & OS_HWOS_RESP_MASK); \
            OSTaskRet[new_ctx_id].r1 = r1; \
        } \
        OS_HWOS_TASK_SW(); \
    } else { \
        OSTaskRet[OSCtxIDCur].r0 = (r0 & OS_HWOS_RESP_MASK); \
        OSTaskRet[OSCtxIDCur].r1 = r1; \
    } \
} while (0)


#define  OS_HWOS_CMD_PROC_ISR(ret_val)   do { \
    CPU_INT32U   r0; \
    CPU_INT32U   r1; \
    CPU_INT32U   new_ctx_id; \
 \
    while(DEF_TRUE) { \
        ret_val = 0u; \
        r0 = RIN32_HWOS->CPUIF.R0; \
        if(r0 & 0x20000000u) { \
            break; \
        } \
    } \
    r1 = RIN32_HWOS->CPUIF.R1; \
     \
    if(r0 & OS_HWOS_CTX_REQ) { \
        new_ctx_id = (r0 & OS_HWOS_CTX_MASK) >> 16u; \
        OSHWOSPendCtxSw = new_ctx_id; \
        if((r0 & OS_HWOS_RESP_MASK) != 0u) { \
            OSTaskRet[new_ctx_id].r0 = (r0 & OS_HWOS_RESP_MASK); \
            OSTaskRet[new_ctx_id].r1 = r1; \
        } \
        if(OSIntNestingCtr != 0u) { \
            ret_val = 0x01u; \
        } \
        OS_HWOS_TASK_SW(); \
    } else { \
        if((OSIntNestingCtr == 0u) && ((r0 & OS_HWOS_RESP_MASK) != 0u)) { \
            OSTaskRet[OSCtxIDCur].r0 = (r0 & OS_HWOS_RESP_MASK); \
            OSTaskRet[OSCtxIDCur].r1 = r1; \
        } else { \
            ret_val = (r0 & OS_HWOS_RESP_MASK); \
        } \
    } \
} while (0)


/*
*********************************************************************************************************
*                                           PORT FUNCTIONS
*********************************************************************************************************
*/

void  OS_CPU_SysTickInit (CPU_INT32U  cnts);


#endif
