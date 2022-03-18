/**
 * @file tpu.c 
 *
 * @author Hans Dulimarta   [dulimart@computer.org]
 *
 * @brief Time Processing Unit routines
 *
 *    This file handles the Time Processor Unit 3 in MPC555 chip. 
 * The primary function used here is the Fast Quadrature Decode which 
 * decodes motor encoder signals into 16-bit counter. This function is
 * needed by a PID controller.
 *
 * MPC555 has two TPU3 units, here identified as TPU_A and TPU_B. 
 * To minimize processing time when simultaneous PID controllers are used,
 * even-numbered PID controller will use TPU_A and odd-numbered ones will
 * use TPU_B. Since, each FQD need two channels, the PID to TPU channels
 * assignment are as follow:
 * 
 *  @arg TPU_A ch-0: PID-0 quadrature chA
 *  @arg TPU_A ch-1: PID-0 quadrature chB
 *  @arg TPU_A ch-2: PID-2 quadrature chB
 *  @arg TPU_A ch-3: PID-2 quadrature chB
 *
 *  @arg TPU_B ch-0: PID-1 quadrature chA
 *  @arg TPU_B ch-1: PID-1 quadrature chB
 *  @arg TPU_B ch-2: PID-3 quadrature chA
 *  @arg TPU_B ch-3: PID-3 quadrature chB
 *
 */
/*
 * $Id: tpu.c,v 2.5 2000/11/08 20:05:14 dulimart Exp $
 *
 * $Log: tpu.c,v $
 * Revision 2.5  2000/11/08 20:05:14  dulimart
 * Replaced unsigned char parameters to int type.
 *
 * Revision 2.4  2000/09/28 19:52:26  dulimart
 * Added doxygen enabled comments in the header
 *
 * Revision 2.3  2000/09/06 22:11:29  dulimart
 * Delete static init in tpu_init()
 *
 * Revision 2.2  2000/08/30 21:30:38  dulimart
 * Changed #if DEBUG to #ifdef DEBUG
 *
 * Revision 2.1  2000/06/28 18:49:44  dulimart
 * Bracketed debugging message with #ifdef DEBUG -- #endif
 *
 * Revision 2.0  2000/04/25 13:51:50  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000  Hans Dulimarta
 */
#include "mpc555.h"
#include "tpu.h"
#ifdef DEBUG
#include "io.h"
#endif

void tpu_init () {
#ifdef DEBUG
   my_printf ("Initializing TPU\n");
#endif
   /* Enable both unit as TPU3 in supervisor mode */
   TPU_A.mcr = BITSET(8,16) | BITSET(10,16);
   TPU_B.mcr = BITSET(8,16) | BITSET(10,16);

   /* TODO: Initialize interrupt: TICR, CIER */
}

void StartTPU (TPU_map *t) {
   t->mcr &= ~BITSET(0,16); 
}

void StopTPU (TPU_map *t) {
   t->mcr |= BITSET(0,16);
}

void SetChanFunc (TPU_map *t, int chan, int func) {
   int indx, shft;

   indx = (15 - chan) / 4;
   shft = (chan % 4) * 4; /* number of bits to shift left */
   t->cfsr[indx] &= ~(0x0F << shft); /* clear the field */
   t->cfsr[indx] |= func << shft; /* set the field */
}

void SetHostSeq (TPU_map *t, int chan, int seq) {
   int indx, shft;

   indx = (15 - chan) / 8;
   shft = (chan % 8) * 2; /* number of bits to shift left */
   t->hsqr[indx] &= ~(0x03 << shft); /* clear the field */
   t->hsqr[indx] |= seq << shft; /* clear the field */
}

void SetHostServReq (TPU_map *t, int chan, int req) {
   int indx, shft;

   indx = (15 - chan) / 8;
   shft = (chan % 8) * 2; /* number of bits to shift left */
   t->hsrr[indx] &= ~(0x03 << shft); /* clear the field */
   t->hsrr[indx] |= req << shft; /* clear the field */
}

void SetChanPrio (TPU_map *t, int chan, int prio) {
   int indx, shft;

   indx = (15 - chan) / 8;
   shft = (chan % 8) * 2; /* number of bits to shift left */
   t->cpr[indx] &= ~(0x03 << shft); /* clear the field */
   t->cpr[indx] |= prio << shft; /* clear the field */
}

void EnableChanIntr (TPU_map *t, unsigned char chan) {
   t->cier |= 1 << chan;
}

void DisableChanIntr (TPU_map *t, unsigned char chan) {
   t->cier &= ~(1 << chan);
}

/* Disable Fast Quadrature Decode function */
void
DisableFQD (int chan)
{
   if ((chan & 1) == 0) { /* even number */
      SetChanPrio (&TPU_A, chan, 0);
      SetChanPrio (&TPU_A, chan + 1, 0);
   }
   else {
      SetChanPrio (&TPU_B, chan - 1, 0);
      SetChanPrio (&TPU_B, chan, 0);
   }
}

/* Enable Fast Quadrature Decode function */
void
EnableFQD (unsigned char chan)
{
   if ((chan & 1) == 0) { /* even number */
      SetChanPrio (&TPU_A, chan, 3); /* high priority */
      SetChanPrio (&TPU_A, chan + 1, 3);
   }
   else {
      SetChanPrio (&TPU_B, chan - 1, 3); /* high priority */
      SetChanPrio (&TPU_B, chan, 3);
   }
}

/* Set Fast Quadrature Decode position counter */
void SetFQDPos (int chan, int pos) {
   if ((chan & 1) == 0) { /* even number */
      TPU_A.ch_parm[chan][1] = pos; /* position count in primary channel */
   }
   else {
      TPU_B.ch_parm[chan-1][1] = pos; /* position count in primary channel */
   }
}

short GetFQDPos (unsigned char chan) {
   if ((chan & 1) == 0) /* even number */
      return TPU_A.ch_parm[chan][1]; /* position count in primary channel */
   else 
      return TPU_B.ch_parm[chan-1][1]; /* position count in primary channel */
}

/* Setup two adjacent channel for Fast Quadrature Decode function */
void
SetupFQD (int chan)
{                               /* chan is the primary channel */
   TPU_map *tpu_ptr;

   DisableFQD (chan);
   /* Set parameter RAM */
	SetFQDPos (chan, 0);

   if ((chan & 1) == 0) { /* even-numbered */
      tpu_ptr = &TPU_A;
   }
   else {
      chan--;
      tpu_ptr = &TPU_B;
   }
   SetChanFunc (tpu_ptr, chan, 0x06); /* primary */
   SetChanFunc (tpu_ptr, chan + 1, 0x06); /* secondary */

   /* Set CORR_PINSTATE_ADDR */
   tpu_ptr->ch_parm[chan][4] = ((chan + 1) << 4) | 0x06; 
   /* Set EDGE_TIME_LSB_ADDR */
   tpu_ptr->ch_parm[chan][5] = (chan << 4) | 0x01; 
   /* CORR_PINSTATE_ADDR */
   tpu_ptr->ch_parm[chan + 1][4] = (chan << 4) | 0x06; 
   /* EDGE_TIME_LSB_ADDR */
   tpu_ptr->ch_parm[chan + 1][5] = (chan << 4) | 0x01; 

   SetHostSeq (tpu_ptr, chan, 0); /* set as primary channel */
   SetHostSeq (tpu_ptr, chan + 1, 1); /* set as secondary channel */
   SetHostServReq (tpu_ptr, chan, 3); /* initialize */
   SetHostServReq (tpu_ptr, chan + 1, 3);
}
