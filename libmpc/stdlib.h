/**
 * @file stdlib.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 *    This files contains partial replacements of some Unix system calls 
 * for PowerPC machine, specifically for the CME-555 evaluation board.
 */
/*
 * $Id: stdlib.h,v 2.4 2000/09/28 19:51:42 dulimart Exp $
 *
 * $Log: stdlib.h,v $
 * Revision 2.4  2000/09/28 19:51:42  dulimart
 * Modified comments to doxygen enabled ones
 *
 * Revision 2.3  2000/09/06 22:07:37  dulimart
 * Added declaration of _exit()
 *
 * Revision 2.2  2000/05/23 16:13:43  dulimart
 * Added elapsetime()
 *
 * Revision 2.1  2000/04/25 14:13:23  dulimart
 * Fix editing error
 *
 * Revision 2.0  2000/04/25 13:02:41  dulimart
 * Initial revision
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#ifndef MYSTDLIB_H
#define MYSTDLIB_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Block the caller
 * @param t number of seconds to block
 */
unsigned int sleep (unsigned int t);

/**
 * Block the caller
 * @param t number of microseconds to block
 */
void usleep (unsigned long t);
/**
 * Determine the elapse time
 * @param t pointer to time_t structure
 *    @arg NON-NULL use the structure pointed by t as the reference
 *    for calculating the elapse time
 *    @arg NULL calculate the elapse since the last time the reference
 *    is set
 * @return the elapse time (if t = NULL) or 0.0 (if t points to a 
 * time_t structure)
 */
double elapsetime (time_t *t);

/// exit the program
void _exit (int)
#ifdef __GNUC__
   __attribute__ ((noreturn))
#endif
   ;
#ifdef __cplusplus
}
#endif

#endif
