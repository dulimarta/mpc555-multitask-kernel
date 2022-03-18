#include <ctype.h>
#include "cmf.h"
#include "io.h"
#include "intr.h"
#include "mpc555.h"
#include "qadc.h"
#include "qsm.h"
#include "stdlib.h"
#include "usiu.h"

void qsm_handler ();
void RxHandler ();

void
PITHandler (void)
{                               /* Periodic-Inteval Timer */
   static short simask;
   static int cnt = 0;

   /*my_printf ("PIT sivec = %x\n", USIU.sivec); */
   simask = USIU.simask;        /* save old mask */
   USIU.simask = 0;             /* disable all interrupt */
   if (USIU.piscr & 0x80) {
      USIU.piscr |= 0x80;       /* clear status */
      if (cnt == 0) {
         if ((QADC1.portqb & 1) == 0) {
            int addr;

            /* Check if DEBUG switch is pressed */
          asm ("mfsrr0 %0":"=r" (addr));
            fputs ("IP=", STDERR);
            fputh (addr, STDERR);
            fputs (" SYPCR=", STDERR);
            fputh (USIU.sypcr, STDERR);
            fputc ('\n', STDERR);
         }
         /* Blink the LEDs */
         cnt = 1000000 / ((USIU.pitc >> 16) + 1);
         QADC2.portqa <<= 1;
         if (QADC2.portqa == 0)
            QADC2.portqa = 1;
      }
      cnt--;
   }
   USIU.sipend = BITSET (2 * PITIRQ + 1, 15); /* Clear pending request */
   USIU.simask = simask;        /* restore mask */
}

#define DotOrCh(s) (isprint(s) ? (s):'.')
#define Nib2Char(s) ((s)<10? ((s)+'0'):((s)-10+'A'))
void printhex (char b) {
   my_fprintf (STDERR, "%c%c ", Nib2Char (b>>4), Nib2Char (b&0x0F));
}

int
main ()
{
   char *ptr;
   char ch;
   char MyData[65];
   /*
   fd_set rfd;
   struct timeval tv;
   int fd = 0;
   */
   int k;

   USIU.dmbr = 0; /* disable dual mapping of internal flash */
   /*
      gettimeofday (0,0);
    */
   /*
   SetLVLHandler (PITIRQ, &PITHandler);
   SetIRQHandler (RXIRQ, &RxHandler);
   my_fprintf (STDERR, "CMF-A MCR = %x\n", CMF_A.mcr);
   SetLVLHandler (COM1IRQ, &qsm_handler);
   EnableInterrupt ();
   */
   my_fprintf (STDOUT, "%s\nHello World!\n", __TIME__);
   strcpy (MyData, "Hans Dulimarta, Michigan State University, East Lansing");
   cmf_write_page (&CMF_A, 0, 0, MyData);
   ptr = 0; 
   while (1) {
      my_fprintf (STDERR, "%6p ", ptr);
      for (k=0; k < 16; k++) printhex (*(ptr+k));
      for (k=0; k < 16; k++) my_fprintf (STDERR, "%c", DotOrCh(*(ptr+k)));
      my_fprintf (STDERR, "\n");
      ptr += 16;
      if (((int)ptr & 0xFF) == 0) {
         ch = fgetc (STDIN);
         if (ch == 0x1B)
            break;
      }
   }
   /*
   my_fprintf (STDERR, "BR0 = %x\n", USIU.br0);
   my_fprintf (STDERR, "OR0 = %x\n", USIU.or0);
   my_fprintf (STDERR, "BR1 = %x\n", USIU.br1);
   my_fprintf (STDERR, "OR1 = %x\n", USIU.or1);
   my_fprintf (STDERR, "BR2 = %x\n", USIU.br2);
   my_fprintf (STDERR, "OR2 = %x\n", USIU.or2);
   my_fprintf (STDERR, "BR3 = %x\n", USIU.br3);
   my_fprintf (STDERR, "OR3 = %x\n", USIU.or3);
   FD_ZERO (&rfd);
   FD_SET (STDIN, &rfd);
   FD_SET (RECVER, &rfd);
   tv.tv_sec = 0;
   tv.tv_usec = 500000;
   if (select (5, &rfd, 0)) {
      for (k = 0; k < 5; k++) {
         if (FD_ISSET (k, &rfd)) {
            fd = k;
            break;
         }
      }
      ch = fgetc (fd);
      while (ch != 0x1B && ch != '0') {
         fputc (ch, STDOUT);
         usleep (500000);
         ch = fgetc (fd);
      }
   }
   DisableInterrupt ();
   */
   return 0;
}
