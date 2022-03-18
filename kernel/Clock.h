/**
 * @file Clock.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief This class is an abstraction of a system clock.
 * This class is written as a Singleton, only one object instance is 
 * allowed to exists.
 *
 */
/*
 * $Id: Clock.h,v 2.5 2000/12/08 22:23:49 dulimart Exp $
 *
 * $Log: Clock.h,v $
 * Revision 2.5  2000/12/08 22:23:49  dulimart
 * Removed tb_handler()
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
#ifndef CLOCK_H
#define CLOCK_H

#include "intr.h"
#include "Task.h"
#include <slist>

class Clock
{
   public:
      /// Create a new (singleton) clock
      static Clock* Instance();

      /**
       * Set the interval of the timer interrupt
       * @param t number of microseconds between two subsequent interrupts
       */      
      void SetInterval (int t);

      /**
       * Register a "wake up call"
       * @param t the task that should receive the call
       * @param c the delta time to the call (wake me up at "now + t")
       */
      static void SetAlarm (int c);

      /**
       * Set user timer handler
       * @param nh pointer to user function for handling timer interrupt 
       * @return pointer to previously define user timer handler function
       */     
      handler_ptr_t SetHandler (handler_ptr_t nh);

   protected:
      /// protected constructor
      Clock();

      /// Wake up the task which previously called SetAlarm()
      static void WakeUpCall();

      /// default handler for Periodic Interrupt Timer
      static void pit_handler();

   private:
      static Clock* _instance;          /**< pointer to singleton instance */

      /// delta list of task and its expiration count
      static slist< pair<Task*,int> > timers;
      int interval;                     /**< interval for PIT */
      static int n_timer;               /**< # of timers */

      static int expire_cnt;            /**< current expiration count */
      static Task* expire_task;         /**< the task at the head of the list */

      /// hook for user-define timer handler
      static handler_ptr_t user_handler;
};
#endif
