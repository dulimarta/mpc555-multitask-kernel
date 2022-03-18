/*
 * $Id: stream.cc,v 2.4 2000/09/28 19:50:14 dulimart Exp $
 *
 * Author: Hans Dulimarta  [dulimart@computer.org]
 * Description:
 *   This is a partial replacement of C++ stream on the MPC555 chip.
 * The interface of istream and ostream as defined here does not comply
 * with the standard C++ stream class. Only the methods used in the wall
 * climber code are redefined here.
 *
 * $Log: stream.cc,v $
 * Revision 2.4  2000/09/28 19:50:14  dulimart
 * Added stream output for general pointers
 *
 * Revision 2.3  2000/09/26 17:30:52  dulimart
 * Replaced FILE with FILE* to enable"I/O" redirection via
 * global stdin, stdout, and stderr define in io.c
 *
 * Revision 2.2  2000/09/06 22:09:50  dulimart
 * Modified ostream and istream ctors from inline to ordinary function
 *
 * Revision 2.1  2000/06/27 16:24:26  dulimart
 * Adapted new function names from io.h.
 *
 * Revision 2.0  2000/04/25 13:42:19  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include <ctype.h>
#include "stream.h"

istream cin(&stdin);
ostream cout(&stdout);
ostream cerr(&stderr);

ostream::ostream (FILE *d) : stream (d)
{
}

ostream& ostream::operator<< (char c) { 
   fputc (c, fileno());
   return *this; 
}

ostream& ostream::operator<< (const char* s) { 
   fputs (s, fileno());
   return *this; 
}

ostream& ostream::operator<< (int d) { 
   fputi (d, -1, fileno()); 
   return *this; 
}

ostream& ostream::operator<< (unsigned int d) { 
   fputu (d, -1, fileno()); 
   return *this; 
}

ostream& ostream::operator<< (long d) { 
   fputi ((int) d, -1, fileno()); 
   return *this; 
}

ostream& ostream::operator<< (unsigned long d) { 
   fputu ((unsigned int) d, -1, fileno()); 
   return *this; 
}

ostream& ostream::operator<< (double d) { 
   fputf (d, -1, fileno());
   return *this; 
}

ostream& ostream::operator<< (void *p)
{
   fputh ((int)p, fileno());
   return *this;
}

/*-------------------------------------------------------------------*/
istream::istream (FILE *d) : stream (d)
{
   char_valid = 0;
}

istream& istream::operator>> (char& ch) { 
   if (char_valid) {
      ch = last_char;
      char_valid = 0;
   }
   else
      ch = fgetc(fileno());
   fputc (ch, fileno());
   return *this; 
}

istream& istream::operator>> (char* s) { 
   if (char_valid) {
      *s = last_char;
      char_valid = 0;
      fgets (s+1, fileno());
   }
   else
      fgets (s, fileno());
   return *this;
}

void istream::prepend (int d, int &n) {
   int v;

   v = n;
   while (v) {
      d *= 10;
      v /= 10;
   }
   n += d;
}

istream& istream::operator>> (int& d) { 
   d = fgeti(fileno());
   if (char_valid) {
      char_valid = 0;
      if (isdigit(last_char))
         prepend (last_char - '0', d);
   }
   return *this; 
}

istream& istream::operator>> (float &f) { 
   f = fgetf(fileno());
   return *this; 
}

#if 0
bool istream::eof () { 
   return 0; 
}

istream& istream::getline (char *s, unsigned int p, char term) { 
   unsigned int k;
   char ch;

   if (p == 0) return *this;
   p--;
   k = 0;
   if (char_valid) {
      *s = last_char;
      char_valid = 0;
      k = 1;
   }
   for (; k<p; k++) {
      ch = fgetc(fileno());
      fputc (ch, STDERR);
      if (ch == term) break;
      *(s+k) = ch;
   }
   *(s+k) = 0; /* Append NULL */
   return *this;
}

bool istream::operator! () {
   return 0;
}

istream& istream::ignore (int, char) { return *this; }
istream& istream::putback (char& ch) { 
   last_char = ch;
   char_valid = 1;
   return *this; 
}

ostream& flush (ostream& s) { return s; }

istream& ws (istream& s) {
   char ch;

   ch = ' ';
   while (isspace(ch))
      ch = fgetc(s.fileno());
   s.putback (ch);
   return s;
}
#endif

ostream& endl (ostream& s) { 
   fputc ('\n', s.fileno());
   return s; 
}
