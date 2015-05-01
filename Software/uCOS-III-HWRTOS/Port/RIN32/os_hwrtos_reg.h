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
* Filename      : os_hwos_reg.h
* Version       : V1.00.00.00
* Programmer(s) : JBL
*********************************************************************************************************
*/

#include  <cpu.h>

typedef struct
{
    CPU_REG32 CNTX_TYPE;
    CPU_REG32 PRTY;
    CPU_REG32 CNTX_STAT;
    CPU_REG32 INIT_ADD;
    CPU_REG32 INIT_R14;
    CPU_REG32 WT_RSN;
    CPU_REG32 SMPH_ID;
    CPU_REG32 EVNT_ID;
    CPU_REG32 EVNT_FLG;
    CPU_REG32 FLG_CND;
    CPU_REG32 WT_TIMEOUT;
    CPU_REG32 AT_CLR;
    CPU_REG32 QUE_ODR;
    CPU_REG32 HWFUNC_ID;
    CPU_REG32 WUP_RQ_QUE;
    CPU_REG32 TO_RQ_ODR;
} HWOS_CONTEXT;

typedef struct
{
    CPU_REG32   EXCPT_MODE;
    CPU_REG32   EXCPT_CNTX;
    CPU_REG32   EXCPT_ADD;
    CPU_REG32   DEBUG_CNTX;
    CPU_REG32   DEBUG_ADD;
    CPU_REG32   WT_TO_PRESCL;
    CPU_REG32   SYS_TIM;
} HWOS_COMMON;

// ------ HW-ISR ------
typedef struct
{
    CPU_REG32   HWISR_PNTR[8];
    CPU_REG32   RESERVED0[56];
    CPU_REG32   HWISP_OPR[32];
    CPU_REG32   RESERVED1[32];
    CPU_REG32   QINT;
} HWOS_HWISR;

// ------ CPU I/F ------
typedef struct
{
    CPU_REG32   SYSC;
    CPU_REG32   R4;
    CPU_REG32   R5;
    CPU_REG32   R6;
    CPU_REG32   R7;
    CPU_REG32   CMD;
    CPU_REG32   RESERVED0[2];
    CPU_REG32   R0;
    CPU_REG32   R1;
} HWOS_CPUIF;

typedef struct
{
    HWOS_CONTEXT            CNTX[64];        
    CPU_REG32               RESERVED0[0x0C00];
    HWOS_COMMON             CMN;             
    CPU_REG32               RESERVED1[0x0FF9];
    CPU_REG32               SEM_TBL[128];
    CPU_REG32               RESERVED2[0x0080];
    CPU_REG32               FLG_TBL[64];
    CPU_REG32               RESERVED3[0x00C0];
    HWOS_HWISR              HWISR;           
    CPU_REG32               RESERVED4[0x007F];
    CPU_REG32               MBX_TBL[64];
    CPU_REG32               RESERVED5[0x00C0];
    CPU_REG32               MBX_ID_TBL[192];
    CPU_REG32               RESERVED6[0x1740];
    HWOS_CPUIF              CPUIF;           
    CPU_REG32               RESERVED7[0x07F6];
    CPU_REG32               QINTSEL[32];
    CPU_REG32               RESERVED8[0x0020];
    CPU_REG32               BUFID;
} RIN32_HWOS;

#define RIN32_HWOS        ((RIN32_HWOS *) (0x40000000UL + 0x80000UL))
