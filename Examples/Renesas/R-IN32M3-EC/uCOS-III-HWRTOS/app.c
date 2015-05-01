/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*                          (c) Copyright 2009-2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                          APPLICATION CODE
*                                         Renesas R-IN32M3-EC
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JBL
*********************************************************************************************************
* Note(s)       : none.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/


#include  <app_cfg.h>
#include  <bsp.h>

#include  <lib_mem.h>

#include  <os.h>
#include  <os_cfg_app.h>


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB        AppTaskStartTCB;
static  CPU_STK       AppTaskStartStk[4096u];


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void AppTaskStart(void *p_arg);


/*
*********************************************************************************************************
*                                               main()
*
* Description : Entry point for C code.
*
* Arguments   : none.
*
* Returns     : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int main ()
{
    OS_ERR os_err;

    BSP_Init();

    CPU_Init();

    OSInit(&os_err);
    
    if(os_err != OS_ERR_NONE) {
        APP_TRACE_INFO(("Error initialising OS. OsInit() returned with error %u\r\n", os_err));
    }

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) 5,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK     )(4096u / 10u),
                 (CPU_STK_SIZE) 4096u,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&os_err);

    if(os_err != OS_ERR_NONE) {
        APP_TRACE_INFO(("Error creating task. OSTaskCreate() returned with error %u\r\n", os_err));
    }
    
    OSStart(&os_err);
    
    if(os_err != OS_ERR_NONE) {
        APP_TRACE_INFO(("Error starting. OSStart() returned with error %u\r\n", os_err));
    }

    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                             AppTaskStart()
*
* Description : Example task.
*
* Arguments   : none.
*
* Returns     : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void AppTaskStart(void *p_arg)
{
    CPU_INT08U led = 1;
    CPU_INT08U dir = 0;
    OS_ERR os_err;

    Mem_Init();                                                      /* Init memory management.                         */
    
    OS_CPU_SysTickInit(APP_CPU_CLK_FREQ / OS_CFG_TICK_RATE_HZ - 1u); /* Configure system tick.                          */

    BSP_Led_On(1u);

    while (DEF_TRUE) {

        if(dir == 0) {
            OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &os_err);

            BSP_Led_Off(0);
            OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &os_err);
            BSP_Led_On(++led);
            if(led == 8) {
                dir = 1;
            }
        } else {
            OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &os_err);

            BSP_Led_Off(0);
            OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &os_err);
            BSP_Led_On(--led);
            if(led == 1) {
                dir = 0;
            }
        }

    }
}
