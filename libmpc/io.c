/*
 * $Id: io.c,v 2.11 2001/02/19 17:11:04 dulimart Exp $
 *
 * File: io.c
 *
 * Author: Hans Dulimarta, Ph.D.
 *
 * Description: 
 *   This file contains the scanf() and printf() replacement for MPC555
 * embedded environment.
 *
 * $Log: io.c,v $
 * Revision 2.11  2001/02/19 17:11:04  dulimart
 * Modified fgeti and fgetf to ignore the numeric value when
 * invalid characters are read.
 *
 * Revision 2.10  2000/11/30 18:24:50  dulimart
 * Ignore unknow format
 *
 * Revision 2.9  2000/11/08 20:08:03  dulimart
 * Added non-recursive version of printbase().
 *
 * Revision 2.8  2000/09/28 19:45:30  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.7  2000/09/26 17:11:45  dulimart
 * Defined global variables stdin, stdout, and stderr
 *
 * Revision 2.6  2000/08/31 15:44:37  dulimart
 * Changed IOCB to const static. Excluded output to LCD.
 *
 * Revision 2.5  2000/07/14 22:42:36  dulimart
 * Implemented select with timeout
 *
 * Revision 2.4  2000/07/13 18:27:20  dulimart
 * Removed my_putc, renamed GetSByte to RecvByte,
 * replaced putchar with fputc, declared IOCB as static.
 *
 * Revision 2.3  2000/06/30 12:39:18  dulimart
 * Renamed function from printXYZ to fputXYZ, readXYZ to fgetXYZ.
 * Split low-level I/O routine into iodev.c.
 * Wrote select() function.
 *
 * Revision 2.2  2000/06/09 13:33:01  dulimart
 * Renamed IRQ4Handler to KeyPadHandler
 *
 * Revision 2.1  2000/05/23 16:41:39  dulimart
 * Added OUTDEV/INDEV global variables, changed 'double' to 'float', replaced 'modf' call, added putc,
 * and redefine keypad mapping.
 *
 * Revision 2.0  2000/04/24 21:02:10  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include <ctype.h>
#include <math.h>
#include "mpc555.h"
#include "io.h"
#include "iodev.h"
#include "lcd.h"
#include "qsm.h"
#include "usiu.h"

#define KP_BUFFSIZE 16
#define RX_BUFFSIZE 16

FILE stdin = STDIN, stdout = STDOUT, stderr = STDERR;

/* print an unsigned value in base 'b'. 
 * Base is assumed to have a maximum value of 16. */
#if 0
int
printbase (unsigned int n, unsigned int b, int w, FILE fdev)
{
   unsigned char d;
   unsigned int nc;
   if (n > 0) {
      nc = printbase (n / b, b, w - 1, fdev);
      d = n % b;
      if (d < 10)
         fputc (d + '0', fdev);
      else
         fputc (d - 10 + 'A', fdev);
      return nc + 1;
   }
   else {
      for (; w > 0; w--)
         fputc (' ', fdev);
   }
   return 0;
}

#else

int printbase (unsigned int n, unsigned int b, int w, FILE fdev)
{
   /*
    * Number of digits needed to print 'n' will be maximum
    * when the base 'b' is binary. Hence, for 32-bit integers
    * array 'digits' has to be reserved with 32 elements.
    */
   char digits[32];
   unsigned int d;
   int len, k;

   len = 0;
   d = n;
   while (d > 0) {
      d /= b;
      len++;
   }
   for (k = len - 1; k >= 0; k--) {
      d = n % b;
      if (d < 10)
         digits[k] = d + '0';
      else
         digits[k] = d - 10 + 'A';
      n /= b;
   }
   for (k=0; k<w-len; k++) fputc (' ', fdev);
   for (k=0; k<len; k++) fputc (digits[k], fdev);
   return w > len ? w : len;
}
#endif

