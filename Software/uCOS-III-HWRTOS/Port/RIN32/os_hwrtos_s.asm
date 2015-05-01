;
;********************************************************************************************************
;                                                uC/OS-III HWOS
;
;
;                           (c) Copyright 2009-2014; Micrium, Inc.; Weston, FL
;                    All rights reserved.  Protected by international copyright laws.
;
;                                           ARM Cortex-M3 Port
;
; File      : os_hwrtos.asm
; Version   : V1.00.00.00
; By        : JBL
;********************************************************************************************************
;

;********************************************************************************************************
;                                     CODE GENERATION DIRECTIVES
;********************************************************************************************************

    RSEG CODE:CODE:NOROOT(2)
    THUMB


;********************************************************************************************************
;                                              DEFINE
;********************************************************************************************************

#define R0_INTERCEPT        (0x40000000)
#define R0_CONTEXT_ID_MASK  (0x00FF0000)

#define HWRTOS_SYSC (0x4008f000)
#define HWRTOS_R4   (0x4008f004)

#define NVIC_PENDSVSET 0xE000ED04


;********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************

    EXPORT  OS_CPU_Start
    EXPORT  OS_CPU_HWOSCtxSwHandler
    EXPORT  OS_CPU_PendSVHandler

    IMPORT  OSSPCurPtr
    IMPORT  OSTaskStkPtrTbl
    IMPORT  OSCtxIDCur
    IMPORT  OSTaskRet
    IMPORT  OSHWOSPendCtxSw
    IMPORT  OS_CPU_ExceptStkBase


;********************************************************************************************************
;                                           HWOS CPU START
;
; Note(s) : 1) This function perform initial setup of the exception/main stack pointers.
;********************************************************************************************************

OS_CPU_Start
    CPSID   I

    MOV32   R0, OS_CPU_ExceptStkBase                            ; Initialize the MSP to the OS_CPU_ExceptStkBase
    LDR     R1, [R0]
    MSR     MSP, R1

    MOV32   r2, OSTaskStkPtrTbl
    LDR     r0, [r2]
    MSR     PSP, r0
    CPSIE   I
    BX lr


;********************************************************************************************************
;                                   HWOS CONTEXT SWITCH HANDLER
;
; Note(s) : 1) This function process an asynchronous context switch from the HWOS.
;********************************************************************************************************

OS_CPU_HWOSCtxSwHandler
    CPSID   I ; Prevent interruption during context switch

    MOV32 r2, #0x4008F000

wait1
    LDR   r0, [r2, #0x20]
    ANDS  r1, r0, #R0_INTERCEPT
    BEQ   wait1

    LDR   r1, [r2, #0x24]

    AND   r2, r0, #R0_CONTEXT_ID_MASK

    UXTH  r3, r0
    CMP   r3, #0000
    BEQ   hwrtos_ctx_isr_skip_ret

    MOV32  r0, OSTaskRet
    ADD    r0, r0, r2, LSR #13
    STR    r3, [r0, #0]
    STR    r1, [r0, #4]


hwrtos_ctx_isr_skip_ret

    MOV32 r12, OSHWOSPendCtxSw
    LSR   r2, r2, #16
    STR   r2, [r12]

    MOV32 r0, #NVIC_PENDSVSET
    MOV   r1, #0x10000000
    STR   r1, [r0]

    CPSIE   I
    BX      lr                                                  ; Exception return will restore remaining context


;********************************************************************************************************
;                                       HANDLE PendSV EXCEPTION
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************

OS_CPU_PendSVHandler
    CPSID   I                             ; Prevent interruption during context switch
    MRS     r0, PSP
    STMFD   r0!, {R4-R11}

    MOV32   r12,OSCtxIDCur
    MOV32   r2, OSTaskStkPtrTbl
    LDR     r3, [r12]                     ; Load current context id
    STR     r0, [r2, r3, LSL #2]          ; Current stack pointer

    MOV32   r11,OSHWOSPendCtxSw
    MOV32   r0, OSTaskStkPtrTbl
    LDR     r4, [r11]                     ; Load pending context id
    LDR     r0, [r0, r4, LSL #2]          ; Load new stack pointer
    STR     r4, [r12]                     ; Store new current context id


    ORR     lr, lr, #0x04                 ; Ensure exception return uses process stack
    LDMFD   r0!, {r4-r11}                 ; Restore r4-11 from new process stack
    MSR     PSP, r0                       ; Load PSP with new process SP
    CPSIE   I
    BX      lr                            ; Exception return will restore remaining context


    END

