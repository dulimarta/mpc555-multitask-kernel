// $Id: WallClimber.cpp,v 2.5 2001/02/19 20:44:12 dulimart Exp $
#ifdef DEBUG
#include "stream.h"
#endif
#include <math.h>
#include "intr.h"
#include "mpc555.h"
#include "stdlib.h"

#include "WallClimber.h"
#include "MPC.h"
#include "SJoint.h"

#ifdef DEBUG
#include "io.h"
#endif
#include "qsm.h"

extern PIDControl * PID[];

WallClimber::WallClimber () {
   int k;

   // turn correction is needed after the robot make a turn
   // on suction cup #1
   turnCorrect = 0;

   // initialize the two suction cups. 
   cup[0] = new Suction (&cx.pwm[0], &cx.ds[12], &cx.as[0]);
   cup[1] = new Suction (&cx.pwm[1], &cx.ds[14], &cx.as[1]);
   // initialize the three motors. The last three numbers are
   // rear ratio, left and right angle correction to be used 
   // during recalibration
   motor[0] = new SJoint (&cx.as[2]);
   motor[1] = new Joint (&cx.ds[4], &cx.ds[5]);
   motor[2] = new Joint (&cx.ds[6], 0);
   for (k = 0; k < 3; k++) {
      PID[k] = motor[k];
   }
   for (k=0; k<6; k++)
      touch_sensor[k] = &cx.qio[k];
#ifdef FIXME
   cout << "Enter the angle of the robot relative to gravity: ";
   cin >> angleToGravity;
   cout << "Is the robot initially on vertical surface? ";

   char ch;

   cin >> ch;
#endif
   angleToGravity = 0;
   onVertical = 0;
   motion_seq = 0;
}

void WallClimber::ReorderTS ()
{
   int k, r;

   my_printf ("\nReordering touch sensor connections\n");
   for (k=0; k<6; k++) {
      my_printf ("\nQIO index of TS-%d [0-5]? ", k+1);
      cin >> r;
      touch_sensor[k] = &cx.qio[r];
   }
}

WallClimber::~WallClimber ()
{
   int k;

   for (k = 0; k < 3; k++) delete motor[k];
   for (k = 0; k < 2; k++) delete cup[k];
}

void
WallClimber::BeginSync () const
{
   Joint::syncmove = 1;
}

void
WallClimber::EndSync ( /*unsigned char limit */ )
{
   extern int PID_Vel[], PID_Acc[];
   /*
      cerr << "Number of motor moving simultaneosly: " << 
      Joint::syncmove << endl;
    */
   if (Joint::syncmove == 1) {
      /* This condition should be flagged as an error.
       * The only motor put in the sync mode will NOT move
       */
      Joint::syncmove = 0;
#ifdef DEBUG
      fputs ("SYNC MODE ERROR!!!\n", STDERR);
#endif
      return;
   }
   /*
      my_fprintf (STDERR, "%s: sync. count = %d\n", 
      __PRETTY_FUNCTION__, Joint::syncmove);
    */

   SyncData sd[3];
   int i, k, pref, p, nmotion;

   // register all the motors involved in synchronized motion
   pref = 0;
   for (i = 0, k = 0; i < 3; i++) {
      if (motor[i]->SyncDist != 0) {
         sd[k].id = i;
         sd[k].dist = motor[i]->SyncDist;
#ifdef DEBUG
         my_fprintf (STDERR, "%s, Motor-%d SyncDist = %d\n", __FUNCTION__,
                     sd[i].id + 1, sd[i].dist);
#endif
         if (abs(sd[k].dist) > abs(sd[pref].dist))
            pref = k;
         sd[k].vel = PID_Vel[sd[k].id];
         sd[k].acc = PID_Acc[sd[k].id];
         k++;
      }
   }
   //peaktime = TriangTime (p->dist, p->vel, p->acc);
   //replace (sd.begin(), sd.end(), 0);
   for (p = 0; p < k; p++) {
      // readjust velocity & acceleration of the motors
      if (p != pref) {
         sd[p].acc = labs (sd[pref].acc * sd[p].dist / sd[pref].dist);
         sd[p].vel = labs (sd[pref].vel * sd[p].dist / sd[pref].dist);
      }
      //my_fprintf (STDERR, "%s, Motor-%d SyncVel = %d\n", __FUNCTION__,
               //sd[p].id + 1, sd[p].vel);
      motor[sd[p].id]->SetVelAcc (sd[p].vel, sd[p].acc);
      motor[sd[p].id]->SetMoveDist (sd[p].dist);
   }
   for (p = 0; p < k; p++)
      motor[sd[p].id]->StartMove ();
   //PIDControl::NUM_MIP = k;     /* Start the motor */
   // wait for all the motors to finish its cycle
   do {
      nmotion = 0;
      for (i = 0; i < 3; i++) {
         if (motor[i]->SyncDist == 0) continue;
         nmotion++;
         if (motor[i]->IsMoveDone ()) {
            //my_printf ("Motor-%d completed sync motion\n", i);
            motor[i]->SyncDist = 0;
            motor[i]->SetVelAcc (PID_Vel[i], PID_Acc[i]);
         }
      }
   } while (nmotion > 0);
   Joint::syncmove = 0;
   //usleep (500000);
}

