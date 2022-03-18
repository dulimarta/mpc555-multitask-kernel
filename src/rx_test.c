#include <ctype.h>
#include "cmf.h"
#include "io.h"
#include "intr.h"
#include "mpc555.h"
#include "qadc.h"
#include "qsm.h"
#include "stdlib.h"
#include "usiu.h"

int PIT_INTERVAL = 0;

int
main ()
{
   char ch;

   my_printf ("Hello\n");
   while (1) {
      ch = RecvByte();
      if (ch == 0x1B) break;
      SendByte (ch);
      SendByte2 (ch);
   }
   my_printf ("Reverse\n");
   while (1) {
      ch = RecvByte2();
      if (ch == 0x1B) break;
      SendByte (ch);
      SendByte2 (ch);
   }
   return 0;
}
