/*
 * $Id: memory.c,v 2.5 2000/12/06 21:47:49 dulimart Exp $
 *
 * Filename: memory.c
 *
 * Author: Hans Dulimarta  [dulimart@computer.org]
 *
 * Description:
 *    The functions in this file are part of a memory manager. The storage
 * allocation uses boundary tags at the beginning and end of a memory
 * block. Free blocks are connected in a doubly linked-list.
 * 
 * Free blocks contain the following four fields: 
 *    MT (My Tag)    : 1-bit tag for this free block
 *    LT (Lower Tag) : 1-bit tag for allocated block before this one
 *    SIZE           : 30-bit length (in words not in bytes)
 *    PREV, NEXT     : 32-bit links to previous and next free blocks
 *    dd             : don't care
 * organized as follows:
 *
 *    +--+--+------------------------------+
 *    |MT|LT|            SIZE              |
 *    +--+--+------------------------------+
 *    |               NEXT                 |
 *    +------------------------------------+
 *    |               PREV                 |
 *    +------------------------------------+
 *    :                                    :
 *    :                                    :
 *    :                                    :
 *    +------------------------------------+
 *    |MT|dd|            SIZE              |
 *    +--+--+------------------------------+
 *
 * Therefore, minimum allocation is 4 words!
 *
 *
 * Allocated blocks contain the following fields:
 *    MT (My Tag) : 1-bit tag for this allocated block
 *    SIZE        : 30-bit length (in words not in bytes)
 *    dd          : don't care
 * organized as follows:
 *
 *    +--+--+------------------------------+
 *    |MT|dd|            SIZE              |
 *    +--+--+------------------------------+
 *    :                                    :
 *    :                                    :
 *    :                                    :
 *    :                                    :
 *    :                                    :
 *    :                                    :
 *    +------------------------------------+
 *
 * $Log: memory.c,v $
 * Revision 2.5  2000/12/06 21:47:49  dulimart
 * Fixed calculation of avail_mem in malloc, unit is in words not in bytes.
 *
 * Revision 2.4  2000/11/17 19:34:30  dulimart
 * Added getmemavail()
 *
 * Revision 2.3  2000/11/08 20:07:16  dulimart
 * Added calloc().
 *
 * Revision 2.2  2000/09/26 17:13:42  dulimart
 * Added some lines for checking highest address and larget allocation block.
 *
 * Revision 2.1  2000/09/06 22:15:37  dulimart
 * Modified staticly defined HEAP to linking-time define names HEAP_START and HEAP_END
 *
 * Revision 2.0  2000/07/14 13:22:40  dulimart
 * Exclude ListAll() from normal operation flow.
 *
 * Revision 1.3  2000/07/03 19:17:32  dulimart
 * Capped debugging message within #ifdef DEBUG
 *
 * Revision 1.2  2000/06/30 13:05:05  dulimart
 * Removed commented main function
 *
 * Revision 1.1  2000/04/24 21:14:54  dulimart
 * Initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#include "memory.h"
#ifdef DEBUG
#include "io.h"
#include "qadc.h"
#endif

#define MIN_ALLOC 4

#define SIZE(p) (*(p) & 0x3FFFFFFF)
#define NEXT(p) ((unsigned int *) *((p)+1))
#define PREV(p) ((unsigned int *) *((p)+2))

#define SetTAG(p) *(p) |= 0x80000000
#define SetLowerTAG(p) *(p) |= 0x40000000
#define ClearTAG(p) *(p) &= 0x7FFFFFFF
#define ClearLowerTAG(p) *(p) &= 0xBFFFFFFF
#define IsFree(p) ((*(p) & 0x80000000) == 0)
#define IsLowerFree(p) ((*(p) & 0x40000000) == 0)

extern unsigned int HEAP_START;
extern unsigned int HEAP_END;
static unsigned int *AVAIL;
static unsigned int *HEAP;
static unsigned int *highest_addr = 0;
static unsigned int largest_block = 0;
static unsigned int avail_mem = 0;

void SetSIZE (unsigned int* p, unsigned int s) {
   *p &= 0xC0000000; 
   *p |= s & 0x3FFFFFFF;
}

#if 1
#define SetSIZE2(p,s) { SetSIZE(p,s); SetSIZE((p)+(s)-1,s); }
#else
void SetSIZE2(unsigned int *p, unsigned int s) 
{ 
   unsigned int *q;

   my_printf ("%s addr=%p, size=%d\n", __FUNCTION__, p,s);
   q = p + s - 1;
   SetSIZE (p,s); 
   SetSIZE (q,s); 
}
#endif

#if 0
void
ListAll ()
{
   unsigned int *p;
   int size;
   p = HEAP;

   my_printf (" Address   Offset  Status   Size\n");
   my_printf ("--------- -------- ------ --------\n");
   while (p < &HEAP_END) {
      my_printf ("%8x %8d   ", (unsigned int) p, p - HEAP);
      size = SIZE (p);
      if (IsFree (p))
         my_printf ("Free   %5d", SIZE (p + SIZE (p) - 1));
      else
         my_printf ("Used");
      my_printf ("   %5d\n", size);
      p += size;
   }
   my_printf ("%8p ==========================\n\n", &HEAP_END);
}
#endif

void
RedirectBlock (unsigned int *p, unsigned int *q)
{
   NEXT (q) = NEXT (p);
   PREV (q) = PREV (p);
   if (PREV (p))
      NEXT (PREV (p)) = q;
   else
      AVAIL = q;
   if (NEXT (p))
      PREV (NEXT (p)) = q;
}

void
DeleteBlock (unsigned int *p)
{
   if (PREV (p))
      NEXT (PREV (p)) = NEXT (p);
   else
      AVAIL = NEXT (p);
   if (NEXT (p))
      PREV (NEXT (p)) = PREV (p);
}

void *
malloc (unsigned int s)
{                               /* s = requested size in bytes */
   unsigned int nword, av;
   unsigned int *p, *q;

   if (!s)
      return 0;

   nword = (s >> 2);            /* assume 4-byte boundary */
   if (s % 4)                   /* if not multiple of four */
      nword++;
   nword++;                     /* additional word(s) for tags */
   if (nword < MIN_ALLOC)
      nword = MIN_ALLOC;

   p = AVAIL;
   while (p) {
      av = SIZE (p);
      if (av >= nword) {
         /* Free blocks needs to be at last 4 words */
         if (av - nword < MIN_ALLOC)
            nword = av;         /* remaining size is too small, 
                                 * used entire block */

         /* Mark used block */
         SetSIZE (p, nword);
         SetTAG (p);
#ifdef DEBUG
         if (highest_addr < p + nword)
            highest_addr = p + nword;
         if (largest_block < nword)
            largest_block = nword;
#endif
         if (av - nword > 0) {
            /* readjust list of free blocks */
            q = p + nword;
            SetLowerTAG (q);
            RedirectBlock (p, q);
            /* adjust free size */
            SetSIZE2 (q, av - nword);
            ClearTAG (q);
         }
         else {
            DeleteBlock (p);
         }
         avail_mem -= nword;
         return p + 1;          /* fixit: return the address */
      }
      p = NEXT (p);
   }
