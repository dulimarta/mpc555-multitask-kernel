/**
 * @file memory.h
 *
 * @author Hans Dulimarta dulimart@computer.org
 *
 * This file defines the memory management routines 
 * in the MPC555 architecture.
 */
/* 
 * $Id: memory.h,v 2.4 2000/12/06 21:47:49 dulimart Exp $
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif
   /// Initialize heap
   void heap_init ();

   /// Allocate a memory segment
   void* malloc (unsigned int);

   /// Allocate a memory segment
   void* calloc (unsigned int, unsigned int);

   /// Free previously allocated memory
   void free (void *);

   /// List all blocks in the heap
   void ListAll();

   /// obtain the amount of available heap (in KBytes)
   unsigned int getavailmem();
#ifdef __cplusplus
}
#endif
#endif
