/**
 * @file Scheduler.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief This class is an abstraction of a task scheduler
 *
 */
/*
 * $Id: Scheduler.h,v 2.6 2000/12/06 20:22:06 dulimart Exp $
 *
 * $Log: Scheduler.h,v $
 * Revision 2.6  2000/12/06 20:22:06  dulimart
 * Removed list of blocked tasks.
 * Added Task Table and member functions for adding and removing task
 * from the Task Table.
 *
 * Revision 2.5  2000/11/17 20:15:28  dulimart
 * Replaced vector with list. vector::erase(pos) might invalidate iterators
 * after pos. This might affect the run_idx iterator. With list, erase() is
 * much safer.
 *
 * Revision 2.4  2000/11/14 14:39:32  dulimart
 * Addedd Scheduler::Suspend() for suspending currently running task.
 *
 * Revision 2.3  2000/11/13 17:23:27  dulimart
 * Replaced formal parameter of SwitchTo from list iterator to integer index.
 * Replaced 'running' from list iterator to index.
 *
 * Revision 2.2  2000/11/06 22:17:04  dulimart
 * Added UnReady()
 *
 * Revision 2.1  2000/11/03 16:45:27  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Task.h"
#define NTASK 8
#include <list>

#define ShowStackFrame() { \
   asm ("mr 3,1; li 4,1; bl fputh"); \
   asm ("li 3,32; li 4,1; bl fputc"); \
   asm ("lwz 3,0(1); li 4,1;bl fputh"); \
   asm ("li 3,0x0D; li 4,1; bl fputc"); \
}

class Scheduler
{
   public:
      static Scheduler* Instance();
      ~Scheduler ();

      /// perform a simple round-robin scheduling
      void Schedule ();

      /**
       * Put a task in Ready Queue
       * @param t pointer to a newly created task
       */
      void Ready (Task *t);

      /**
       * Remove a task from Ready Queue
       * @param t pointer to the task to be removed
       */
      void UnReady (Task *t);

      /**
       * Put a task in Blocked Queue
       * @param t pointer to a task to be blocked
       */
      void Suspend (Task *t);

      /**
       * Remove a task from Blocked Queue
       * @param t pointer to a task to be freed
       */
      void UnBlock (Task *t);

      /**
       * Move a task from Blocked Queue to Ready Queue
       * @param t pointer to a task to be moved
       */
      //void Resume (Task *t);

      /*
       * Switch context from the current task to the new task
       * @param nt index (in the ready queue) of the new task to run
       */
      //void SwitchTo (list<Task*>::iterator nt);
      void SwitchTo (Task* nt);

      /// list all tasks known to the scheduler
      void ListTasks();

      /**
       * Add a new task to the Task Table
       * @param t pointer to the task to be added
       */
      void AddTask (Task *);

      /**
       * Delete a task from the Task Table
       * @param t pointer to the task to be removed
       */
      void DeleteTask (Task *);

      /** 
       * Who is  currently running 
       * @return pointer to the currently running task
       */
      Task* Current() const;

   protected:
      /// protected constructor
      Scheduler ();

      int n_ready;      /**< number of ready task */
      Task** TaskTab;

      Task *current;            /**< pointer to the running task */
      Task *idle;               /**< pointer to the idle task */
      list<Task *>ready;      /**< list of ready tasks */

   private:
      static Scheduler* _instance;
};
#endif
