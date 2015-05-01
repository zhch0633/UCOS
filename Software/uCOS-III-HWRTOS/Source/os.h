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
*                                            HWOS HEADER
*
* Filename      : os.h
* Version       : V1.00.00
* Programmer(s) : JBL
*********************************************************************************************************
*/

#ifndef   OS_H
#define   OS_H


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <bsp.h>

#include  <os_cfg.h>
#include  <os_cfg_app.h>
#include  <cpu_core.h>
#include  <lib_def.h>
#include  <os_hwrtos.h>


/*
*********************************************************************************************************
*                                   uC/OS-III HW-RTOS VERSION NUMBER
*********************************************************************************************************
*/

#define  OS_VERSION         10000u                              /* Version of uC/OS-III HW-RTOS (Vx.yy.zz mult. by 10000)*/
#define  HWRTOS_OS_VERSION  OS_VERSION                          /* Same as OS_VERSION.                                  */


/*
*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************
*/

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            ENUMERATIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
---------------------------------------------------------------------------------------------------------
*                                             ERROR CODES
---------------------------------------------------------------------------------------------------------
*/

typedef  enum  os_err {
    OS_ERR_NONE                      =     0u,

    OS_ERR_A                         = 10000u,
    OS_ERR_ACCEPT_ISR                = 10001u,

    OS_ERR_B                         = 11000u,

    OS_ERR_C                         = 12000u,
    OS_ERR_CREATE_ISR                = 12001u,

    OS_ERR_D                         = 13000u,
    OS_ERR_DEL_ISR                   = 13001u,

    OS_ERR_E                         = 14000u,

    OS_ERR_F                         = 15000u,
    OS_ERR_FATAL_RETURN              = 15001u,

    OS_ERR_FLAG_GRP_DEPLETED         = 15101u,
    OS_ERR_FLAG_NOT_RDY              = 15102u,
    OS_ERR_FLAG_PEND_OPT             = 15103u,
    OS_ERR_FLUSH_ISR                 = 15104u,

    OS_ERR_G                         = 16000u,

    OS_ERR_H                         = 17000u,

    OS_ERR_I                         = 18000u,
    OS_ERR_ILLEGAL_CREATE_RUN_TIME   = 18001u,
    OS_ERR_INT_Q                     = 18002u,
    OS_ERR_INT_Q_FULL                = 18003u,
    OS_ERR_INT_Q_SIZE                = 18004u,
    OS_ERR_INT_Q_STK_INVALID         = 18005u,
    OS_ERR_INT_Q_STK_SIZE_INVALID    = 18006u,

    OS_ERR_J                         = 19000u,

    OS_ERR_K                         = 20000u,

    OS_ERR_L                         = 21000u,
    OS_ERR_LOCK_NESTING_OVF          = 21001u,

    OS_ERR_M                         = 22000u,

    OS_ERR_MEM_CREATE_ISR            = 22201u,
    OS_ERR_MEM_FULL                  = 22202u,
    OS_ERR_MEM_INVALID_P_ADDR        = 22203u,
    OS_ERR_MEM_INVALID_BLKS          = 22204u,
    OS_ERR_MEM_INVALID_PART          = 22205u,
    OS_ERR_MEM_INVALID_P_BLK         = 22206u,
    OS_ERR_MEM_INVALID_P_MEM         = 22207u,
    OS_ERR_MEM_INVALID_P_DATA        = 22208u,
    OS_ERR_MEM_INVALID_SIZE          = 22209u,
    OS_ERR_MEM_NO_FREE_BLKS          = 22210u,

    OS_ERR_MSG_POOL_EMPTY            = 22301u,
    OS_ERR_MSG_POOL_NULL_PTR         = 22302u,

    OS_ERR_MUTEX_NOT_OWNER           = 22401u,
    OS_ERR_MUTEX_OWNER               = 22402u,
    OS_ERR_MUTEX_NESTING             = 22403u,

    OS_ERR_N                         = 23000u,
    OS_ERR_NAME                      = 23001u,
    OS_ERR_NO_MORE_ID_AVAIL          = 23002u,

    OS_ERR_O                         = 24000u,
    OS_ERR_OBJ_CREATED               = 24001u,
    OS_ERR_OBJ_DEL                   = 24002u,
    OS_ERR_OBJ_PTR_NULL              = 24003u,
    OS_ERR_OBJ_TYPE                  = 24004u,

    OS_ERR_OPT_INVALID               = 24101u,

    OS_ERR_OS_NOT_RUNNING            = 24201u,
    OS_ERR_OS_RUNNING                = 24202u,

    OS_ERR_P                         = 25000u,
    OS_ERR_PEND_ABORT                = 25001u,
    OS_ERR_PEND_ABORT_ISR            = 25002u,
    OS_ERR_PEND_ABORT_NONE           = 25003u,
    OS_ERR_PEND_ABORT_SELF           = 25004u,
    OS_ERR_PEND_DEL                  = 25005u,
    OS_ERR_PEND_ISR                  = 25006u,
    OS_ERR_PEND_LOCKED               = 25007u,
    OS_ERR_PEND_WOULD_BLOCK          = 25008u,

    OS_ERR_POST_NULL_PTR             = 25101u,
    OS_ERR_POST_ISR                  = 25102u,

    OS_ERR_PRIO_EXIST                = 25201u,
    OS_ERR_PRIO                      = 25202u,
    OS_ERR_PRIO_INVALID              = 25203u,

    OS_ERR_PTR_INVALID               = 25301u,

    OS_ERR_Q                         = 26000u,
    OS_ERR_Q_FULL                    = 26001u,
    OS_ERR_Q_EMPTY                   = 26002u,
    OS_ERR_Q_MAX                     = 26003u,
    OS_ERR_Q_SIZE                    = 26004u,

    OS_ERR_R                         = 27000u,
    OS_ERR_REG_ID_INVALID            = 27001u,
    OS_ERR_ROUND_ROBIN_1             = 27002u,
    OS_ERR_ROUND_ROBIN_DISABLED      = 27003u,

    OS_ERR_S                         = 28000u,
    OS_ERR_SCHED_INVALID_TIME_SLICE  = 28001u,
    OS_ERR_SCHED_LOCK_ISR            = 28002u,
    OS_ERR_SCHED_LOCKED              = 28003u,
    OS_ERR_SCHED_NOT_LOCKED          = 28004u,
    OS_ERR_SCHED_UNLOCK_ISR          = 28005u,

    OS_ERR_SEM_OVF                   = 28101u,
    OS_ERR_SET_ISR                   = 28102u,

    OS_ERR_STAT_RESET_ISR            = 28201u,
    OS_ERR_STAT_PRIO_INVALID         = 28202u,
    OS_ERR_STAT_STK_INVALID          = 28203u,
    OS_ERR_STAT_STK_SIZE_INVALID     = 28204u,
    OS_ERR_STATE_INVALID             = 28205u,
    OS_ERR_STATUS_INVALID            = 28206u,
    OS_ERR_STK_INVALID               = 28207u,
    OS_ERR_STK_SIZE_INVALID          = 28208u,
    OS_ERR_STK_LIMIT_INVALID         = 28209u,

    OS_ERR_T                         = 29000u,
    OS_ERR_TASK_CHANGE_PRIO_ISR      = 29001u,
    OS_ERR_TASK_CREATE_ISR           = 29002u,
    OS_ERR_TASK_DEL                  = 29003u,
    OS_ERR_TASK_DEL_IDLE             = 29004u,
    OS_ERR_TASK_DEL_INVALID          = 29005u,
    OS_ERR_TASK_DEL_ISR              = 29006u,
    OS_ERR_TASK_INVALID              = 29007u,
    OS_ERR_TASK_NO_MORE_TCB          = 29008u,
    OS_ERR_TASK_NOT_DLY              = 29009u,
    OS_ERR_TASK_NOT_EXIST            = 29010u,
    OS_ERR_TASK_NOT_SUSPENDED        = 29011u,
    OS_ERR_TASK_OPT                  = 29012u,
    OS_ERR_TASK_RESUME_ISR           = 29013u,
    OS_ERR_TASK_RESUME_PRIO          = 29014u,
    OS_ERR_TASK_RESUME_SELF          = 29015u,
    OS_ERR_TASK_RUNNING              = 29016u,
    OS_ERR_TASK_STK_CHK_ISR          = 29017u,
    OS_ERR_TASK_SUSPENDED            = 29018u,
    OS_ERR_TASK_SUSPEND_IDLE         = 29019u,
    OS_ERR_TASK_SUSPEND_INT_HANDLER  = 29020u,
    OS_ERR_TASK_SUSPEND_ISR          = 29021u,
    OS_ERR_TASK_SUSPEND_PRIO         = 29022u,
    OS_ERR_TASK_WAITING              = 29023u,

    OS_ERR_TCB_INVALID               = 29101u,

    OS_ERR_TLS_ID_INVALID            = 29120u,
    OS_ERR_TLS_ISR                   = 29121u,
    OS_ERR_TLS_NO_MORE_AVAIL         = 29122u,
    OS_ERR_TLS_NOT_EN                = 29123u,
    OS_ERR_TLS_DESTRUCT_ASSIGNED     = 29124u,

    OS_ERR_TICK_PRIO_INVALID         = 29201u,
    OS_ERR_TICK_STK_INVALID          = 29202u,
    OS_ERR_TICK_STK_SIZE_INVALID     = 29203u,
    OS_ERR_TICK_WHEEL_SIZE           = 29204u,

    OS_ERR_TIME_DLY_ISR              = 29301u,
    OS_ERR_TIME_DLY_RESUME_ISR       = 29302u,
    OS_ERR_TIME_GET_ISR              = 29303u,
    OS_ERR_TIME_INVALID_HOURS        = 29304u,
    OS_ERR_TIME_INVALID_MINUTES      = 29305u,
    OS_ERR_TIME_INVALID_SECONDS      = 29306u,
    OS_ERR_TIME_INVALID_MILLISECONDS = 29307u,
    OS_ERR_TIME_NOT_DLY              = 29308u,
    OS_ERR_TIME_SET_ISR              = 29309u,
    OS_ERR_TIME_ZERO_DLY             = 29310u,

    OS_ERR_TIMEOUT                   = 29401u,

    OS_ERR_TMR_INACTIVE              = 29501u,
    OS_ERR_TMR_INVALID_DEST          = 29502u,
    OS_ERR_TMR_INVALID_DLY           = 29503u,
    OS_ERR_TMR_INVALID_PERIOD        = 29504u,
    OS_ERR_TMR_INVALID_STATE         = 29505u,
    OS_ERR_TMR_INVALID               = 29506u,
    OS_ERR_TMR_ISR                   = 29507u,
    OS_ERR_TMR_NO_CALLBACK           = 29508u,
    OS_ERR_TMR_NON_AVAIL             = 29509u,
    OS_ERR_TMR_PRIO_INVALID          = 29510u,
    OS_ERR_TMR_STK_INVALID           = 29511u,
    OS_ERR_TMR_STK_SIZE_INVALID      = 29512u,
    OS_ERR_TMR_STOPPED               = 29513u,

    OS_ERR_U                         = 30000u,

    OS_ERR_V                         = 31000u,

    OS_ERR_W                         = 32000u,

    OS_ERR_X                         = 33000u,

    OS_ERR_Y                         = 34000u,
    OS_ERR_YIELD_ISR                 = 34001u,

    OS_ERR_Z                         = 35000u,

    OS_ERR_HWOS                      = 70000u,
    OS_ERR_HWOS_UNSUPPORTED          = 70001u,
    OS_ERR_HWOS_RSRC_NOT_AVAIL       = 70002u,
    OS_ERR_HWOS_INT_ID_INVALID       = 70003u,
    OS_ERR_HWOS_QINT_ID_INVALID      = 70004u,
    OS_ERR_HWOS_QINT_CFG             = 70005u
} OS_ERR;



