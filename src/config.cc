// $Id: config.cc,v 2.4 2001/02/21 20:20:03 dulimart Exp $
//
// Author: Hans Dulimarta  dulimart@[egr.msu.edu, computer.org]
//
// Description:
//    This file contains the routines for run-time environment
// configuration.
//
// $Log: config.cc,v $
// Revision 2.4  2001/02/21 20:20:03  dulimart
// Changed executable modules from function to subclass of Command.
//
// Revision 2.3  2001/02/19 20:44:12  dulimart
// Modified signature of RotateRel
//
// Revision 2.2  2000/10/11 19:47:18  dulimart
// Added code for setting debug_level.
//
// Revision 2.1  2000/10/02 20:33:24  dulimart
// Initial checkin
//
//
// Copyright (c) 1999,2000  Hans Dulimarta, Ph.D.
//

#include "WallClimber.h"
#include "config.h"
#include "io.h"
#include "iodev.h"
#include "intr.h"
#include "qsm.h"
#include "usiu.h"
#include "mpc555.h"

void ClockSetting()
{
   int val;
   char ch;
   static bool intr_en = 1;

   while (1) {
      puts ("\f[1]  Change PLL Multiplication Factor\n");
      puts ("[2]  Change PLL Division Factor\n");
      puts ("[3]  Change PIT interval\n");
      my_printf ("[4]  %sable PIT interrupt\n\n>",
            intr_en ? "Dis" : "En");
      ch = getchar();
      if (ch == '0') break;
      switch (ch)
      {
         case '1':
            my_printf ("Current MulF = %d ", USIU.plprcr >> 20);
            puts ("New MulF: ");
            cin >> val;
            USIU.plprcr &= 0x000FFFFF;
            USIU.plprcr |= val << 20;
            qsm_init();
            break;
         case '2':
            my_printf ("Current Divf = %d ", USIU.plprcr && 0X1F);
            puts ("New DivF: ");
            cin >> val;
            USIU.plprcr &= 0xFFFFFFE0;
            USIU.plprcr |= val;
            qsm_init();
            break;
         case '3':
            /* 
             * PIT period (in sec) = (PITC + 1) * PTDIV / 4000000 
             *
             * or
             *
             * PIT period (in usec) = (PITC + 1) * PTDIV / 4 
             *
             */
            val = (USIU.pitc >> 16) + 1;
            if (USIU.sccr & BITSET(7,32)) val <<= 6;
            my_printf ("PIT count interval = %d usec\n", val);
            puts ("New interval (in usec): ");

            cin >> PIT_INTERVAL;
            USIU.piscrik = KEY;
            USIU.piscr = 0;

            val = 4 * PIT_INTERVAL; /* in microsecond */
            /* divide by 256 or 4 */
            val >>= (USIU.sccr & BITSET(7,32)) ? 8 : 2;
            val --;   
            USIU.pitck = KEY;
            USIU.pitc = val << 16;  

            USIU.piscrik = KEY;
            USIU.piscr = (IRQ(PITIRQ) << 8) | BITSET(13,16) | BITSET(15,16);
            break;
         case '4':
            if (intr_en) /* disable IRQ */
               DisableIRQ (PITIRQ);
            else /* re-enable IRQ */
               EnableIRQ (PITIRQ);
            intr_en = 1 - intr_en;
            break;
      }
   }
}

void PressureSetting(WallClimber *wc)
{
   char ch;
   int joint, val;

   puts ("Set Pressure Threshold\nWhich foot [1-2]? ");
   cin >> ch;
   if (ch == '0') return;
   joint = ch - '1';
   my_printf ("Joint-%d: Lo-Thresh=%d, Hi-Thresh=%d\n", joint,
         wc->cup[joint]->GetLowThreshold(), 
         wc->cup[joint]->GetHighThreshold()
         );
   puts ("[1] Set Low Threshold\n[2] Set High Threshold\n\n>");
   cin >> ch;
   switch (ch) {
      case '1':
         puts ("New low threshold = ");
         cin >> val;
         wc->cup[joint]->SetThreshold(val, -1); 
         break;
      case '2':
         puts ("New high threshold = ");
         cin >> val;
         wc->cup[joint]->SetThreshold(-1, val); 
         break;
   }
}

