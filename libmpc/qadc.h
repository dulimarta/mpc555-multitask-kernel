/**
 * @file qadc.h
 *
 * @author Hans Dulimarta, dulimart@computer.org
 *
 * This files defines the memory map of Queued Analag/Digital Converter 
 * module in MPC555 architecture.
 */
/*
 * $Id: qadc.h,v 2.1 2000/09/28 19:48:23 dulimart Exp $
 *
 * $Log: qadc.h,v $
 * Revision 2.1  2000/09/28 19:48:23  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.0  2000/04/24 21:35:42  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 *
 */
#ifndef QADC_H
#define QADC_H
#define QMODE_DISABLE 0
#define QMODE_SWTRIG 1
#define QMODE_ETRIGR 2
#define QMODE_ETRIGF 3
#define QMODE_INT7   4
#define QMODE_INT8   5
#define QMODE_INT9   6
#define QMODE_INT10  7
#define QMODE_INT11  8
#define QMODE_INT12  9
#define QMODE_INT13 10
#define QMODE_INT14 11
#define QMODE_INT15 12
#define QMODE_INT16 13
#define QMODE_INT17 14
#define QMODE_CONT  16
#define QMODE_SSE   32
#define QMODE_SINGLE 0

#define QUEUE1 1                /* index of control register for queue 1 */
#define QUEUE2 2                /* index of control register for queue 2 */


/** 
 * @def qadcDisable
 * Disable QADC \a r
 */
#define qadcDisable(r) r.mcr |= BITSET(0,16)
/** 
 * @def qadcEnable
 * Enable QADC \a r
 */
#define qadcEnable(r) r.mcr &= ~BITSET(0,16)

/// Set QADC mode
#define qadcSetMode(ADC,q,mode) \
   (ADC).cr[q] &= ~0x1F00; \
   (ADC).cr[q] |= (mode) << 8;

/// Set beginning of Queue-2
#define qadcSetBQ2(ADC,q) \
   (ADC).cr[2] &= ~0x3F; \
   (ADC).cr[2] |= q; \

/**
 * @struct QADC_map
 * QADC memory map
 */
typedef struct
{
   short mcr;                   /**< module configuration register */
   short test;                  /**< test register */
   short intr;                  /**< interrupt register */
   unsigned char portqa;        /**< port A data */
   unsigned char portqb;        /**< port B data */
   short ddrqa;                 /**< data direction register */
   short cr[3];                 /**< control registers */
   short sr[2];                 /**< status registers */
   unsigned char rsv814[0xa00 - 0x814];
   short ccw[64];               /**< conversion command word table */
   short rjurr[64];             /**< right-justified unsigned result register */
   short ljsrr[64];             /**< left-justified signed result register */
   short ljurr[64];             /**< left-justified unsigned result register */
}
QADC_map;

/**
 * @var QADC1
 * Global variables for accessing registers in QADC1
 */
extern QADC_map QADC1;
/**
 * @var QADC2
 * Global variables for accessing registers in QADC2
 */
extern QADC_map QADC2;

#ifdef __cplusplus
extern "C"
{
#endif
   /// QADC initialization routine
   void qadc_init ();

   /** 
    * Add Channel to scan
    * @param qadc pointer to QADC_map structure
    * @param q queue location
    * @param chan channel number to scan
    */
   int qadcAddChannel (QADC_map * qadc, unsigned char q, char chan);
#ifdef __cplusplus
}
#endif

#endif
