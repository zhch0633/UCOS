/**
 *******************************************************************************
 * @file  vectors_M.c
 * @brief sample program for R-IN32M3
 *
 * @note
 * Copyright (C) 2012 Renesas Electronics Corporation
 * Copyright (C) 2012 Renesas Micro Systems Co., Ltd.
 *
 * @par
 *  This is a sample program.
 *  Renesas Electronics assumes no responsibility for any losses incurred.
 *
 *******************************************************************************
 */

#include  <cpu.h>


#pragma language=extended
#pragma segment="CSTACK"



/*============================================================================*/
/* I N C L U D E                                                              */
/*============================================================================*/

/*============================================================================*/
/* D E F I N E                                                                */
/*============================================================================*/

/*============================================================================*/
/* E X T E R N                                                                */
/*============================================================================*/
extern void __iar_program_start( void );

typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/

void TAUJ0I0_IRQHandler(void)     ;
void TAUJ0I1_IRQHandler(void)     ;
void TAUJ0I2_IRQHandler(void)     ;
void TAUJ0I3_IRQHandler(void)     ;
void UAJ0TIT_IRQHandler(void)     ;
void UAJ0TIR_IRQHandler(void)     ;
void UAJ1TIT_IRQHandler(void)     ;
void UAJ1TIR_IRQHandler(void)     ;
void CSIH0IC_IRQHandler(void)     ;
void CSIH0IR_IRQHandler(void)     ;
void CSIH0IJC_IRQHandler(void)    ;
void CSIH1IC_IRQHandler(void)     ;
void CSIH1IR_IRQHandler(void)     ;
void CSIH1IJC_IRQHandler(void)    ;
void IICB0TIA_IRQHandler(void)    ;
void IICB1TIA_IRQHandler(void)    ;
void FCN0REC_IRQHandler(void)     ;
void FCN0TRX_IRQHandler(void)     ;
void FCN0WUP_IRQHandler(void)     ;
void FCN1REC_IRQHandler(void)     ;
void FCN1TRX_IRQHandler(void)     ;
void FCN1WUP_IRQHandler(void)     ;
void DMA00_IRQHandler(void)       ;
void DMA01_IRQHandler(void)       ;
void DMA02_IRQHandler(void)       ;
void DMA03_IRQHandler(void)       ;
void DMA10_IRQHandler(void)       ;
void CATSYNC0_IRQHandler(void)    ;
void CATSYNC1_IRQHandler(void)    ;
void CAT_IRQHandler(void)         ;
void CATSOF_IRQHandler(void)      ;
void CATEOF_IRQHandler(void)      ;
void BUFDMA_IRQHandler(void)      ;
void PHY0_IRQHandler(void)        ;
void PHY1_IRQHandler(void)        ;
void ETHMII_IRQHandler(void)      ;
void ETHPAUSE_IRQHandler(void)    ;
void ETHTX_IRQHandler(void)       ;
void ETHSW_IRQHandler(void)       ;
void ETHSWDLR_IRQHandler(void)    ;
void ETHSWSEC_IRQHandler(void)    ;
void ETHRXFIFO_IRQHandler(void)   ;
void ETHTXFIFO_IRQHandler(void)   ;
void ETHRXDMA_IRQHandler(void)    ;
void ETHTXDMA_IRQHandler(void)    ;
void MACDMARXFRM_IRQHandler(void) ;
void HOSTIF_IRQHandler(void)      ;
void INTPZ0_IRQHandler(void)      ;
void INTPZ1_IRQHandler(void)      ;
void INTPZ2_IRQHandler(void)      ;
void INTPZ3_IRQHandler(void)      ;
void INTPZ4_IRQHandler(void)      ;
void INTPZ5_IRQHandler(void)      ;
void INTPZ6_IRQHandler(void)      ;
void INTPZ7_IRQHandler(void)      ;
void INTPZ8_IRQHandler(void)      ;
void INTPZ9_IRQHandler(void)      ;
void INTPZ10_IRQHandler(void)     ;
void INTPZ11_IRQHandler(void)     ;
void INTPZ12_IRQHandler(void)     ;
void INTPZ13_IRQHandler(void)     ;
void INTPZ14_IRQHandler(void)     ;
void INTPZ15_IRQHandler(void)     ;
void INTPZ16_IRQHandler(void)     ;
void INTPZ17_IRQHandler(void)     ;
void INTPZ18_IRQHandler(void)     ;
void INTPZ19_IRQHandler(void)     ;
void INTPZ20_IRQHandler(void)     ;
void INTPZ21_IRQHandler(void)     ;
void INTPZ22_IRQHandler(void)     ;
void INTPZ23_IRQHandler(void)     ;
void INTPZ24_IRQHandler(void)     ;
void INTPZ25_IRQHandler(void)     ;
void INTPZ26_IRQHandler(void)     ;
void INTPZ27_IRQHandler(void)     ;
void INTPZ28_IRQHandler(void)     ;
void HWRTOS_IRQHandler(void)      ;
void BRAMERR_IRQHandler(void)     ;
void IICB0TIS_IRQHandler(void)    ;
void IICB1TIS_IRQHandler(void)    ;
void WDTA0_IRQHandler(void)       ;
void SFLASH_IRQHandler(void)      ;
void UAJ0TIS_IRQHandler(void)     ;
void UAJ1TIS_IRQHandler(void)     ;
void CSIH0IRE_IRQHandler(void)    ;
void CSIH1IRE_IRQHandler(void)    ;
void FCN0ERR_IRQHandler(void)     ;
void FCN1ERR_IRQHandler(void)     ;
void DERR0_IRQHandler(void)       ;
void DERR1_IRQHandler(void)       ;
void ETHTXFIFOERR_IRQHandler(void);
void ETHRXERR_IRQHandler(void)    ;
void ETHRXDERR_IRQHandler(void)   ;
void ETHTXDERR_IRQHandler(void)   ;
void BUFDMAERR_IRQHandler(void)   ;
void ECATWDT_IRQHandler(void)     ;
void ECATRST_IRQHandler(void)     ;