void PIDSetting ()
{
   int val, joint;
   char ch;
   extern PIDControl* PID[3];
   extern int PID_LIMIT;

   while (1) {
      puts ("Change PID parameters\nWhich joint [1-3]? ");
      cin >> ch;
      joint = ch - '0';
      if (joint == 0 || joint > 3) break;
      joint--;
      PID[joint]->ShowParm();
      puts ("[1]  Set Kp\n[2]  Set Ki\n[3]  Set Kd\n");
      puts ("[4]  Set Vmax\n[5]  Set Accel\n");
      puts ("[6]  Set Saturation Limit\n\n>");
      ch = getchar();
      switch (ch) {
         case '1':
            my_printf ("Joint-%d: new Kp = ", joint);
            cin >> val;
            PID[joint]->SetParm (val, -1, -1);
            break;
         case '2':
            my_printf ("Joint-%d: new Ki = ", joint);
            cin >> val;
            PID[joint]->SetParm (-1, val, -1);
            break;
         case '3':
            my_printf ("Joint-%d: new Kd = ", joint);
            cin >> val;
            PID[joint]->SetParm (-1, -1, val);
            break;
         case '4':
            my_printf ("Joint-%d: new Vmax = ", joint);
            cin >> val;
            PID[joint]->SetVelAcc (val, -1);
            break;
         case '5':
            my_printf ("Joint-%d: new Accel = ", joint);
            cin >> val;
            PID[joint]->SetVelAcc (-1, val);
            break;
         case '6':
            my_printf ("Saturation limit is %d\n", PID_LIMIT);
            my_printf ("New limit = ");
            cin >> PID_LIMIT;
            break;
      }
   }
}

int
ConfigCmd::Execute (/*Robot * r, istream &,*/ int)
{
   int k;
   int mtr_supp, pwm_width;
   char ch;
   //WallClimber *wc = (WallClimber *) r;
   MPC *cx;

   while (1) {
      puts ("\fConfiguration\n");
      puts ("[1]  Set Motor Supply Voltage\n");
      puts ("[2]  Change PID parameters\n");
      puts ("[3]  Change Clock Setting\n");
      puts ("[4]  Change Pressure Threshold\n");
      puts ("[5]  Set Debug Level\n");
      puts ("[6]  Reorder Touch Sensor Connection\n\n>");
      ch = getchar();
      if (ch == '0') break;
      switch (ch) {
         case '1':
            cx = (MPC *) wc->GetController ();
            puts ("Mtr Supp Volt? ");
            my_scanf ("%d", &mtr_supp);
            if (mtr_supp == 0)
               return 0;
            /* The multiplication factor 3072 is 6 * 512:
             * Pump motor has to be powered at 6 volts.
             * 100% duty cycle of PWM signal = 512 counts
             */
            pwm_width = 3072 / mtr_supp;
            if (pwm_width >= 510)
               pwm_width = 510;
            for (k = 0; k < 2; k++)
               cx->pwm[k].SetMaxWidth (pwm_width);
            break;
         case '2': 
            PIDSetting(); 
            break;
         case '3': 
            ClockSetting(); break;
         case '4': 
            PressureSetting(wc); break;
         case '5': 
            my_printf ("Current debug level = %d\n", debug_level);
            puts ("New debug level = ");
            my_scanf ("%d", &debug_level);
            break;
         case '6': 
            wc->ReorderTS();
            break;
      }
   }
   /*
   puts ("Press any key...\n");
   getchar ();
   */
   return 0;
}
