// $Id: PIDControl.cc,v 2.8 2001/02/19 20:44:12 dulimart Exp $
//
// Author: Hans Dulimarta   [dulimart@egr.msu.edu,  dulimart@computer.org]
//
// Description: see PIDControl.h
//
// Note: implementation of this PID is adapted from Microchip Application
//       notes 532 and 718. See http://www.microchip.com
//
// $Log: PIDControl.cc,v $
// Revision 2.8  2001/02/19 20:44:12  dulimart
// Modified signature of RotateRel
//
// Revision 2.7  2000/10/13 15:55:41  dulimart
// Added code for detecting stuck motors.
//
// Revision 2.6  2000/10/02 20:30:16  dulimart
// Added SetPos and modified GetPos to const member
//
// Revision 2.5  2000/09/28 18:08:58  dulimart
// Modified to enable continous servoing when the motor
// is not controlled under trapezoidal velocity profile
//
// Revision 2.4  2000/09/26 18:36:56  dulimart
// Used parameter table to initialize PID parameters. Distinguish the
// case of "end of trajectory" and "end of move".
//
// Revision 2.3  2000/07/17 17:47:18  dulimart
// Removed reference to sat_cnt
//
// Revision 2.2  2000/06/27 16:50:35  dulimart
// Redirected putchar to STDERR
//
// Revision 2.1  2000/05/23 17:09:32  dulimart
// Changed PID constants, remove debugging messages, used MGPIO for direction bit
//
// Revision 2.0  2000/04/25 16:50:13  dulimart
// implement the 16-bit wraparound error correction
//
//
// Copyright (c) 2000   Hans Dulimarta, Ph.D.
//
#include "PIDControl.h"
#include "mpc555.h"
#include "tpu.h"
#include "mios.h"
#include "io.h"
#include "stream.h"

int PIDControl::NUM_MIP = 0;     /* number of move in progress */
int
   PIDControl::CHAN_ID = 0;     /* global Channel ID */

extern int PID_LIMIT;
extern int PID_Kp[], PID_Ki[], PID_Kd[], PID_Vel[], PID_Acc[];

PIDControl::PIDControl ()
{
   my_printf ("%s at %p\n", __PRETTY_FUNCTION__, this);
   chan_id = CHAN_ID++;

   /* PID parameters */
   Kp = PID_Kp[chan_id];
   Ki = PID_Ki[chan_id];
   Kd = PID_Kd[chan_id];
   vel_lim = PID_Vel[chan_id];
   accel = PID_Acc[chan_id];

   /* position information */
   apos = 0;
   dpos = 0;
   wrap_correction = 0;

   prev_err = 0;
   tot_err = 0;

   saturate = 0;
   end_move = 1;
   enc_cnt = 0;
   SetFQDPos (chan_id, 0);

   /* Quadrature Decoder */
   SetupFQD (chan_id);
   EnableFQD (chan_id);

   /* PWM generator */
   EnablePWM (chan_id);
   SetPWMPeriod (chan_id, 512);
   //SetPWMWidth (chan_id, 0);
#ifdef SN754410
   SetPWMDutyCycle (chan_id, 50);
#else
   SetPWMDutyCycle (chan_id, 1);
#endif
}

PIDControl::PIDControl (const PIDControl&) 
{
   exit (0);
}

PIDControl::~PIDControl ()
{
   DisableFQD (chan_id);
   DisablePWM (chan_id);
}

void
PIDControl::StartMove (int)
{
   if (!end_move)
      return;
   end_move = 0;
   NUM_MIP++;
}

void
PIDControl::SetParm (short _kp, short _ki, short _kd)
{
   if (_kp >= 0) Kp = _kp;
   if (_ki >= 0) Ki = _ki;
   if (_kd >= 0) Kd = _kd;
}

void PIDControl::GetParm (short* _kp, short* _ki, short* _kd)
{
   if (_kp) *_kp = Kp;
   if (_ki) *_ki = Ki;
   if (_kd) *_kd = Kd;
}