bool emergency_stop = 0;

int
WallClimber::Climb (const int dir /*, const float dist*/)
{
   float gm /*, gm_max */ ;
   int whisker, whisk_h, whisk_v;
   whisker = touch_sensor[2]->GetState() | /* front */
      (touch_sensor[3]->GetState() << 1) | /* back */
      (touch_sensor[4]->GetState() << 2) | /* left */
      (touch_sensor[5]->GetState() << 3);  /* right */
   if (whisker != 15) {
      my_printf ("Whisker is %d: faulty touch sensors\n", whisker);
      return -1;
   }

   motion_seq++;

   gm = 10;
   BeginSync ();
   motor[0]->RotateTo (dir * -gm);
   motor[1]->RotateTo (dir * 2 * gm);
   motor[2]->RotateTo (dir * (3 * gm - 90));
   EndSync ();

   cup[1]->SetValve (0);
   while (!emergency_stop) {
      whisker = touch_sensor[2]->GetState() | /* front */
         (touch_sensor[3]->GetState() << 1) | /* back */
         (touch_sensor[4]->GetState() << 2) | /* left */
         (touch_sensor[5]->GetState() << 3);  /* right */
      my_printf ("Whisker is %d\n", whisker);
      if (whisker != 15) break;
      BeginSync ();
      motor[0]->RotateRel (dir * -1);
      motor[1]->RotateRel (dir * 2.0);
      motor[2]->RotateRel (dir * 3);
      EndSync ();
   }
   cup[1]->SetSuction (1);
   while (!emergency_stop) { 
      whisk_v = touch_sensor[2]->GetState() | /* front */
         (touch_sensor[3]->GetState() << 1);  /* back */
      whisk_h = touch_sensor[4]->GetState() | /* left */
         (touch_sensor[5]->GetState() << 1);  /* right */
      /* check if any 3 of the whiskers are touching */
      if (cup[1]->PressureHigh()) break;
      if ((whisk_h == 0 && whisk_v != 3) ||
          (whisk_v == 0 && whisk_h != 3)) {
         my_printf ("At least 3 touch\n");
         BeginSync ();
         motor[0]->RotateRel (dir * -0.5);
         motor[1]->RotateRel (dir * +1.0);
         motor[2]->RotateRel (dir * +1.5);
         EndSync ();
      }
      else {
         //cup[1]->SetSuction (0);
         switch (whisk_h) {
            case 1: /* left is not touching */
               my_printf ("Left is not touching\n");
               motor[0]->RotateRel (-dir * 2);
               break;
            case 2: /* right is not touching */
               my_printf ("Right is not touching\n");
               motor[0]->RotateRel (dir * 2);
               break;
            case 0: /* both are touching */
               my_printf ("Both horizontal whiskers are touching\n");
               break;
            case 3:
               my_printf ("Both horizontal whiskers are floating\n");
               //cup[1]->SetSuction (0);
               if (whisk_v == 3) {
                  my_printf ("Move the foot forward (whisk_h)\n");
                  BeginSync ();
                  motor[0]->RotateRel (dir * -0.5);
                  motor[1]->RotateRel (dir * +1.0);
                  motor[2]->RotateRel (dir * +1.5);
                  EndSync ();
               }
         }
         switch (whisk_v) {
            case 1: /* front is not touching */
               my_printf ("Front is floating\n");
               motor[2]->RotateRel (dir);
               break;
            case 2: /* back is not touching */
               my_printf ("Back is floating\n");
               motor[2]->RotateRel (-dir);
               break;
            case 0:
               my_printf ("Both vertical whiskers are touching\n");
               break;
            case 3:
               my_printf ("Both vertical whiskers are floating\n");
               if (whisk_h == 3) {
                  my_printf ("Move the foot forward (whisk_v)\n");
                  BeginSync ();
                  motor[0]->RotateRel (dir * -0.5);
                  motor[1]->RotateRel (dir * +1.0);
                  motor[2]->RotateRel (dir * +1.5);
                  EndSync ();
               }
               break;
         } 
      }
   }
   if (!emergency_stop)
      PullCup1 ();
   else {
      cup[1]->SetSuction (0);
      my_printf ("Emergency stop requested\n");
   }
   BeginSync();
   motor[0]->RotateTo (0);
   motor[1]->RotateTo (0);
   motor[2]->RotateTo (0);
   EndSync();
   return emergency_stop ? -1 : 0;
}


