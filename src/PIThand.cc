// $Id: PIThand.cc,v 2.6 2000/10/04 19:25:42 dulimart Exp $
//
// Author: Hans Dulimarta  dulimart@[egr.msu.edu, computer.org]
//
// Description:
//    This files defines the PIT handler on the MPC-555 chip. On each
// interrupt, the routine handler checks whether any PIDControl object
// is servoing a motor. If yes, the ServoMove method of that object
// is invoked. Otherwise, flash some LEDs to indicate that the handler
// is working.
//
// $Log: PIThand.cc,v $
// Revision 2.6  2000/10/04 19:25:42  dulimart
// Added code for emergency_stop
//
// Revision 2.5  2000/09/28 18:10:11  dulimart
// Modified calling sequence to enable PIDController be called
// at each timer interrupt
//
// Revision 2.4  2000/09/13 23:40:05  dulimart
// Put debugging code within #ifdef DEBUG
//
// Revision 2.3  2000/06/30 16:27:17  dulimart
// Used new function names in io.h
//
// Revision 2.2  2000/06/06 18:50:08  dulimart
// Changed debug switch pin assigment from PORTQA to PORTQB.
//
// Revision 2.1  2000/05/23 17:12:26  dulimart
// Blinked LED in different pattern when Servo is active
//
// Revision 2.0  2000/04/25 17:33:47  dulimart
// initial revision
//
//
// Copyright (c) 2000  Hans Dulimarta, Ph.D.
//
#include "PIDControl.h"
#include "Scheduler.h"
#include "mpc555.h"
#include "qadc.h"
#include "usiu.h"
#ifdef DEBUG
#include "io.h"
#endif

#define NUM_PIDC 3

PIDControl* PID[3] = {0, 0, 0};

void
OldPITHandler (void)
{                               /* Periodic-Inteval Timer */
   static short simask;
   static int cnt = 0, chan = 0;
   extern bool emergency_stop;

   /*my_printf ("PIT sivec = %x\n", USIU.sivec); */
   simask = USIU.simask;        /* save old mask */
   USIU.simask = 0;             /* disable all interrupt */
   if (USIU.piscr & 0x80) {
      USIU.piscr |= 0x80;       /* clear status */
      if (PID[chan]) {
         PID[chan]->ServoMove();
      }
      chan++; /* do a round robin scheduling */
      if (chan == NUM_PIDC) {
         chan = 0;
      }
      if (PIDControl::NUM_MIP) {
         if (cnt == 0) {
            //cnt = 2000;
            cnt = 500000 / ((USIU.pitc >> 16) + 1);
            //my_printf ("PID-%d at %p\n", chan, PID[chan]);
            QADC2.portqa <<= 2;
            if (QADC2.portqa == 0)
               QADC2.portqa = emergency_stop ? 0xF : 2;
         }
         cnt--;
      }
      else {
         if (cnt == 0) {
#if 0
            if ((QADC1.portqb & 1) == 0) { 
               int addr;

               /* Check if DEBUG switch is pressed */
               asm ("mfsrr0 %0" : "=r" (addr));
               fputs ("IP=", STDERR);
               fputh (addr, STDERR);
               fputs (" SYPCR=", STDERR);
               fputh (USIU.sypcr, STDERR);
               fputc ('\n', STDERR);
            }
#endif
#if 0
            /* Blink the LEDs every 1_000_000 usec */
            cnt = 1000000 / ((USIU.pitc >> 16) + 1);
#else
            /* Blink the LEDs 1000 times slower */
            cnt = 1000;
#endif
            QADC2.portqa <<= 1;
            if (QADC2.portqa == 0)
               QADC2.portqa = emergency_stop ? 0xF : 1;
         }
         cnt--;
      }
   }
   USIU.sipend = BITSET(2*PITIRQ+1,15);        /* Clear pending request */
   USIU.simask = simask;        /* restore mask */
}

void
PITHandler (void)
{                               /* Periodic-Inteval Timer */
   //static short simask;
   static int cnt = 0, chan = 0;
   extern bool emergency_stop;

   if (PID[chan]) {
      PID[chan]->ServoMove();
   }
   chan++; /* do a round robin scheduling */
   if (chan == NUM_PIDC) {
      Scheduler::Instance()->Schedule();
   }
   else if (chan == NUM_PIDC + 1) {
      chan = 0;
   }
   if (PIDControl::NUM_MIP) {
      if (cnt == 0) {
         //cnt = 2000;
         cnt = 500000 / ((USIU.pitc >> 16) + 1);
         //my_printf ("PID-%d at %p\n", chan, PID[chan]);
         QADC2.portqa <<= 2;
         if (QADC2.portqa == 0)
            QADC2.portqa = emergency_stop ? 0xF : 2;
      }
      cnt--;
   }
   else {
      if (cnt == 0) {
#if 0
         /* Blink the LEDs every 1_000_000 usec */
         cnt = 1000000 / ((USIU.pitc >> 16) + 1);
#else
         /* Blink the LEDs 1000 times slower */
         cnt = 1000;
#endif
         QADC2.portqa <<= 1;
         if (QADC2.portqa == 0)
            QADC2.portqa = emergency_stop ? 0xF : 0x10;
      }
      cnt--;
   }
   USIU.sipend = BITSET(2*PITIRQ+1,15);        /* Clear pending request */
}
