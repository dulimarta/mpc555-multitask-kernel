/**
 * @file PIDControl.h
 *
 * @author Hans Dulimarta  [dulimart@computer.org]
 *
 *   This is an implementation of a PID controller which employs a
 * trapezoidal velocity profile to generate the desired position at
 * each servo tick.
 */

//
// $Id: PIDControl.h,v 2.6 2001/02/19 20:44:12 dulimart Exp $
//
// $Log: PIDControl.h,v $
// Revision 2.6  2001/02/19 20:44:12  dulimart
// Modified signature of RotateRel
//
// Revision 2.5  2000/10/02 20:30:50  dulimart
// Added SetPos and modified GetPos to const member
//
// Revision 2.4  2000/09/28 18:08:58  dulimart
// Modified to enable continous servoing when the motor
// is not controlled under trapezoidal velocity profile
//
// Revision 2.3  2000/09/26 18:35:37  dulimart
// Added doxygen enable comment lines.
//
// Revision 2.2  2000/09/13 23:35:52  dulimart
// Moved cctor to protected section
//
// Revision 2.1  2000/05/23 17:11:20  dulimart
// Defined StopMotor's body, changed GetPos to const member
//
// Revision 2.0  2000/04/25 16:43:26  dulimart
// initial implementation on MP555
//
//
// Copyright (c) 2000   Hans Dulimarta, Ph.D.
//
#ifndef PID_CONTROL_H
#define PID_CONTROL_H

/* BEGIN: constants from PIC-Servo */
#define LOAD_POS 0x01
#define LOAD_VEL 0x02
#define LOAD_ACC 0x04
#define POS_SERVO 0x10
#define AMP_ENABLE 0x01
#define STOP_SMOOTH 0x08
#define STOP_ABRUPT 0x04
//#define DEFAULT_VEL 30000

#define MOVE_DONE    0x01
#define WAIT         1
#define POS_ERROR    0x10
/* END: constants from PIC-Servo */

/**
 * PIDControl in an abstraction of a PID digital controller
 */
class PIDControl {
   public:
      /// Default constructor
      PIDControl ();

      /// destructor
      virtual ~PIDControl ();


      /**
       * Set the PID parameters. Passing a value of -1 to any of these
       * parameters will not change the current setting.
       * @param Kp proportional constant
       * @param Ki integral constant
       * @param Kd derivative constant
       */
      void SetParm (short Kp, short Ki, short Kd);

      /**
       * Get the PID parameters. NULL pointers will be ignored.
       * @param p pointer to a short integer to receive Kp
       * @param i pointer to a short integer to receive Ki
       * @param d pointer to a short integer to receive Kd
       */
      void GetParm (short* p, short* i, short* d);

      /**
       * Set the maximum velocity and acceleration to be used in the
       * trapezoidal velocity profile generation. 
       * Passing a value of -1 to any of these
       * parameters will not change the current setting.
       * @param Vel maximum velocity
       * @param Acc maximum acceleration
       */
      void SetVelAcc (unsigned int Vel, unsigned short Acc);

      /**
       * Get the velocity and acceleration settings. NULL pointers
       * will be ignored.
       * @param v a pointer to unsigned integer to receive velocity
       * @param a a pointer to unsigned short to receive acceleration
       */
      void GetVelAcc (unsigned int* v, unsigned short *a);

      /// Show the current parameter settings .
      void ShowParm();

      /**
       * Set the distance the PID has to move. Desired position is computed
       * by adding this distance to the current position
       * @param dist the distance given in encoder counts
       */
      virtual void SetMoveDist (int dist);

      /**
       * Start the move after all required variables were properly set.
       * @param _ this parameter is ignored. Provided only for compatibility
       */
      virtual void StartMove (int = 0);

      /**
       * Handle the PIT interrupt and control the servo move
       */
      void ServoMove ();

      /**
       * Check to see if the trapezoidal move is completed
       */
      bool IsMoveDone () const;

      /* BEGIN: routines from PIC-Servo */
      /**
       * Stop the servo
       * @parm mode STOP_SMOOTH or STOP_ABRUPT
       */
      virtual void StopMotor(int mode);

      /// Reset encoder position 
      virtual void ResetPos();

      /// Get status byte, provided for compatibility only 
      virtual unsigned char GetStat() const;

      /// Get auxiliary byte, provided for compatibility only
      virtual unsigned char GetAux() const;

      /// Clear sticky bits, provided for compatibility only
      virtual void ClearBits();

      /// Get current actual position of the encoder
      virtual int GetPos() const;
      /* END: routines from PIC-Servo */

      /** 
       * Set encoder position to a certain value
       * @param pos new position to set
       */
      virtual void SetPos(int pos);

      /// Number of motion in progress
      static int NUM_MIP;

      /// channel id
      static int CHAN_ID;

   protected:
      /// copy constructor is not available to public
      PIDControl (const PIDControl&);

      /// channel id
      int chan_id;

      /** 
       * Saturate a value to a number of bit
       * @param val pointer to the value to be saturated
       * @param bit number of bits to use in saturation
       */
      void Saturate (int *val, int bit);

#define LED_MOVEON    0x0001
   
   private:
      short Kp, Ki, Kd;

      /// Velocity limit: 24-bit integer + 8-bit fraction
      int vel_lim;                 

      /// Acceleration: 8-bit interger + 8-bit fraction
      unsigned short accel;        

      /// position encoder counter
      int enc_cnt;                 

      /// actual position: 24-bit integer + 8-bit fraction
      int apos;

      /// computed velocity: 24-bit integer + 8-bit fraction
      int cvel; 

      /// computed position: 24-bit integer + 8-bit fraction
      int cpos;

      /// desired position: 24-bit integer + 8-bit fraction
      int dpos;

      /// previous error, needed for calculating the derivative term
      int prev_err;

      /// integration error term
      int tot_err;
      /// # of ticks in the constant vel. section
      int flatcnt;

      /// half of total distance
      int ph1dist;

      /// wrap around correction for 16-bit encoder to 32-bit value
      int wrap_correction;

      /// flag for integrator wind-up function
      bool saturate;

      /// are we in the first half of trapezoid?
      bool first_half;

      /// end of move?
      bool end_move;

      /// move backward (negative distance)
      bool neg_move; 

      };
#endif