/*
========================================================================================================================
*                                           Possible values for 'opt' argument
========================================================================================================================
*/

#define  OS_OPT_NONE                         (OS_OPT)(0x0000u)


/*
------------------------------------------------------------------------------------------------------------------------
*                                                    DELETE OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_DEL_NO_PEND                  (OS_OPT)(0x0000u)
#define  OS_OPT_DEL_ALWAYS                   (OS_OPT)(0x0001u)


/*
------------------------------------------------------------------------------------------------------------------------
*                                                     PEND OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_PEND_FLAG_MASK               (OS_OPT)(0x000Fu)
#define  OS_OPT_PEND_FLAG_CLR_ALL            (OS_OPT)(0x0001u)  /* Wait for ALL    the bits specified to be CLR       */
#define  OS_OPT_PEND_FLAG_CLR_AND            (OS_OPT)(0x0001u)

#define  OS_OPT_PEND_FLAG_CLR_ANY            (OS_OPT)(0x0002u)  /* Wait for ANY of the bits specified to be CLR       */
#define  OS_OPT_PEND_FLAG_CLR_OR             (OS_OPT)(0x0002u)

#define  OS_OPT_PEND_FLAG_SET_ALL            (OS_OPT)(0x0004u)  /* Wait for ALL    the bits specified to be SET       */
#define  OS_OPT_PEND_FLAG_SET_AND            (OS_OPT)(0x0004u)

