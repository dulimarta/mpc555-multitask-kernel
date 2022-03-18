/*
 * $Id: except.S,v 2.9 2000/12/08 22:22:43 dulimart Exp $
 *
 * Filename: except.S
 * Author: Hans Dulimarta [dulimart@computer.org]
 * Description:
 *    Exception Vector Definition
 *
 * $Log: except.S,v $
 * Revision 2.9  2000/12/08 22:22:43  dulimart
 * Added DECHandler
 *
 * Revision 2.8  2000/12/06 17:51:48  dulimart
 * Reuse general stack for some of the interrupt handler
 *
 * Revision 2.7  2000/12/05 16:20:47  dulimart
 * Removed DisableInt and EnableInt macros, added Trace Interrupt Handler
 *
 * Revision 2.6  2000/11/16 19:59:02  dulimart
 * Used symbolic name for displacement in SaveArea. Declared the SaveArea
 * structure.
 *
 * Revision 2.5  2000/11/03 20:41:38  dulimart
 * Changed layout of system ave area to include R1 and LR.
 * Moved SaveRegs and RestRegs into a function.
 * Added LoadContext for multi-tasking context switching.
 * Moved definition of system save area to crt0.S.
 *
 * Revision 2.4  2000/10/13 04:58:54  dulimart
 * Enabled SaveFPT and RestFPR (save and restore floating point registers).
 *
 * Revision 2.3  2000/09/26 16:47:44  dulimart
 * Simplified initialization code, deleted savefprs function
 *
 * Revision 2.2  2000/06/09 13:38:27  dulimart
 * Modified handling of external interrupt from fixed branch instructions
 * to array of function pointers.
 *
 * Revision 2.1  2000/05/23 16:35:10  dulimart
 * Used R2 as temporary register, added code for saving/restoring FP registers
 * Reversed the order of restoration.
 *
 * Revision 2.0  2000/04/24 20:20:50  dulimart
 * Add Alignment, Program, and Floating Point interrupt handler
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#--- Macro: Interrupt Handler Prolog Code 
# on entry, SPRG0 points to the save area
          .macro    IntrProlog
#          isync
          mtspr     81,%r0        /* disable external interrupt */
          mtsprg    1,%r1         /* save R1 to SPRG 1 */
          mflr      %r1
          mtsprg    2,%r1         /* save LR to SPRG 2 */
          mfsprg    %r1,1
          subi      %r1,%r1,EndSA
          bl        SaveRegs
          /* R1 and LR were not saved by 'SaveRegs' */
          mfsprg    %r2,1         /* R2 contains copy of original R1 */
          stw       %r2,GPR1(%r1) /* original R1 is now saved */
          mfsprg    %r2,2         /* R2 contains copy of original LR */
          stw       %r2,LR(%r1)   /* oridinal LR is now saved */
          stwu      %r1,-8(%r1)
          .endm

#--- Macro: Interrupt Handler Prolog Code
          .macro    IntrEpilog
          addi      %r1,%r1,8
          bl        RestRegs
          /* R1 and LR were not restored by 'RestRegs' */
          mtsprg    2,%r2         /* save R2 in SPRG2 */
          lwz       %r2,LR(%r1)   /* get copy of LR */ 
          mtlr      %r2           /* restore LR */
          lwz       %r2,GPR1(%r1) /* get copy of R1 */ 
          mr        %r1,%r2       /* restore R1 */
          mfsprg    %r2,2         /* restore R2 */
          .endm

#------------------------ END OF MACROS ------------------------
 
