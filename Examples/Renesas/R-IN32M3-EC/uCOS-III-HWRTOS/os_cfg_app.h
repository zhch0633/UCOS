/*
*********************************************************************************************************
*                                           uC/OS-III-HWOS
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
*                                         HWOS CONFIGURATION
*
* Filename      : os_cfg_app.h
* Version       : V0.00.00
* Programmer(s) : JBL
*********************************************************************************************************
*/


#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

                                                            /* --------------------- MISCELLANEOUS ------------------ */

#define  OS_CFG_ISR_STK_SIZE             1024u              /* Stack size of ISR stack (number of CPU_STK elements)   */


                                                            /* ---------------------- IDLE TASK --------------------- */
#define  OS_CFG_IDLE_TASK_STK_SIZE       1024u              /* Stack size (number of CPU_STK elements)                */

                                                            /* ------------------------ TICKS ----------------------- */
#define  OS_CFG_TICK_RATE_HZ             1000u              /* Tick rate in Hertz (10 to 1000 Hz)                     */


#endif
