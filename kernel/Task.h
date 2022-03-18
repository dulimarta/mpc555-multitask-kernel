/**
 * @file Task.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief This class is an abstraction of a system task
 *
 */
/*
 * $Id: Task.h,v 2.9 2001/10/25 16:37:51 dulimart Exp $
 *
 * $Log: Task.h,v $
 * Revision 2.9  2001/10/25 16:37:51  dulimart
 * Increased Default Stack Size.
 *
 * Revision 2.8  2000/12/06 20:42:53  dulimart
 * Changed stack size parameter from byte to word
 *
 * Revision 2.7  2000/12/06 17:55:03  dulimart
 * Modified default stack size and added h_sp to track stack overflow
 *
 * Revision 2.6  2000/11/17 19:19:43  dulimart
 * Renamed 'Run()' to 'Start()'. Added Ready() and Dispatch().
 *
 * Revision 2.5  2000/11/15 22:08:54  dulimart
 * Added task name and Task::Resume()
 *
 * Revision 2.4  2000/11/14 14:44:13  dulimart
 * Moved sp, state and declration of enum State to public section.
 * Added Suspend().
 *
 * Revision 2.3  2000/11/09 14:10:29  dulimart
 * Removed Task::Activate().
 *
 * Revision 2.2  2000/11/06 22:14:38  dulimart
 * Added static member EndTask to capture execution of exiting task.
 *
 * Revision 2.1  2000/11/03 16:45:27  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef TASK_H
#define TASK_H

#define DEF_STACK_SIZE 576 /* in words not bytes */
#define INIT_TASK_ID 1

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !! Warning: the layout of Context has to match !!
 !! that defined in except.S                    !!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#if 0
struct Context
{
   int srr0, srr1;              /**< Special Registers */
   int gpr[32];                 /**< General Purpose Registers */
   int cr,                      /**< Condition Register */
     xer,                       /**< Integer Exception Register */
     ctr,                       /**< Count Register */
     lr;                        /**< Link Register */
   double fpr[32];              /**< Floating Point Registers */
   double fpscr;                /**< FP Status & Control Register */
};
#endif

class Task
{
   public:
      enum State { TERMINATED, RUNNING, BLOCKED, READY };

      /** 
       * Task default constructor
       * @param n name of the task
       */
      Task (char *n = 0);

      /**
       * Create a new task
       * @param entry starting point of the task
       * @param n name of the task
       * @param stack requested stack size (in words)
       */
      Task (void (*entry) (), char *n = 0, int stack = DEF_STACK_SIZE);

      /// destroy an existing task
      void Destroy ();

      /// suspend a task
      void Suspend ();

      /// resume a suspended task
      void Resume ();

      /// change task status to running
      void Dispatch ();

      /// change task status to ready
      void Ready ();

      /// save the current CPU context
      //void SaveContext ();

      /// restore the previously stored CPU context
      //void LoadContext ();

      /// Get the task id
      int gettid() const;

      /// Get the task name
      char* getname() const;

      /// Get the current state of the task 
      State GetState () const;

      int sp;                   /**< stack pointer */ 

   protected:
#ifdef DEBUG
      void ShowSA();
#endif
      static void Start();

      State state;              /**< current state of the task */

      static int ID;
      int id;                   /**< task identification*/
   private:
      int *stack_area;          /**< allocated stack for this task */
      int h_sp;
      char *name;
};
#endif
