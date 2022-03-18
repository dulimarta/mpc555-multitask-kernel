/* 
 * $Id: cmf.c,v 2.2 2000/10/02 20:14:18 dulimart Exp $
 *
 * Author: Hans Dulimarta, dulimart@computer.org
 * Description:
 *    This module defines functions for programming and erasing the
 * CMF (CDR MoneT Flash) EEPROM modules.
 *
 * $Log: cmf.c,v $
 * Revision 2.2  2000/10/02 20:14:18  dulimart
 * Changed header comment
 *
 * Revision 2.1  2000/08/02 15:51:23  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */

#include "cmf.h"
#include "io.h"
#include "mpc555.h"
#include "stdlib.h"

void cmf_write (char *dest, const char *src, int len) {
}

void
cmf_write_page (CMF_map *cmf, short block, int offset, char *data) {
   int k, chk;
   int ctl, o_ctl;
   char *ptr;

   my_fprintf (STDERR, "CMR = %x, CTL = %x\n", cmf->mcr, cmf->ctl);
   cmf->mcr &= ~BITSET(24+block,32);
   /* At 20 MHz a 25.6 us program pulse is obtained by
    * SCLKR = 0b011    (R = 2),
    * CLKPE = 0b00     (N = 5),
    * CLKPM = 0b0000111 (M = 8)
    *
    * 00 011 0 00 0 0000111 ==> 0x1807
    */
   cmf->ctl = 0x18070000 | 
      BITSET(16+block,32) /* Enable block */ | 
      BITSET(30,32); /* Set SES */
   my_fprintf (STDERR, "%s at line %d: CTL = %x\n", 
         __FUNCTION__, __LINE__, cmf->ctl);
   for (k=0, ptr = (char *) (0x8000 * block) + offset; k<64; k++, ptr++) {
      *ptr = data[k];
   }
   while (1) {
      my_fprintf (STDERR, "%s at line %d: CTL = %x\n", 
            __FUNCTION__, __LINE__, cmf->ctl);
      fgetc (STDIN);
      cmf->ctl |= 1; /* Set EHV */
      ctl = o_ctl = 0;
      o_ctl = 0;
      my_fprintf (STDERR, "%s at line %d: CTL = %x\n", 
            __FUNCTION__, __LINE__, cmf->ctl);
      usleep (500000);
      while (1) {
         ctl = cmf->ctl;
         my_fprintf (STDERR, "CTL = %x\n", ctl);
         if (o_ctl != ctl) {
            o_ctl = ctl;
         }
         if ((ctl & BITSET (0,32)) == 0)
            break;
      }
      cmf->ctl &= ~1; /* Reset EHV */
      chk = 0;
      for (k=0, ptr = (char *) (0x8000 * block) + offset; k<64; k++, ptr++) {
         chk += *ptr;
      }
      my_fprintf (STDERR, "Check after write = %d\n", chk);
      if (chk == 0) break;
   }
   cmf->ctl &= BITSET(30,32); /* Reset SES */
}
