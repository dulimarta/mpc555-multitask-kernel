/* 
 * $Id: crt0.S,v 2.7 2000/12/06 17:52:25 dulimart Exp $
 *
 * Filename: cr0.S
 * Author: Hans Dulimarta  [dulimart@computer.org]
 * Description:
 *    This file contains the startup code for C programs
 * in the MPC555 environment.
 *
 * $Log: crt0.S,v $
 * Revision 2.7  2000/12/06 17:52:25  dulimart
 * Removed .bss section
 *
 * Revision 2.6  2000/11/30 18:24:23  dulimart
 * Enabled floating point exception bits in MSR
 *
 * Revision 2.5  2000/11/16 19:54:00  dulimart
 * Commented out code for loading of SystSaveArea and TopKernelStack to special
 * purpose registers.
 *
 * Revision 2.4  2000/11/03 21:01:08  dulimart
 * Split system save area into Save Area and Kernel Stack.
 *
 * Revision 2.3  2000/09/18 14:06:56  dulimart
 * Moved call to copy_rom to init_hardware.
 *
 * Revision 2.2  2000/09/14 20:36:07  dulimart
 * IMMR setting depends on board type.
 * Simplified setting up of initial stack frame
 *
 * Revision 2.1  2000/08/02 15:42:45  dulimart
 * Enabled internal flash
 *
 * Revision 2.0  2000/04/24 20:17:57  dulimart
 * C startup code
 *
 *
 * Copyright (c) 2000   Hans Dulimarta
 */

          .text
          .global   START 
START:
          lis       %r11,STKTOP@ha
          addi      %r1,%r11,STKTOP@l

          /* R13 = base address of R/W small data area */
          lis       %r13,_SDA_BASE_@ha
          addi      %r13,%r13,_SDA_BASE_@l

          /* R2 = base address of R/O small data area */
          lis       %r2,_SDA2_BASE_@ha
          addi      %r2,%r2,_SDA2_BASE_@l

          /*
           * System Save Area is used for exception handling.
           * Its base addr is saved in Special Purpose Register 0
           */
#         lis       %r3,SystSaveArea@ha
#         addi      %r3,%r3,SystSaveArea@l
#         mtsprg    0,%r3

          /*
           * Kernel stack is used during exception handling.
           * The top stack is saved in Special Purpose Register 3
           */
#         lis       %r3,TopKernelStack@ha
#         addi      %r3,%r3,TopKernelStack@l-16
#         mtsprg    3,%r3

          /* initialize MPC555 hardware */
          mfmsr     %r0
          ori       %r0,%r0,0x2900
          mtmsr     %r0

           /* Initialize Internal Memory Mapping Register (IMMR) */
#ifdef PB555
          li        %r0,0x800 /* enable internal flash */
#else
          li        %r0,0
#endif
          mtspr     638,%r0

          /* Initialize Burst Buffer MCR */
          li        %r0,0
          mtspr     560,%r0

          /* setup initial stack frame */
          li        %r0,0
          stwu      %r0,-8(%r1)   /* 8-byte alignment */

          bl        init_hardware
          bl        heap_init
          bl        __do_global_ctors
          bl        main     
          /* upon return from main, 
           * %r3 is set to its return value */
#ifdef DEBUG
          bl        heap_fini
#endif
          bl        exit
.LSTART:
          .size     START,.LSTART-START

#          .section  .bss
#SystSaveArea:
#          .rep      104
#          .long     0
#          .endr

#BotKernelStack:
#          .rep      1024
#          .long     0
#          .endr
#          .balign   8      /* ensure double word boundary */
#TopKernelStack:

          .end
