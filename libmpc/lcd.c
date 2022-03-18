/*
 * $Id: lcd.c,v 2.3 2000/08/02 15:49:20 dulimart Exp $
 *
 * Author: Hans Dulimarta, Ph.D.
 *
 * Description:
 *    This file contains routines for handling the LCD on the
 * CME-555 evaluation board. To the MPC555 chip, the LCD is just
 * a peripheral connected to its external bus.
 *
 * $Log: lcd.c,v $
 * Revision 2.3  2000/08/02 15:49:20  dulimart
 * Removed debugging messages.
 *
 * 
 * Copyright (c) 1999,2000   Hans Dulimarta, Ph.D.
 *
 */
#include <ctype.h>
#include "lcd.h"

static int LCDrow = 1;

void lcd_init(void) {
   LCDCommand (0x38); /* 8-bit data, 2 lines */
   LCDCommand (0x14); /* shift cursor right */
   LCDCommand (0x0f); /* display & cursor on */
   LCDCommand (0x01); /* clear display */
   LCDCommand (0x02); /* return home */
}

int
isLCDBusy ()
{
   return LCD_CMD & 0x80;
}

void
LCDCommand (unsigned char c)
{
   /*my_printf ("Writing command %x\n", c); */
   while (isLCDBusy ());
   LCD_CMD = c;
}

unsigned char
LCDgetch ()
{

   while (isLCDBusy ());
   return LCD_DATA;
}

void
LCDSetAddr (unsigned char ddram)
{
   LCDCommand (0x80 | ddram);
}

void
LCDwrite (unsigned char c)
{
   while (isLCDBusy ());
   LCD_DATA = c;
}

void
LCDCopy (unsigned char from, unsigned char to)
{
   int k;
   unsigned char line[20];

   LCDSetAddr (from);
   for (k = 0; k < 20; k++)
      line[k] = LCDgetch ();
   LCDSetAddr (to);
   for (k = 0; k < 20; k++)
      LCDwrite (line[k]);
}

void
LCDScroll ()
{
   int k;

   LCDCopy (0x40, 0x00);        /* copy line 2 to line 1 */
   LCDCopy (0x14, 0x40);        /* copy line 3 to line 2 */
   LCDCopy (0x54, 0x14);        /* copy line 4 to line 3 */
   LCDSetAddr (0x54);
   for (k = 0; k < 20; k++)
      LCDwrite (' ');
}

void
LCDputch (unsigned char c)
{
   static int num_ch_after_set = 0;
   char dummy,
     addr;

   while (isLCDBusy ());
   if (iscntrl (c)) {
      addr = LCD_CMD;
      switch (c) {
      case '\n':
         if (num_ch_after_set == 1) break;
         if (addr <= 0x27)
            LCDSetAddr (addr + 0x40);
         else if (addr <= 0x53)
            LCDSetAddr (addr - 0x2C);
         else {
            LCDScroll ();
            LCDSetAddr (addr);
         }
         break;
      case '\r':               /* carriage return */
         if (addr <= 0x13)
            LCDSetAddr (0x00);
         else if (addr <= 0x27)
            LCDSetAddr (0x14);
         else if (addr <= 0x53)
            LCDSetAddr (0x40);
         else
            LCDSetAddr (0x54);
         break;
      case '\f': /* form feed */
         LCDCommand (0x01); /* clear display */
         LCDCommand (0x02); /* return home */
         break;
      }
      num_ch_after_set++;
   }
   else {
      LCD_DATA = c;
      num_ch_after_set++;

      /* the following dummy read cycle is needed to properly 
       * set the internal mode of the LCD controller
       */
      while (isLCDBusy ());
      dummy = LCD_CMD;

      while (isLCDBusy ());
      if (LCD_CMD == 0x14) {    /* map line 3 */
         num_ch_after_set = 0;
         LCDSetAddr (0x40);     /* to line 2 */
         LCDrow = 2;
      }
      else if (LCD_CMD == 0x40) { /* map line 2 */
         num_ch_after_set = 0;
         LCDSetAddr (0x54);     /* to line 4 */
         LCDrow = 4;
      }
      else if (LCD_CMD == 0x54) { /* map line 4 */
         num_ch_after_set = 0;
         LCDSetAddr (0x14);     /* to line 3 */
      }
      else if (LCD_CMD == 0x00) {
         num_ch_after_set = 0;
         LCDScroll ();          /* content of line 2-4 to 1-3 */
         LCDSetAddr (0x54);     /* to line 4 */
      }
   }
   while (isLCDBusy ());
   addr = LCD_CMD;
}
