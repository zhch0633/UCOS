/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                           Renesas RIN32M3
*                                               on the
*
*                                             R-IN32M3-EC
*                                          Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : JBL
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT

#include  <cpu.h>

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


#define  BSP_CPU_CLK_FREQ  100000000u


void        BSP_Init       (void);

void        BSP_IntEnAll   (void);

void        BSP_IntDisAll  (void);

void        BSP_IntEn      (CPU_DATA       int_id);

void        BSP_IntHandler (CPU_DATA       int_id);

void        BSP_IntInit    (void);

void        BSP_IntEn      (CPU_DATA       int_id);

void        BSP_IntDis     (CPU_DATA       int_id);

void        BSP_IntClr     (CPU_DATA       int_id);

void        BSP_IntVectSet (CPU_DATA       int_id,
                            CPU_FNCT_VOID  isr);

void        BSP_IntPrioSet (CPU_DATA       int_id,
                            CPU_INT08U     prio);

void        BSP_Init(void);

void        BSP_Led_Init(void);

void        BSP_Led_On  (CPU_INT08U led);
void        BSP_Led_Off (CPU_INT08U led);

CPU_INT32U  BSP_CPU_ClkFreq(void);

#endif

