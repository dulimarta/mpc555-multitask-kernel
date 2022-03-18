/*
 * $Id: qsm.c,v 2.5 2000/10/11 19:45:32 dulimart Exp $
 *
 * Author: Hans Dulimarta, Ph.D 
 *
 * Description:
 *    This file contains routines for interfacing with the 
 * Serial Communication modules on the MPC555 chip.
 *
 * $Log: qsm.c,v $
 * Revision 2.5  2000/10/11 19:45:32  dulimart
 * Clarified comment for PORT symbol.
 *
 * Revision 2.4  2000/10/04 19:16:33  dulimart
 * Modified for output to either COM1 or COM2
 *
 * Revision 2.3  2000/09/26 17:15:35  dulimart
 * Enabled Receiver Interrupt
 *
 * Revision 2.2  2000/09/06 22:06:32  dulimart
 * Modified the calculation of baud rate divisor based on actual system frequency.
 *
 * Revision 2.1  2000/08/02 15:50:52  dulimart
 * Wrote code for queued operation
 *
 *
 * Copyright (c) 1999,2000   Hans Dulimarta, Ph.D.
 *
 */
#ifdef DEBUG
#include "io.h"
#include "qadc.h"
#endif
#include "mpc555.h"
#include "qsm.h"
#include "uimb.h"
#include "usiu.h"
#include "intr.h"

#define BAUD_RATE 19200
/* Use COM1 if PORT = 1, or COM2 if PORT = 2 */
#define PORT 1

//#undef QUEUED_TX
//#undef QUEUED_RX
//#define RX_INTR

#ifdef QUEUED_RX
static int SCptr = 0;
#endif

#define SC_BUFFSIZE 10
static char SCbuff1 [SC_BUFFSIZE], SCbuff2 [SC_BUFFSIZE];
#ifdef RX_INTR
static short SChead1 = 0, SChead2 = 0;
#endif
static short SCtail1 = 0, SCtail2 = 0;
static volatile short SCcnt1 = 0, SCcnt2 = 0;

int SYS_FREQ;  /* system frequency */

#if PORT == 1
#define STAT_REG  QSM.sc1sr
#define DATA_REG  QSM.sc1dr
#else
#define STAT_REG  QSM.sc2sr
#define DATA_REG  QSM.sc2dr
#endif

void qsm_rx_handler ();
void
qsm_init ()
{
   int mf,          /* multiplication factor */
      divf,        /* division factor */
      dfmode;      /* div factor mode */

   divf = USIU.plprcr & 0x1F;
   mf = USIU.plprcr >> 20;
   dfmode = USIU.plprcr & BITSET (21,32);
   /* Oscillator Crystal is 4MHz */
   SYS_FREQ = 4000000 * (mf + 1) / (divf + 1);
   if (dfmode) /* system clock is determined by DFNL */
      SYS_FREQ >>= ((USIU.sccr >> 4) & 0x07) + 1;
   else
      SYS_FREQ >>= (USIU.sccr & 0x07);
   QSM.sc1cr0 = SYS_FREQ / (32 * BAUD_RATE);
   QSM.sc2cr0 = SYS_FREQ / (32 * BAUD_RATE);
   if (UIMB.mcr & BITSET(3,32)) {
      QSM.sc1cr0 >>= 1;
      QSM.sc2cr0 >>= 1;
   }
   QSM.sc1cr1 = 
#ifdef RX_INTR
      BITSET (10, 16) |         /* Rx interrupt enable */
#endif
      BITSET (12, 16) |         /* Tx enable */
      BITSET (13, 16) |         /* Rx enable */
      0;
   QSM.sc2cr1 = 
#ifdef RX_INTR
      BITSET (10, 16) |         /* Rx interrupt enable */
#endif
      BITSET (12, 16) |         /* Tx enable */
      BITSET (13, 16) |         /* Rx enable */
      0;
#ifdef QUEUED_RX
   QSM.qsc1cr &= ~BITSET(10,16);
   QSM.qsc1cr |= /*BITSET(4,16) QTHFI
      | BITSET(5,16) QBHFI
      | */BITSET(10,16); /* Queued Rx enable */
   QSM.qsc1sr &= ~BITSET(4,16); /* Clear QTHF */
   QSM.qsc1sr &= ~BITSET(5,16); /* Clear QBHF */
#endif
#ifdef RX_INTR
   QSM.dsci_il = COM1IRQ;
   SetLVLHandler (COM1IRQ, qsm_rx_handler);
#endif
   my_printf ("COM ports are initialized at %d baud\n", BAUD_RATE);
}

void
qsm_reset ()
{
   QSM.sc1cr1 = 0;
   QSM.sc2cr1 = 0;
   QSM.qsc1cr = 0;
}

