/**
 * @file usiu.c
 *
 * @author Hans Dulimarta  [dulimart@computer.org]
 *
 * @brief Unified System Interface Unit routine
 *
 * USIU is the Unified System Interface Unit in MPC555. It manages a 
 * number of system-wide configurations for the MPC555 cpu. This 
 * includes Timebase, Periodic Interrupt Timer, Memory mapping, etc.
 *
 */
/*
 * $Id: usiu.c,v 2.10 2001/10/25 16:42:22 dulimart Exp $
 *
 * $Log: usiu.c,v $
 * Revision 2.10  2001/10/25 16:42:22  dulimart
 * Declared PIT_INTERVAL as external symbol.
 *
 * Revision 2.9  2000/11/16 02:55:11  dulimart
 * Moved GetTB() to assembly arch.S
 *
 * Revision 2.8  2000/11/14 18:03:53  dulimart
 * Moved GetDEG() to arch.S
 *
 * Revision 2.7  2000/11/08 20:04:19  dulimart
 * Enabled GetTB and checked for NULL pointer.
 *
 * Revision 2.6  2000/09/28 19:53:37  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.5  2000/09/26 17:52:23  dulimart
 * Modified system initialization to adapt flash, ram, and internal
 * flash operations
 *
 * Revision 2.4  2000/08/02 15:45:58  dulimart
 * Changed the order of BRx/ORx initialization
 *
 * Revision 2.3  2000/06/28 18:47:35  dulimart
 * Removed debugging messages
 *
 * Revision 2.2  2000/06/14 19:08:13  dulimart
 * Set ED2 in SIEL to enable edge interrupt for IRQ2 (Receiver circuit)
 *
 * Revision 2.1  2000/05/23 16:13:09  dulimart
 * Change SetDEC initialization, disable SIE bit in RTCSC,
 * Clear the PRDS bit in PDMCR
 *
 * Revision 2.0  2000/04/25 14:31:20  dulimart
 * initial revision
 *
 *
 * Copyright (c) 1999, 2000  Hans Dulimarta, Ph.D.
 */
#include "mpc555.h"
#include "usiu.h"

/// Interval of the Periodic Interrupt Timer
extern int PIT_INTERVAL; /* in microsecond */

void usiu_preinit () {
   /* Table 6-5 SIU Module Configuration Register */
#ifdef PB555
   USIU.mcr = 0x00420800; 
#else
   USIU.mcr = 0x0; 
#endif

   /*
    * SYPCR can only be written once after Power On Reset 
    * Table 6-13 System Protection Control Register 
    */
   USIU.sypcr = 0xFFFFFF88; 
   /* memory base/option registers */
#ifdef flash
   USIU.br0 = 0x00400003;
   USIU.or0 = 0xFFF80022;
   USIU.br1 = 0x00800003;
   USIU.or1 = 0xFFF80002;
   USIU.br2 = 0x00C00003;
   USIU.or2 = 0xFFFE0022;
   USIU.br3 = 0x01000403;
   USIU.or3 = 0xFFFE0FF1;
#endif

   /* dual mapping base register, Table 10-9 */
#ifdef flash
#ifdef PB555
   USIU.dmbr = 0x00000000; /* disable dual mapping */
#else
   USIU.dmbr = 0x00000001; /* enable dual mapping to CS0 */
#endif
#else
   USIU.dmbr = 0x00000003; /* Enable dual mapping to CS1 */
#endif
   USIU.dmor = 0x60000000; /* Mask the 13 bits of MSB */
}

void usiu_init () {
   int pitc;
   /* SCCR bit description (Table 8-9)
    */
   USIU.sccrk = KEY;
   /* 1000 0000 0010 0000 0000 0000 0000 0000 */
   // |[]| |||| | || |[]| [][-----]  [-]  [-]
   // || | |||| | || || | |    |      |    +-> DFNH   = div 1 
   // || | |||| | || || | |    |      +------> DFNL   = div 2
   // || | |||| | || || | |    +-------------> ENVDIV = div 1
   // || | |||| | || || | +------------------> EECLK  = disabled
   // || | |||| | || || +--------------------> LME    = disabled
   // || | |||| | || |+----------------------> EBDF   = full
   // || | |||| | || +-----------------------> BUCS   = not backup
   // || | |||| | || 
   // || | |||| | |+-------------------------> RTSEL  = external
   // || | |||| | +--------------------------> PRQEN  = switch to hi
   // || | |||| |
   // || | |||| +----------------------------> STBUC  = don't switch
   // || | |||| 
   // || | |||+------------------------------> RTDIV  = div 4
   // || | ||+-------------------------------> TBS    = osc
   // || | |+--------------------------------> LPML   = writable
   // || | +---------------------------------> MFPDL  = writable
   // || | 
   // || +-----------------------------------> DCSLR  = enable switching
   // |+-------------------------------------> COM    = full strength
   // +--------------------------------------> DBCT   = disable
   USIU.sccr = 0x80200000;

   /* clock & low power control registers */
   /* PLPRCR bit descriptions
    * Bit 0:11   : SPLL multiplication factor is 9 (MF = 10)
    * Bit 27:31  : SPLL division factor is 1 (DIVF = 0) 
    * 
    * With the above parameters, VCOOUT = 80 MHz
    */
   USIU.plprcrk = KEY; 
   USIU.plprcr = 0x00900000; 

   /* interrupt flags */
   USIU.sipend = 0xFFFF; /* clear all pending request */
   USIU.simask = 0x0000; /* disable all request to reach CPU */
   /* Set ED2 for Receiver Interrupt and ED4 for Keyboard interrupt */
   USIU.siel   = BITSET(4,16) | BITSET(8,16); 

   /* Timebase Initialization */
   USIU.tbscrk = KEY;
   USIU.tbscr = (IRQ (TBIRQ) << 8) | 0x01; /* Enable Time Base */
   SetDEC (-1); /* required to prevent immediate interrupt */

   /* Setup Real-Time Clock */
   USIU.rtcsck = KEY;      /* unlock RTCSC */
   /* RTCSC: 4MHz driver, Enable RTC, Second Interrupt Enable */
   USIU.rtcsc = (IRQ (RTCIRQ) << 8) |
      BITSET(11,16) | /* BITSET(12,16) | */ BITSET(15,16); 
   USIU.rtck = KEY;       /* unlock RTC */
   USIU.rtc = 1;          /* Reset RTC  */

   /* Setup Periodic Interrupt Timer */
   /* With 4 MHz external clock 
    * PIT Period = PREDIV * (PITC+1) / 4000000 (in second) 
    * or 
    * PIT Period = PREDIV * (PITC+1) / 4 (in microsecond) 
    *
    * PREDIV value (4 or 256) depends on the RTDIV bit in SCCR
    */
   if (PIT_INTERVAL > 0) {
      pitc = 4 * PIT_INTERVAL; /* in microsecond */
      /* divide by 256 or 4 */
      pitc >>= (USIU.sccr & BITSET(7,32)) ? 8 : 2;
      pitc --;
      USIU.pitck = KEY;
      USIU.pitc  = pitc << 16;
      USIU.piscr = (IRQ(PITIRQ) << 8) | BITSET(13,16) | BITSET(15,16);
   }

   //USIU.pdmcr &= ~BITSET (6,32);
}
