// $Id: main.cpp,v 2.11 2001/02/21 20:20:03 dulimart Exp $
//
// Author: Hans Dulimarta  dulimart@[egr.msu.edu, computer.org]
//
// Description:
//    This is the main function of the wall climber motion control
// code. This file defines all the executable modules callable from
// the CommandInt object.
// The code runs on both PC-based PIC-board controller as well as
// embedded MPC-555 chip.
//
// $Log: main.cpp,v $
// Revision 2.11  2001/02/21 20:20:03  dulimart
// Changed executable modules from function to subclass of Command.
//
// Revision 2.10  2001/02/19 20:44:12  dulimart
// Modified signature of RotateRel
//
// Revision 2.9  2000/11/06 22:15:42  dulimart
// Changed '&Config' to run only in state 0.
//
// Revision 2.8  2000/10/13 15:54:10  dulimart
// Reactivated Turn module, renamed FlipDown and FlipUp.
//
// Revision 2.7  2000/10/11 19:46:46  dulimart
// Changed velocity and acceleration setting for individual joints commands.
//
// Revision 2.6  2000/10/04 20:40:19  dulimart
// Initialized new_state to remove warning message
//
// Revision 2.5  2000/10/04 19:27:04  dulimart
// Modified Forwardx and Backwardx to perform multiple
// steps
//
// Revision 2.4  2000/08/02 16:07:35  dulimart
// Modified output messages, added Config module
//
// Revision 2.3  2000/05/23 17:24:59  dulimart
// Adjusted the output to fit LCD screen.
// Modified AddModule instructions to comply with the Module
// definition in CommandInt.
//
// Revision 2.2  2000/05/22 19:01:54  dulimart
// Adapted for the new state transition diagram
//
// Revision 2.1  2000/04/25 17:19:02  dulimart
// remove <CR> from lines
//
// Revision 2.0  2000/04/25 17:17:03  dulimart
// initial port to MPC-555
//
//
// Copyright (c) 1999,2000  Hans Dulimarta, Ph.D.
//

#include <ctype.h>
#include <string.h>

#include "config.h"
#include "io.h"
#include "stream.h"
#include "iodev.h"
#include "intr.h"
#include "stdlib.h"

#include "CommandInt.h"
#include "Clock.h"
#include "Task.h"
#include "WallClimber.h"

#define CUP2 0x08
#define CUP1 0x04

extern bool emergency_stop;
extern void PITHandler (void);

