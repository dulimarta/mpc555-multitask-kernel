#include "io.h"
#include "mios.h"
#include "intr.h"
#include "qsm.h"
#include "usiu.h"
#include "uimb.h"
#include "stdlib.h"

extern int SYS_FREQ;

int main () {
   int n;

   MIOS.io.ddr = 0xFFFF;
   n = 0;
   my_printf ("System Freq is %d\n", SYS_FREQ);
   my_printf ("SCCR = %x\n", USIU.sccr);
   my_printf ("PLPRCR = %x\n", USIU.plprcr);
   my_printf ("UMCR = %x\n", UIMB.mcr);
   /* EnableInterrupt(); */
   while (1) {
      my_printf ("%d\n", USIU.rtc);
      sleep (1);
   }
   my_printf ("End of code\n");
   return 0;
}
