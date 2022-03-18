// $Id: Joint.h,v 2.7 2001/02/19 20:44:12 dulimart Exp $
/*
 * Filename: Joint.h
 * Author: Hans Dulimarta [dulimart@computer.org]
 * Description:
 *   A Joint is an abstraction of a robot joint.
 *
 * $Log: Joint.h,v $
 * Revision 2.7  2001/02/19 20:44:12  dulimart
 * Modified signature of RotateRel
 *
 * Revision 2.6  2000/10/04 19:19:24  dulimart
 * Removed UpdateCenterInfo and Recalibrate
 *
 * Revision 2.5  2000/09/26 17:54:34  dulimart
 * Added doxygen enable comment lines
 *
 * Revision 2.4  2000/09/13 23:35:15  dulimart
 * Moved copy constructor to protected section
 *
 * Revision 2.3  2000/05/23 17:05:10  dulimart
 * Used DSignal, added GetAngle, redefined SIDE_SW and CENTER_SW
 *
 * Revision 2.2  2000/04/27 11:54:23  dulimart
 * Change SideFromCenter from char to short
 * (the compiler gave a warning of "limited range")
 *
 * Revision 2.1  2000/04/25 15:53:11  dulimart
 * Remove StopMotor, SetVel, SetAccel, SetTargetPos, IsMoveDone
 *
 *
 * Copyright (c) 2000   Hans Dulimarta
 */
#ifndef JOINT_H
#define JOINT_H

#include "PIDControl.h"
#include "DSignal.h"
#define   CENTER_SW 0
#define   SIDE_SW   1

/**
   Joint is an abstraction of a mechanical joint of a robot.
   For further extension, this class should be an abstract class which
   derived into Revolute and Prismatic Joint.
 */
class Joint : public PIDControl {
   public:
      /// number of joints in a synchronized move
      static int syncmove;

      /// the distance this motor has to go in a synchronized move
      long SyncDist;

      /// flag indicating the position of the center switch relative
      /// to current position
      short SideFromCenter;

      /**
         Construct a joint object
         @param center_sw a pointer to center limit switch digital input
         signal
         @param end_sw a pointer to end limit switch digital input
         signal
       */
      Joint(DSignal* center_sw, DSignal* end_sw);

      /// virtual destructor
      virtual ~Joint();

      /**
         Rotate the joint relative to the current position
         @param angle amount of angle to rotate
         @param SIDE_SW stop when end switch is reached
       */
      void RotateRel (float angle);

      /**
         Rotate the joint to an absolute angle. This absolute angle
         is measured relative to the initial joint position.
         @param angle amount of angle to rotate
       */
      void RotateTo (float angle);

      /**
         Rotate the joint relative to the current position
         @param e_cnt number of encoder counts to rotate
       */
      void MoveRel (int e_cnt);

      /**
         Rotate the joint to an absolute position
         @param a_pos absolute position to move to
       */
      void MoveTo (int a_pos);

      /**
         Wait until the joint completes the rotation
       */
      void WaitUntilDone(unsigned char = SIDE_SW);

      /// Adjust the correction factors
      void Adjust ();

      /// Convert currect position to absolute angle
      float GetAngle();

      /** 
         Check the status of limit switch
         @param s which switch to check (center / end)
       */
      bool IsLimitOn (unsigned char s);

      /** 
         Check the status of limit switch
         @param s which switch to check (center / end)
       */
      bool IsLimitOff (unsigned char s);

      /**
         Update center switch information
       */
      //void UpdateCenterInfo();

      /**
         Recalibrate the zero position of the joint using
         information from the center switch
       */
      //virtual int Recalibrate ();

   protected:

      /// copy constructor is not used for public
      Joint(const Joint&);

   private:
      int RecalFrom (float);

      /// the gear ratio of the gear box connected to this motor
      int GearRatio;
      
      /// Left and right calibration factors
      float CalibLCorr, CalibRCorr;

      /// Number of lines in the encoder connected to this motor
      static const int EncoderLines;

      long DetCenterPos;
      long targetpos;

      /// array of pointer to limit switches (end / center switch)
      DSignal* limit_sw[2];
};
#endif
