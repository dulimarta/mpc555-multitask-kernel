/**
 * @file Clock.cc
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 */
/*
 * $Id: Clock.cc,v 2.6 2000/12/08 22:25:54 dulimart Exp $
 *
 * $Log: Clock.cc,v $
 * Revision 2.6  2000/12/08 22:25:54  dulimart
 * Moved WakeUpCall from pit_handler to DEC_handler
 *
 * Revision 2.5  2000/12/06 20:00:52  dulimart
 * Disable Time Base Handler
 *
 * Revision 2.4  2000/11/30 18:28:44  dulimart
 * Replaced signature of SetAlarm make current task as the target.
 *
 * Revision 2.3  2000/11/17 21:31:48  dulimart
 * Implemented SetAlarm(), delta-list, and WakeUpCall() to handle blocking
 * sleep call.
 *
 * Revision 2.2  2000/11/09 14:06:24  dulimart
 * Added Time Base Interrupt handler.
 *
 * Revision 2.1  2000/11/03 16:45:27  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#include "Clock.h"
#include "Scheduler.h"
#include "mpc555.h"
#include "usiu.h"
#include "sync.h"

#ifdef DEBUG
#include "io.h"
#endif

Clock* Clock::_instance = 0;
handler_ptr_t Clock::user_handler = 0;
int Clock::n_timer = 0;
int Clock::expire_cnt = 0;
Task* Clock::expire_task = 0;
slist<pair<Task*,int> > Clock::timers;

Clock::Clock() 
{
   interval = 0;
}

Clock* Clock::Instance ()
{
   if (_instance == 0) {
      _instance = new Clock;
      _instance->SetInterval (1000);
      SetLVLHandler (PITIRQ, &Clock::pit_handler);
      SetDECHandler (&Clock::WakeUpCall);
   }
   return _instance;
}

void Clock::SetInterval (int t)
{
   interval = t;
}

void Clock::SetAlarm (int cnt)
{
   //cnt /= PIT_INTERVAL;
   if (cnt == 0) return;

   int orig_cnt, k;
   slist<pair<Task *,int> >::iterator p, pp;

   k = 0;
   p = timers.begin();
   pp = 0;
   orig_cnt = cnt;
   while (k < n_timer) {
      if (cnt < p->second)
         break;
      cnt -= p->second;
      pp = p;
      p++;
      k++;
   }
   int int_stat = DisableInterrupt();
   Task* t = Scheduler::Instance()->Current();
   if (pp == 0) { /* insert first */
      timers.push_front (make_pair(t,cnt));
      expire_cnt = cnt;
      expire_task = t;
   }
   else { /* insert after pp */
      timers.insert_after (pp, make_pair(t,cnt));
   }
   if (p != 0) p->second -= cnt;
   n_timer++;
   if (n_timer == 1) /* set DEC if no other timer is active */
      SetDEC (cnt);
   RestoreInterrupt (int_stat);
   Scheduler::Instance()->Suspend(t);
}

void Clock::WakeUpCall ()
{
   if (n_timer == 0) return;
   do {
      Scheduler::Instance()->Ready (expire_task);
      timers.pop_front ();
      n_timer--;
      if (n_timer > 0) {
         slist<pair<Task *,int> >::iterator p;

         p = timers.begin();
         expire_task = p->first;
         expire_cnt = p->second;
      }
      else {
         expire_task = 0;
         expire_cnt = -1; /* to exit the loop */
      }
   } while (expire_cnt == 0);
   if (expire_cnt > 0)
      SetDEC (expire_cnt);
}

void Clock::pit_handler()
{
   if (USIU.piscr & 0x80) {
      USIU.piscr |= 0x80;
      if (user_handler) user_handler();
   }
}

handler_ptr_t Clock::SetHandler (handler_ptr_t nh)
{
   DisableIRQ (PITIRQ);
   handler_ptr_t old_h = user_handler;
   user_handler = nh;
   EnableIRQ (PITIRQ);

   return old_h;
}