void PIDControl::SetVelAcc (unsigned int _v, unsigned short _a) 
{
   if (end_move) {
      if (_v > 0)
         vel_lim = _v;
      if (_a > 0)
         accel = _a;
   }
}

void PIDControl::GetVelAcc (unsigned int *_v, unsigned short* _a)
{
   if (_v) *_v = vel_lim;
   if (_a) *_a = accel;
}

void PIDControl::ShowParm () 
{
   my_printf ("\nPID parameter of channel %d\n", chan_id);
   my_printf ("Kp=%6d, Ki=%6d, Kd=%6d\n", Kp, Ki, Kd);
   my_printf ("Vel. Limit=%6d, Accel=%6d, Pos=%.8d\n", 
         vel_lim, accel, apos);
   my_printf ("Actual Pos=%d, Desired Pos=%d\n",
         apos, dpos);
}

void
PIDControl::SetMoveDist (int dist)
{
#if PICBOARD
   dist *= -1; /* reverse the distance to comply with the PIC Board 
               * direction
               */
#endif
   tot_err = 0;
   prev_err = 0;
   saturate = 0;
   flatcnt = 0;
   first_half = 1;
   cvel = 0;
   ph1dist = dist << 8;         /* shift left to the 24-bit integer part */
   apos = (GetFQDPos (chan_id) + wrap_correction) << 8;
   cpos = apos;
   dpos = apos + ph1dist;       /* desired position */
   neg_move = ph1dist < 0;
   if (neg_move)
      ph1dist *= -1;            /* Phase 1 distance is always positive */
   ph1dist >>= 1;               /* divide by 2 */
   enc_cnt = GetFQDPos (chan_id);
}

void
PIDControl::ServoMove ()
 /* This function is called via a PIT interrupt handler.

  * In the following algorithm, distances and velocities are assumed 
  * to have 8-bit fractional part.
  * The trapezoidal profile consists of 2 phases, the first half
  * and second half.
  */
{
   int avel; /* actual velocity */
   int cdist; /* computed distance */
   int pos_err; /* diff. between actual & desired pos */
   int pid_out; /* PID output signal */
   int dir_out; /* motor direction */

   /* Update current position and velocity */
   asm ("li 4,1; bl fputh; 1: b 1b;");
#if 0
   avel = -enc_cnt;
   enc_cnt = GetFQDPos (chan_id);
   avel += enc_cnt;
   if (abs(avel) > 0x7FFF) {   /* Overflow/Underflow adjustment */
      if (avel & 0x8000) { /* is it negative? */
         avel -= 0x10000;
         wrap_correction -= 0x10000;
         fputc ('w', STDERR);
      }
      else {
         avel += 0x10000;
         wrap_correction += 0x10000;
         fputc ('W', STDERR);
      }
   }
   apos = (enc_cnt + wrap_correction) << 8;

   if (!end_move) {
      /* Update trajectory using trapezoidal profile */
      if (!saturate) {          /* if previous step is saturated */
         /*
          * Update position using the following recursive
          * formula:
          *     Pos(k) = Pos(k-1) + Vel(k-1) + Acc/2
          *
          * Update velocity using the following recursive
          * formula:
          *     Vel(k) = Vel(k-1) + Acc
          */
         cdist = cvel;
         if (first_half) {      /* first half of trapezoid profile */
            if (cvel + accel <= vel_lim) { /* has to accelerate */
               cdist += accel / 2;
               cvel += accel;
            }
            else {              /* reach max. vel., start constant velocity */
               flatcnt++;
            }
            ph1dist -= cdist;
            if (0 < ph1dist && ph1dist <= cdist) { /* Last step of first half? */
               /* at this point ph1dist is equal to the undershoot error */
               if (2 * ph1dist < cdist) { /* undershoot is better */
                  first_half = 0;
                  //my_printf ("Undershoot is preferred\n");
               }
            }
            else if (ph1dist <= 0) {
               first_half = 0;
               /*
               my_printf ("PID-%d, End of 1st: Flat=%d, PH1=%.8d CPOS=%.8d\n", 
                     chan_id, flatcnt, ph1dist, cpos);
                     */
            }
         }
         else {
            /* second half of trapezoid profile */
            if (flatcnt) {      /* are we in the flat portion? */
               flatcnt--;
            }
            else {
               if (cvel >= 0) { /* has to decelerate */
                  cdist -= accel / 2;
                  cvel -= accel;
               }
               else {           /* move is done */
                  end_move = 1;
                  NUM_MIP--;
               }
            }
         }
         if (neg_move)
            cpos -= cdist;
         else
            cpos += cdist;
      }
      pos_err = (cpos - apos) >> 8; /* ignore the 8-bit fraction */
   }
   else {
      /* We do not use trapezoidal profile here! */
      pos_err = dpos - apos; 
      pos_err >>= 8; /* ignore the 8-bit fraction */
   }

   Saturate (&pos_err, 16);     /* limit to 16-bit signed integer */
   if (!saturate)
      tot_err += pos_err;

   /* Calculate PID output */
   pid_out = Kp * pos_err + Kd * (pos_err - prev_err)
      + Ki * tot_err;

   Saturate (&pid_out, 24);     /* limit to 24-bit signed integer */
   pid_out >>= 8;
   prev_err = pos_err;
   saturate = 0;

   dir_out = pid_out < 0;
   if (pid_out < 0)
      pid_out = abs (pid_out);
   if (pid_out > PID_LIMIT) {
      pid_out = PID_LIMIT;
      saturate = 1;
   }
   SetPWMWidth (chan_id, pid_out);
   // Servo-0 direction bit is connected to MGPIO11
   // Servo-1 direction bit is connected to MGPIO13
   // Servo-2 direction bit is connected to MGPIO15
   if (dir_out)
      MIOS.io.dr |= BITSET (4 - chan_id*2, 16);
   else
      MIOS.io.dr &= ~BITSET (4 - chan_id*2, 16);
#endif
}

