/*
 * $Id: stream.h,v 2.5 2000/09/28 19:50:14 dulimart Exp $
 *
 * Author: Hans Dulimarta  [dulimart@computer.org]
 * Description:
 *   This is a partial replacement of C++ stream on the MPC555 chip.
 * The interface of istream and ostream as defined here does not comply
 * with the standard C++ stream class. Only the methods used in the wall
 * climber code are redefined here.
 *
 * $Log: stream.h,v $
 * Revision 2.5  2000/09/28 19:50:14  dulimart
 * Added stream output for general pointers
 *
 * Revision 2.4  2000/09/26 17:29:30  dulimart
 * Added doxygen enable comment lines
 *
 * Revision 2.3  2000/09/06 22:09:50  dulimart
 * Modified ostream and istream ctors from inline to ordinary function
 *
 * Revision 2.2  2000/06/27 15:53:15  dulimart
 * Use device identifier (similar to "file handler number") in
 * constructor.
 *
 * Revision 2.1  2000/06/22 14:07:32  dulimart
 * Created class stream, derived istream and ostream from stream.
 *
 * Revision 2.0  2000/04/25 13:41:56  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef _COMPAT_STREAM_H
#define _COMPAT_STREAM_H

#define _STREAM_COMPAT
#define _IOSTREAM_H

#include "io.h"

class istream;
class ostream;

typedef istream& (*__iManip) (istream&);
typedef ostream& (*__oManip) (ostream&);

/**
 * This class is a replacement of C++ stream
 */
class stream {
   public:
      /// create an input stream by connecting it to an open file
      stream (FILE *d) { dev = d; }

      /// get the file number of the stream
      FILE fileno () const { return *dev; }
   private:

      /// pointer to the open FILE
      FILE *dev;
};

/**
 * This class is a replacement of C++ output stream
 */
class ostream : public stream {
   public:
      //ostream() { dev = STDOUT; }
      //
      /// create an output stream by connecting it to an open file
      ostream(FILE*);
       
      /// this method has no effect; provided for compatibility
      ostream& flush() { return *this; }

      /// send a character to the output stream
      virtual ostream& operator<< (char);

      /// send a character to the output stream
      virtual ostream& operator<< (unsigned char c) { 
         return *this << (char) c; 
      }

      /// send a character to the output stream
      virtual ostream& operator<< (signed char c) {return *this << (char) c; } 

      /// send a string to the output stream
      virtual ostream& operator<< (const char *);

      /// send a string to the output stream
      virtual ostream& operator<< (const unsigned char* s) {
         return *this << (const char *) s;
      }

      /// send a string to the output stream
      virtual ostream& operator<< (const signed char* s) {
         return *this << (const char *) s;
      }

      /// send a short integer to the output stream
      virtual ostream& operator<< (short n) { return *this << (int) n; }

      /// send a unsigned short integer to the output stream
      virtual ostream& operator<< (unsigned short n) { 
         return *this << (unsigned int) n; 
      }

      /// send a signed integer to the output stream
      virtual ostream& operator<< (int);

      /// send a unsigned integer to the output stream
      virtual ostream& operator<< (unsigned int);

      /// send a signed long integer to the output stream
      virtual ostream& operator<< (long);

      /// send a unsigned long integer to the output stream
      virtual ostream& operator<< (unsigned long);

      /// send a floating number to the output stream
      virtual ostream& operator<< (float f) {
         return *this << (double) f;
      }

      /// send a general pointer to the output stream
      virtual ostream& operator<< (void *);

      /// send a double floating number to the output stream
      virtual ostream& operator<< (double);

      /// execute an output stream manipulator
      ostream& operator<< (__oManip f) { return (*f)(*this); }
};

/**
 * This class is a replacement of C++ input stream
 */
class istream : public stream {
   public:
      /// create an input stream by connecting it to an open file
      istream(FILE*);
      //virtual bool eof();

      /// get a floting number from the input stream
      virtual istream& operator>> (float&);

      /// get an integer number from the input stream
      virtual istream& operator>> (int&);

      /// get a character from the input stream
      virtual istream& operator>> (char&);

      /// get a string from the input stream
      virtual istream& operator>> (char*);

      //virtual istream& putback (char&);
      //virtual istream& ignore (int, char);
      //istream& getline (char*, unsigned int, char term = '\r');
      //istream& getline (char*, int);

      /// execute an input stream manipulator
      istream& operator>> (__iManip f) { return (*f)(*this); }

      //bool operator!();
   private:
      void prepend (int, int&);
      char last_char;
      char char_valid;
};

extern ostream cout;
extern ostream cerr;
extern ostream& endl (ostream&);
extern ostream& flush (ostream&);

extern istream cin;
extern istream& ws (istream&);

#endif