/*
! System Save Area is used for saving register during interrupt handler.
! The space required for saving all registers is 416 bytes, but the end
! (higher address) of the System Save Area is used for stack during
! interrupt handler call processing.
!
! The layout of this area is as follows:
!      Offset          Content
!        0               SRR0
!        4               SRR1
!        8                R0
!       12                R1
!       16                R2
!       ...              ...
!      132                R31
!      136                CR   (condition register)
!      140                XER  (integer exception register)
!      144                CTR  (counter register)
!      148                LR   (link register)
!      152                FPR0 (floating-point reg0)
!      160                FPR1 (floating-point reg1)
!      ...                ...
!      152+k*8            FPRk 
!      ...                ...
!      400                FPR31 (floating-point reg31)
!      408                FPSCR (FP status & control register)
*/

          .struct   0
SRR0:     .org      .+4
SRR1:     .org      .+4
          .irp      k,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,\
                      17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
GPR\k:    .org      .+4
          .endr
CR:       .org      .+4
XER:      .org      .+4 
CTR:      .org      .+4 
LR:       .org      .+4 
          .irp      k,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,\
                      17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
FPR\k:    .org      .+8
          .endr
FPSCR:    .org      .+8
EndSA:    .org      .+8

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! WARNING: The above layout has to match with the Task::Context !!
!!          structure                                            !!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

          .global   HandlerPtr
          .data
HandlerPtr: /* array of pointer to function */
          .rep      16
          .long     0
          .endr

          .global   DECHandler
DECHandler:
          .long     0

#--- Macro: Interrupt Handler Loop
          .macro    HandlerLoop msg
          lis       %r1,STKTOP@ha
          addi      %r1,%r1,STKTOP@l-16
          lis       %r3,($+20)@ha
          addi      %r3,%r3,($+16)@l
          li        %r4,1
          bl        fputs
          b         $         /* loop */
          .asciz    "\msg"
          .endm

#-----------------------------------------------------------
#--- System Reset ------------------------------------------
          .section  .init
          .org      0x100
Reset:
#if (defined(ram) || defined (PB555))
          b         START
#else
          b         START+0x400000
#endif
          .asciz    "$Revision: 2.9 $"
          .asciz    "$Date: 2000/12/08 22:22:43 $"

#--- Machine Check -----------------------------------------
          .org      0x200
          HandlerLoop MachineCheck

          .org      0x300
          rfi
          
          .org      0x400
          rfi

#--- External Interrupt ------------------------------------
          .org      0x500
          IntrProlog
          /* Used of SIVEC has an advantage due to the 
           * availability of highest priority detector circuit
           */
          lis       %r9,0x30 /* load SIVEC at 0x2FC01C */
          addi      %r10,%r9,-16384
          lbz       %r11,0x1C(%r10)

          /* get the Array of Ptr to Handler Function */
          lis       %r5,HandlerPtr@ha
          addi      %r5,%r5,HandlerPtr@l

          /* get the Ptr to Handler Function */
          lwzx      %r3,%r5,%r11

          /* R3   is  HandlerPtr[SIVEC] */
          cmpwi     %r3,0
          beq       1f
          /* at this point, R3 is not zero, call the function */
          mtctr     %r3
          bctrl
1:        IntrEpilog
          rfi

#--- Alignment Interrupt -----------------------------------
          .org      0x600
          HandlerLoop Alignment

#--- Program Interrupt -------------------------------------
          .org      0x700
          HandlerLoop Program

#--- Floating-Point Interrupt ------------------------------
          .org      0x800
          HandlerLoop Floating_Point_Unavailable

#--- Decrementer -------------------------------------------
          .org      0x900
          IntrProlog
          lis       %r4,DECHandler@ha
          lwz       %r5,DECHandler@l(%r4)
          cmpwi     %r5,0
          beq       2f
          mtlr      %r5
          blrl
2:        IntrEpilog
          rfi

          .org      0xA00
          rfi

          .org      0xB00
          rfi

#--- System Call -------------------------------------------
          .org      0xC00
          HandlerLoop SystemCall

#--- Trace -------------------------------------------------
          .org      0xD00
          IntrProlog
          bl        TraceHandler
          IntrEpilog
          rfi

