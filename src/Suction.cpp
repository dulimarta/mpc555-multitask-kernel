// $Id: Suction.cpp,v 2.6 2000/09/28 19:17:07 dulimart Exp $
//
// Author: Hans Dulimarta  dulimart@[egr.msu.edu,computer.org]
//
// Description: see Suction.h
//
// $Log: Suction.cpp,v $
// Revision 2.6  2000/09/28 19:17:07  dulimart
// Remove UpdateHiThreshold(), added GetXXXThreshold() and SetThrehold().
//
// Revision 2.5  2000/09/06 22:28:25  dulimart
// Put debugging messages within #ifdef DEBUG
//
// Revision 2.4  2000/06/30 14:37:50  dulimart
// Removed SetSuctionPWM
//
// Revision 2.3  2000/06/27 16:52:46  dulimart
// Remove LOOP local constant, redirected messages to STDERR
//
// Revision 2.2  2000/05/23 17:16:10  dulimart
// Removed istream from constructor, print pressure output inside PressureHigh/PressureLow
//
// Revision 2.1  2000/04/27 11:56:52  dulimart
// Use global parameter LOW_P and HIGH_P in constructor
// (see param.S)
//
// Revision 2.0  2000/04/25 17:06:12  dulimart
// ported to MPC-555
//
//
// Copyright (c) 1999,2000  Hans Dulimarta. Ph.D.
//

#define LOOP  3
#include "Suction.h"
#include "io.h"
#include "stream.h"
#include "stdlib.h"

int Suction::ID = 0;

extern int LOW_P[], HIGH_P[];

/**
   @param p digital output signal for controlling the pump motor
   @param v digital output signal for controlling the microvalve
   @param a analog input signal from the pressure sensor
 */
Suction::Suction (DSignal *p, DSignal *v, ASignal *a) 
{ 
   id = ID++;
   pump = p;
   valve = v;
   psense = a;
#if 0
   int k;

   for (k=0; k<LOOP; k++) /* ignore the first few readings */
      LOWPRESSURE = AveragePressure() + 20; 
#else
   LOWPRESSURE = LOW_P[id];
#endif
   HIGHPRESSURE = HIGH_P [id];
#ifdef DEBUG
   cerr << "Suction-" << id
      << ": LP=" << LOWPRESSURE << "  HP=" << HIGHPRESSURE << endl;
#endif
}

Suction::Suction (const Suction& s) 
    : LOWPRESSURE (s.LOWPRESSURE), HIGHPRESSURE (s.HIGHPRESSURE) 
{
   id = s.id;
   pressure = s.pressure;
}

/**
   @param turn_on boolean value to indicate ON/OFF
*/
void Suction::SetValve (bool turn_on)
{
   if (turn_on)
      valve->SetHi();
   else
      valve->SetLo();
}

/**
   @param turn_on boolean value to indicate ON/OFF
*/
void Suction::SetSuction (bool turn_on) { 
/* the two suction cups can be controlled independently.
 * Turning ON/OFF one cup should not affect the other suction cup.
 * To achieve this, a static status byte has to be maintained.
 */
   if (turn_on)
      pump->SetHi();
   else
      pump->SetLo();
   usleep (500000);
}

int Suction::AveragePressure () { 
   int i, tot;

   tot = 0;
   for (i=0; i<LOOP; i++) {
      tot += psense->GetAnalog();
      usleep (100000);
   }
   pressure = tot / LOOP;
   return pressure;
}

int Suction::GetPressure () const
{
   return pressure;
}

bool Suction::PressureLow () {
   AveragePressure();
   my_fprintf (STDERR, "%s, Foot-%d Pressure = %d, Low Thresh. = %d\n",
         __FUNCTION__, id+1, pressure, LOWPRESSURE);
   return pressure < LOWPRESSURE;
}

bool Suction::PressureHigh () {
   AveragePressure();
   my_fprintf (STDERR, "%s, Foot-%d Pressure = %d, High Thresh. = %d\n",
         __FUNCTION__, id+1, pressure, HIGHPRESSURE);
   return pressure > HIGHPRESSURE;
}

void Suction::SetThreshold (int lo, int hi) {
   if (lo >= 0)
      LOWPRESSURE = lo;
   if (hi >= 0)
      HIGHPRESSURE = hi;
}

int Suction::GetLowThreshold () const
{
   return LOWPRESSURE;
}

int Suction::GetHighThreshold () const
{
   return HIGHPRESSURE;
}
