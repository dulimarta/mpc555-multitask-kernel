/**
 * @file sync.S
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief Low level storage synchronization routines
 */
/*
 * $Id: sync.S,v 2.1 2000/12/06 20:31:13 dulimart Exp $
 *
 * $Log: sync.S,v $
 * Revision 2.1  2000/12/06 20:31:13  dulimart
 * initial checkin
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

          .global lock

#
#     void lock (int * s);
#
lock:
          li        %r4,1
loop:     
          lwarx     %r5,0,%r3       /* load word (*w) */
          cmpwi     %r5,0
          bne-      loop
          stwcx.    %r4,0,%r3       /* store 'val' to *w */
          bne-      loop            /* no branch prediction */
          isync
          blr

          .global unlock
#
#     void unlock (int * s);
#
unlock:
          sync
          li        %r5,0
          stw       %r5,0(%r3)
          blr

######################################################################
#
#     void test_and_set (int *w, int val);
#
#     atomically load *w and set it to a new value 'val' if
#     the loaded value is zero.
#

          .end
