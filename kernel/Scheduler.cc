/**
 * @file Scheduler.cc
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 */
/*
 * $Id: Scheduler.cc,v 2.8 2001/10/25 16:34:44 dulimart Exp $
 *
 * $Log: Scheduler.cc,v $
 * Revision 2.8  2001/10/25 16:34:44  dulimart
 * Added comment for ContextSwitch
 *
 * Revision 2.7  2000/12/06 20:49:07  dulimart
 * Renamed running to current, added Task Table, fixed initialization of idle and
 * current in Ready().
 * Reimplemented round-robin scheduling.
 *
 * Revision 2.6  2000/11/17 21:28:34  dulimart
 * Added Current() which will return a pointer to running task.
 * Replaced EnableInterrupt() with RestoreInterrupt().
 * Modified direct access to Task::state to using Task's member function.
 *
 * Revision 2.5  2000/11/14 14:39:32  dulimart
 * Addedd Scheduler::Suspend() for suspending currently running task.
 *
 * Revision 2.4  2000/11/13 17:23:27  dulimart
 * Replaced formal parameter of SwitchTo from vector iterator to integer index.
 * Replaced 'running' from vector iterator to index.
 *
 * Revision 2.3  2000/11/09 14:08:25  dulimart
 * Added code to update number of ready tasks when UnReady is called.
 *
 * Revision 2.2  2000/11/06 22:34:59  dulimart
 * Added UnReady().
 *
 * Revision 2.1  2000/11/03 16:45:27  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include "Scheduler.h"
#include <algorithm>
#include "intr.h"
#ifdef DEBUG
#include "io.h"
#endif

extern "C" {
   /**
      Switch Context of execution
      @param old_ctx old execution context
      @param new_ctx new execution context
    */
   void SwitchCtx (int* old_ctx, int* new_ctx);
}

Scheduler* Scheduler::_instance = 0;

Scheduler::Scheduler()
{
   current = 0;
   idle = 0;
   n_ready = 0;
   TaskTab = (Task **) calloc (NTASK, sizeof(Task *));
}

Scheduler::~Scheduler()
{
   free (TaskTab);
}

Scheduler* Scheduler::Instance ()
{
   if (_instance == 0) {
      _instance = new Scheduler;
   }
   return _instance;
}

Task* Scheduler::Current() const
{
   return current;
}

void Scheduler::SwitchTo (Task* nt)
{
   if (current == nt) /* check if we are switching to the same task */
      return;

   register Task *o_run;

   o_run = current;
   int int_stat = DisableInterrupt();
   current = nt;
   RestoreInterrupt (int_stat);

   /* SwitchCtx will possibly return to a different task */
   SwitchCtx (o_run ? &(o_run->sp) : 0, &(current->sp));
}

void Scheduler::Ready (Task *t)
{
   //my_printf ("Put task %d in the ready queue\n", t->gettid());
   int st = DisableInterrupt();
   t->Ready(); /* change the state */

   if (t == idle) {
      RestoreInterrupt (st); 
      return; /* do not put INIT in the queue */
   }

   if (t->gettid() == INIT_TASK_ID && idle == 0) {
      idle = t;
      current = t;
   }
   else {
      ready.push_back (t);
      n_ready++;
   }
   RestoreInterrupt (st);
}

void Scheduler::UnReady (Task *t)
{
   if (current == t) {
      my_printf ("%s on current task\n", __PRETTY_FUNCTION__);
      while (1);
   }
   list<Task*>::iterator p;
   int st = DisableInterrupt();
   p = find (ready.begin(), ready.end(), t);
   if (p != ready.end()) {
      ready.erase (p);
      n_ready--;
   }
#ifdef DEBUG
   else {
      my_printf ("task %d [%s] not in ready Q\n", 
            t->gettid(), t->getname());
   }
#endif
   RestoreInterrupt(st);
}

void Scheduler::Suspend (Task *t)
{
   t->Suspend();
   Schedule();
}

void Scheduler::Schedule()
{
   /* Do a round-robin scheduling */
   Task* ntp;

   int st = DisableInterrupt();
   if (current)
      if (current->GetState() == Task::RUNNING)
         Ready (current); /* put the current task in ready Q */

   if (ready.size() > 0) {
      ntp = ready.front();
      ready.pop_front();
      n_ready--;
   }
   else
      ntp = idle;
   ntp->Dispatch();
   RestoreInterrupt (st);
   SwitchTo (ntp);
}

void print (Task *t)
{
   my_printf ("%d\t%s\t\t%d\n", 
         t->gettid(), t->getname(), t->GetState());
}

void Scheduler::AddTask (Task *t)
{
   int k;

   for (k=0; k<NTASK; k++)
   {
      if (TaskTab[k] == 0) {
         TaskTab[k] = t;
         my_printf ("TaskTab[%d] is assigned to Task %d\n", k, t->gettid());
         return;
      }
   }
   my_printf ("No space in Task Table for Task %d\n", t->gettid());
   while (1);
}

void Scheduler::DeleteTask (Task *t)
{
   int k;

   for (k=0; k<NTASK; k++)
   {
      if (TaskTab[k] == t) {
         TaskTab[k] = 0;
         return;
      }
   }
   my_printf ("Task %d not in Task Table\n", t->gettid());
   while (1);
}

void Scheduler::ListTasks ()
{
   if (current)
      my_printf ("Running: %d [%s]\n", current->gettid(), current->getname());
   my_printf ("%d (%d) task ready\n", n_ready, ready.size());
   if (n_ready) {
      my_printf("ID\tName\t\tStatus\n");
      for_each (ready.begin(), ready.end(), print);
   }
}