#ifdef DEBUG
   my_printf ("Not enough space to allocate %d words\n", nword);
#endif
   /*ListAll();*/
   exit (0);
   return 0;
}

void*
calloc (unsigned int nb, unsigned int s)
{
   void *p;

   p = malloc (nb * s);
   bzero (p, nb * s);
   return p;
}

/*
 * Freeing a block might call for coalescing adjacent blocks. Four different
 * actions have to be considered depending on the allocation status
 * of the lower (L) and upper (U) blocks.
 *
 *   Upper Lower | Action
 *   ------------+-------------------------------------
 *   Free  Free  | Update SIZE on U, delete L from AVAIL
 *   Free  Used  | Update SIZE on U
 *   Used  Free  | Relocate pointer L to current free block
 *   Used  Used  | Insert current free block as new
 */
void
free (void *pt)
{
   unsigned int *p0,
    *p;
   unsigned int nsize;
   char lowmerge,
     upmerge;

   p0 = (unsigned int *) pt;
   p0--;
   avail_mem += SIZE(p0);
   lowmerge = IsLowerFree (p0);
   if (IsLowerFree (p0)) {
      /* lower block is free, merge with this block */
      p = p0 - SIZE (p0 - 1);   /* start address of lower block */
      nsize = SIZE (p) + SIZE (p0);
      SetSIZE (p, nsize);
      p0 = p;
   }
   else {
      ClearTAG (p0);
   }
   p = p0 + SIZE (p0);          /* next block */
   upmerge = IsFree (p);
   if (IsFree (p)) {
      nsize = SIZE (p) + SIZE (p0);
      SetSIZE (p0, nsize);
   }
   else {
      ClearTAG (p0 + SIZE (p0) - 1);
   }
   SetSIZE (p0 + SIZE (p0) - 1, SIZE (p0)); /* copy size to end tag */
   ClearLowerTAG (p0 + SIZE (p0));
   if (!lowmerge) {
      if (!upmerge) {
         /* add block beginning at p0 to AVAIL list (insert first) */
         if (AVAIL)
            PREV (AVAIL) = p0;
         NEXT (p0) = AVAIL;
         PREV (p0) = 0;
         AVAIL = p0;
      }
      else {
         /* relocate pointer from p to p0 */
         RedirectBlock (p, p0);
      }
   }
   else {
      if (upmerge) {
         /* delete upper free block from AVAIL list */
         DeleteBlock (p);
      }
   }
}

void heap_init () {
   highest_addr = 0;
   largest_block = 0;
   HEAP = &HEAP_START;
   AVAIL = HEAP;
   avail_mem = &HEAP_END - HEAP; /* in words, not in bytes */
   my_printf ("Heap is %dK bytes, %p - %p\n", 
         getavailmem(), &HEAP_START, &HEAP_END);
   SetSIZE2 (HEAP, avail_mem);
   NEXT (HEAP) = 0;
   PREV (HEAP) = 0;
   SetLowerTAG (HEAP);          /* nothing prior to the HEAP */
   SetTAG (&HEAP_END);   /* nothing after the HEAP */
}

unsigned int getavailmem()
{
   return avail_mem >> 8; /* return in KBytes */
}

#ifdef DEBUG
void heap_fini ()
{
   my_printf ("Highest address is %p (offset %d)\n", 
         highest_addr, highest_addr - &HEAP_START);
   my_printf ("Largest block is %d\n", largest_block);
}
#endif
