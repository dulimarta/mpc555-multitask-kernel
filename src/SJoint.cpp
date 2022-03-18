// $Id: SJoint.cpp,v 2.6 2000/10/04 19:25:13 dulimart Exp $
//
// Author: Hans Dulimarta  [dulimart@egr.msu.edu,  dulimart@computer.org]
//
// Description: see SJoint.h
//
// $Log: SJoint.cpp,v $
// Revision 2.6  2000/10/04 19:25:13  dulimart
// Excluded Recalibrate function member
//
// Revision 2.5  2000/10/02 20:31:46  dulimart
// Modified debugging messages
//
// Revision 2.4  2000/09/06 22:28:25  dulimart
// Put debugging messages within #ifdef DEBUG
//
// Revision 2.3  2000/06/27 16:51:15  dulimart
// Redirected messages to STDERR
//
// Revision 2.2  2000/05/23 17:13:11  dulimart
// Remove stream from constructor
//
// Revision 2.1  2000/04/27 11:56:11  dulimart
// Use global parameter ZEROPOS in constructor. (see param.S)
//
// Revision 2.0  2000/04/25 16:59:37  dulimart
// Ported to MPC-555
//
//
// Copyright (c) 1999, 2000   Hans Dulimarta, Ph.D.
//

#include "SJoint.h"
#ifdef DEBUG
#include "io.h"
#include "stream.h"
#endif

extern int ZEROPOS;

SJoint::SJoint (ASignal *io) : Joint (0, 0)
{
   zeroPos = ZEROPOS;
   io_dev = io;
}

SJoint::SJoint (const SJoint& j) 
   : Joint (j), zeroPos (j.zeroPos)
{
#ifdef DEBUG
   cerr << __PRETTY_FUNCTION__ << " should not be called " << endl;
#endif
   exit (0);
}

#if 0
int SJoint::Recalibrate() {
   long int delta;

#ifdef DEBUG
   cerr << "Recalibrating SJoint-" << (int) chan_id << endl;
#endif
   MoveTo (0);
   while (1) {
      delta = zeroPos - io_dev->GetAnalog (/*2*/);
      if (labs(delta) < 2) break;
      RotateRel ((float) delta * 0.75);
   }
   ResetPos();
   return 0;
}
#endif