/*
void Com1Handler()
{
   short ch, st;
   static char last_char = 0;

   st = QSM.sc1sr;
   ch = QSM.sc1dr;
   if ((st & 0xF) == 0)
   {
      if (last_char == '*') 
      {
         switch (ch) 
         {
            case '0': // EMERGENCY STOP REQUESTED
               emergency_stop = 1;
               break;
         }
      }
      last_char = ch;
   }
}
*/

int
WallClimber::FlipDownOn1 (const int dir)
{
   motion_seq++;
#ifdef DEBUG
   my_fprintf (STDERR, "%s motion-%d\n", __FUNCTION__, motion_seq);
#endif
   //handler_ptr_t old_handler = SetLVLHandler (COM1IRQ, Com1Handler);
   emergency_stop = 0;
   if (onVertical) {
      cout << "Angle Correction on vertical wall" << endl;
      motor[0]->RotateRel (ROT_CORRECTION);
      cout << "angleToGravity = " << angleToGravity << endl;
      if (angleToGravity > 45) {
         motor[0]->RotateRel (-5);
      }
      else if (angleToGravity < -45)
         motor[0]->RotateRel (+5);
   }

   BeginSync ();
   motor[0]->RotateTo (dir * -35 + turnCorrect);
   motor[1]->RotateTo (dir *  70);
   motor[2]->RotateTo (dir * -75);
   EndSync ();

   bool whisker = touch_sensor[2]->GetState() && touch_sensor[3]->GetState();

   BeginSync ();
   motor[0]->RotateTo (dir * -45 + turnCorrect);
   motor[1]->RotateTo (dir *  90);
   motor[2]->RotateTo (dir * -45);
   EndSync ();

   int rc = whisker ?
      PushCup2T () /* with touch sensor */
      :
      PushCup2 (); /* without touch sensor */
   //SetLVLHandler (COM1IRQ, old_handler);
   return rc;
}