#define  OS_OPT_PEND_FLAG_SET_ANY            (OS_OPT)(0x0008u)  /* Wait for ANY of the bits specified to be SET       */
#define  OS_OPT_PEND_FLAG_SET_OR             (OS_OPT)(0x0008u)

#define  OS_OPT_PEND_FLAG_CONSUME            (OS_OPT)(0x0100u)  /* Consume the flags if condition(s) satisfied        */


#define  OS_OPT_PEND_BLOCKING                (OS_OPT)(0x0000u)  /* Blocking call.                                     */
#define  OS_OPT_PEND_NON_BLOCKING            (OS_OPT)(0x8000u)  /* Non-blocking call.                                 */


/*
------------------------------------------------------------------------------------------------------------------------
*                                                  PEND ABORT OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_PEND_ABORT_1                 (OS_OPT)(0x0000u)  /* Pend abort a single waiting task                   */
#define  OS_OPT_PEND_ABORT_ALL               (OS_OPT)(0x0100u)  /* Pend abort ALL tasks waiting                       */


/*
------------------------------------------------------------------------------------------------------------------------
*                                                     POST OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/


#define  OS_OPT_POST_NONE                    (OS_OPT)(0x0000u)

#define  OS_OPT_POST_FLAG_SET                (OS_OPT)(0x0000u)
#define  OS_OPT_POST_FLAG_CLR                (OS_OPT)(0x0001u)

#define  OS_OPT_POST_FIFO                    (OS_OPT)(0x0000u)  /* Default is to post FIFO                            */
#define  OS_OPT_POST_LIFO                    (OS_OPT)(0x0010u)  /* Post to highest priority task waiting              */
#define  OS_OPT_POST_1                       (OS_OPT)(0x0000u)  /* Post message to highest priority task waiting      */
#define  OS_OPT_POST_ALL                     (OS_OPT)(0x0200u)  /* Broadcast message to ALL tasks waiting             */

