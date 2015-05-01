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
*                                     MEMORY PARTITION MANAGEMENT
*
* Filename      : os_mem.c
* Version       : V1.00.00
* Programmer(s) : JBL
*********************************************************************************************************
*/

#define   MICRIUM_SOURCE
#include "os.h"


#if (OS_CFG_MEM_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                      CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-III-HWOS.
*
* Arguments   : p_mem    Pointer to a memory partition control block which is allocated in
*                        user memory space.
*
*               p_name   Pointer to an ASCII string to provide a name to the memory partition.
*
*               p_addr   Starting address of the memory partition
*
*               n_blks   Number of memory blocks to create from the partition.
*
*               blk_size Size (in bytes) of each block in the memory partition.
*
*               p_err    Pointer to a variable containing an error message which will be set by this function to
*                        either:
*
*                        OS_ERR_NONE                    if the memory partition has been created correctly.
*                        OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the memory partition after you
*                                                         called OSSafetyCriticalStart().
*                        OS_ERR_MEM_INVALID_BLKS        user specified an invalid number of blocks (must be >= 2)
*                        OS_ERR_MEM_INVALID_P_ADDR      if you are specifying an invalid address for the memory
*                                                       storage of the partition or, the block does not align on a
*                                                       pointer boundary
*                        OS_ERR_MEM_INVALID_SIZE        user specified an invalid block size
*                                                         - must be greater than the size of a pointer
*                                                         - must be able to hold an integral number of pointers
* Returns    : none
*********************************************************************************************************
*/

void  OSMemCreate (OS_MEM       *p_mem,
                   CPU_CHAR     *p_name,
                   void         *p_addr,
                   OS_MEM_QTY    n_blks,
                   OS_MEM_SIZE   blk_size,
                   OS_ERR       *p_err)
{
#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    CPU_DATA       align_msk;
#endif
    OS_MEM_QTY     i;
    OS_MEM_QTY     loops;
    CPU_INT08U    *p_blk;
    void         **p_link;
    CPU_SR_ALLOC();


#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to call from an ISR                      */
       *p_err = OS_ERR_MEM_CREATE_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_addr == DEF_NULL) {                                   /* Must pass a valid address for the memory part.       */
       *p_err   = OS_ERR_MEM_INVALID_P_ADDR;
        return;
    }
    if (n_blks < 2u) {                                          /* Must have at least 2 blocks per partition            */
       *p_err = OS_ERR_MEM_INVALID_BLKS;
        return;
    }
    if (blk_size < sizeof(void *)) {                            /* Must contain space for at least a pointer            */
       *p_err = OS_ERR_MEM_INVALID_SIZE;
        return;
    }
    align_msk = sizeof(void *) - 1u;
    if (align_msk > 0u) {
        if (((CPU_ADDR)p_addr & align_msk) != 0u){              /* Must be pointer size aligned                         */
           *p_err = OS_ERR_MEM_INVALID_P_ADDR;
            return;
        }
        if ((blk_size & align_msk) != 0u) {                     /* Block size must be a multiple address size           */
           *p_err = OS_ERR_MEM_INVALID_SIZE;
            return;
        }
    }
#endif

    p_link = (void **)p_addr;                                   /* Create linked list of free memory blocks             */
    p_blk  = (CPU_INT08U *)p_addr;
    loops  = n_blks - 1u;
    for (i = 0u; i < loops; i++) {
        p_blk +=  blk_size;
       *p_link = (void  *)p_blk;                                /* Save pointer to NEXT block in CURRENT block          */
        p_link = (void **)(void *)p_blk;                        /* Position     to NEXT block                           */
    }
   *p_link             = DEF_NULL;                              /* Last memory block points to NULL                     */

    CPU_CRITICAL_ENTER();
#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    p_mem->Type        = OS_OBJ_TYPE_MEM;                       /* Set the type of object                               */
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_mem->NamePtr     = p_name;                                /* Save name of memory partition                        */
#else
    (void)&p_name;
#endif
    p_mem->AddrPtr     = p_addr;                                /* Store start address of memory partition              */
    p_mem->FreeListPtr = p_addr;                                /* Initialize pointer to pool of free blocks            */
    p_mem->NbrFree     = n_blks;                                /* Store number of free blocks in MCB                   */
    p_mem->NbrMax      = n_blks;
    p_mem->BlkSize     = blk_size;                              /* Store block size of each memory blocks               */

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_MemDbgListAdd(p_mem);
#endif

    OSMemQty++;

    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
}


