// $Id: SJoint.h,v 2.4 2000/10/04 19:25:13 dulimart Exp $
//
// Author: Hans Dulimarta  [dulimart@egr.msu.edu, dulimart@computer.org]
//
// Description:
//    SJoint is a Joint which equipped with additional position sensor
// 
// $Log: SJoint.h,v $
// Revision 2.4  2000/10/04 19:25:13  dulimart
// Excluded Recalibrate function member
//
// Revision 2.3  2000/09/26 18:22:28  dulimart
// Added doxygen enable comment lines
//
// Revision 2.2  2000/09/06 22:18:42  dulimart
// Moved cctor from public to private
//
// Revision 2.1  2000/05/23 17:13:11  dulimart
// Remove stream from constructor
//
// Revision 2.0  2000/04/25 16:55:13  dulimart
// Ported to MPC-555
//
//
// Copyright (c) 1999,2000  Hans Dulimarta, Ph.D.

#ifndef S_JOINT_H
#define S_JOINT_H

#include "Joint.h"
#include "ASignal.h"

/**
 * SJoint is an abstraction of a Joint equipped with
 * external position sensor (potentiometer)
 */
class SJoint : public Joint {
   public:
      /**
       * Initialize the object and connect the analog signal
       * channel connected to the potentionmeter
       * @param d pointer to analog signal channel
       */
      SJoint (ASignal* d);

      /// Recalibrate the zero position of the joint using
      /// analog value returned from the potentiometer
      //virtual int Recalibrate();
   private:

      /// Copy constructor is not available to public
      SJoint (const SJoint&);

      /// The value of the analog signal corresponding to the zero
      /// position of the joint.
      int zeroPos;

      /// pointer to the analog channel object
      ASignal* io_dev;
};
#endif
