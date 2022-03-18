/*
 * $Id: mem.cc,v 2.0 2000/04/24 21:13:39 dulimart Exp $
 *
 * Filename: mem.cc
 * Author: Hans Dulimarta  [dulimart@computer.org]
 *
 * Description:
 *    This file contains replacement functions for dynamic storage 
 * allocation in C++ run-time environment.
 *
 * $Log: mem.cc,v $
 * Revision 2.0  2000/04/24 21:13:39  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#include "io.h"
#include "memory.h"

typedef unsigned int size_t;

void* operator new (size_t s) { 
   //my_printf ("%s\nAllocate %d bytes\n", __PRETTY_FUNCTION__, s);
   return malloc (s);
}
void* operator new (size_t s, void *p) { 
   //my_printf ("%s\nAllocate %d bytes at %p\n", __PRETTY_FUNCTION__, s, p);
   return p;
}

void* operator new[] (size_t s) {
   //my_printf ("%s\nAllocate %d bytes\n", __PRETTY_FUNCTION__, s);
   return malloc (s);
}

void operator delete (void *p) {
   free (p);
}

void operator delete [] (void *p) {
   //my_printf ("%s deallocate %p\n", __PRETTY_FUNCTION__, p);
   free (p);
}
