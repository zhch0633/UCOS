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
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : JBL
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   BSP_MODULE

#include  <cpu.h>
#include  <bsp.h>


/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

#define  RIN32_REG_PIO_PORT_03 (*((CPU_REG32 *)0x400A3403))     /* Port Register 03.                                    */
#define  RIN32_REG_PIO_MODE_03 (*((CPU_REG32 *)0x400A3413))     /* Port Mode Register 03.                               */

/*
*********************************************************************************************************
*                                          BSP_CPU_ClkFreq()
*
* Description : Returns the cpu clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : cpu clock frequency in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq(void)
{
    return (BSP_CPU_CLK_FREQ);
}


/*
*********************************************************************************************************
*                                             BSP_Init()
*
* Description : Initialize BSP related functions.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Init(void)
{

    BSP_Led_Init();

    return;
}


/*
*********************************************************************************************************
*                                           BSP_Led_Init()
*
* Description : Initialize the on board LEDs.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void BSP_Led_Init(void)
{
    RIN32_REG_PIO_PORT_03 = 0u;

    RIN32_REG_PIO_MODE_03 = 0u;

    RIN32_REG_PIO_PORT_03 = 0xFFu;

    return;
}


/*
*********************************************************************************************************
*                                            BSP_Led_On()
*
* Description : Turn a LED on.
*
* Argument(s) : led - LED number to turn on.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void BSP_Led_On (CPU_INT08U led)
{

    if((led == 0u) || (led > 8u)) {
        RIN32_REG_PIO_PORT_03= 0xFFu;
    } else {
        RIN32_REG_PIO_PORT_03 |= (CPU_DATA)(1u<<((CPU_DATA)led-1u));
    }
}


/*
*********************************************************************************************************
*                                            BSP_Led_On()
*
* Description : Turn a LED off.
*
* Argument(s) : led - LED number to turn off.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void BSP_Led_Off (CPU_INT08U led)
{

    if((led == 0u) || (led > 8u)) {
        RIN32_REG_PIO_PORT_03 = 0x00u;
    } else {
        RIN32_REG_PIO_PORT_03 &= ~(CPU_DATA)(1u<<((CPU_DATA)led-1u));
    }
}