#ifdef QUEUED_TX
void
SendByte (unsigned char sbyte)
{
   /* check TC */
   while ((STAT_REG & BITSET(8,16)) == 0);
   QSM.qsc1cr &= 0xFFF0; /* clear QTSZ: sending 1 byte */

   /* check QTHE */
   while ((QSM.qsc1sr & BITSET(6,16)) == 0);
   QSM.qsc1sr &= ~BITSET(6,16); /* Clear QTHE */
   QSM.sctq [0] = sbyte;
   QSM.qsc1cr |= BITSET(9,16); /* Queued Tx enable */
#else
void
SendByte (unsigned char sbyte)
{
   /* wait until Tx register is empty */
   while ((QSM.sc1sr & BITSET (7,16)) == 0);
   QSM.sc1dr = (short) sbyte;
}

void
SendByte2 (unsigned char sbyte)
{
   /* wait until Tx register is empty */
   while ((QSM.sc2sr & BITSET (7,16)) == 0);
   QSM.sc2dr = (short) sbyte;
}
#endif

/*
 * returns 0 if no serial data is waiting to be received 
 */
#ifdef QUEUED_RX
int
SerialReady ()
{
   return ((QSM.qsc1sr & 0xF0) >> 4 != SCptr);
}
#else
int SerialReady ()
{
#ifdef RX_INTR
   return SCcnt1;
#else
   return QSM.sc1sr & BITSET (9, 16);
#endif
}
int COM2Ready ()
{
#ifdef RX_INTR
   return SCcnt2;
#else
   return QSM.sc2sr & BITSET (9, 16);
#endif
}
#endif

/*-------------------------------------------------------------------------*/
/*
 * wait for a byte to be received from the serial port.
 */
#ifdef QUEUED_RX
char
RecvByte ()
{
   char rbyte;
   int nc;
   short qst, /*qcr, */st;

   //my_fprintf (STDOUT, "%s %d\n", __FUNCTION__, k);
   do {
      qst = QSM.qsc1sr;
      nc = (qst & 0xF0) >> 4;
   } while (nc == SCptr);
   rbyte = QSM.scrq [SCptr];
   SCptr++;
   if (SCptr == 16) SCptr = 0;
   if (qst & BITSET(4,16)) {
      st  = STAT_REG; /* this read sequence is necessary */
      QSM.qsc1sr &= ~BITSET(4,16);
   }
   if (qst & BITSET(5,16)) {
      st  = STAT_REG; /* this read sequence is necessary */
      QSM.qsc1sr &= ~BITSET(5,16);
   }
   return rbyte;
}
#else
char
RecvByte ()
{
   char rbyte;

#ifdef RX_INTR
   while (SCcnt1 == 0);
   rbyte = SCbuff1 [SChead1];
   SChead1++;
   if (SChead1 == SC_BUFFSIZE)
      SChead1 = 0;
   SCcnt1--;
#else
   while (!SerialReady ());     /* wait for new incomming data */
   rbyte = QSM.sc1dr;           /* read the port */
#endif
   return rbyte;
}

char
RecvByte2 ()
{
   char rbyte;
#ifdef RX_INTR
   while (SCcnt2 == 0);
   rbyte = SCbuff2 [SChead2];
   SChead2++;
   if (SChead2 == SC_BUFFSIZE)
      SChead2 = 0;
   SCcnt2--;
#else
   while (!COM2Ready ());     /* wait for new incomming data */
   rbyte = QSM.sc2dr;           /* read the port */
#endif
   return rbyte;
}
#endif

#ifdef QUEUED_RX
void
qsm_q_handler ()
{
   short st;
   short qst;
#ifdef DEBUG
   static int k = 0;
   int i;
#endif

   qst = QSM.qsc1sr;
   st = STAT_REG;
   if (qst & BITSET(4,16)) { /* QTHF is set */
      st = STAT_REG; 
      QSM.qsc1sr &= ~BITSET(4,16);
#ifdef DEBUG
      for (i=0; i<8; i++)
         fputc (QSM.scrq[i], STDOUT);
      my_fprintf (STDOUT, "qsm_h %d: %x %x\n", k++, qst, st);
#endif
   }
   else if (qst & BITSET(5,16)) { /* QBHF is set */
      st = STAT_REG; 
      QSM.qsc1sr &= ~BITSET(5,16);
#ifdef DEBUG
      for (i=0; i<8; i++)
         fputc (QSM.scrq[i+8], STDOUT);
      my_fprintf (STDOUT, "QSM_H %d: %x %x\n", k++, qst, st);
#endif
   }
   /*
   else
      my_fprintf (STDOUT, "Here %d %x %x %x\n", k++, qst, QSM.qsc1cr, st);
      */
}
#endif

void qsm_rx_handler ()
{
   short ch;
   short st;

   st = QSM.sc1sr; 
   ch = QSM.sc1sr;
   if ((st & 0xF) == 0) {
      if (SCcnt1 < SC_BUFFSIZE) {
         SCbuff1[SCtail1++] = ch;
         if (SCtail1 == SC_BUFFSIZE)
            SCtail1 = 0;
         SCcnt1++;
      }
   }
   st = QSM.sc2sr; 
   ch = QSM.sc2sr;
   if ((st & 0xF) == 0) {
      if (SCcnt2 < SC_BUFFSIZE) {
         SCbuff2[SCtail2++] = ch;
         if (SCtail2 == SC_BUFFSIZE)
            SCtail2 = 0;
         SCcnt2++;
      }
   }
}
