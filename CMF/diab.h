/* ===========================================================================
	diab.h
	------
    A header file for diab compiled source code

 (C) Copyright 1999 Axiom Manufacturing.  All rights reserved.
 ============================================================================*/

#ifndef D_diab
#define D_diab

/*------------
 CONSTANTS
------------*/

#if 0
typedef unsigned char UNS8;
typedef unsigned short UNS16;
typedef unsigned long UNS32;

typedef unsigned char BOOL;	   /* unsigned 8 bits      */
typedef unsigned int UINT32;   /* unsigned int 32 bits */
typedef int SINT32; 		   /* signed int 32 bits   */
typedef unsigned short UINT16; /* unsigned int 16 bits */
typedef short SINT16; 		   /* signed int 16 bits   */
typedef unsigned char UINT8;   /* unsigned int 8 bits  */
typedef char SINT8;    	       /* signed int 8 bits    */

typedef volatile unsigned char VUNS8;
typedef volatile unsigned short VUNS16;
typedef volatile unsigned long VUNS32;

typedef unsigned char* UNS8P;

#define TRUE 1
#define FALSE 0
#define NULL 0

#define put8(p, x)	((*((volatile UNS8*)(p))) = (x))
#define put16(p, x)	((*((volatile UNS16*)(p))) = (x))
#define put32(p, x)	((*((volatile UNS32*)(p))) = (x))

#define get8(p)		(*((volatile UNS8*)(p)))
#define get16(p)	(*((volatile UNS16*)(p)))
#define get32(p)	(*((volatile UNS32*)(p)))
#endif


#endif // D_diab