#define  OS_OPT_POST_NO_SCHED                (OS_OPT)(0x8000u)  /* Do not call the scheduler if this is selected      */


/*
------------------------------------------------------------------------------------------------------------------------
*                                                     TASK OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TASK_NONE                    (OS_OPT)(0x0000u)  /* No option selected                                 */
#define  OS_OPT_TASK_STK_CHK                 (OS_OPT)(0x0001u)  /* Enable stack checking for the task                 */
#define  OS_OPT_TASK_STK_CLR                 (OS_OPT)(0x0002u)  /* Clear the stack when the task is create            */
#define  OS_OPT_TASK_SAVE_FP                 (OS_OPT)(0x0004u)  /* Save the contents of any floating-point registers  */
#define  OS_OPT_TASK_NO_TLS                  (OS_OPT)(0x0008u)  /* Specifies the task DOES NOT require TLS support    */


/*
------------------------------------------------------------------------------------------------------------------------
*                                                     TIME OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TIME_DLY                             DEF_BIT_NONE
#define  OS_OPT_TIME_TIMEOUT                ((OS_OPT)DEF_BIT_01)
#define  OS_OPT_TIME_MATCH                  ((OS_OPT)DEF_BIT_02)
#define  OS_OPT_TIME_PERIODIC               ((OS_OPT)DEF_BIT_03)

#define  OS_OPT_TIME_HMSM_STRICT            ((OS_OPT)DEF_BIT_NONE)
#define  OS_OPT_TIME_HMSM_NON_STRICT        ((OS_OPT)DEF_BIT_04)

#define  OS_OPT_TIME_MASK                   ((OS_OPT)(OS_OPT_TIME_DLY      | \
                                                      OS_OPT_TIME_TIMEOUT  | \
                                                      OS_OPT_TIME_PERIODIC | \
                                                      OS_OPT_TIME_MATCH))

#define  OS_OPT_TIME_OPTS_MASK                       (OS_OPT_TIME_DLY            | \
                                                      OS_OPT_TIME_TIMEOUT        | \
                                                      OS_OPT_TIME_PERIODIC       | \
                                                      OS_OPT_TIME_MATCH          | \
                                                      OS_OPT_TIME_HMSM_NON_STRICT)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     QINT OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_QINT_ENABLED                 (OS_OPT)(0x0000u)  /* Specifies that the qint be enabled by default.       */
#define  OS_OPT_QINT_DISABLED                (OS_OPT)(0x0001u)  /* Specifies that the qint be diksabled by default.     */


/*
------------------------------------------------------------------------------------------------------------------------
*                                                    TIMER OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TMR_NONE                          (OS_OPT)(0u)  /* No option selected                                 */

#define  OS_OPT_TMR_ONE_SHOT                      (OS_OPT)(1u)  /* Timer will not auto restart when it expires        */
#define  OS_OPT_TMR_PERIODIC                      (OS_OPT)(2u)  /* Timer will     auto restart when it expires        */

