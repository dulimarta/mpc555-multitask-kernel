/*
 * $Id: mpc555.c,v 2.3 2000/10/11 19:44:36 dulimart Exp $
 *
 * Author: Hans Dulimarta  [dulimart@compute.org]
 * Description:
 *    This file contains routines needed for initializing the MPC555
 * hardware and various units in it.
 *
 * $Log: mpc555.c,v $
 * Revision 2.3  2000/10/11 19:44:36  dulimart
 * Replaced init function declaration with include to create correct
 * dependency. Inserted declaration of debug_level.
 *
 * Revision 2.2  2000/09/26 17:14:57  dulimart
 * Changed order of initialization and call copy_rom() from init_hardware()
 *
 * Revision 2.1  2000/07/03 19:16:45  dulimart
 * Initialize MPC-555 subsystems inside init_hardware().
 *
 * Revision 2.0  2000/04/24 21:31:25  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include "io.h"
#include "dptram.h"
#include "mpc555.h"
#include "mios.h"
#include "qsm.h"
#include "qadc.h"
#include "tpu.h"
#include "uimb.h"
#include "usiu.h"

#ifdef flash
void copy_rom();
#endif

int debug_level = 0;
void
init_hardware ()
{
   usiu_preinit ();             /* MCR, Base Address */
   mios_preinit ();
   usiu_init ();                /* System Protection, PLL, TimeBase, 
                                 * Real-Time Clock, Periodic Interrupt Timer */
   uimb_init ();                /* U-bus to IMB-bus module */
#ifdef flash
   copy_rom();
#endif

   //DPTRAM.mcr = 0;
   //DPTRAM.bar = 0xFFA0;

   mios_init ();                /* Multi I/O Subsystem */
   qsm_init ();                 /* Queued Serial Module */
   /*lcd_init ();*/
   tpu_init ();                 /* Time Processor Unit */
   qadc_init ();                /* Queued Analog/Digital Converter */
   my_printf ("Reset Status Register is %x\n", USIU.rsr);
   USIU.rsr = 0xFFFF;
}

#ifdef flash
void
copy_rom ()
{
   /* Copy static data initializer from ROM to RAM */
   char *src,
    *dst;
   extern char __DATA_ROM__;
   extern char __DATA_RAM__,
     __DATA_END__;

   src = &__DATA_ROM__;
   dst = &__DATA_RAM__;
   while (dst <= &__DATA_END__) {
      *dst = *src;
      src++;
      dst++;
   }
}
#endif