int WallClimber::PushCup2T ()
{
   int dir;
   int whisker;

   cup[1]->SetValve (0);
   cup[1]->SetSuction (1);
   dir = motor[2]->GetPos() > 0 ? +1 : -1;
   cout << __PRETTY_FUNCTION__ << endl;
   while (!emergency_stop) {
      if (cup[1]->PressureHigh())
         break;
      whisker = touch_sensor[2]->GetState() | /* front */
         (touch_sensor[3]->GetState() << 1); /* back */
      my_printf ("%s: touch sensor is %x\n", __PRETTY_FUNCTION__, whisker);
      //cup[1]->SetSuction (whisker == 0);
      switch (whisker)
      {
         case 1: /* only back is touching */
            motor[2]->RotateRel (+1);
            break;
         case 2: /* only front is touching */
            motor[2]->RotateRel (-1);
            break;
         case 0: /* Both sensors are touching, push more */
         case 3: /* no sensor is touching */
            BeginSync();
            motor[0]->RotateRel (dir);
            motor[1]->RotateRel (-2 * dir);
            EndSync();
            break;
      }
   }
   return 0;
}

int WallClimber::PushCup2 ()
{
   long int /*lastpos, currpos, */ appos;
   int /*avg_p, */ num_try, push_cnt, cycle_cnt /*, small_threshold */ ;
   float corrAngle;
   bool limit, small;
   int dir;

   appos = motor[2]->GetPos ();
   dir = appos > 0 ? -1 : +1;
   cup[1]->SetSuction (1);
   num_try = 0;
   corrAngle = 1.0;
   limit = small = 0;
   /*
      small_threshold = (fabs(angleToGravity) > 45) ? 
      SMALL_THRESH1 / 2 : SMALL_THRESH1;
      */
   cout << "Approaching surface : ";
   //puts ("Press any key..."); getchar();
   push_cnt = 0;
   cycle_cnt = 0;
   while (!emergency_stop) {
      //sleep (1);
      if (cup[1]->PressureHigh () /* || push_cnt >= 5 */ ) {
         break;
      }
      if (num_try < 10) {
         motor[2]->RotateRel (corrAngle);
         corrAngle *= -1;
         if (corrAngle < 0)
            corrAngle -= 1.0;
         else
            corrAngle += 1.0;
         num_try++;
      }
      else {
#ifdef DEBUG
         my_fprintf (STDERR, "%s pushing down %d\n", __PRETTY_FUNCTION__,
                     push_cnt);
#endif
         motor[2]->MoveTo (appos);
         corrAngle = 1.0;
         num_try = 0;
         cycle_cnt++;
         BeginSync ();
         motor[0]->RotateRel (dir * -1);
         motor[1]->RotateRel (dir * 2);
         motor[2]->RotateRel (dir * 3);
         EndSync ();
         push_cnt++;
      }
   }
   //cup[1]->UpdateHiThreshold();
   return 0;
}

int
WallClimber::FlipDownOn2 (const int dir)
{
   motion_seq++;
#ifdef DEBUG
   cerr << "Motion-" << motion_seq << " "
      << ((dir == FORWARD) ? "FORWARD" : "BACKWARD") << " on CUP #2" << endl;
#endif

   //handler_ptr_t old_handler = SetLVLHandler (COM1IRQ, Com1Handler);
   emergency_stop = 0;
   BeginSync ();
   motor[0]->RotateTo ((float) dir * 45);
   motor[1]->RotateTo ((float) dir * -90);
   motor[2]->RotateTo ((float) dir * 0);
   EndSync ();
   turnCorrect = 0;
   bool whisker = touch_sensor[0]->GetState() && touch_sensor[1]->GetState();

   motor[2]->RotateTo ((float) dir * 45);

   int rc = whisker ?
      PushCup1T () /* with touch sensor */
      :
      PushCup1 (); /* w/o touch sensor */
   //SetLVLHandler (COM1IRQ, old_handler);
   return rc;
}