#pragma call_graph_root = "interrupt"
__weak void NMI_Handler(void)            { while(1){}; }
#pragma call_graph_root = "interrupt"
__weak void HardFault_Handler(void)      { while(1){}; }
#pragma call_graph_root = "interrupt"
__weak void SVC_Handler(void)            { while(1){}; }
#pragma call_graph_root = "interrupt"
__weak void DebugMon_Handler(void)       { while(1){}; }
#if 0
#pragma call_graph_root = "interrupt"
__weak void PendSV_Handler(void)         { while(1){}; }
#pragma call_graph_root = "interrupt"
__weak void SysTick_Handler(void)        { while(1){}; }
#pragma call_graph_root = "interrupt"
#endif
__weak void MemManageFault_Handler(void) { while(1){}; }
#pragma call_graph_root = "interrupt"
__weak void BusFault_Handler(void)       { while(1){}; }
#pragma call_graph_root = "interrupt"
__weak void UsageFault_Handler(void)     { while(1){}; }

void Default_IRQHandler(void)
{
    while(1) {
    };
}

__weak void TAUJ0I0_IRQHandler(void)     { Default_IRQHandler(); }
__weak void TAUJ0I1_IRQHandler(void)     { Default_IRQHandler(); }
__weak void TAUJ0I2_IRQHandler(void)     { Default_IRQHandler(); }
__weak void TAUJ0I3_IRQHandler(void)     { Default_IRQHandler(); }
__weak void UAJ0TIT_IRQHandler(void)     { Default_IRQHandler(); }
__weak void UAJ0TIR_IRQHandler(void)     { Default_IRQHandler(); }
__weak void UAJ1TIT_IRQHandler(void)     { Default_IRQHandler(); }
__weak void UAJ1TIR_IRQHandler(void)     { Default_IRQHandler(); }
__weak void CSIH0IC_IRQHandler(void)     { Default_IRQHandler(); }
__weak void CSIH0IR_IRQHandler(void)     { Default_IRQHandler(); }
__weak void CSIH0IJC_IRQHandler(void)    { Default_IRQHandler(); }
__weak void CSIH1IC_IRQHandler(void)     { Default_IRQHandler(); }
__weak void CSIH1IR_IRQHandler(void)     { Default_IRQHandler(); }
__weak void CSIH1IJC_IRQHandler(void)    { Default_IRQHandler(); }
__weak void IICB0TIA_IRQHandler(void)    { Default_IRQHandler(); }
__weak void IICB1TIA_IRQHandler(void)    { Default_IRQHandler(); }
__weak void FCN0REC_IRQHandler(void)     { Default_IRQHandler(); }
__weak void FCN0TRX_IRQHandler(void)     { Default_IRQHandler(); }
__weak void FCN0WUP_IRQHandler(void)     { Default_IRQHandler(); }
__weak void FCN1REC_IRQHandler(void)     { Default_IRQHandler(); }
__weak void FCN1TRX_IRQHandler(void)     { Default_IRQHandler(); }
__weak void FCN1WUP_IRQHandler(void)     { Default_IRQHandler(); }
__weak void DMA00_IRQHandler(void)       { Default_IRQHandler(); }
__weak void DMA01_IRQHandler(void)       { Default_IRQHandler(); }
__weak void DMA02_IRQHandler(void)       { Default_IRQHandler(); }
__weak void DMA03_IRQHandler(void)       { Default_IRQHandler(); }
__weak void DMA10_IRQHandler(void)       { Default_IRQHandler(); }
__weak void CATSYNC0_IRQHandler(void)    { Default_IRQHandler(); }
__weak void CATSYNC1_IRQHandler(void)    { Default_IRQHandler(); }
__weak void CAT_IRQHandler(void)         { Default_IRQHandler(); }
__weak void CATSOF_IRQHandler(void)      { Default_IRQHandler(); }
__weak void CATEOF_IRQHandler(void)      { Default_IRQHandler(); }
__weak void BUFDMA_IRQHandler(void)      { Default_IRQHandler(); }
__weak void PHY0_IRQHandler(void)        { Default_IRQHandler(); }
__weak void PHY1_IRQHandler(void)        { Default_IRQHandler(); }
__weak void ETHMII_IRQHandler(void)      { Default_IRQHandler(); }
__weak void ETHPAUSE_IRQHandler(void)    { Default_IRQHandler(); }
__weak void ETHTX_IRQHandler(void)       { Default_IRQHandler(); }
__weak void ETHSW_IRQHandler(void)       { Default_IRQHandler(); }
__weak void ETHSWDLR_IRQHandler(void)    { Default_IRQHandler(); }
__weak void ETHSWSEC_IRQHandler(void)    { Default_IRQHandler(); }
__weak void ETHRXFIFO_IRQHandler(void)   { Default_IRQHandler(); }
__weak void ETHTXFIFO_IRQHandler(void)   { Default_IRQHandler(); }
__weak void ETHRXDMA_IRQHandler(void)    { Default_IRQHandler(); }
__weak void ETHTXDMA_IRQHandler(void)    { Default_IRQHandler(); }
__weak void MACDMARXFRM_IRQHandler(void) { Default_IRQHandler(); }
__weak void HOSTIF_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ0_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ1_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ2_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ3_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ4_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ5_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ6_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ7_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ8_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ9_IRQHandler(void)      { Default_IRQHandler(); }
__weak void INTPZ10_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ11_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ12_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ13_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ14_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ15_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ16_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ17_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ18_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ19_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ20_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ21_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ22_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ23_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ24_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ25_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ26_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ27_IRQHandler(void)     { Default_IRQHandler(); }
__weak void INTPZ28_IRQHandler(void)     { Default_IRQHandler(); }
__weak void HWRTOS_IRQHandler(void)      { Default_IRQHandler(); }
__weak void BRAMERR_IRQHandler(void)     { Default_IRQHandler(); }
__weak void IICB0TIS_IRQHandler(void)    { Default_IRQHandler(); }
__weak void IICB1TIS_IRQHandler(void)    { Default_IRQHandler(); }
__weak void WDTA0_IRQHandler(void)       { Default_IRQHandler(); }
__weak void SFLASH_IRQHandler(void)      { Default_IRQHandler(); }
__weak void UAJ0TIS_IRQHandler(void)     { Default_IRQHandler(); }
__weak void UAJ1TIS_IRQHandler(void)     { Default_IRQHandler(); }
__weak void CSIH0IRE_IRQHandler(void)    { Default_IRQHandler(); }
__weak void CSIH1IRE_IRQHandler(void)    { Default_IRQHandler(); }
__weak void FCN0ERR_IRQHandler(void)     { Default_IRQHandler(); }
__weak void FCN1ERR_IRQHandler(void)     { Default_IRQHandler(); }
__weak void DERR0_IRQHandler(void)       { Default_IRQHandler(); }
__weak void DERR1_IRQHandler(void)       { Default_IRQHandler(); }
__weak void ETHTXFIFOERR_IRQHandler(void){ Default_IRQHandler(); }
__weak void ETHRXERR_IRQHandler(void)    { Default_IRQHandler(); }
__weak void ETHRXDERR_IRQHandler(void)   { Default_IRQHandler(); }
__weak void ETHTXDERR_IRQHandler(void)   { Default_IRQHandler(); }
__weak void BUFDMAERR_IRQHandler(void)   { Default_IRQHandler(); }
__weak void ECATWDT_IRQHandler(void)     { Default_IRQHandler(); }
__weak void ECATRST_IRQHandler(void)     { Default_IRQHandler(); }

