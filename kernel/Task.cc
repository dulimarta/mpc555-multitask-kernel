/**
 * @file Task.cc
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 */
/*
 * $Id: Task.cc,v 2.7 2000/12/06 20:45:19 dulimart Exp $
 *
 * $Log: Task.cc,v $
 * Revision 2.7  2000/12/06 20:45:19  dulimart
 * Changed stack size specification from bytes to words.
 * Added code for checking possible stack overflow.
 *
 * Revision 2.6  2000/11/17 19:19:43  dulimart
 * Renamed 'Run()' to 'Start()'. Added Ready() and Dispatch().
 *
 * Revision 2.5  2000/11/15 22:08:54  dulimart
 * Added task name and Task::Resume()
 *
 * Revision 2.4  2000/11/14 15:01:59  dulimart
 * Rewrote initialization in Task constructor: the stack was initialized
 * with 6 words: 2 for context switching and 4 for initial stack frame.
 * Instead of directly running the task, the dispatcher will select
 * Task::Dispatch() so return from exiting task is "natural".
 *
 * Revision 2.3  2000/11/09 14:10:29  dulimart
 * Removed Task::Activate().
 *
 * Revision 2.2  2000/11/06 22:20:50  dulimart
 * Modified initialization of task save area to allow exiting tasks
 * execution to return  to Task::Dispatch and Task::Destroy
 *
 * Revision 2.1  2000/11/03 16:45:27  dulimart
 * Initial checkin
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#include "Scheduler.h"
#include "Task.h"

#ifdef DEBUG
#include "io.h"
#endif

int Task::ID = 10;
#define INIT_MSR 0xA900

extern "C" {
   void _LoadContext (int *);
   void _SaveContext (int *);
   int sprintf (char *, const char *, ...);
}

Task::Task (char *n)
{
   id = INIT_TASK_ID; /* init task */
   /*
    * although this constructor converts currently running line
    * of execution into a task, the state is initialized to READY.
    * The scheduler will handle this special task and select it for
    * running.
    */
   state = READY;
   if (n) {
      name = new char (strlen(n)+1);
      strcpy (name, n);
   }
   else
      name = 0;
#if 0
   my_printf ("%s: Task %d: this=%p\n", __PRETTY_FUNCTION__, id, this);
#endif
   Scheduler::Instance()->Ready(this);
}

Task::Task (void (*entry)(), char *n, int nword)
{
   int r;

   /* make 'stack' multiple of 4 */
   r = nword & 3;
   if (r)
      nword += 4 - r;
   id = ID++;
   state = READY;
   if (n) {
      name = new char (strlen(n)+1);
      strcpy (name, n);
   }
   else
      name = 0;
   stack_area = new int [nword];

   /*
    * Initialization of the stack area follows the following layout:
    *
    *         +----------------+
    * R1 -->  |   current MSR  |   nword - 6
    *         +----------------+
    *         |   Task::Start  |   nword - 5
    *         +----------------+    
    *         |   Back Chain   |   nword - 4
    *         +----------------+    
    *         |   LR SaveWord  |   nword - 3
    *         +----------------+    
    *         |      this      |   nword - 2
    *         +----------------+
    *         |   entry-point  |   nword - 1
    *         +----------------+ 
    */

   /* Save the 'this' pointer to be used by Start later */

   /* set initial SP */
   sp = int (stack_area + nword - 6);
   h_sp = sp;

   /* The following 2 words simulate a context switch */
   stack_area [nword - 6] = INIT_MSR;
   stack_area [nword - 5] = (int) &Task::Start;

   /* The following 4 words constitute the initial 
    * stack frame of the task 
    */
   stack_area [nword - 4] = 0; /* Back Chain: value does not matter */
   stack_area [nword - 3] = 0; /* LR save word */
   stack_area [nword - 2] = (int) this;
   stack_area [nword - 1] = (int) entry;

#if 1
   my_printf ("%s: Task %d: this=%p SP=%x\n",
         __PRETTY_FUNCTION__, id, this, sp);
#endif
   Scheduler *sch;
   sch = Scheduler::Instance();
   sch->Ready(this);
   sch->AddTask(this);
}

int Task::gettid() const { return id; }
char* Task::getname() const { return name; }

Task::State Task::GetState () const
{
   return state;
}

void Task::Start()
{
   /*
    * WARNING: the assembly code used here relies on the 
    * -fomit-frame-pointer GCC option. With this option active, 
    * no new stack frame will be reserved for Task::Start()
    * if it does not make any other function call.
    */

   /* When Task::Start is chosen by scheduler to run (via SwitchCtx), 
    * SP will point to stack_area [nword-4]. 
    * The entry point of the task is located at offset 12
    */
#if 0
   asm ("lwz   %r3,0(%r1)");    /* get caller's stack frame */
   asm ("lwz   %r5,12(%r3)");   /* get entry point */ 
   asm ("mtlr  %r5;  blrl");    /* load the address and branch to it */

   /* this my_printf call caused a new stack frame to be allocated */
   my_printf ("%s: End of Task\n", __PRETTY_FUNCTION__);

   asm ("lwz   %r3,0(%r1)");    /* get caller's stack frame */
   asm ("lwz   %r3,8(%r3)");    /* load 'this' pointer */
#else
   /* no stack frame was allocated for Task::Start */
   asm ("lwz   %r5,12(%r1)");   /* get entry point */ 
   asm ("mtlr  %r5;  blrl");    /* load the address and branch to it */
   asm ("lwz   %r3,8(%r1)");    /* SP points to caller's stack frame */
#endif
   /* R3 now contains 'this' of the callee object */

   /* ANOTHER WARNING:
    *    Plain function name depends on the name mangling mode used
    *    by the compiler!
    */
   asm ("b     Destroy__4Task"); /* call Task::Destroy() */
}

void Task::Suspend()
{
   if (id == INIT_TASK_ID) return;
   /*
   if (state != RUNNING) {
      my_printf ("%s on %s status %d\n", __PRETTY_FUNCTION__, name, state);
      //while (1);
   }
   */
   state = BLOCKED;
}

void Task::Resume()
{
   if (state == BLOCKED)
      state = READY;
   else {
      my_printf ("%s on %s status %d\n", __PRETTY_FUNCTION__, name, state);
      while (1);
   }
}

void Task::Dispatch()
{
   if (id != INIT_TASK_ID) {
      if (sp < (int) (stack_area + DEF_STACK_SIZE/4)) {
         my_printf ("Stack overflow on Task-%d [%s]\n", id, name);
         my_printf ("Curr SP=%x, Allocated Stack=%x\n", sp, (int) stack_area);
         while (1);
      }
      if (sp < h_sp)
         h_sp = sp;
   }
   if (state == READY)
      state = RUNNING;
   else {
      my_printf ("%s on %s status %d\n", __PRETTY_FUNCTION__, name, state);
   }
}

void Task::Ready()
{
   state = READY;
}

void Task::Destroy()
{
   Scheduler *sch;

   sch = Scheduler::Instance();
   if (state == READY) {
      my_printf ("%s on ready task\n", __PRETTY_FUNCTION__);
      sch->UnReady(this);
   }
   sch->DeleteTask (this);
   state = TERMINATED;
   if (id > INIT_TASK_ID)
      delete [] stack_area;
   delete [] name;
   Scheduler::Instance()->Schedule();
}
