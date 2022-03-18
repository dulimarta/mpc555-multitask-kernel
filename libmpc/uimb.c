/*
 * $Id: uimb.c,v 2.2 2000/09/28 19:53:05 dulimart Exp $
 *
 * Author: Hans Dulimarta   [dulimart@computer.org]
 * Description: see uimb.h
 *
 * $Log: uimb.c,v $
 * Revision 2.2  2000/09/28 19:53:05  dulimart
 * Replaced OR-assign with assignment
 *
 * Revision 2.1  2000/09/06 22:12:18  dulimart
 * Delete static init
 *
 * Revision 2.0  2000/04/25 14:19:23  dulimart
 * Remove initialization message
 *
 * Revision 1.2  2000/04/25 14:14:46  dulimart
 * Add a initialization message
 *
 * Revision 1.1  2000/04/25 13:52:26  dulimart
 * Initial revision
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#include "uimb.h"

/* generate ILBS sequence 00, 01, ..., IRQMUX */
#define IRQMUX 3 

void uimb_init () {
   UIMB.mcr = IRQMUX << 29;
}