extern void OS_CPU_HWOSCtxSwHandler (void);
extern void OS_CPU_PendSVHandler (void);
extern void BenchSGIHandler (void);

/*============================================================================*/
/* Vector Table                                                               */
/*============================================================================*/

/* The vector table is normally located at address 0.
 When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
 If you need to define interrupt service routines,
 make a copy of this file and include it in your project.
 The name "__vector_table" has special meaning for C-SPY:
 it is where the SP start value is found, and the NVIC vector
 table register (VTOR) is initialized to this address if != 0.
 */

#pragma location = "vectors"
const intvec_elem __vector_table[] =
{
    { .__ptr = __sfe( "CSTACK" ) },         /* Top of Stack */
    __iar_program_start,                    /* Reset Handler */
    NMI_Handler,                            /* NMI Handler */
    HardFault_Handler,                      /* Hard Fault Handler */
    MemManageFault_Handler,                 /* MemManage fault Handler */
    BusFault_Handler,                       /* BusFault Handler */
    UsageFault_Handler,                     /* Usage Fault Handler */
    0,                                      /* Reserved */
    0,                                      /* Reserved */
    0,                                      /* Reserved */
    0,                                      /* Reserved */
    0,                                      /* SVCall Handler */
    DebugMon_Handler,                       /* Debug Monitor Handler */
    0,                                      /* Reserved */
    OS_CPU_PendSVHandler,                   /* PendSV Handler */
    0,                                      /* SysTick Handler */

  /* External Interrupts 1 - 240 */
  /* These are essentially unused, so will all */
  /* take the same default handler if invoked. */
    TAUJ0I0_IRQHandler,
    TAUJ0I1_IRQHandler,
    TAUJ0I2_IRQHandler,
    TAUJ0I3_IRQHandler,
    UAJ0TIT_IRQHandler,
    UAJ0TIR_IRQHandler,
    UAJ1TIT_IRQHandler,
    UAJ1TIR_IRQHandler,
    CSIH0IC_IRQHandler,
    CSIH0IR_IRQHandler,
    CSIH0IJC_IRQHandler,
    CSIH1IC_IRQHandler,
    CSIH1IR_IRQHandler,
    CSIH1IJC_IRQHandler,
    IICB0TIA_IRQHandler,
    IICB1TIA_IRQHandler,
    FCN0REC_IRQHandler,
    FCN0TRX_IRQHandler,
    FCN0WUP_IRQHandler,
    FCN1REC_IRQHandler,
    FCN1TRX_IRQHandler,
    FCN1WUP_IRQHandler,
    DMA00_IRQHandler,
    DMA01_IRQHandler,
    DMA02_IRQHandler,
    DMA03_IRQHandler,
    DMA10_IRQHandler,
    CATSYNC0_IRQHandler,
    CATSYNC1_IRQHandler,
    CAT_IRQHandler,
    CATSOF_IRQHandler,
    CATEOF_IRQHandler,
    BUFDMA_IRQHandler,
    PHY0_IRQHandler,
    PHY1_IRQHandler,
    ETHMII_IRQHandler,
    ETHPAUSE_IRQHandler,
    ETHTX_IRQHandler,
    ETHSW_IRQHandler,
    ETHSWDLR_IRQHandler,
    ETHSWSEC_IRQHandler,
    ETHRXFIFO_IRQHandler,
    ETHTXFIFO_IRQHandler,
    ETHRXDMA_IRQHandler,
    ETHTXDMA_IRQHandler,
    MACDMARXFRM_IRQHandler,
    HOSTIF_IRQHandler,
    INTPZ0_IRQHandler,
    INTPZ1_IRQHandler,
    INTPZ2_IRQHandler,
    INTPZ3_IRQHandler,
    INTPZ4_IRQHandler,
    INTPZ5_IRQHandler,
    INTPZ6_IRQHandler,
    INTPZ7_IRQHandler,
    INTPZ8_IRQHandler,
    INTPZ9_IRQHandler,
    INTPZ10_IRQHandler,
    INTPZ11_IRQHandler,
    INTPZ12_IRQHandler,
    INTPZ13_IRQHandler,
    INTPZ14_IRQHandler,
    INTPZ15_IRQHandler,
    INTPZ16_IRQHandler,
    INTPZ17_IRQHandler,
    INTPZ18_IRQHandler,
    INTPZ19_IRQHandler,
    INTPZ20_IRQHandler,
    INTPZ21_IRQHandler,
    INTPZ22_IRQHandler,
    INTPZ23_IRQHandler,
    INTPZ24_IRQHandler,
    INTPZ25_IRQHandler,
    INTPZ26_IRQHandler,
    INTPZ27_IRQHandler,
    INTPZ28_IRQHandler,
    OS_CPU_HWOSCtxSwHandler,
    BRAMERR_IRQHandler,
    IICB0TIS_IRQHandler,
    IICB1TIS_IRQHandler,
    WDTA0_IRQHandler,
    SFLASH_IRQHandler,
    UAJ0TIS_IRQHandler,
    UAJ1TIS_IRQHandler,
    CSIH0IRE_IRQHandler,
    CSIH1IRE_IRQHandler,
    FCN0ERR_IRQHandler,
    FCN1ERR_IRQHandler,
    DERR0_IRQHandler,
    DERR1_IRQHandler,
    ETHTXFIFOERR_IRQHandler,
    ETHRXERR_IRQHandler,
    ETHRXDERR_IRQHandler,
    ETHTXDERR_IRQHandler,
    BUFDMAERR_IRQHandler,
    ECATWDT_IRQHandler,
    ECATRST_IRQHandler
};