#define  OS_OPT_TMR_CALLBACK                      (OS_OPT)(3u)  /* OSTmrStop() option to call 'callback' w/ timer arg */
#define  OS_OPT_TMR_CALLBACK_ARG                  (OS_OPT)(4u)  /* OSTmrStop() option to call 'callback' w/ new   arg */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                 TIMER TICK THRESHOLDS
------------------------------------------------------------------------------------------------------------------------
*/
                                                                    /* Threshold to init previous tick time           */
#define  OS_TICK_TH_INIT                    (OS_TICK)(DEF_BIT       ((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) - 1u))

                                                                    /* Threshold to check if tick time already ready  */
#define  OS_TICK_TH_RDY                     (OS_TICK)(DEF_BIT_FIELD(((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) / 2u), \
                                                                    ((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) / 2u)))


/*
========================================================================================================================
*                                                   OS OBJECT TYPES
*
* Note(s) : (1) OS_OBJ_TYPE_&&& #define values specifically chosen as ASCII representations of the kernel
*               object types.  Memory displays of kernel objects will display the kernel object TYPEs with
*               their chosen ASCII names.
========================================================================================================================
*/

#define  OS_OBJ_TYPE_NONE                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('N', 'O', 'N', 'E')
#define  OS_OBJ_TYPE_FLAG                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('F', 'L', 'A', 'G')
#define  OS_OBJ_TYPE_MEM                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('M', 'E', 'M', ' ')
#define  OS_OBJ_TYPE_MUTEX                   (OS_OBJ_TYPE)CPU_TYPE_CREATE('M', 'U', 'T', 'X')
#define  OS_OBJ_TYPE_Q                       (OS_OBJ_TYPE)CPU_TYPE_CREATE('Q', 'U', 'E', 'U')
#define  OS_OBJ_TYPE_SEM                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('S', 'E', 'M', 'A')
#define  OS_OBJ_TYPE_TASK_MSG                (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'M', 'S', 'G')
#define  OS_OBJ_TYPE_TASK_RESUME             (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'R', 'E', 'S')
#define  OS_OBJ_TYPE_TASK_SIGNAL             (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'S', 'I', 'G')
#define  OS_OBJ_TYPE_TASK_SUSPEND            (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'S', 'U', 'S')
#define  OS_OBJ_TYPE_TICK                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'I', 'C', 'K')
#define  OS_OBJ_TYPE_TMR                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'M', 'R', ' ')
#define  OS_OBJ_TYPE_TASK                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'C', 'B', ' ')


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             CORE TYPEDEFS
*********************************************************************************************************
*********************************************************************************************************
*/

typedef   CPU_INT32U      OS_CTR;                               /* Counter,                                             */

typedef   CPU_INT32U      OS_CYCLES;                            /* CPU clock cycles,                                    */

typedef   CPU_INT16U      OS_FLAGS;                             /* Event flags,                                         */

typedef   CPU_INT16U      OS_MSG_QTY;                           /* Number of OS_MSGs in the msg pool,                   */
typedef   CPU_INT16U      OS_MSG_SIZE;                          /* Size of messages in number of bytes,                 */

typedef   CPU_INT08U      OS_NESTING_CTR;                       /* Interrupt and scheduler nesting,                     */

typedef   CPU_INT16U      OS_OBJ_QTY;                           /* Number of kernel objects counter,                    */
typedef   CPU_INT32U      OS_OBJ_TYPE;                          /* Special flag to determine object type,               */

typedef   CPU_INT16U      OS_OPT;                               /* Holds function options                               */

typedef   CPU_INT08U      OS_PRIO;                              /* Priority of a task,                                  */

typedef   CPU_INT16U      OS_QTY;                               /* Quantity                                             */

typedef   CPU_INT32U      OS_RATE_HZ;                           /* Rate in Hertz                                        */

typedef   CPU_INT08U      OS_SEM_CTR;                           /* Semaphore value                                      */

typedef   CPU_INT08U      OS_STATE;                             /* State variable                                       */

typedef   CPU_INT08U      OS_STATUS;                            /* Status                                               */

typedef   CPU_INT32U      OS_TICK;                              /* Clock tick counter                                   */

typedef   CPU_INT16U      OS_MEM_QTY;                           /* Number of memory blocks                              */
typedef   CPU_INT16U      OS_MEM_SIZE;                          /* Size in bytes of a memory block                      */

typedef   CPU_INT32U      OS_QINT_ID;                           /* QINT signal id                                       */
typedef   CPU_INT32U      OS_INT_ID;                            /* Interrupt signal id                                  */


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef  struct  os_tcb              OS_TCB;
typedef  void                      (*OS_TASK_PTR)(void *p_arg);


