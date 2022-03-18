/**
 * @file arch.S
 *
 * @author Hans Dulimarta  [dulimart@computer.org]
 *
 * @brief Routines for accessing the PowerPC architecture.
 *
 * PowerPC defines three level of architecture:
 *   @arg UISA: User Instruction Set Architecture
 *   @arg VEA: Virtual Environment Architecture
 *   @arg OEA: Operating Environment Architecture
 *
 */
/*
 * $Id: arch.S,v 2.2 2000/12/05 16:18:45 dulimart Exp $
 *
 * $Log: arch.S,v $
 * Revision 2.2  2000/12/05 16:18:45  dulimart
 * Used SPRs 80 and 81 for enabling/disabling external interrupt.
 * Added coded to enable/disable trace interrupt.
 *
 * Revision 2.1  2000/11/16 19:57:14  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 1999, 2000  Hans Dulimarta, Ph.D.
 */

#----------------------------------------------------------------
# int GetDEC();
          .global  GetDEC
GetDEC:
          mfdec    %r3
          blr

#----------------------------------------------------------------
# int GetMSR();
          .global  GetMSR
GetMSR:
          mfmsr    %r3
          blr

#----------------------------------------------------------------
# void GetTB (int *, int *);
          .global  GetTB
GetTB:
          mftbu    %r5
          mftb     %r6
          mftbu    %r7
          # check for possible carry from TBL to TBU
          cmpw     %r5,%r7
          bne      GetTB

          mr.      %r3,%r3
          beq      1f
          stw      %r6,0(%r3)
1:
          mr.      %r4,%r4
          beqlr
          stw      %r7,0(%r4)
          blr

#----------------------------------------------------------------
# void EnableInterrupt()
          .global   EnableInterrupt
EnableInterrupt:
          mtspr     80,%r0            /* See section 3.9.10.1 MPC555 Manual */
          blr

#----------------------------------------------------------------
#int DisableInterrupt();
          .global   DisableInterrupt
DisableInterrupt:
          mfmsr     %r3 
          mtspr     81,%r0            /* See section 3.9.10.1 MPC555 Manual */
          blr                         /* return value in %r3 */

#----------------------------------------------------------------
#void RestoreInterrupt(int s);
          .global   RestoreInterrupt
RestoreInterrupt:
          sync
          mtmsr     %r3
          blr

#----------------------------------------------------------------
EnablePowerMgt:
          mfmsr     %r0;
          oris      %r0,%r0,0x0004;
          mtmsr     %r0
          blr

#----------------------------------------------------------------
DisablePowerMgt:
          mfmsr     %r4;
          andis.    %r5,%r4,0xFFFB; /* 16 bits LSB is 0 */
          rlwimi    %r4,%r5,0,0,15;
          mtmsr     %r4
          blr

#----------------------------------------------------------------
# void EnableTrace()
          .global   EnableTrace
EnableTrace:
          mfmsr     %r4
          ori       %r4,%r4,0x600
          mtmsr     %r4
          blr

#----------------------------------------------------------------
# void DisableTrace()
          .global   DisableTrace
DisableTrace:
          mfmsr     %r4
          andis.    %r5,%r4,0xF9FF
          rlwimi    %r4,%r5,0,16,31
          mtmsr     %r4
          blr

          .end
