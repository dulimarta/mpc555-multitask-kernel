/*
 * $Id: strstream.cc,v 2.2 2000/10/02 20:26:03 dulimart Exp $
 *
 * Author: Hans Dulimarta   [dulimart@computer.org]
 * Description:
 *    Partial replacement of strstream on MPC555.
 *
 * $Log: strstream.cc,v $
 * Revision 2.2  2000/10/02 20:26:03  dulimart
 * Disabled getline
 *
 * Revision 2.1  2000/04/25 14:14:11  dulimart
 * Change order of inclusion between stream.h and strstream.h
 *
 * Revision 2.0  2000/04/25 13:44:50  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000, Hans Dulimarta, Ph.D.
 */

#include "strstream.h"
#ifdef DEBUG
#include "stream.h"
#endif
#include <ctype.h>

istrstream::istrstream (const char *s) { 
   len = strlen(s);
   str = new char [len+1];
   strcpy (str, s);
   p = 0;
   state = STRSTREAM_VALID;
}

istrstream::~istrstream () { 
   delete [] str;
}

#if 0
istrstream& istrstream::operator>> (float&) { 
   cout << __PRETTY_FUNCTION__ << endl;
   return *this;
}
#endif

istrstream& istrstream::operator>> (int& d) { 
   /* cout << __PRETTY_FUNCTION__ << endl; */
   d = readints (str, &p);
   return *this;
}

istrstream& istrstream::operator>> (char *w) { 
   int k;

   //cout << __PRETTY_FUNCTION__ << endl;
   //cout << "String is " << str+p << endl;
   while (p < len) {
      if (!isspace (str[p]))
         break;
      p++;
   }
   for (k=0; p<len; k++, p++) {
      w[k] = str[p];
      if (isspace (str[p])) {
         break;
      }
   }
   w[k] = 0;
   //cout << "Result is " << w << endl;
   return *this;
}

#if 0
istrstream& istrstream::getline (char*, unsigned int l) { 
   cout << __PRETTY_FUNCTION__ << " get " << l << " bytes " << endl;
   return *this;
}

istrstream& istrstream::getline (char*, int) { 
   return *this;
}
#endif

istrstream::operator void* (void) { 
   /**
   cout << __PRETTY_FUNCTION__ << " : " << str+p << endl;
   cout << "Index = " << p << " length = " << len << endl;
   **/
   switch (state) {
   case STRSTREAM_VALID:
      if (len == 0) state = STRSTREAM_EOF;
      else if (p == len) state = STRSTREAM_EOS;
      break;
   case STRSTREAM_EOS:
      state = STRSTREAM_EOF;
      break;
   }
   return (void *) (state == STRSTREAM_EOF ? 0 : -1);
}