typedef  struct  os_sem              OS_SEM;
typedef  struct  os_sem              OS_MUTEX;

typedef  struct  os_q                OS_Q;

typedef  struct  os_flag_grp         OS_FLAG_GRP;

typedef  struct  os_hwos_ret         OS_HWOS_RET;

typedef  struct  os_mem              OS_MEM;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           DATA STRUCTURES
*********************************************************************************************************
*********************************************************************************************************
*/

struct os_tcb {
    CPU_STK     *StkPtr;                                        /* Saved stack pointer.                                 */
#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
	OS_OBJ_TYPE Type;
#endif
    CPU_INT32U   CtxID;                                         /* HWOS Context ID.                                     */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    CPU_CHAR    *NamePtr;                                       /* Task Name.                                           */
#endif
    OS_TICK      TickCtrPrev;                                   /* Used by OSTimeDlyXX() in PERIODIC mode.              */

};


struct  os_sem {                                                /* Semaphore.                                           */
#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
	OS_OBJ_TYPE Type;
#endif
    CPU_INT32U  SemID;                                          /* HWOS Semaphore ID.                                   */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    CPU_CHAR   *NamePtr;                                        /* Mutex Name.                                          */
#endif
    CPU_INT32U  Mutex;                                          /* DEF_YES if semaphore is a mutex.                     */
};


struct  os_q {                                                  /* Message Queue.                                       */
#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
	OS_OBJ_TYPE Type;
#endif
    CPU_INT32U  QID;                                            /* HWOS Queue ID.                                       */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    CPU_CHAR   *NamePtr;                                        /* Queue Name.                                          */
#endif
};


struct  os_flag_grp {                                           /* Event Flags                                          */
#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
	OS_OBJ_TYPE Type;
#endif
    CPU_INT32U  GrpID;                                          /* HWOS Flag ID.                                        */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    CPU_CHAR   *NamePtr;                                        /* Flag Group Name.                                     */
#endif
};


struct os_hwos_ret {                                            /* HWOS return value.                                   */
    CPU_INT32U  r0;
    CPU_INT32U  r1;
};