int WallClimber::PushCup1T()
{
   int dir, whisker, w, tot_rot;
   unsigned int oldvel;
   unsigned short oldacc;

   cup[0]->SetValve (0);
   cup[0]->SetSuction (1);
   dir = motor[1]->GetPos() > 0 ? +1 : -1;
   cout << __PRETTY_FUNCTION__ << endl;
   tot_rot = 0;
   while (!emergency_stop) {
      if (cup[0]->PressureHigh())
         break;
      whisker = touch_sensor[0]->GetState() | /* front */
         (touch_sensor[1]->GetState() << 1); /* back */
      my_printf ("%s: touch sensor is %x\n", __PRETTY_FUNCTION__, whisker);
      //cup[0]->SetSuction (whisker == 0);
      switch (whisker)
      {
         case 1: /* only back is touching */
         case 2: /* only front is touching */
            /* lift the foot until it is not touching */
            motor[2]->GetVelAcc (&oldvel, &oldacc);
            motor[2]->SetVelAcc (oldvel/2, oldacc/2);
            do {
               motor[2]->RotateRel (dir);
               w = touch_sensor[0]->GetState() | /* front */
                  (touch_sensor[1]->GetState() << 1); /* back */
               my_printf ("%s: touch sensor is %x\n", __PRETTY_FUNCTION__, w);
               if (cup[0]->PressureHigh()) break;
            } while (w != 3 && !emergency_stop);
            motor[2]->SetVelAcc (oldvel, oldacc);

            // correct the angle
            motor[1]->RotateRel (whisker == 1 ? -2 : +2);
            //motor[2]->RotateRel (-5 * dir);
            break;
         case 0: /* Both sensors are touching, push more */
         case 3: /* no sensor is touching */
            motor[2]->RotateRel (-2 * dir);
            tot_rot += -2 * dir;
            if (abs(tot_rot) > 20) {
               motor[2]->RotateTo (dir * -45);
               tot_rot = 0;
            }
            break;
      }
   }
   return 0;
}

int
WallClimber::PushCup1 ()
{
   int k, pressed_cnt, in_try, push_cnt,
      out_try, max_try, cycle_cnt;
   int appos[3], lastpos;
   //bool small;
   float corrAngle;
   int dir;

   dir = motor[2]->GetPos () > 0 ? +1 : -1;
   cup[0]->SetValve (0);
   cup[0]->SetSuction (1);

   cout << "Approaching surface " << endl;
   for (k = 0; k < 3; k++) {
      //motor[k]->NoOp ();
      appos[k] = motor[k]->GetPos (); /* last approach position */
   }
   out_try = pressed_cnt = in_try = 0;
   cycle_cnt = 0;
   corrAngle = -2.0;
   /* if FORWARD: move closer, if BACKWARD: move further */
   max_try = 3;
   push_cnt = 0;
   while (!emergency_stop) {
      /* push the other suction cup by rotating joint-3 */
      lastpos = motor[2]->GetPos ();
      motor[2]->RotateRel (dir);
#ifdef DEBUG
      my_fprintf (STDERR, "%s: encoder change = %d\n",
                  __FUNCTION__, abs (lastpos - motor[2]->GetPos ()));
#endif
      push_cnt++;
      //usleep (500000);
      if (cup[0]->PressureHigh ()) break;
      if (push_cnt > 4) {
         push_cnt = 0;
#ifdef DEBUG
         //cout << "Retrying with a different angle of approach\n" << endl;
#endif
         cup[0]->SetSuction (0);
         motor[2]->MoveTo (appos[2]);
         motor[2]->RotateRel (-5 * dir);
         if ((float) dir * corrAngle > 0) { /* moving away from the switch */
            out_try++;
#ifdef DEBUG
            cout << "Outward attempts #" << out_try << endl;
#endif
            if (out_try > max_try) {
#ifdef DEBUG
               cout << "Too many outward attempts, reversing direction" <<
                  endl;
#endif
               corrAngle *= -1.0;
               BeginSync ();
               motor[0]->MoveTo (appos[0]);
               motor[1]->MoveTo (appos[1]);
               EndSync ();
               out_try = 0;
               cycle_cnt++;
               if (cycle_cnt % 2 == 0) {
#ifdef DEBUG
                  cout << "Cycle " << cycle_cnt <<
                     " is completed without success" << endl;
#endif
                  max_try++;
               }
            }
         }
         else {
            in_try++;
#ifdef DEBUG
            cout << "Inward attempts #" << in_try << endl;
#endif
            if (motor[1]->IsLimitOff (SIDE_SW)) {
               pressed_cnt++;
#ifdef DEBUG
               cout << "Number of attempts while switch is PRESSED: " <<
                  pressed_cnt << endl;
#endif
            }
            if (pressed_cnt > 1 || in_try > max_try) {
#ifdef DEBUG
               cout << "Too many inward attempts, reversing direction" << endl;
#endif
               corrAngle *= -1.0;
               BeginSync ();    // return to the 'old' position
               motor[0]->MoveTo (appos[0]);
               motor[1]->MoveTo (appos[1]);
               EndSync ();
               pressed_cnt = in_try = 0;
               cycle_cnt++;
               if (cycle_cnt % 2 == 0) {
#ifdef DEBUG
                  cout << "Cycle " << cycle_cnt <<
                     " is completed without success" << endl;
#endif
                  max_try++;
               }
            }
         }
#ifdef DEBUG
         cout << "Correcting cup #1 " << corrAngle << " degree(s)" << endl;
#endif
         BeginSync ();
         motor[0]->RotateRel (-corrAngle / 2);
         motor[1]->RotateRel (corrAngle);
         EndSync ();
         //motor[2]->RotateRel ((float) dir * 5);
         motor[2]->MoveTo (appos[2]);
         cup[0]->SetSuction (1);
      }
   }
   //cup[0]->UpdateHiThreshold();
   cout << "done" << endl;
   return 0;
}

