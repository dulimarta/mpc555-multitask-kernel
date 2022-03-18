/*
 * $Id: iodev.c,v 2.7 2000/10/13 15:39:59 dulimart Exp $
 *
 * File: iodev.c
 *
 * Author: Hans Dulimarta, Ph.D.
 *
 * Description: 
 *    This file contains the low-level interface to I/O devices on the
 * MPC555-based controller board, i.e., KeyPad and Receiver.
 *
 * $Log: iodev.c,v $
 * Revision 2.7  2000/10/13 15:39:59  dulimart
 * Excluded RXPeek.
 *
 * Revision 2.6  2000/09/28 19:45:30  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.5  2000/09/14 20:39:44  dulimart
 * Removed debugging messages
 *
 * Revision 2.4  2000/09/12 20:44:37  dulimart
 * Put debugging message inside #ifdef DEBUG
 *
 * Revision 2.3  2000/08/31 14:55:36  dulimart
 * Modified character mapping tables (RXtable and KPtable) from
 * static array to static const array.
 *
 * Revision 2.2  2000/08/03 22:46:37  dulimart
 * Defined new key mapping for the new transmitter.
 *
 * Revision 2.1  2000/07/03 19:35:22  dulimart
 * Removed debugging messages, added KPReady() and RXReady(),
 * added "volatile" modifier to let the compiler generate correct
 * optimized code when checking KPcnt and RXcnt.
 *
 * Revision 2.0  2000/06/22 13:58:31  dulimart
 * Routines for handling low-level I/O devices (receiver & keypad).
 * (Originally written in io.c).
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#include "mpc555.h"
#include "qadc.h"
#include "usiu.h"

#define RX_BUFFSIZE 4
#define KP_BUFFSIZE 4

extern unsigned char KP_DATA;

/* Receiver data */
/* Old Tx: static char RXtable [] = "0123456789ABCDE\n"; */
static const char RXtable [] = "ABC\n369#2580147*";
static char RXbuff [RX_BUFFSIZE];
static short RXhead = 0;
static short RXtail = 0;
static volatile short RXcnt = 0;

/* keypad data */
static const char KPtable [] = "*0#\n789C456B123A";
static char KPbuff [KP_BUFFSIZE];
static short KPhead = 0;
static short KPtail = 0;
static volatile short KPcnt = 0;

void RxHandler () 
{
   unsigned char rx_data;

   rx_data = QADC1.portqa & 0x0F;
   if (RXcnt < RX_BUFFSIZE) {
      RXbuff[RXtail++] = RXtable[rx_data];
      if (RXtail == RX_BUFFSIZE)
         RXtail = 0;
      RXcnt++;
   }
   USIU.sipend = BITSET(4,16);
}

int RXReady () {
   return RXcnt;
}

/**
   Receiver look ahead function.  Check the character in the receiver 
   buffer without reading it.
   @param offset non-negative number indicating the offset in the buffer.
      @arg 0: the first unread character (oldest),
      @arg 1: the second unread character, ...
      @arg n: the most recent unread character
   @return the character requested at the offset (if available) or -1 
   to indicate unavailability
 */
#if 0
unsigned char RXPeek (int offset)
{
   if (offset < RXcnt) {
      offset += RXtail;
      if (offset >= RX_BUFFSIZE)
         offset -= RX_BUFFSIZE;
      return RXbuff[offset];
   }
   else
      return -1;
}
#endif

unsigned char RXGetKey () 
{
   unsigned char d;

   while (RXcnt == 0);
   d = RXbuff [RXhead];
   RXhead++;
   if (RXhead == RX_BUFFSIZE)
      RXhead = 0;
   RXcnt--;
   return d;
}

/* KEYPAD routines */

void
KPHandler () {
   if (KPcnt < KP_BUFFSIZE) {
      KPbuff[KPtail++] = KPtable[KP_DATA];
      if (KPtail == KP_BUFFSIZE)
         KPtail = 0;
      KPcnt++;
   }
   USIU.sipend = BITSET(8,16);
}

int KPReady () {
   return KPcnt;
}

unsigned char KPGetKey() {
   unsigned char d;

   while (KPcnt == 0);
   KPhead++;
   if (KPhead == KP_BUFFSIZE)
      KPhead = 0;
   KPcnt--;
   d = KPbuff[KPhead-1];
   return d;
}