struct os_mem {                                             /* MEMORY CONTROL BLOCK                                   */
#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    OS_OBJ_TYPE          Type;                              /* Should be set to OS_OBJ_TYPE_MEM                       */
#endif
    void                *AddrPtr;                           /* Pointer to beginning of memory partition               */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    CPU_CHAR            *NamePtr;
#endif
    void                *FreeListPtr;                       /* Pointer to list of free memory blocks                  */
    OS_MEM_SIZE          BlkSize;                           /* Size (in bytes) of each block of memory                */
    OS_MEM_QTY           NbrMax;                            /* Total number of blocks in this partition               */
    OS_MEM_QTY           NbrFree;                           /* Number of memory blocks remaining in this partition    */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_MEM              *DbgPrevPtr;
    OS_MEM              *DbgNextPtr;
#endif
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              EXTERNALS
*********************************************************************************************************
*********************************************************************************************************
*/

OS_EXT  volatile  OS_STATE               OSRunning;             /* Flag indicating that kernel is running.              */
OS_EXT            CPU_INT32U             OSCtxIDCur;            /* Current context id.                                  */
OS_EXT  volatile  CPU_INT32U             OSHWOSPendCtxSw;       /* Pending context switch information.                  */


OS_EXT  volatile  CPU_STK               *OSTaskStkPtrTbl[OS_HWOS_CNTX_CNT + 1u]; /* Task stack pointers.                */
OS_EXT  volatile  OS_HWOS_RET            OSTaskRet[OS_HWOS_CNTX_CNT]; /* Global task return values.                     */
OS_EXT            OS_HWOS_RET            OSISRRet;              /* ISR return value.                                    */


OS_EXT            OS_TCB                 OSIdleTaskTCB;         /* Idle task TCB.                                       */
OS_EXT            CPU_STK                OSCfg_IdleTaskStk [OS_CFG_IDLE_TASK_STK_SIZE]; /* Idle task stack.             */


OS_EXT            OS_SEM                *OSSemPtrTbl[OS_HWOS_SEM_CNT]; /* Semaphore table.                              */
OS_EXT            OS_Q                  *OSQPtrTbl[OS_HWOS_Q_CNT]; /* Queue table.                                      */
OS_EXT            OS_FLAG_GRP           *OSGrpPtrTbl[OS_HWOS_FLAG_CNT]; /* Flag table.                                  */


OS_EXT  volatile  OS_NESTING_CTR         OSIntNestingCtr;       /* Interrupt nesting level                              */

OS_EXT            CPU_STK                OS_CPU_ExceptStk[OS_CFG_ISR_STK_SIZE]; /* Exception stack.                     */
OS_EXT            CPU_STK               *OS_CPU_ExceptStkBase;  /* Exception stack base.                                */

#if (OS_CFG_MEM_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT            OS_MEM                   *OSMemDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSMemQty;           /* Number of memory partitions created                  */
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          INTERRUPT MACROS
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OSIntEnter() do { OSIntNestingCtr++; } while (0u)
#define  OSIntExit()  do { OSIntNestingCtr--; } while (0u)


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


/* ================================================================================================================== */
/*                                                     SEMAPHORES                                                     */
/* ================================================================================================================== */

#if (OS_CFG_SEM_EN == DEF_ENABLED)

void          OSSemCreate               (OS_SEM                *p_sem,
                                         CPU_CHAR              *p_name,
                                         OS_SEM_CTR             cnt,
                                         OS_ERR                *p_err);

OS_OBJ_QTY    OSSemDel                  (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

OS_SEM_CTR    OSSemPend                 (OS_SEM                *p_sem,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if (OS_CFG_SEM_PEND_ABORT_EN == DEF_ENABLED)
OS_OBJ_QTY    OSSemPendAbort            (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

OS_SEM_CTR    OSSemPost                 (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#if (OS_CFG_SEM_DEL_EN == DEF_ENABLED)
OS_OBJ_QTY    OSSemDel                  (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

#if (OS_CFG_SEM_SET_EN == DEF_ENABLED)
void          OSSemSet                  (OS_SEM                *p_sem,
                                         OS_SEM_CTR             cnt,
                                         OS_ERR                *p_err);
#endif

#endif /* OS_CFG_SEM_EN == DEF_ENABLED */


/* ================================================================================================================== */
/*                                             MUTUAL EXCLUSION SEMAPHORES                                            */
/* ================================================================================================================== */

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
void          OSMutexCreate             (OS_MUTEX              *p_mutex,
                                         CPU_CHAR              *p_name,
                                         OS_ERR                *p_err);

#if (OS_CFG_MUTEX_DEL_EN == DEF_ENABLED)
OS_OBJ_QTY    OSMutexDel                (OS_MUTEX              *p_mutex,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif /* OS_CFG_MUTEX_DEL_EN == DEF_ENABLED */

void          OSMutexPend               (OS_MUTEX              *p_mutex,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

void          OSMutexPost               (OS_MUTEX              *p_mutex,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif /* OS_CFG_MUTEX_EN == DEF_ENABLED */


/* ================================================================================================================== */
/*                                                   MESSAGE QUEUES                                                   */
/* ================================================================================================================== */

#if (OS_CFG_Q_EN == DEF_ENABLED)
void          OSQCreate                 (OS_Q                  *p_q,
                                         CPU_CHAR              *p_name,
                                         OS_MSG_QTY             max_qty,
                                         OS_ERR                *p_err);

void         *OSQPend                   (OS_Q                  *p_q,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         OS_MSG_SIZE           *p_msg_size,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

void          OSQPost                   (OS_Q                  *p_q,
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if (OS_CFG_Q_DEL_EN == DEF_ENABLED)
OS_OBJ_QTY    OSQDel                    (OS_Q                  *p_q,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif /* OS_CFG_Q_DEL_EN == DEF_ENABLED */

#endif /* OS_CFG_Q_EN == DEF_ENABLED */


/* ================================================================================================================== */
/*                                                    EVENT FLAGS                                                     */
/* ================================================================================================================== */

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
void          OSFlagCreate              (OS_FLAG_GRP           *p_grp,
                                         CPU_CHAR              *p_name,
                                         OS_FLAGS               flags,
                                         OS_ERR                *p_err);

#if (OS_CFG_FLAG_DEL_EN == DEF_ENABLED)
OS_OBJ_QTY    OSFlagDel                 (OS_FLAG_GRP           *p_grp,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif /* OS_CFG_FLAG_DEL_EN == DEF_ENABLED */

OS_FLAGS      OSFlagPend                (OS_FLAG_GRP           *p_grp,
                                         OS_FLAGS               flags,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

OS_FLAGS      OSFlagPost                (OS_FLAG_GRP           *p_grp,
                                         OS_FLAGS               flags,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif /* OS_CFG_FLAG_EN == DEF_ENABLED */


/* ================================================================================================================== */
/*                                                 MISCELLANEOUS                                                      */
/* ================================================================================================================== */

void          OSInit                    (OS_ERR                *p_err);

void          OSStart                   (OS_ERR                *p_err);


/* ================================================================================================================== */
/*                                                 TASK MANAGEMENT                                                    */
/* ================================================================================================================== */

#if (OS_CFG_TASK_CHANGE_PRIO_EN == DEF_ENABLED)
void          OSTaskChangePrio          (OS_TCB                *p_tcb,
                                         OS_PRIO                prio_new,
                                         OS_ERR                *p_err);
#endif

void          OSTaskCreate              (OS_TCB                *p_tcb,
                                         CPU_CHAR              *p_name,
                                         OS_TASK_PTR            p_task,
                                         void                  *p_arg,
                                         OS_PRIO                prio,
                                         CPU_STK               *p_stk_base,
                                         CPU_STK_SIZE           stk_limit,
                                         CPU_STK_SIZE           stk_size,
                                         OS_MSG_QTY             q_size,
                                         OS_TICK                time_quanta,
                                         void                  *p_ext,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);


void          OSTaskSuspend             (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);


void          OSTaskResume              (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);

#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
void          OSTaskDel                 (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif


/* ================================================================================================================== */
/*                                                 TIME MANAGEMENT                                                    */
/* ================================================================================================================== */

void          OSTimeDly                 (OS_TICK                dly,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if (OS_CFG_TIME_DLY_HMSM_EN == DEF_ENABLED)
void  OSTimeDlyHMSM                     (CPU_INT16U             hours,
                                         CPU_INT16U             minutes,
                                         CPU_INT16U             seconds,
                                         CPU_INT32U             milli,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

#if (OS_CFG_TIME_DLY_RESUME_EN == DEF_ENABLED)
void          OSTimeDlyResume           (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif

OS_TICK       OSTimeGet                 (OS_ERR                *p_err);

void          OSTimeSet                 (OS_TICK                ticks,
                                         OS_ERR                *p_err);

/* ================================================================================================================== */
/*                                          FIXED SIZE MEMORY BLOCK MANAGEMENT                                        */
/* ================================================================================================================== */

#if (OS_CFG_MEM_EN == DEF_ENABLED)

void          OSMemCreate               (OS_MEM                *p_mem,
                                         CPU_CHAR              *p_name,
                                         void                  *p_addr,
                                         OS_MEM_QTY             n_blks,
                                         OS_MEM_SIZE            blk_size,
                                         OS_ERR                *p_err);

void         *OSMemGet                  (OS_MEM                *p_mem,
                                         OS_ERR                *p_err);

void          OSMemPut                  (OS_MEM                *p_mem,
                                         void                  *p_blk,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void          OS_MemDbgListAdd          (OS_MEM                *p_mem);
#endif

void          OS_MemInit                (OS_ERR                *p_err);

#endif


/* ================================================================================================================== */
/*                                                   HW ISR                                                           */
/* ================================================================================================================== */

void          OSHWISRSemPostSet         (OS_SEM                *p_sem,
                                         OS_QINT_ID             qint_id,
                                         OS_INT_ID              int_id,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

void          OSHWISRFlagPostSet        (OS_FLAG_GRP           *p_grp,
                                         OS_FLAGS               flags,
                                         OS_QINT_ID             qint_id,
                                         OS_INT_ID              int_id,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

void          OSHWISRTaskResumeSet      (OS_TCB                *p_tcb,
                                         OS_QINT_ID             qint_id,
                                         OS_INT_ID              int_id,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

void          OSHWISREn                 (OS_QINT_ID             qint_id,
                                         OS_ERR                *p_err);

void          OSHWISRDis                (OS_QINT_ID             qint_id,
                                         OS_ERR                *p_err);

void          OSHWISRClr                (OS_QINT_ID             qint_id,
                                         OS_ERR                *p_err);


/* ================================================================================================================== */
/*                                                    KERNEL                                                          */
/* ================================================================================================================== */


/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_IdleTask               (void                  *p_arg);

void          OS_IdleTaskInit           (OS_ERR                *p_err);


/*
************************************************************************************************************************
************************************************************************************************************************
*                                    T A R G E T   S P E C I F I C   F U N C T I O N S
************************************************************************************************************************
************************************************************************************************************************
*/

void          OS_CPU_Start              (void);

void          OS_CPU_Init               (void);

CPU_STK      *OS_CPU_TaskInit           (OS_TASK_PTR    p_task,
                                         void          *p_arg,
                                         CPU_STK       *p_stk_base,
                                         CPU_STK_SIZE   stk_size);

#endif
