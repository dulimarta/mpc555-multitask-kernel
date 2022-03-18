/**
 * @file  ctors.c
 *
 * @author Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * @brief Global constructors and destructors
 *
 *    The GNU compiler generates a table of pointer to static
 * constructors and destructors. Each entry of this table contains
 * the address of constructor to be called at the beginning of a program
 * and the address of destructor to be called at the termination of a
 * program.
 */

/*
 * $Id: ctors.c,v 2.2 2000/09/28 19:45:30 dulimart Exp $
 *
 * $Log: ctors.c,v $
 * Revision 2.2  2000/09/28 19:45:30  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.1  2000/07/14 22:41:43  dulimart
 * Removed debugging message from pure_virtual
 *
 * Revision 2.0  2000/04/24 20:22:50  dulimart
 * Add "atexit()" and "__pure_virtual"
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include <stdlib.h>

/// pointer to function
typedef void (*func_ptr) (void);

/// Start of array of pointers to constructor
extern func_ptr __CTOR_LIST__[];
/// End of array of pointers to constructor
extern func_ptr __CTOR_END__[];

/// Start of array of pointers to destructor
extern func_ptr __DTOR_LIST__[];
/// End of array of pointers to destructor
extern func_ptr __DTOR_END__[];

/// Call global destructors via their pointer
void __do_global_dtors () {
   func_ptr *s = __DTOR_LIST__;
   func_ptr *t = __DTOR_END__;

   for (; s != t; s++) if (s) (*s)();
}

/// Call global constructors via their pointer
void __do_global_ctors () {
   func_ptr *s = __CTOR_LIST__;
   func_ptr *t = __CTOR_END__;

   for (; s != t; s++) if (s) (*s)();
   atexit (__do_global_dtors);
}

/// defined to resolve undefined external reference
void __pure_virtual () 
{ 
}
