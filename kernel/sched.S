/**
 * @file sched.S
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief Low level routine that performs context switching between
 * two tasks.
 */
/*
 * $Id: sched.S,v 2.4 2000/12/06 17:54:13 dulimart Exp $
 *
 * $Log: sched.S,v $
 * Revision 2.4  2000/12/06 17:54:13  dulimart
 * Added disable ext interrupt in SwitchCtx
 *
 * Revision 2.3  2000/11/17 19:11:26  dulimart
 * Added checking of zero pointer (first parameter) in SwitchCtx
 *
 * Revision 2.2  2000/11/14 14:58:47  dulimart
 * Pushed both MSR and LR to the stack area.
 *
 * Revision 2.1  2000/11/13 17:21:33  dulimart
 * Initial check-in
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

          .global SwitchCtx

# Perform Context Switching
# The corresponding C declaration of SwitchCtx is
#
#     void SwitchCtx (int * old_sp, int * new_sp);
#
SwitchCtx:
          mtspr     81,%r0          /* disable interrupt */
          mr.       %r3,%r3
          beq       1f
          mflr      %r5
          stwu      %r5,-4(%r1)     /* Push LR */ 
          mfmsr     %r5
          stwu      %r5,-4(%r1)     /* Push MSR */ 
          stw       %r1,0(%r3)      /* Save SP to old_sp */
1:
          lwz       %r1,0(%r4)      /* Load new SP from new_sp */
          lwz       %r5,4(%r1)      /* Pop LR */
          mtlr      %r5
          lwz       %r5,0(%r1)      /* Pop MSR */
          addi      %r1,%r1,8
          mtmsr     %r5
          blr                       /* return to a different caller */

          .end
