/**
 * @file mios.h
 *
 * @author Hans Dulimarta, dulimart@computer.org
 *
 * @brief Multi I/O subsystem
 *
 * This file defines the memory map of the Multi I/O Subsystem
 * in MPC555 architecture.
 *
 * MIOS1 consists of: 
 * @arg Modulus Counter, 
 * @arg Double Action, 
 * @arg Pulse Width Modulation
 * @arg Interrupt Control Section
 * @arg Counter Prescaler
 * @arg IMB Bus Interface
 */
/*
 * $Id: mios.h,v 2.3 2000/10/10 13:45:55 dulimart Exp $
 *
 * $Log: mios.h,v $
 * Revision 2.3  2000/10/10 13:45:55  dulimart
 * Added declaration of mios_preinit
 *
 * Revision 2.2  2000/09/28 19:48:23  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.1  2000/05/23 16:49:54  dulimart
 * Remove SetPWMOutxx macros
 *
 * Revision 2.0  2000/04/24 21:17:36  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef MIOS_H
#define MIOS_H

/**
 * @struct MIOSMC
 * MIOS modulus counter module
 */
typedef struct
{
   unsigned short cnt;          /**< up-counter register */
   unsigned short mml;          /**< modulus latch register */
   unsigned short scrd;         /**< status/control register (duplicate copy) */
   unsigned short scr;          /**< status/control register */
}
MIOSMC;

/**
 * @struct MIOSDA
 * MIOS double ation sub module 
 */
typedef struct
{
   unsigned short mar;          /**< data A register */
   unsigned short mbr;          /**< data B register */
   unsigned short scrd;         /**< status/control register (duplicate copy) */
   unsigned short scr;          /**< status/control register */
}
MIOSDA;

/**
 * @struct MIOSPWM
 * MIOS pulse width modulation sub module
 */
typedef struct
{
   unsigned short perr;         /**< period register */
   unsigned short pulr;         /**< pulse register */
   unsigned short cntr;         /**< count register */
   unsigned short scr;          /**< status/control register */
}
MIOSPWM;

/**
 * @struct MIOSIO
 * MIOS I/O sub module
 */
typedef struct
{
   unsigned short dr;           /**< data register */
   unsigned short ddr;          /**< data direction register */
   unsigned short rsv[2];
}
MIOSIO;

/**
 * @struct MIOSBI
 * MIOS bus interface module (MBISM)
 */
typedef struct
{
   unsigned short tpcr;         /**< Test & Pin Control Register */
   unsigned short vect;         /**< reserved for MIOS vector register */
   unsigned short vnr;          /**< MIOS version number register */
   unsigned short mcr;          /**< MIOS module control register */
}
MIOSBI;

/**
 * @struct MIOSCP
 * MIOS counter prescaler
 */
typedef struct
{
   unsigned short rsv[3];
   unsigned short scr;          /**< Status / Control Register */
}
MIOSCP;

/**
 * @struct MIOSICS
 * MIOS interrupt control section
 */
typedef struct
{
   unsigned short isr;          /**< interrupt status register */
   unsigned short rsv02;
   unsigned short ier;          /**< interrupt enable register */
   unsigned short rpr;          /**< request pending register */
   unsigned short rsv08[20];
   unsigned short lvl;          /**< interrupt level register */
   unsigned short rsv32[7];
}
MIOSICS;

/// MIOS reserved area
typedef struct
{
   unsigned short rsv[4];
}
MIOSRSV;

/**
 * @struct MIOSCH_map
 * MIOS Channel map 
 */
typedef struct
{
   MIOSPWM pwm0[4];             /**< PWM */
   MIOSRSV rsv4[2];
   MIOSMC mc6;                  /**< Modulus Counter */
   MIOSRSV rsv7[4];
   MIOSDA da11[5];              /**< Double Action */
   /* the following section is similar to the previous section */
   MIOSPWM pwm16[4];            /**< PWM */
   MIOSRSV rsv20[2];
   MIOSMC mc22;                 /**< Modulus Counter */
   MIOSRSV rsv23[4];
   MIOSDA da27[5];              /**< Double Action */

   MIOSIO io;                   /**< General Purpose I/O */
   MIOSRSV rsv33[256 - 33];
   MIOSBI bi;                   /**< Bus Interface */
   MIOSRSV rsv257;
   MIOSCP cp;                   /**< Counter Prescaler */
   MIOSRSV rsv259[384 - 259];
   MIOSICS ics[2];              /**< Interrupt Control Section */
}
MIOSCH_map;

/**
 * @var MIOS
 * Global variable for accessing MIOS registers
 */
extern MIOSCH_map MIOS;

/* MCPSM (MIOS Counter Prescaler Submodule) */
//extern short MCPSMSCR; /* MCPSM Status/Control Register */

//#define SetPWMOutHi(ch) MIOS.pwm16[ch].scr |= BITSET(4,16)
//#define SetPWMOutLo(ch) MIOS.pwm16[ch].scr &= ~BITSET(4,16)

#ifdef __cplusplus
extern "C"
{
#endif
   /// MIOS pre-initialization routine
   void mios_preinit ();

   /// MIOS initialization routine
   void mios_init ();

   /// Enable a PWM channel
   void EnablePWM (unsigned char chan);
   /// Disable a PWM channel
   void DisablePWM (unsigned char chan);

   /**
    * Set Period of PWM channel
    * @param chan PWM channel to set
    * @param per period of PWM signal
    */
   void SetPWMPeriod (unsigned char chan, unsigned short per);

   /**
    * Set duty cycle of PWM channel
    * @param chan PWM channel to set
    * @param cycle PWM signal duty cycle (0-100)
    */
   void SetPWMDutyCycle (unsigned char chan, unsigned short cycle);

   /**
    * Set width of PWM channel
    * @param chan PWM channel to set
    * @param width PWM signal width (must be less than period)
    */
   void SetPWMWidth (unsigned char chan, unsigned short width);
#ifdef __cplusplus
}
#endif
#endif