/*
************************************************************************************************************************
*                                                  GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : p_mem   is a pointer to the memory partition control block
*
*               p_err   is a pointer to a variable containing an error message which will be set by this function to
*                       either:
*
*                       OS_ERR_NONE               if the memory partition has been created correctly.
*                       OS_ERR_MEM_INVALID_P_MEM  if you passed a NULL pointer for 'p_mem'
*                       OS_ERR_MEM_NO_FREE_BLKS   if there are no more free memory blocks to allocate to the caller
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
************************************************************************************************************************
*/

void  *OSMemGet (OS_MEM  *p_mem,
                 OS_ERR  *p_err)
{
    void    *p_blk;
    CPU_SR_ALLOC();


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_mem == DEF_NULL) {                                    /* Must point to a valid memory partition               */
       *p_err  = OS_ERR_MEM_INVALID_P_MEM;
        return (DEF_NULL);
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_mem->NbrFree == 0u) {                                 /* See if there are any free memory blocks              */
        CPU_CRITICAL_EXIT();
       *p_err = OS_ERR_MEM_NO_FREE_BLKS;                        /* No,  Notify caller of empty memory partition         */
        return (DEF_NULL);                                      /* Return NULL pointer to caller                        */
    }
    p_blk              = p_mem->FreeListPtr;                    /* Yes, point to next free memory block                 */
    p_mem->FreeListPtr = *(void **)p_blk;                       /* Adjust pointer to new free list                      */
    p_mem->NbrFree--;                                           /* One less memory block in this partition              */
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;                                        /* No error                                             */
    return (p_blk);                                             /* Return memory block to caller                        */
}


/*
************************************************************************************************************************
*                                                 RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : p_mem    is a pointer to the memory partition control block
*
*               p_blk    is a pointer to the memory block being released.
*
*               p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE               if the memory block was inserted into the partition
*                            OS_ERR_MEM_FULL           if you are returning a memory block to an already FULL memory
*                                                      partition (You freed more blocks than you allocated!)
*                            OS_ERR_MEM_INVALID_P_BLK  if you passed a NULL pointer for the block to release.
*                            OS_ERR_MEM_INVALID_P_MEM  if you passed a NULL pointer for 'p_mem'
************************************************************************************************************************
*/

void  OSMemPut (OS_MEM  *p_mem,
                void    *p_blk,
                OS_ERR  *p_err)
{
    CPU_SR_ALLOC();


#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_mem == DEF_NULL) {                                    /* Must point to a valid memory partition               */
       *p_err  = OS_ERR_MEM_INVALID_P_MEM;
        return;
    }
    if (p_blk == DEF_NULL) {                                    /* Must release a valid block                           */
       *p_err  = OS_ERR_MEM_INVALID_P_BLK;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_mem->NbrFree >= p_mem->NbrMax) {                      /* Make sure all blocks not already returned            */
        CPU_CRITICAL_EXIT();
       *p_err = OS_ERR_MEM_FULL;
        return;
    }
    *(void **)p_blk    = p_mem->FreeListPtr;                    /* Insert released block into free block list           */
    p_mem->FreeListPtr = p_blk;
    p_mem->NbrFree++;                                           /* One more memory block in this partition              */
    CPU_CRITICAL_EXIT();
   *p_err              = OS_ERR_NONE;                           /* Notify caller that memory block was released         */
}


/*
************************************************************************************************************************
*                                           ADD MEMORY PARTITION TO DEBUG LIST
*
* Description : This function is called by OSMemCreate() to add the memory partition to the debug table.
*
* Arguments   : p_mem    Is a pointer to the memory partition
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void  OS_MemDbgListAdd (OS_MEM  *p_mem)
{
    p_mem->DbgPrevPtr               = DEF_NULL;
    if (OSMemDbgListPtr == DEF_NULL) {
        p_mem->DbgNextPtr           = DEF_NULL;
    } else {
        p_mem->DbgNextPtr           =  OSMemDbgListPtr;
        OSMemDbgListPtr->DbgPrevPtr =  p_mem;
    }
    OSMemDbgListPtr                 =  p_mem;
}
#endif


/*
************************************************************************************************************************
*                                           INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by uC/OS-III to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_MemInit (OS_ERR  *p_err)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OSMemDbgListPtr = DEF_NULL;
#endif

    OSMemQty        = 0u;
   *p_err           = OS_ERR_NONE;
}
#endif
