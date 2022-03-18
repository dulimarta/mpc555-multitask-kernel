/**
 * @file uimb.h
 *
 * @author Hans Dulimarta, dulimart@computer.org
 *
 * This file defines the memory map of U-Bus to IMB-Bus interface
 * module in MPC555 architecture
 */
/*
 * $Id: uimb.h,v 2.3 2000/09/28 19:51:42 dulimart Exp $
 *
 * $Log: uimb.h,v $
 * Revision 2.3  2000/09/28 19:51:42  dulimart
 * Modified comments to doxygen enabled ones
 *
 * Revision 2.2  2000/08/30 21:29:50  dulimart
 * Added #ifdef UIMB_H at the header
 *
 * Revision 2.1  2000/04/25 15:14:00  dulimart
 * Remove <CR>
 *
 * Revision 2.0  2000/04/25 14:27:39  dulimart
 * initial revision
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef UIMB_H
#define UIMB_H

/**
 * @struct UIMB_map
 * U-bus to IMB-bus memory map
 */
typedef struct {
   unsigned int mcr; /**< module configuration register */
   unsigned char rsv04 [12];
   unsigned int tcr; /**< test control register */
   unsigned char rsv14 [12];
   unsigned int ipend; /**< UI interrupt pending */
} UIMB_map;

/// Global variable used for accessing the UIMB registers
extern UIMB_map UIMB;

/// UIMB initialization code
void uimb_init();
#endif
