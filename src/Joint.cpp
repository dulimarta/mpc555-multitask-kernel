// $Id: Joint.cpp,v 2.6 2001/02/21 16:20:24 dulimart Exp $
// Author: Hans Dulimarta  [dulimart@egr.msu.edu, dulimart@computer.org]
//
// Description: see Joint.h
// 
// $Log: Joint.cpp,v $
// Revision 2.6  2001/02/21 16:20:24  dulimart
// Removed L_CORR and R_CORR
//
// Revision 2.5  2001/02/19 20:44:12  dulimart
// Modified signature of RotateRel
//
// Revision 2.4  2000/10/04 19:19:24  dulimart
// Removed UpdateCenterInfo and Recalibrate
//
// Revision 2.3  2000/06/27 16:49:07  dulimart
// Redirected messages to cerr
//
// Revision 2.2  2000/05/23 17:04:11  dulimart
// Removed istream and added DSignal to constructor, Rewrote Rotatexxx to call Movexxx
//
// Revision 2.1  2000/04/27 11:53:38  dulimart
// Use external parameter GEAR_RATIO, L_CORR, R_CORR, etc.
// (see param.S)
//
// Revision 2.0  2000/04/25 16:39:00  dulimart
// Ported to MPC555 chip
//
//
// Copyright (c) 1999, 2000  Hans Dulimarta, Ph.D.
//

#include <math.h>
#include "Joint.h"
#ifdef DEBUG
#include "io.h"
#include "stream.h"
#endif
#include "stdlib.h"

int Joint::syncmove = 0;
const int Joint::EncoderLines = 64;

extern int GEAR_RATIO[];
//extern float L_CORR[];
//extern float R_CORR[];

Joint::Joint (DSignal* c_sw, DSignal* e_sw)
{
#if 0
   my_printf ("%s at %p, CSW=%p, ESW=%p\n", __PRETTY_FUNCTION__, this,
         c_sw, e_sw);
#endif
   limit_sw[CENTER_SW] = c_sw;
   limit_sw[SIDE_SW] = e_sw;
   SideFromCenter = 2;
   SyncDist = 0;
   GearRatio = GEAR_RATIO[chan_id];
   //CalibLCorr = L_CORR [chan_id];
   //CalibRCorr = R_CORR [chan_id]; // FIX THIS
   cerr << "Joint-" << chan_id << ":  GearRatio = " << GearRatio << endl; 
	   //" Center Correction " << CalibLCorr << ", " << CalibRCorr << endl;
   DetCenterPos = 0;
}

Joint::Joint (const Joint & j)
   : PIDControl (j), GearRatio (j.GearRatio)
{
   exit (0);
#if 0
   SideFromCenter = j.SideFromCenter;
   SyncDist = j.SyncDist;
   CalibLCorr = j.CalibLCorr;
   CalibRCorr = j.CalibRCorr;
   DetCenterPos = j.DetCenterPos;
   targetpos = j.targetpos;
#endif
}

Joint::~Joint () {
   //cerr << __PRETTY_FUNCTION__ << endl;
}

void Joint::RotateRel (float angle)
{
   float p;

   p = angle * (float) (GearRatio * EncoderLines) / 360.0;
   MoveRel ((int)p);
}

void Joint::WaitUntilDone (unsigned char limit)
{
   int lastpos /*, currpos */;
   int /* stuck_cnt = 0, */ warn_cnt;

   lastpos = GetPos();
   warn_cnt = 0;
   while (1) {
      if (GetStat () & (MOVE_DONE | POS_ERROR)
            || IsLimitOff (limit)) break;
   }
   if (IsLimitOff (SIDE_SW)) {
      StopMotor (AMP_ENABLE | STOP_ABRUPT);
#ifdef DEBUG
      cerr << "WARNING:  : Motor-" << (int) chan_id + 1 <<
         " hit its limit switch" << endl;
#endif
   }
   else if (IsLimitOff (CENTER_SW)) {
      if (! IsMoveDone()) {
         StopMotor (AMP_ENABLE | STOP_SMOOTH);
#ifdef DEBUG
         cerr << "Motor-" << (int) chan_id + 1 
            << " stopped at its center" << endl;
#endif
      }
   }
   //sleep (1);
}

bool Joint::IsLimitOn (unsigned char which)
{
#if 0
   my_printf ("%s at %p, %d -> %p\n", __PRETTY_FUNCTION__, this, which,
         limit_sw [which]);
#endif
   if (limit_sw[which])
      return limit_sw [which]->GetState();
   else
      return 1; /* Default is always ON */
}

bool Joint::IsLimitOff (unsigned char which)
{
   return !IsLimitOn (which);
}

void Joint::MoveTo (int tpos)
{
   int dist = tpos - GetPos();

   if (!syncmove) {
      SetMoveDist (dist);
      StartMove ();
      WaitUntilDone ();
   }
   else {
      SyncDist = dist;
      if (SyncDist) {
      	syncmove++;
      }
   }
}

void Joint::MoveRel (int rpos)
{
  if (!syncmove) {
     SetMoveDist (rpos);
     StartMove ();
     WaitUntilDone ();
  }
  else {
     SyncDist = rpos;
     if (SyncDist != 0)
        syncmove++;
  }
}

void Joint::RotateTo (float angle)
{
   float p;

   p = angle * (float) (GearRatio * EncoderLines) / 360.0;
   MoveTo ((int)p);
}
 
float Joint::GetAngle () {
   return GetPos() * 360 / (float) (GearRatio * EncoderLines);
}
