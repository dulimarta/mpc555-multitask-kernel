/*
 * $Id: qadc.c,v 2.5 2000/10/02 20:18:12 dulimart Exp $
 *
 * Filename: qadc.c
 *
 * Author: Hans Dulimarta [dulimart@computer.org]
 *
 * Description:
 *    This file handles the interface with the Queued A/D Converter
 * on the MPC555 chip.
 *
 * $Log: qadc.c,v $
 * Revision 2.5  2000/10/02 20:18:12  dulimart
 * Added blink() for debugging purpose
 *
 * Revision 2.4  2000/08/30 21:31:13  dulimart
 * Changed #if DEBUG to #ifdef DEBUG
 *
 * Revision 2.3  2000/06/30 13:07:09  dulimart
 * Capped debuging messages within #ifdef DEBUG
 *
 * Revision 2.2  2000/06/14 19:49:45  dulimart
 * Set AN56 for digital output. This line controls the momentary/latched
 * output behavior of the receiver circuit.
 *
 * Revision 2.1  2000/05/23 16:53:03  dulimart
 * Added initialization of portqa on QADC1
 *
 * Revision 2.0  2000/04/24 21:35:07  dulimart
 * Queued ADC
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 *
 */
#include "mpc555.h"
#include "qadc.h"
#ifdef DEBUG
#include "io.h"
#endif
#define QADC_PRESCALER 10

void SetADCPrescaler (QADC_map* qa, int a) 
{
   /* Set QADC Clock to FSYS/a Hz, where FSYS is the system frequency */
   int hi,lo;

   lo = a/2 - 1;
   if (lo > 7) lo = 7; /* PSL is a 3-bit field */
   hi = a - lo - 1;
   if (hi > 31) hi = 31; /* PSH is a 5-bit field */
   qa->cr[0] = (hi << 4) | lo;
}

void qadc_init () 
{
   static int init = 0;

   if (init) return;
#ifdef DEBUG
   my_printf ("Initializing QADC\n");
#endif
   init = 1;
   /* Activate prescaler */
   SetADCPrescaler (&QADC1, QADC_PRESCALER);
   /* Setup Queue 1 */
   qadcSetMode (QADC1, QUEUE1, QMODE_SWTRIG);

   /* Disable Queue 1, set beginning of Q2 */
   qadcSetMode (QADC1, QUEUE2, QMODE_DISABLE);
   /* qadcSetBQ2 (QADC1, 32); */
   QADC2.ddrqa  = 0xFF00; /* BN52-BN59 are set as digital output */
   QADC2.portqa = 0x00; /* set output to low */
}

/* Add channel 'ch' to scanning queue 'q' */
int qadcAddChannel(QADC_map *p_adc, unsigned char q, char ch) 
{
   static int num_ch[2] = {0,0};
   int q2;

   q2 = p_adc->cr[QUEUE2] & 0x3F;
   if (q == QUEUE1) {
      p_adc->ccw[num_ch[0]++] = ch;
      p_adc->ccw[num_ch[0]] = 0x3F; /* end of queue */
      return num_ch[0]-1;
   }
   else {
      p_adc->ccw[q2+num_ch[1]++] = ch;
      p_adc->ccw[q2+num_ch[1]] = 0x3F; /* end of queue */
      return num_ch[1]-1;
   }
}

#ifdef DEBUG
#include "stdlib.h"
void blink (int n)
{
   while (n > 0) {
      QADC2.portqa = 0xAA;
      usleep (500000);
      QADC2.portqa = 0x00;
      usleep (500000);
      n--;
   }
}
#endif