void
WallClimber::PullCup2 ()
{
   int cnt, dir;

   cup[1]->SetValve (1);        /* open valve 2 */
   cup[1]->SetSuction (0);
   // wait until pressure decreases
   dir = motor[1]->GetPos () > 0 ? -1 : +1;
   cnt = 0;
   while (cnt < 10) {
      sleep (1);
      if (cup[1]->PressureLow ()) break;
      sleep (1);
      //BeginSync ();
      //motor[0]->RotateRel (-dir);
      //motor[1]->RotateRel (dir * 2);
      //motor[2]->RotateRel (-dir * 2);
      //EndSync ();
      cnt++;
      //my_printf ("%s %d\n", __PRETTY_FUNCTION__, cnt); getchar();
   }
   //cerr << "done" << endl;
}

int WallClimber::FlipUpOn1 ()
{
   motion_seq++;

   PullCup2 ();

   BeginSync ();
   motor[0]->RotateTo (0);
   motor[1]->RotateTo (0);
   motor[2]->RotateTo (0);
   EndSync ();

   cup[1]->SetValve (0);        /* close valve 2 */

   //if (onVertical)
   //motor[0]->RotateRel (-5);

   return 0;
}

void WallClimber::PullCup1 ()
{
   int cnt, dir;

   cup[0]->SetSuction (0);      // turn off pump #1
   cup[0]->SetValve (1);        // open valve #1
   sleep (3);
   /* Wait until pressure decreases */
   cerr << __PRETTY_FUNCTION__;
   //rel_correct = 0.0;
   dir = motor[2]->GetPos () > 0 ? -1 : +1;
   cnt = 0;
   while (cnt < 10) {
      sleep (1);
      if (cup[0]->PressureLow ()) break;
      //motor[0]->RotateRel (rot);
      sleep (1);
      //motor[2]->RotateRel (dir);
      cnt++;
      //my_printf ("%s %d\n", __PRETTY_FUNCTION__, cnt); getchar();
   }
   cerr << "done" << endl;
}

int WallClimber::FlipUpOn2 ()
{

   cerr << __PRETTY_FUNCTION__ << endl;
   motion_seq++;

   PullCup1 ();

   BeginSync ();
   motor[0]->RotateTo (0);
   motor[1]->RotateTo (0);
   motor[2]->RotateTo (0);
   EndSync ();
   turnCorrect = 0.0;
   cup[0]->SetValve (0);
   return 0;
}

int WallClimber::Turn (float angle)
{
   motion_seq++;
   cout << "Motion-" << motion_seq << " Turn " << 
      angle << " degrees" << endl;
   motor[0]->RotateRel (angle);
   angleToGravity += angle;
   turnCorrect += angle;
   return 0;
}
