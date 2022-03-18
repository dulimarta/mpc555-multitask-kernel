/*
 * $Id: strstream.h,v 2.1 2000/09/28 19:51:42 dulimart Exp $
 *
 * Author: Hans Dulimarta [dulimart@computer.org]
 * Description:
 *    Partial replacement of strstream class
 *
 * $Log: strstream.h,v $
 * Revision 2.1  2000/09/28 19:51:42  dulimart
 * Modified comments to doxygen enabled ones
 *
 * Revision 2.0  2000/04/25 13:44:34  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef __STRSTREAM_H
#define __STRSTREAM_H

#define STRSTREAM_VALID 0
#define STRSTREAM_EOS   1
#define STRSTREAM_EOF   2

/**
 * istrstream is a replacement of input string stream in
 * MPC555 environment
 */
class istrstream /* : public istream */ {
   public:
   /// initialize the stream from an existing string
   istrstream (const char *);

   /// destructor
   ~istrstream();

   /// read an integer from the string stream
   istrstream& operator>> (int&);

   /// read a floating-point from the string stream
   istrstream& operator>> (float&);

   /// read a string from the string stream
   istrstream& operator>> (char*);

   /** 
    * Read a complete line string stream
    * @param buff store the line in this parameter
    * @param delim use this delimiter to mark "end of line"
    */
   istrstream& getline (char* buff, unsigned int delim);

   /** 
    * Read a complete line string stream
    * @param buff store the line in this parameter
    * @param delim use this delimiter to mark "end of line"
    */
   istrstream& getline (char* buff, int delim);
   //bool operator!();

   /// conversion operator
   operator void*();
   protected:
      /// internal state used in for EOF operation
      char state; 

      /// pointer to the string connected to this stream
      char *str;

      /// internal index on the string
      int p;

      /// length of the string
      int len;
};
#endif