int
fputh (unsigned int a, FILE fdev)
{
   fputs ("0x", fdev);
   if (a > 0)
      return printbase (a, 16, -1, fdev) + 2;
   else {
      fputc ((unsigned char) '0', fdev);
      return 3;
   }
}

int
fputu (unsigned int a, int w, FILE fdev)
{
   if (a == 0) {
      for (w--; w > 0; w--)
         fputc (' ', fdev);
      fputc ((unsigned char) '0', fdev);
      return 1;
   }
   else
      return printbase (a, 10, w, fdev);
}

int
fputi (int a, int w, FILE fdev)
{
   int nc;
   nc = 0;
   if (a < 0) {
      fputc ((unsigned char) '-', fdev);
      w--;
      a *= -1;
      nc++;
   }
   return fputu ((unsigned) a, w, fdev) + nc;
}

int
fputf (float a, int d, FILE fdev)
{
   int ipart;
   float fpart;
   int nc;
   nc = 0;
   if (a < 0.0) {
      fputc ((unsigned char) '-', fdev);
      nc++;
      a *= -1.0;
   }
   /* fpart = modf (a, &ipart); */
   ipart = (int) a;
   fpart = a - (int) a;
   nc += fputi (ipart, -1, fdev);
   fputc ((unsigned char) '.', fdev);
   nc++;
   for (; d > 0; d--) {
      fpart *= 10.0;
      //fpart = modf (fpart, &ipart);
      ipart = (int) fpart;
      fpart = fpart - (int) fpart;
      fputc (ipart + '0', fdev);
      nc++;
   }
   nc += fputi ((int) fpart, -1, fdev);
   return nc;
}

int
fputfr (int a, int n, FILE fdev)
{
   int w,
     nc;
   nc = 0;
   if (a < 0) {
      fputc ('-', fdev);
      nc++;
      a *= -1;
   }
   nc += fputi (a >> n, -1, fdev);
   fputc ((unsigned char) '.', fdev);
   nc++;
   w = 1 << n;
   while (a) {
      a %= w;                   /* get the "fraction" */
      a *= 10;
      if (a >= w)
         fputc ((a >> n) + '0', fdev);

      else
         fputc ((unsigned char) '0', fdev);
      nc++;
   }
   return nc;
}

#ifdef _ARCH_PPC
void
fputc (unsigned char ch, FILE fdev)
{
   static int trm_col = 1;

   switch (fdev) {
   case STDOUT:
#if 0
      if (ch == '\n')
         LCDputch ('\r');
      LCDputch (ch);
      break;
#endif
   case STDERR:
      if (ch == '\n' || ch == '\f') {
         SendByte ('\r');
         trm_col = 0;
      }
      if (trm_col == 80) {
         trm_col = 0;
         SendByte ('\r');
         SendByte ('\n');
      }
      SendByte (ch);
      trm_col++;
      break;
   default: /* this label is needed to remover compiler warning message */
   }
}
#endif

char
fgetc (FILE fdev)
{
   char ch = 0;

   switch (fdev) {
   case STDIN:
      ch = RecvByte ();
      break;
   case KEYPAD:
      ch = KPGetKey ();
      break;
   case RECVER:
      ch = RXGetKey ();
      break;
   default: /* this label is needed to remover compiler warning message */
   }
   return ch;
}

int
fputs (const char *sptr, FILE fdev)
{
   int nc = 0;
   while (*sptr) {
      fputc ((unsigned char) *sptr, fdev);
      ++sptr;
      nc++;
   }
   return nc;
}

int
fgets (char *s, FILE fdev)
{
   char *p,
     ch;
   int nc = 0;

   /* skip space */
   do {
      ch = fgetc (fdev);
      putchar (ch);
      nc++;
   } while (isspace (ch));
   p = s;

   do {
      *p = ch;
      ch = fgetc (fdev);
      putchar (ch);
      p++;
   } while (!isspace (ch));
   *p = 0;                      /* append null character to the end of string */
   nc += p - s;
   return nc;
}