#--- Floating-Point Assist ---------------------------------
          .org      0xE00
          HandlerLoop FloatingPointAssist

          .org      0xF00
          rfi

#--- Software-Emulation ------------------------------------
          .org      0x1000
          lis       %r1,STKTOP@ha
          addi      %r1,%r1,STKTOP@l-16
          mfsrr0    %r3
          bl        SoftEmulHandler
          b         $         /* loop */

          .org      0x1100
          rfi

          .org      0x1200
          rfi

#--- Instruction Protection Error --------------------------
          .org      0x1300
          HandlerLoop InstructionProtectionError

#--- Data Protection Error ---------------------------------
          .org      0x1400
          HandlerLoop DataProtectionError

          .irp n,1500,1600,1700,1800,1900,1A00,1B00
          .org 0x\n
          li        %r3,0x\n
          mfsrr0    %r4
          b         ExceptionHandler
          .endr

#--- Data Breakpoint ---------------------------------------
          .org      0x1C00
          rfi

#--- Instruction Breakpoint --------------------------------
          .org      0x1D00
          rfi
          
#--- Maskable External Breakpoint --------------------------
          .org      0x1E00
          rfi
          
#--- Non-Maskable External Breakpoint ----------------------
          .org      0x1F00
          rfi

#-----------------------------------------------------------
# Save all Registers (except R1 and LR)
# R1 points to the save area to use
SaveRegs:
          stw       %r0,GPR0(%r1)    /* save R0 */
          stmw      %r2,GPR2(%r1)   /* save R2 - R31 */
          mfsrr0    %r2
          stw       %r2,SRR0(%r1)    /* save SRR0 */
          mfsrr1    %r2
          stw       %r2,SRR1(%r1)    /* save SRR1 */
          mfcr      %r2
          stw       %r2,CR(%r1)  /* save Condition Register */
          mfxer     %r2
          stw       %r2,XER(%r1)  /* save Integer Exception Register */
          mfctr     %r2
          stw       %r2,CTR(%r1)  /* save Count Register */

#--- Save Floating Point Registers
          /* Note: 
           *    On entry to interrupt handler the FP bit in MSR 
           * is cleared. Therefore, to use any FP instructions
           * the FP bit has to be set again
           */
          mfmsr     %r2
          ori       %r2,%r2,0x2000 /* FP available */
          mtmsr     %r2
          .irp      k,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,\
                      17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
          stfd      %f\k,FPR0+8*\k(%r1)
          .endr
          mffs      %f2
          stfd      %f2,FPSCR(%r1)
          blr

#-----------------------------------------------------------
# Restore all Registers (except R1 and LR)
# R1 points to the save area to use
RestRegs:

#--- Restore Floating Point Registers
          lfd       %f2,FPSCR(%r1)
          mtfsf     255,%f2
          .irp      k,31,30,29,28,27,26,25,24,24,22,21,20,19,18,\
                      17,16,15,14,14,12,11,10,9,8,7,6,5,4,4,2,1,0
          lfd       %f\k,FPR0+8*\k(%r1)
          .endr

#--- Restore General Purpose  and Other Registers
#
# CAUTION: DO NOT restore LR here because it will screw up
#          the return address when the 'blr' instruction is executed
#
          lwz       %r2,CTR(%r1)  /* restore Count Register */
          mtctr     %r2
          lwz       %r2,XER(%r1)  /* restore Int. Exception Register */
          mtxer     %r2
          lwz       %r2,CR(%r1)   /* restore Condition Register */
          mtcr      %r2
          lwz       %r2,SRR1(%r1)    /* restore SRR1 */
          mtsrr1    %r2
          lwz       %r2,SRR0(%r1)    /* restore SRR0 */
          mtsrr0    %r2
          lmw       %r2,GPR2(%r1)    /* restore R2 - R31 */
          lwz       %r0,GPR0(%r1)    /* restore R0 */
          blr

          .end
