// $Id: MPC.cc,v 2.11 2000/10/13 15:39:22 dulimart Exp $
/* 
 * Filename: MPC.cc
 *
 * Author: Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * Description: See also MPC.h
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D. 
 */
#include "MPC.h"
#include "mpc555.h"
#include "intr.h"

//extern void PITHandler(void);
extern "C" {
   void KPHandler(void);
   void RxHandler(void);
}

#include "io.h"
MPC::MPC () {
   int k;

   /* MIOS I/O */
   for (k=0; k<8; k++)
      ds[k].SetChType (k, DSIGNAL_INPUT);
   for (k=8; k<16; k++)
      ds[k].SetChType (k, DSIGNAL_OUTPUT);

   /* MIOS PWM */
   for (k=0; k<2; k++) {
      pwm[k].SetChType (16 + k, DSIGNAL_OUTPUT);
      pwm[k].SetMaxWidth (512);
   }

   /* QADC analog inputs */
   as[0].SetChannel (&QADC1, 1); /* Pressure Sensor 1 */
   as[1].SetChannel (&QADC1, 3); /* Pressure Sensor 2 */
   as[2].SetChannel (&QADC2, 1); /* Position sensor */

   /* QADC digital inputs */
   for (k=0; k<6; k++)
      qio[k].SetModule (&QADC2);
   for (k=0; k<2; k++) /* BN2 - BN3 */
      qio[k].SetChType (k+2, DSIGNAL_INPUT);
   for (k=0; k<4; k++) /* BN48 - BN51 */
      qio[k+2].SetChType (k+48, DSIGNAL_INPUT);

   my_printf ("%s at %p\n", __PRETTY_FUNCTION__, this);

   //SetLVLHandler (PITIRQ, &PITHandler__Fv);
   SetIRQHandler (RXIRQ, &RxHandler);
   //SetIRQHandler (KPIRQ, &KPHandler);
   EnableInterrupt ();
}

MPC::~MPC () {
   int k;

   DisableInterrupt();
   QADC2.portqa = 0; /* turn off LEDs */
   for (k=8; k<16; k++)
      ds[k].SetLo();
   for (k=0; k<2; k++)
      pwm[k].SetLo();
}