void
PIDControl::Saturate (int *val, int bit)
{
   int limit;

   limit = (2 << (bit - 1)) - 1;
   if (*val > limit)
      *val = limit;
   else if (*val < -limit)
      *val = -limit;
}

unsigned char PIDControl::GetStat (void) const {
   unsigned char stat;
   stat = 0x68; /* Limit 1, Limit 2, Power On */
   if (end_move) stat |= MOVE_DONE;
   //my_printf ("Status of PID-%d is %x\n", chan_id, stat);
   return stat;
}

unsigned char PIDControl::GetAux () const {
#ifdef DEBUG
   my_printf ("%s\n", __PRETTY_FUNCTION__);
#endif
   return 0x04; /* SERVO ON */ 
}

void PIDControl::ClearBits () {
#ifdef DEBUG
   my_printf ("%s\n", __PRETTY_FUNCTION__);
#endif
}

void PIDControl::StopMotor (int _mode) {
#ifdef DEBUG
   my_printf ("%s ch-%d, Parm = %x\n", __PRETTY_FUNCTION__, chan_id, _mode);
#endif
   first_half = 0;
   flatcnt = 0;
   if (_mode & STOP_ABRUPT) 
   {
      cvel = -1; /* stop here */
   }
   saturate = 0;
}

int PIDControl::GetPos () const
{
   //my_printf ("%s ch-%d\n", __PRETTY_FUNCTION__, chan_id);
   return GetFQDPos(chan_id) + wrap_correction;
}

void PIDControl::SetPos (int pos)
{
   short enc_val;

   enc_val = pos % 0x10000;
   apos = pos << 8;
   dpos = apos;
   SetFQDPos (chan_id, enc_val);
   wrap_correction = pos - enc_val;
   my_printf ("Position %d = encoder %d + wrap correction %d\n",
         pos, enc_val, wrap_correction);
}

void PIDControl::ResetPos () { 
   apos = 0; 
   dpos = 0;
   wrap_correction = 0;
   SetFQDPos (chan_id, 0);
}

bool PIDControl::IsMoveDone() const {
   return end_move;
}
