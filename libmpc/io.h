/**
 * @file io.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief A substitute of "stdio.h"
 */
/*
 * $Id: io.h,v 2.6 2000/09/28 19:45:30 dulimart Exp $
 *
 * $Log: io.h,v $
 * Revision 2.6  2000/09/28 19:45:30  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.5  2000/09/26 17:12:22  dulimart
 * Replaced constants STDIN, STDOUT, STDERR with global variables
 * stdin, stdout, stderr
 *
 * Revision 2.4  2000/09/14 20:38:26  dulimart
 * Reorder enumeration type of STDIN, STDOUT, ....
 *
 * Revision 2.3  2000/07/14 22:48:50  dulimart
 * Used FD_SET, FD_ZERO, FD_ISSET, FD_CLR definition from <sys/types.h>
 *
 * Revision 2.2  2000/07/03 19:38:55  dulimart
 * Rename read... to fget..., print... to fput..., added select().
 *
 * Revision 2.1  2000/05/23 16:43:55  dulimart
 * Defined STDIN, STDOUT, STDERR, KEYPAD constants, defined Keypad symbols, changed
 * my_printf / my_scanf format to const
 *
 * Revision 2.0  2000/04/24 21:10:38  dulimart
 * initial revision
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef MY_STDIO_H
#define MY_STDIO_H

#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

/**
 * This is the <stdio.h> replacement for MPC555 environment
 */

/**
 * @enum io::FILE
 * enumeration of all devices available on the MPC555 environment
 */
typedef enum {STDIN, STDOUT, STDERR, RECVER, KEYPAD} FILE;

/** 
 * @var stdin 
 * global standard input 
 */
extern FILE stdin; 
/** 
 * @var stdout 
 * global standard output 
 */
extern FILE stdout;
/** 
 * @var stderr 
 * global standard error 
 */
extern FILE stderr;

//! @see fputc
#define putchar(c) fputc(c,stderr)
//! @see fgetc
#define getchar() fgetc(stdin)
//! @see fputs
#define puts(a) fputs(a,stdout)
//! @see fgets
#define gets(s)    fgets(s,stdin)

//! @see fputi
#define puti(a,b)  fputi(a,b,stdout)
//! @see fputu
#define putu(a,b)  fputu(a,b,stdout)
//! @see fputh
#define puth(a)    fputh(a,stdout)
//! @see fputf
#define putf(a,b)  fputf(a,b,stdout)
//! @see fputfr
#define putfr(a,b) fputfr(a,b,stdout)

//! @see fgeti
#define geti()     fgeti(stdin)
//! @see fgetf
#define getf()     fgetf(stdin)

#ifdef _ARCH_PPC
#ifdef __cplusplus
extern "C" {
#endif
   /// write a single character to a FILE
   void fputc (unsigned char, FILE);
   /// read a single character from a FILE
   char fgetc (FILE);
   /// write a string to a FILE
   int fputs (const char *, FILE);
   /// read a string from a FILE
   int fgets(char*, FILE);
   /** 
    * write an integer to a FILE
    * @param val the value to write
    * @param w minimum width of field to use (in characters)
    * @param f FILE to use
    */
   int fputi (int val, int w, FILE f);
   /** 
    * write an unsigned integer to a FILE
    * @param val the value to write
    * @param w minimum width of field to use (in characters)
    * @param f FILE to use
    */
   int fputu (unsigned int val, int w, FILE f);
   /** 
    * write a integer in hexadecimal format to a FILE
    * @param val the value to write
    * @param f FILE to use
    */
   int fputh (unsigned int val, FILE f);
   /** 
    * write a floating-point number to a FILE
    * @param val the value to write
    * @param d number of decimal places
    * @param f FILE to use
    */
   int fputf (float val, int d, FILE);
   /** 
    * write a fix-point number in fraction format to a FILE
    * @param val the value to write
    * @param b number of bits used for fraction
    * @param f FILE to use
    */
   int fputfr (int val, int b, FILE f);

   // clone of printf()
   int my_printf (const char *,...) 
#ifdef __GNUC__
      __attribute__ ((format(printf, 1, 2)))
#endif
	;
   // clone of fprintf()
   int my_fprintf (FILE, const char *, ...)
#ifdef __GNUC__
      __attribute__ ((format(printf, 2, 3)))
#endif
	;
   // clone of scanf()
   int my_scanf (const char *,...) 
#ifdef __GNUC__
      __attribute__ ((format(scanf, 1, 2)))
#endif
	;
   // clone of fscanf()
   int my_fscanf (FILE, const char *,...) 
#ifdef __GNUC__
      __attribute__ ((format(scanf, 2, 3)))
#endif
	;
   /// read an integer from a FILE
   int fgeti(FILE);
   /// read an floating-point from a FILE
   float fgetf(FILE);
   /// read an integer from a string
   int readints(const char*, int*);
   /** 
    * clone of select(): check for a number of read file descriptors
    * @param n highest id of file descriptor
    * @param s set of file descriptors
    * @param t timeval structure used for specifying timeout
    *    @arg NULL blocking select
    *    @arg NON-NULL non-blocking select wait for at least
    *       the amount of time specified in the structure
    */
   int select (int n, fd_set* s, struct timeval* t);
#ifdef __cplusplus
}
#endif
#endif
#endif
