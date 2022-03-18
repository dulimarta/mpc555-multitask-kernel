/**
 * @file sram.h
 *
 * @author Hans Dulimarta, dulimart@computer.org
 *
 * This file defines the memory map of Static RAM
 * module in MPC555 architecture
 */
/*
 * $Id: sram.h,v 2.1 2000/10/02 20:45:16 dulimart Exp $
 *
 * $Log: sram.h,v $
 * Revision 2.1  2000/10/02 20:45:16  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef SRAM_H
#define SRAM_H
typedef struct {
   unsigned short mcr; /* module configuration register */
} SRAM_map;

/**
 * @var SRAM
 * Global variable for accessing SRAM registers
 */
extern SRAM_map SRAM;

#endif