int
readints (const char *s, int *p)
{
   unsigned char ch,
     neg = 0;
   int val = 0;

   while (1) {
      ch = s[*p];
      if (ch == '+' || ch == '-' || isdigit (ch) || ch == 0)
         break;
      (*p)++;
   }
   if (ch == 0)
      return 0;
   neg = 0;
   if (ch == '-')
      neg = 1;
   else if (ch != '+')
      val = ch - '0';
   while (1) {
      (*p)++;
      ch = s[*p];
      if (isdigit (ch))
         val = 10 * val + ch - '0';
      else
         break;
   }
   return neg ? -val : val;
}

int
fgeti (FILE fdev)
{
   unsigned char ch;
   short sign;
   int val = 0;

   sign = 1;
   while (1) {
      ch = fgetc (fdev);
      putchar (ch);
      if (ch == '+' || ch == '-' || isdigit (ch))
         break;
   }
   if (ch == '-') {
      sign = -1;
      val = 0;
   }
   else if (isdigit (ch))
      val = ch - '0';
   while (1) {
      ch = fgetc (fdev);
      putchar (ch);
      if (ch == '\r' || ch == '\n') break;
      if (isdigit (ch))
         val = 10 * val + ch - '0';
      else
         val = 0;
   }
   return sign * val;
}

float
fgetf (FILE fdev)
{
   unsigned char ch;
   int intval;
   float fracval,
     place,
     sign;

   while (1) {
      ch = fgetc (fdev);
      putchar (ch);
      if (!isspace(ch)) break;
   }
   if (ch != '+' && ch != '-' && ch != '.' && !isdigit (ch))
      return 0.0;
   intval = 0;
   sign = 1.0;
   if (ch == '-') {
      sign = -1.0;
      ch = fgetc (fdev);
      putchar (ch);
   }
   while (isdigit (ch)) {
      intval *= 10;
      intval += ch - '0';
      ch = fgetc (fdev);
      putchar (ch);
   }
   fracval = 0.0;
   if (ch == '.') {
      place = .1;
      while (1) {
         ch = fgetc (fdev);
         putchar (ch);
         if (!isdigit (ch))
            break;
         fracval += (ch - '0') * place;
         place *= .1;
      }
   }
   if (ch == '\r' || ch == '\n')
      return sign * ((float) intval + fracval);
   else
      return 0.0;
}

int
my_vscanf (FILE dev, const char *fmt, va_list ap)
{
   while (*fmt) {
      if (*fmt == '%') {
         fmt++;
         switch (*fmt) {
         case 'c':             /* read char value */
            *(va_arg (ap, char *)) = fgetc (dev);
            break;
         case 'd':             /* read integer value */
            *(va_arg (ap, int *)) = fgeti (dev);
            break;
         case 'f':             /* read integer value */
            *(va_arg (ap, float *)) = fgetf (dev);
            break;
         case 's':             /* read string value */
            fgets (va_arg (ap, char *), dev);
            break;
         }
      }
      fmt++;
   }
   //va_end (ap);
   return 0;
}

int
my_scanf (const char *fmt, ...)
{
   int ret;
   va_list ap;

   va_start (ap, fmt);
   ret = my_vscanf (stdin, fmt, ap);
   va_end (ap);
   return ret;
}

int
my_fscanf (FILE dev, const char *fmt, ...)
{
   int ret;
   va_list ap;

   va_start (ap, fmt);
   ret = my_vscanf (dev, fmt, ap);
   va_end (ap);
   return ret;
}

