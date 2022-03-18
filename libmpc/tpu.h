/**
 * @file
 *
 * @author Hans Dulimarta dulimart@computer.org
 *
 * This file defines the memory map of the Time Processor Unit 3
 * in the MPC555 architecture.
 *
 */
/* 
 * $Id: tpu.h,v 2.2 2000/11/08 20:04:53 dulimart Exp $
 * 
 * $Log: tpu.h,v $
 * Revision 2.2  2000/11/08 20:04:53  dulimart
 * Replaced unsigned char parameters to int type.
 *
 * Revision 2.1  2000/09/28 19:51:42  dulimart
 * Modified comments to doxygen enabled ones
 *
 * Revision 2.0  2000/04/25 13:52:06  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#ifndef TPU_H
#define TPU_H

/**
 * @struct TPU memory map
 */
typedef struct {
	unsigned short mcr; /**< module configuration register */
	unsigned short tcr; /**< test configuration register */
	unsigned short dscr; /**< development support control register */
	unsigned short dssr; /**< development support status register */
	unsigned short icr; /**< interrupt control register */
	unsigned short cier; /**< channel interrupt enable register */
	unsigned short cfsr[4]; /**< channel function select register */
	unsigned short hsqr[2]; /**< host sequence register */
	unsigned short hsrr[2]; /**< host service request register */
	unsigned short cpr[2]; /**< channel priority register */
	unsigned short cisr; /**< channel interrupt status register */
	unsigned short lr; /**< link register */
	unsigned short sglr; /**< service grant latch register */
	unsigned short dcnr; /**< decode channel number register */
	unsigned short mcr2; /**< module configuration register 2 */
	unsigned short mcr3; /**< module configuration register 3 */
	unsigned short isdr; /**< internal scan data register */
	unsigned short iscr; /**< internal scan control register */
	char dummy[0x100-0x30];
	unsigned short ch_parm[16][8]; /**< channel parameters register */
} TPU_map;

/// Global variables for accessing TPU registers in first TPU. 
extern TPU_map TPU_A; 
/// Global variables for accessing TPU registers in second TPU. 
extern TPU_map TPU_B;

#ifdef __cplusplus
extern "C" {
#endif

/// TPU initialization code
void tpu_init ();

/// Enable TPU clock
void StartTPU (TPU_map *t);

/// Disable TPU clock
void StopTPU (TPU_map *t);

/** 
 * Set Channel Function (Table 17-11)
 * @param t pointer to TPU to use
 * @param chan TPU channel number
 * @param func function to select on that channel
 */
void SetChanFunc (TPU_map *t, int chan, int func);

/** 
 * Set Host Sequence (Table 17-12)
 * @param t pointer to TPU to use
 * @param chan TPU channel number
 * @param seq encoded host sequence
 */
void SetHostSeq (TPU_map *t, int chan, int seq);

/** 
 * Set Host Service Request (Table 17-13)
 * @param t pointer to TPU to use
 * @param chan TPU channel number
 * @param req host service request
 */
void SetHostServReq (TPU_map *t, int chan, int req);

/** 
 * Set Channel Priority (Table 17-14, 17-15)
 * @param t pointer to TPU to use
 * @param chan TPU channel number
 * @param prio priority level
 *    @arg 0 disable
 *    @arg 1 low
 *    @arg 2 middle
 *    @arg 3 high
 */
void SetChanPrio (TPU_map *t, int chan, int prio);

/** 
 * Enable Channel Interrupt (Table 17-10)
 * @param t pointer to TPU to use
 * @param chan TPU channel number
 */
void EnableChanIntr (TPU_map *t, unsigned char chan);

/** 
 * Disable Channel Interrupt (Table 17-10)
 * @param t pointer to TPU to use
 * @param chan TPU channel number
 */
void DisableChanIntr (TPU_map *t, unsigned char chan);

/**
 * Enable Fast Quadrature Decode Function
 * @param chan TPU channel number
 */
void EnableFQD (unsigned char chan);

/**
 * Disable Fast Quadrature Decode Function
 * @param chan TPU channel number
 */
void DisableFQD (int chan);

/**
 * Setup Fast Quadrature Decode Function
 * @param chan TPU channel number
 */
void SetupFQD (int chan);

/**
 * Set the value of Fast Quadrature Decode position
 * @param chan TPU channel number
 * @param pos new position value to set
 */
void SetFQDPos (int chan, int pos);

/**
 * Read the value of Fast Quadrature Decode position
 * @param chan TPU channel number
 * @return current FQD position
 */
short GetFQDPos (unsigned char chan);
#ifdef __cplusplus
}
#endif

#endif
