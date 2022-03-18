// $Id: Suction.h,v 2.4 2000/09/26 18:25:19 dulimart Exp $
//
// Author: Hans Dulimarta    dulimart@[egr.msu.edu, computer.org]
//
// Description:
//    This is an abstraction of a suction foot.
//
// $Log: Suction.h,v $
// Revision 2.4  2000/09/26 18:25:19  dulimart
// Added doxygen enable comment lines
//
// Revision 2.3  2000/06/30 14:38:14  dulimart
// Removed PWM data member
//
// Revision 2.2  2000/05/23 17:16:10  dulimart
// Removed istream from constructor, print pressure output inside PressureHigh/PressureLow
//
// Revision 2.1  2000/04/27 11:57:29  dulimart
// Add static member ID to properly initialize pressure threshold
// from global parameters
//
// Revision 2.0  2000/04/25 17:03:25  dulimart
// ported to MPC-555.
//
//
// Copyright (c) 1999,2000   Hans Dulimarta, Ph.D.
//
#ifndef SUCTION_H
#define SUCTION_H

#include "ASignal.h"
#include "DSignal.h"

class Suction {
   public:
      /// User-defined constructor
      Suction (DSignal*, DSignal*, ASignal*);

      /// copy constructor
      Suction (const Suction&);

      /// Microvalve control
      void SetValve (bool);

      /// Suction pump control
      void SetSuction (bool);

      /// Read average value of pressure sensor
      int AveragePressure ();

      /// Check if the pressure sensor is low
      bool PressureLow();

      /// Check if the pressure sensor is high
      bool PressureHigh();
      
      /// Read the last pressure reading
      int GetPressure() const;
      
      /// Set low and high threshold for pressure value
      void SetThreshold (int, int);

      /// Get low threshold value
      int GetLowThreshold () const;

      /// Get high threshold value
      int GetHighThreshold () const;

      static int ID;
   private:
      /// pointer to the digital output for controlling the pump
      DSignal *pump;

      /// pointer to the digital output for controlling the valve
      DSignal *valve;

      /// pointer to the analog input connected to the pressure sensor
      ASignal *psense;

      /// id of this object (0, 1, ...)
      int id; 

      /// pressure sensor reading
      int pressure;

      /// Low threshold of pressure sensor reading
      int LOWPRESSURE;

      /// High threshold of pressure sensor reading
      int HIGHPRESSURE;

      // status of valves
      static short valve_stat;

      // status of suction pumps
      static unsigned char suction_stat;
};
#endif