int
my_vprintf (FILE fdev, const char *fmt, va_list ap)
{
   int prec,
     width;
   int nc;

   nc = 0;                      /* number of character printed */
   while (*fmt) {
      if (*fmt == '%') {
         prec = -1;
         width = -1;
         fmt++;
         if (isdigit (*fmt)) {  /* handle width */
            width = 0;

            do {
               width = width * 10 + *fmt - '0';
               fmt++;
            } while (isdigit (*fmt));
         }
         if (*fmt == '.') {     /* handle precision */
            fmt++;              /* digit has to follow the '.' */
            prec = 0;
            while (isdigit (*fmt)) {
               prec = prec * 10 + *fmt - '0';
               fmt++;
            }
         }
         switch (*fmt) {
         case '%':
            fputc ('%', fdev);
            nc++;
            break;
         case 'c':
            fputc (va_arg (ap, int), fdev);
            nc++;
            break;
         case 'd':
            if (prec >= 0)
               nc += fputfr (va_arg (ap, int), prec, fdev);
            else
               nc += fputi (va_arg (ap, int), width, fdev);
            break;
         case 'e':
         case 'f':
         case 'g':
            nc += fputf (va_arg (ap, double), prec >= 0 ? prec : 2, fdev);
            break;
         case 'l':             /* ignore 'long' modifier */
            break;
         case 's':
            nc += fputs (va_arg (ap, char *), fdev);
            break;
         case 'u':
            nc += fputu (va_arg (ap, unsigned int), width, fdev);
            break;
         case 'p':
         case 'x':
         case 'X':
            nc += fputh (va_arg (ap, int), fdev);
            break;
         default:
            /*fputs ("Unknown format: ", fdev);
            fputc (*fmt, fdev);*/
         }
      }
      else {
         fputc (*fmt, fdev);
         nc++;
      }
      fmt++;
   }
   //va_end (ap);
   return nc;
}

int
my_printf (const char *fmt, ...)
{
   int ret;
   va_list ap;

   va_start (ap, fmt);
   ret = my_vprintf (stdout, fmt, ap);
   va_end (ap);
   return ret;
}

int
my_fprintf (FILE fdev, const char *fmt, ...)
{
   int ret = 0;
   va_list ap;

   va_start (ap, fmt);
   ret = my_vprintf (fdev, fmt, ap);
   va_end (ap);
   return ret;
}

/**
 * @struct IOCB_t
 * I/O control block used by <tt>select()</tt>
 */
typedef struct
{
   /**
    * pointer to function for checking the availability of any 
    * character on an input device
    */
   int (*in_ready) (); 
}
IOCB_t;

/*
 * WARNING: take precaution when reordering the function names in
 * the IOCB array. The order has to match that of the FILE enum
 * type
 */
static const IOCB_t IOCB[5] = {{SerialReady}, {0}, {0}, {RXReady}, {KPReady}};

int
select (int n, fd_set * r_fds, struct timeval *t_out)
{
   int fd, num_rd;
   fd_set tmp_set;
   int (*f_ptr) ();
   struct timeval t_end, t_now;

   if (t_out) {
      gettimeofday (&t_end, 0);
      t_end.tv_sec += t_out->tv_sec;
      t_end.tv_usec += t_out->tv_usec;
   }

   num_rd = 0;
   bcopy ((void *)r_fds, (void *)&tmp_set, sizeof(fd_set));
   FD_ZERO (r_fds);
   while (1) {
      for (fd = 0; fd < n; fd++) {
         /* check every possible file descriptor */
         if (FD_ISSET (fd, &tmp_set)) {
            /* If this descriptor is to be checked further,
             * get the inquiry function address
             */
            f_ptr = IOCB[fd].in_ready;
            if (f_ptr) {
               if (f_ptr ()) {
                  num_rd++;
                  FD_SET (fd, r_fds);
               }
            }
         }
      }
      if (t_out == 0) { /* blocking select */
         if (num_rd != 0)
            break;
      }
      else { /* non-blocking select */
         if (t_out->tv_sec == 0 && t_out->tv_usec == 0) {
            break;
         }
         gettimeofday (&t_now, 0);
         if (t_now.tv_sec > t_end.tv_sec ||
               (t_now.tv_sec == t_end.tv_sec && 
                t_now.tv_usec > t_now.tv_usec)) {
            break;
         }
      }
   }
   return num_rd;
}