class StatusCmd: public Command {
   public:
      StatusCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
StatusCmd::Execute ( /*Robot * r, istream&, */ int s)
{
   int k;

   //WallClimber *wc = (WallClimber *) r;

   my_printf ("\n");
   for (k=0; k<2; k++)
      my_printf ("PS%d = %d ", k+1, wc->cup[k]->AveragePressure ());
   my_printf ("\nTS = ");
   for (k=0; k<6; k++)
      my_printf ("%d", wc->touch_sensor[k]->GetState ());
   my_printf ("\n");
   for (k=0; k<3; k++)
      wc->motor[k]->ShowParm();
   puts ("Press any key...\n");
   getchar ();
   return s;
}

#if 0
int
Move (Robot * r, int s)
{
   int axis,
     pos;
   //WallClimber *wc = (WallClimber *) r;

   cin >> axis >> pos;
   if (0 < axis && axis < 4)
      wc->motor[axis - 1]->MoveTo (pos);
   return s;
}

int
Center (Robot * r, int s)
{
   int axis;
   float angle;
   //WallClimber *wc = (WallClimber *) r;

   cin >> axis >> angle;
   if (0 < axis && axis < 4)
      wc->motor[axis - 1]->RotateRel (angle, CENTER_SW);
   return s;
}
#endif

class RotateCmd: public Command {
   public:
      RotateCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
RotateCmd::Execute (/*Robot * r, istream & ,*/ int s)
{
   int axis;
   //bool pos, abs_rot;
   bool neg;
   //char first_char;
   float angle;
   //WallClimber *wc = (WallClimber *) r;

   puts ("\fRotate which Joint? ");
   axis = getchar () - '0';
   if (axis < 1 || axis > 3)
      return s;
   my_printf ("\n%c=Pos, %c=Neg ", KP_PLUS, KP_MINUS);
   neg = toupper(getchar()) == KP_MINUS;
   //abs_rot = toupper(getchar()) == KP_PLUS;
   puts ("\fRotation Angle = ");
   putchar (neg ? '-' : '+');
   cin >> angle;
   if (neg) angle *= -1.0;
#ifdef DEBUG
   cerr << "Rotating axis-" << axis << " " << angle << " degrees" << endl;
#endif
   if (0 < axis && axis < 4)
   {
      unsigned short oldAcc;
      unsigned int oldVel;

      //wc->motor[axis - 1]->GetParm (&old_Kp, 0, 0);
      wc->motor[axis - 1]->GetVelAcc (&oldVel, &oldAcc);
      wc->motor[axis - 1]->SetVelAcc (oldVel/2, oldAcc/2);
      wc->motor[axis - 1]->RotateRel (angle);
      wc->motor[axis - 1]->SetVelAcc (oldVel, oldAcc);
      //wc->motor[axis - 1]->SetParm (old_Kp, -1, -1);
   }
   return s;
}

class ResetCmd: public Command {
   public:
      ResetCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
ResetCmd::Execute (/*Robot * r, istream &,*/ int s)
{
   int k;
   //WallClimber *wc = (WallClimber *) r;

   for (k=0; k<3; k++) {
      my_printf ("Resetting encoder position of joint-%d\n", k);
      wc->motor[k]->ResetPos ();
   }
   return s;
}

class SuctionCmd: public Command {
   public:
      SuctionCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
SuctionCmd::Execute (/*Robot * r, istream & , */int state)
{
   int axis,
     nstate;
   char st;
   //WallClimber *wc = (WallClimber *) r;

   puts ("\fPump Motor Setting\nWhich Foot?");
   axis = getchar () - '0';
   if (1 > axis || axis > 2)
      return state;
   puts ("\f1=On 0=Off");
   st = getchar ();
   if (st == KP_ON) {
      nstate = state | (axis * 4);
      if (nstate != state) {
         wc->cup[axis - 1]->SetSuction (1);
         return nstate;
      }
   }
   else if (st == KP_OFF) {
      time_t t1;
      double elapse;

      nstate = state & ~(axis * 4);
      if (nstate != state) {
         wc->cup[axis - 1]->SetSuction (0);
         time (&t1);
         elapsetime (&t1);
         while (!wc->cup[axis - 1]->PressureLow () &&
                (elapse = elapsetime (0)) < 10.0) {
            //p = wc->cup[axis - 1]->GetPressure ();
            //cout << p << " ";
         }
         cout << endl;
         return nstate;
      }
   }
   return state;
}

class ValveCmd: public Command {
   public:
      ValveCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
ValveCmd::Execute (/*Robot * r, istream &,*/ int s)
{
   int axis;
   char st;
   //WallClimber *wc = (WallClimber *) r;

   puts ("\fValve setting\nWhich Foot?");
   axis = getchar () - '0';
   if (axis < 1 || axis > 2)
      return s;
   puts ("\f1=On 0=Off");
   st = getchar ();
   if (st == KP_ON)
      wc->cup[axis - 1]->SetValve (1);
   else if (st == KP_OFF)
      wc->cup[axis - 1]->SetValve (0);
   return s;
}

class Fwd1Cmd: public Command {
   public:
      Fwd1Cmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

/**
   The robot is standing on Feet-1 and forward command is given
 */
int Fwd1Cmd::Execute (/*Robot * r, istream &,*/ int s)
{
   int new_state = 0, k;
   fd_set r_fds, t_fds;
   timeval t_out;
   t_out.tv_sec = 0;
   t_out.tv_usec = 0;

   //WallClimber *wc = (WallClimber *) r;
   if (s & CUP2) // check if Feet-2 sticks to the surface
      wc->PullCup2 ();
   FD_ZERO (&t_fds);
   FD_SET (STDIN, &t_fds);
   FD_SET (RECVER, &t_fds);
   while (1) {
      wc->FlipDownOn1 (1);
      if (emergency_stop) break;
      wc->PullCup1 (); 
      if (emergency_stop) break;
      new_state = 9;
      r_fds = t_fds;
      if (select (5, &r_fds, &t_out)) break;

      wc->FlipDownOn2 (1);
      if (emergency_stop) break;
      wc->PullCup2 (); 
      if (emergency_stop) break;
      new_state = 6;
      r_fds = t_fds;
      if (select (5, &r_fds, &t_out)) break;
   }
   if (emergency_stop)
   {
      cout << "Emergency exit in " << __FUNCTION__ << endl;
      return 0;
   }
   if (new_state == 6)
   {
      wc->FlipUpOn1();
      new_state = 4;
   }
   else
   {
      wc->FlipUpOn2();
      new_state = 8;
   }
   do {
      for (k=0; k<5; k++)
         if (FD_ISSET (k, &r_fds)) {
            fgetc ((FILE)k);
            break;
         }
      r_fds = t_fds;
   } while (select (5, &r_fds, &t_out));
   return new_state;
}

class Fwd2Cmd: public Command {
   public:
      Fwd2Cmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

/**
   The robot is standing on Feet-2 and forward command is given
 */
int Fwd2Cmd::Execute (/*Robot * r, istream & ,*/ int s)
{
   int new_state = 0, k;
   fd_set r_fds, t_fds;
   timeval t_out;
   t_out.tv_sec = 0;
   t_out.tv_usec = 0;
   //WallClimber *wc = (WallClimber *) r;

   if (s & CUP1)
      wc->PullCup1 ();
   FD_ZERO (&t_fds);
   FD_SET (STDIN, &t_fds);
   FD_SET (RECVER, &t_fds);
   while (1) {
      wc->FlipDownOn2 (1);
      if (emergency_stop) break;
      wc->PullCup2 (); 
      if (emergency_stop) break;
      new_state = 6;
      r_fds = t_fds; 
      if (select (5, &r_fds, &t_out)) break;

      wc->FlipDownOn1 (1);
      if (emergency_stop) break;
      wc->PullCup1 (); 
      if (emergency_stop) break;
      new_state = 9;
      r_fds = t_fds; 
      if (select (5, &r_fds, &t_out)) break;
   }
   if (emergency_stop)
   {
      cout << "Emergency exit in " << __FUNCTION__ << endl;
      return 0;
   }
   if (new_state == 6)
   {
      wc->FlipUpOn1();
      new_state = 4;
   }
   else
   {
      wc->FlipUpOn2();
      new_state = 8;
   }
   do {
      for (k=0; k<5; k++) {
         if (FD_ISSET (k, &r_fds)) {
            fgetc ((FILE)k);
            break;
         }
      }
      r_fds = t_fds;
   } while (select (5, &r_fds, &t_out));
   return new_state;
}

class Bwd1Cmd: public Command {
   public:
      Bwd1Cmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
Bwd1Cmd::Execute (/*Robot * r, istream &,*/ int s)
{
   int new_state = 0, k;
   fd_set r_fds, t_fds;
   timeval t_out;
   t_out.tv_sec = 0;
   t_out.tv_usec = 0;
   //WallClimber *wc = (WallClimber *) r;

   if (s & CUP2)
      wc->PullCup2 ();
   FD_ZERO (&t_fds);
   FD_SET (STDIN, &t_fds);
   FD_SET (RECVER, &t_fds);
   while (1) {
      wc->FlipDownOn1 (-1);
      if (emergency_stop) break;
      wc->PullCup1 (); 
      if (emergency_stop) break;
      new_state = 10;
      r_fds = t_fds;
      if (select (5, &r_fds, &t_out)) break;

      wc->FlipDownOn2 (-1);
      if (emergency_stop) break;
      wc->PullCup2 ();
      if (emergency_stop) break;
      new_state = 5;
      r_fds = t_fds;
      if (select (5, &r_fds, &t_out)) break;
   }
   if (emergency_stop)
   {
      cout << "Emergency exit in " << __FUNCTION__ << endl;
      return 0;
   }
   if (new_state == 5) {
      wc->FlipUpOn1();
      new_state = 4;
   }
   else {
      wc->FlipUpOn2();
      new_state = 8;
   }
   do {
      for (k=0; k<5; k++)
         if (FD_ISSET (k, &r_fds)) {
            fgetc ((FILE)k);
            break;
         }
      r_fds = t_fds;
   } while (select (5, &r_fds, &t_out));
   return new_state;
}

class Bwd2Cmd: public Command {
   public:
      Bwd2Cmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
Bwd2Cmd::Execute (/*Robot * r, istream &,*/ int s)
{
   int new_state = 0, k;
   fd_set r_fds, t_fds;
   timeval t_out;
   t_out.tv_sec = 0;
   t_out.tv_usec = 0;
   //WallClimber *wc = (WallClimber *) r;

   if (s & CUP1)
      wc->PullCup1 ();
   FD_ZERO (&t_fds);
   FD_SET (STDIN, &t_fds);
   FD_SET (RECVER, &t_fds);
   while (1) {
      wc->FlipDownOn2 (-1);
      if (emergency_stop) break;
      wc->PullCup2 ();
      if (emergency_stop) break;
      new_state = 5;
      r_fds = t_fds;
      if (select (5, &r_fds, &t_out)) break;

      wc->FlipDownOn1 (-1);
      if (emergency_stop) break;
      wc->PullCup1 ();
      if (emergency_stop) break;
      new_state = 10;
      if (select (5, &r_fds, &t_out)) break;
   }
   if (emergency_stop)
   {
      cout << "Emergency exit in " << __FUNCTION__ << endl;
      return 0;
   }
   if (new_state == 5)
   {
      wc->FlipUpOn1();
      new_state = 4;
   }
   else
   {
      wc->FlipUpOn2();
      new_state = 8;
   }
   do {
      for (k=0; k<5; k++)
         if (FD_ISSET (k, &r_fds)) {
            fgetc ((FILE)k);
            break;
         }
      r_fds = t_fds;
   } while (select (5, &r_fds, &t_out));
   return new_state;
}

class Up1Cmd: public Command {
   public:
      Up1Cmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
Up1Cmd::Execute (/*Robot * r, istream & ,*/ int)
{
   //WallClimber *wc = (WallClimber *) r;

   wc->FlipUpOn1 ();
   return 0;
}

class Up2Cmd: public Command {
   public:
      Up2Cmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
Up2Cmd::Execute (/*Robot * r, istream & ,*/ int)
{
   //WallClimber *wc = (WallClimber *) r;

   wc->FlipUpOn2 ();
   return 0;
}

class TurnCmd: public Command {
   public:
      TurnCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
TurnCmd::Execute (/*Robot * r, istream & ,*/ int s)
{
   //WallClimber *wc = (WallClimber *) r;
   bool neg;
   float angle;

   my_printf ("\nTurn on steering foot\n%c=Pos, %c=Neg ", KP_PLUS, KP_MINUS);
   neg = toupper(getchar()) == KP_MINUS;
   puts ("\fRotation Angle = ");
   putchar (neg ? '-' : '+');
   cin >> angle;
   if (neg) angle *= -1.0;
   wc->Turn (angle);
   return s;
}

class ClimbCmd: public Command {
   public:
      ClimbCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

int
ClimbCmd::Execute (/*Robot * r, istream & ,*/ int s)
{
   //float dist;
   bool neg;

   my_printf ("\nThe target surface is on the\n");
   my_printf ("[%c] Pump side\n[%c] Valve side ", KP_FORWARD, KP_BACKWARD);
   //WallClimber *wc = (WallClimber *) r;
   switch (toupper(getchar())) {
      case KP_BACKWARD:
         wc->Climb (BACKWARD);
         break;
      case KP_FORWARD:
         wc->Climb (FORWARD);
         break;
      default:
         return -1; /* stay in the last state */
   }
   //wc->motor[1]->Recalibrate ();
   //wc->motor[0]->Recalibrate ();
   //wc->motor[2]->Recalibrate ();
   return s; /* change into new state */
}

WallClimber w;

void
MyMain ()
{

   my_printf ("RobNav 2.0\nby Hans Dulimarta\nCompile date: %s %s\n",
               __DATE__, __TIME__);

   CommandInt ci (new istream (&stdin), new ostream (&stdout));

   ci.AddModule (new
                 Module (new Bwd1Cmd (&w), "Backward", "4 5 13", -1, KP_BACKWARD));
   ci.AddModule (new
                 Module (new Bwd2Cmd (&w), "Backward", "8 10 14", -1, KP_BACKWARD));
   ci.AddModule (new Module (new ClimbCmd (&w), "Climb Surface", "4 6", 8, 'C'));
   ci.AddModule (new
                 Module (new Fwd1Cmd (&w), "Forward", "4 6 14", -1, KP_FORWARD));
   ci.AddModule (new
                 Module (new Fwd2Cmd (&w), "Forward", "8 9 13", -1, KP_FORWARD));
   ci.AddModule (new Module (0, "Quit", "", -1, '9'));
   ci.AddModule (new Module (new RotateCmd (&w), "Rotate Joint", "", -1, '1'));
   ci.AddModule (new Module (new StatusCmd (&w), "Sensor Status", "", -1, '2'));
   ci.AddModule (new Module (new ResetCmd (&w), "Reset All Joints", "", -1, '3'));
   ci.AddModule (new Module (new TurnCmd (&w), "Turn Robot", "4", -1, '4'));
   ci.AddModule (new Module (new SuctionCmd (&w), "Suction", "", -1, KP_SUCTION));
   ci.AddModule (new Module (new Up1Cmd (&w), "Flip Up ", "5 6", 4, KP_UP));
   ci.AddModule (new Module (new Up2Cmd (&w), "Flip Up ", "9 10", 8, KP_UP));
   ci.AddModule (new Module (new ValveCmd (&w), "Valve", "", -1, KP_VALVE));
   ci.AddModule (new Module (new ConfigCmd (&w), "Configuration", "0", -1, 'C'));
   ci.CmdLoop ();
}

int main ()
{
   new Task (&MyMain);
   Clock* timer = Clock::Instance();
   timer->SetHandler (&PITHandler);
   EnableInterrupt();
   while (1);
}
