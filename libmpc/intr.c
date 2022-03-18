/*
 * $Id: intr.c,v 2.14 2000/12/08 22:22:18 dulimart Exp $
 *
 * Filename: intr.c 
 * Author: Hans Dulimarta  [dulimart@computer.org]
 * Description:
 *    Interrupt handler function definition.
 *
 * $Log: intr.c,v $
 * Revision 2.14  2000/12/08 22:22:18  dulimart
 * Added SetDECHandler
 *
 * Revision 2.13  2000/12/06 17:50:51  dulimart
 * Added Software Emulation Interrupt Handler
 *
 * Revision 2.12  2000/12/05 16:21:43  dulimart
 * Moved Enabled/Disable Interrupt/Power to assembly code
 *
 * Revision 2.11  2000/10/13 15:40:38  dulimart
 * Modified the message at the ExceptionHandler to display the
 * type of exception.
 *
 * Revision 2.10  2000/10/02 20:15:03  dulimart
 * Disabled PITIRQ in ExceptionHandler
 *
 * Revision 2.9  2000/09/26 18:51:48  dulimart
 * Added doxygen enable comment lines.
 *
 * Revision 2.8  2000/09/18 14:08:32  dulimart
 * Replaced debugging messages with shorter ones.
 *
 * Revision 2.7  2000/09/12 20:45:23  dulimart
 * Commented out LVL7Handler
 *
 * Revision 2.6  2000/08/31 14:49:09  dulimart
 * Replaced #if DEBUG with #ifdef DEBUG
 *
 * Revision 2.5  2000/06/30 12:36:54  dulimart
 * Capped debugging message within #ifdef DEBUG
 *
 * Revision 2.4  2000/06/27 16:05:28  dulimart
 * Removed debugging message from SetxxxHandler()
 *
 * Revision 2.3  2000/06/13 02:50:33  dulimart
 * Renamed hand_ptr_t to handler_ptr_t.
 *
 * Revision 2.2  2000/06/09 13:31:37  dulimart
 * Added SetIRQHandler and SetLVLHandler
 *
 * Revision 2.1  2000/05/23 16:36:06  dulimart
 * Removed SystIntrHandler and ProgramHandler
 *
 * Revision 2.0  2000/04/24 21:00:13  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#include "intr.h"
#include "usiu.h"
#include "mpc555.h"
#ifdef DEBUG
#include "io.h"
#include "qadc.h"
#endif

extern handler_ptr_t HandlerPtr[];

/**
 * Set handler for IRQ interrupt
 * @param num IRQ number
 * @param handler the new IRQ interrupt handler
 */
handler_ptr_t SetIRQHandler (int num, handler_ptr_t handler) {
   handler_ptr_t old_h;

   DisableIRQ (num);
   old_h = HandlerPtr[num*2];
   HandlerPtr [num*2] = handler;
   EnableIRQ (num);
   return old_h;
}

/**
 * Set handler for LEVEL interrupt
 * @param num LEVEL number
 * @param handler the new level interrupt handler
 */
handler_ptr_t SetLVLHandler (int num, handler_ptr_t handler) {
   handler_ptr_t old_h;

   DisableIRQ (num);
   old_h = HandlerPtr[num*2+1];
   HandlerPtr [num*2+1] = handler;
   EnableIRQ (num);
   return old_h;
}

handler_ptr_t SetDECHandler (handler_ptr_t h)
{
   handler_ptr_t old_h;

   old_h = DECHandler;
   DECHandler = h;
   return old_h;
}

void
RTCHandler (void)
{
   /*my_printf ("RTC sivec = %x\n", USIU.sivec);*/
   USIU.rtcsc |= 0x80;          /* clear status */
   /*MIOS.io.dr = USIU.rtc << 2; */
}

/**
 * Default handler for IRQ interrupt
 */
void
IRQ_IH (void)  {
   unsigned short bit_pos;

#ifdef DEBUG
   puts ("Got IRQ ");
   puth (USIU.sivec >> 3);
#endif
   bit_pos = USIU.sivec >> 3;
   USIU.simask &= ~BITSET(bit_pos,16);
   USIU.sipend  =  BITSET(bit_pos,16);
}

/**
 * Default handler for level interrupt
 */
void
LVL_IH (void)  {
   unsigned short bit_pos;

#ifdef DEBUG
   puts ("Got LVL ");
   puth (USIU.sivec >> 3);
#endif
   bit_pos = USIU.sivec >> 3;
   USIU.simask &= ~BITSET(bit_pos,16);
   USIU.sipend  =  BITSET(bit_pos,16);
}

#if 0
void LVL7Handler (void) {
   static int cnt = 0;
   static short oldmask;

   oldmask = USIU.simask;
   USIU.simask = 0;
   cnt++;
   if (UIMB.ipend & BITSET(MIOSLVL+8*MIOSTM0,32)) {
#ifdef DEBUG
      my_printf ("MIOS Handler %d, SR0=%x, ER0=%x, PR0=%x\n", 
            cnt, MIOS.ics[0].isr, MIOS.ics[0].ier, MIOS.ics[0].rpr);
#endif
      MIOS.ics[0].isr = 0; /* indicate that the request has been served */
   }
   else if (UIMB.ipend & BITSET(MIOSLVL+8*MIOSTM1,32)) {
#ifdef DEBUG
      my_printf ("MIOS Handler %d, SR1=%x, ER1=%x, PR1=%x\n", 
            cnt, MIOS.ics[1].isr, MIOS.ics[1].ier, MIOS.ics[1].rpr);
#endif
   }
   USIU.simask = oldmask;
}

void DecHandler (int addr) {
   my_printf ("%s at %x\n", __PRETTY_FUNCTION__, addr);
}
#endif

void
ExceptionHandler (int type, int addr)
{
   int pc, *ptr, k;

   DisableIRQ (PITIRQ);
   asm ("mfsrr0 %0" : "=r" (pc));
   puts ("EH ");
   puth (type);
   putchar (' ');
   puth (pc);
   putchar ('\n');
   ptr = (int *) pc;
   for (k=0; k<8; k++) {
      puth ((int)ptr); putchar (' '); puth (*ptr); putchar ('\n');
      ptr++;
   }
   while (1);
}

void SoftEmulHandler (int addr)
{
   my_printf ("Software Emulation interrupt at %x\n", addr);
}

void TraceHandler ()
{
   asm ("mfsrr0 %r3; li %r4,1; bl fputh");
   putchar (' ');
}
