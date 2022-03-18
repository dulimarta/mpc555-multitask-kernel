/**
 * @file mpc555.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief symbol declaration of MPC555 environment
 *
 *    This header file contains definitions/declarations of
 * symbols used accross different modules in the MPC555 programming
 * environment.
 *
 */
/*
 * $Id: mpc555.h,v 2.6 2000/10/11 19:44:57 dulimart Exp $
 *
 * $Log: mpc555.h,v $
 * Revision 2.6  2000/10/11 19:44:57  dulimart
 * Inserted declaration of debug_level.
 *
 * Revision 2.5  2000/09/28 19:48:23  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.4  2000/09/06 22:04:24  dulimart
 * Removed declaration of Saturate and _exit
 *
 * Revision 2.3  2000/07/11 13:43:07  dulimart
 * Defined IRQ for Receiver, Keypad, and COM1
 *
 * Revision 2.2  2000/06/09 13:36:39  dulimart
 * Removed macro definition of LVLxHandler.
 *
 * Revision 2.1  2000/05/23 16:51:56  dulimart
 * Added 'noreturn' attribute to exit
 *
 * Revision 2.0  2000/04/24 21:26:53  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef MPC555_H
#define MPC555_H

#define PITIRQ  1 /* Periodic Interrupt Timer IRQ */
#define RXIRQ   2 /* Receiver IRQ */
#define TBIRQ   3 /* Time Base IRQ */
#define KPIRQ   4 /* KeyPad IRQ */
#define RTCIRQ  5 /* Real Time Clock IRQ */
#define COM1IRQ 6 /* Serial Communication IRQ */
#define MIOSLVL 7 /* Multi I/O Subsystem IRQ */
#define MIOSTM0 1 /* Timemultiplexing slot for MIOS modules  0-15 */
#define MIOSTM1 2 /* Timemultiplexing slot for MIOS modules 16-31 */

/**
 * @def BITSET
 * Generate a bit pattern to set bit \a a in \b b-bit register
 */
#define BITSET(a,b) (1<<((b)-(a)-1))

extern int debug_level;

#endif
