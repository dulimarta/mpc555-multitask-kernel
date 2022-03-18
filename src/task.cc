#include "intr.h"
#include "mpc555.h"
#include "memory.h"
#include "qadc.h"
#include "stdlib.h"
#include "usiu.h"

#include "Scheduler.h"
#include "Task.h"
#include "Clock.h"

#include "io.h"

#define GetCaller() \
   { \
      int pc; \
      asm ("lwz 3,0(1); lwz 3,4(3); stw 3,%0" : "=m" (pc)); \
      my_printf ("%s will return to %x\n", __FUNCTION__, pc); \
   }

void doit()
{
   static int tick = 0, cnt = 0;
   Scheduler *os = Scheduler::Instance();

   if (tick <= 0) {
      tick = 200;
      os->Schedule();
      if (cnt <= 0) {
         cnt = 10;
         QADC2.portqa <<= 1; 
         if (QADC2.portqa == 0) QADC2.portqa = 1; 
      }
      cnt--;
   }
   tick--;
}

void Numeric()
{
   int c = 0;
   my_printf ("%s\n", __FUNCTION__);
   while (1) {
      my_printf ("%d ", c);
      c++;
      usleep (500000);
   }
}

void LowAlphabet()
{
   char c = 'a';
   double x;
   //int k;
   x = 0.0;
   for (c = 'a'; c <= 'z'; c++) { 
      my_printf ("%c ", c);
      x += .01;
      usleep (150000);
   }
}

void UpAlphabet()
{
   char c = 'A';
   my_printf ("%s\n", __FUNCTION__);
   while (1) {
      my_printf ("%c ", c++);
      usleep (1250000);
      if (c > 'Z') c = 'A';
   }
}

void Time()
{
   unsigned int l;
   while (1) {
      GetTB (&l,0);
      my_printf ("TB=%u ", l);
      usleep (1750000);
   }
}

void MonitorMem()
{
   while (1) {
      my_printf ("Avail Mem is %dK bytes\n", getavailmem());
      sleep (2);
   }
}

void Command()
{
   char c;
   while (1) {
      my_printf ("%s\n", __FUNCTION__);
      my_scanf ("%c", &c);
      if (c == 0x1B) break;
      switch (c) {
         case 't':
            Scheduler::Instance()->ListTasks();
            /*
            if (Clock::n_timer) {
               my_printf ("%d timer(s)\n", Clock::n_timer);
               my_printf ("Timer for %d will expire in %d counts\n", 
                     Clock::expire_task->gettid(), Clock::expire_cnt);
            }
            */
            break;
         case 'm':
            my_printf ("MSR=%x\n", GetMSR());
            break;
         default:
            my_printf ("\nCharacter is %c\n", c);
      }
   }
}

void WatchDog ()
{
   while (1) {
      USIU.swsr = 0x556C;
      USIU.swsr = 0xAA39;
   }
}

int main ()
{
   new Task ("main"); /* convert 'main' to 'init' task */
   new Task (&WatchDog, "Dog");
   new Task (&MonitorMem, "Mem");
   new Task (&Command, "Command");
   new Task (&Numeric, "Num");
   new Task (&LowAlphabet, "Low Alpha");
   new Task (&UpAlphabet, "UpAlfa");
   new Task (&Time, "Time");
   Clock* timer = Clock::Instance();
   timer->SetHandler (&doit);
   EnableInterrupt();
   while (1) {
      /* do not make any blocking call */
      putchar ('_');
   }
}
