/**
 * @file dptram.h
 *
 * @author Hans Dulimarta, dulimart@computer.org
 *
 * @brief Dual Port TPU RAM
 *
 * This file defines the memory map of Dual-Port TPU RAM
 * module in MPC555 architecture
 */

/*
 * $Id: dptram.h,v 2.3 2000/09/28 19:45:30 dulimart Exp $
 *
 * $Log: dptram.h,v $
 * Revision 2.3  2000/09/28 19:45:30  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.2  2000/09/26 18:51:19  dulimart
 * Changed comment lines to doxygen enable ones.
 *
 * Revision 2.1  2000/09/26 16:46:41  dulimart
 * Initial checkin
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef DPTRAM_H
#define DPTRAM_H

/**
 * Memory map of Dual TPU RAM
 */
typedef struct {
   /*! module configuration register */
   unsigned short mcr;
   /*! test configuration register */
   unsigned short tcr;
   /*! base address register */
   unsigned short bar;
   /*! multiple input signature (high) */
   unsigned short mishr;
   /*! multiple input signature (low) */
   unsigned short mislr;
   /*! multiple input signature counter */
   unsigned short miscnt;
} DPTRAM_map;

/*! an instance of DPTRAM, actual address resolved in the linker script */
extern DPTRAM_map DPTRAM;

#endif
