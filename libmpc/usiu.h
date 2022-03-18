/**
 * @file usiu.h
 *
 * @author Hans Dulimarta, dulimart@computer.org
 *
 *    This file defines the Unified System Interface Unit in MPC555 
 * architecture.
 */
/*
 * $Id: usiu.h,v 2.4 2000/11/14 18:03:20 dulimart Exp $
 *
 * $Log: usiu.h,v $
 * Revision 2.4  2000/11/14 18:03:20  dulimart
 * Added GetMSR().
 *
 * Revision 2.3  2000/11/08 20:03:32  dulimart
 * Fixed bug in tbref0 and tbref1 declaration: change from short to int.
 *
 * Revision 2.2  2000/09/28 19:51:42  dulimart
 * Modified comments to doxygen enabled ones
 *
 * Revision 2.1  2000/09/26 17:52:56  dulimart
 * Changed PIT_INTERVAL from a constant to modifiable parameter
 *
 * Revision 2.0  2000/04/25 14:32:51  dulimart
 * initial revision
 *
 * Copyright (c) 1999,2000  Hans Dulimarta, Ph.D.
 *
 */

#ifndef USIU_H
#define USIU_H
#define KEY 0x55CCAA33

/// Interval of Periodic Interrupt Timer 
/// (value in microsecond, defined in param.S)
extern int PIT_INTERVAL;

/// Assign value \a a to DEC register
#define SetDEC(a) ({ asm ("mtdec %0" : : "r" (a));})

/// Generate bit pattern for IRQ-\a a
#define IRQ(a) (0x80 >> a)

/// Enable IRQ \a r
#define EnableIRQ(r)  USIU.simask |= 0xC000 >> (r*2)

/// Disable IRQ \a r
#define DisableIRQ(r) USIU.simask &= ~(0xC000 >> (r*2))

/**
 * @struct USIU_map
 * memory map of USIU registers
 */
typedef struct
{
   unsigned int mcr;            /**< module configuration register */
   unsigned int sypcr;          /**< System Protection Control Register */
   unsigned int rsv08;
   unsigned short rsv0C;
   unsigned short swsr;         /**< Software Service Register */
   unsigned short sipend;       /**< Interrupt Pending Register */
   unsigned short rsv12;
   unsigned short simask;       /**< Interrupt Mask Register */
   unsigned short rsv16;
   unsigned short siel;         /**< Interrupt Edge Level Mask */
   unsigned short rsv1A;
   unsigned char sivec;         /**< Interrupt Vector */
   unsigned char rsv1D[3];
   unsigned int tesr;           /**< Transfer Error Status Register */
   unsigned int sgpiodt1;       /**< USIU General-Purpose I/O Data Register 1 */
   unsigned int sgpiodt2;       /**< USIU General-Purpose I/O Data Register 2 */
   unsigned int sgpiocr;        /**< USIU General-Purpose I/O Control Register */
   unsigned int emcr;           /**< External Master Mode Control Register */
   unsigned char rsv34[8];
   unsigned int pdmcr;          /**< Pads Module Configuration Register */

   unsigned char rsv40[0x100 - 0x40];
/* Memory Controller Registers */
   unsigned int br0;            /**< Base Register 0 */
   unsigned int or0;            /**< Option Register 0 */
   unsigned int br1;            /**< Base Register 1 */
   unsigned int or1;            /**< Option Register 1 */
   unsigned int br2;            /**< Base Register 2 */
   unsigned int or2;            /**< Option Register 2 */
   unsigned int br3;            /**< Base Register 3 */
   unsigned int or3;            /**< Option Register 3 */

   unsigned char rsv120[0x140 - 0x120];

   unsigned int dmbr;           /**< Dual-Mapping Base Register */
   unsigned int dmor;           /**< Dual-Mapping Option Register */
   unsigned char rsv148[0x178 - 0x148];
   unsigned short mstat;        /**< Memory Status */

   unsigned char rsv17A[0x200 - 0x17A];

/* System Integration Timers */
   unsigned short tbscr;        /**< Time Base Status and Control */
   unsigned short rsv202;
   unsigned int tbref0;         /**< Time Base Reference 0 */
   unsigned int tbref1;         /**< Time Base Reference 1 */
   unsigned char rsv20A[0x220 - 0x20C];
   unsigned short rtcsc;        /**< Real Time Clock Status and Control */
   unsigned short rsv222;
   unsigned int rtc;            /**< Real Time Clock */
   unsigned int rtsec;          /**< Real Time Alarm Seconds */
   unsigned int rtcal;          /**< Real Time Alarm */

   unsigned char rsv230[0x240 - 0x230];
   unsigned short piscr;        /**< PIT Status and Control */
   unsigned short rsv242;
   unsigned int pitc;           /**< PIT Count */
   unsigned int pitr;           /**< PIT Register */

   unsigned char rsv24c[0x280 - 0x24C];
/* Clocks and Reset */
   unsigned int sccr;           /**< System Clock Control Register */
   unsigned int plprcr;         /**< PLL Low Power and Reset Control Register */
   unsigned short rsr;          /**< Reset Status Register */
   unsigned short rsv28A;
   unsigned short colir;        /**< Change of Lock Interrupt Register */
   unsigned short rsv28E;
   unsigned short vsrmcr;       /**< VDDSRM Control Register */
   unsigned char rsv292[0x300 - 0x292];
/* System Integration Timer Keys */
   unsigned int tbscrk;         /**< Time Base Status and Control Key */
   unsigned int tbref0k;        /**< Time Base Reference 0 Key */
   unsigned int tbref1k;        /**< Time Base Reference 1 Key */
   unsigned int tbk;            /**< Time Base and Decrementer Key */

   unsigned char rsv310[0x320 - 0x310];

   unsigned int rtcsck;         /**< Real-Time Clock Status and Control Key */
   unsigned int rtck;           /**< Real-Time Clock Key */
   unsigned int rtseck;         /**< Real-Time Alarm Seconds Key */
   unsigned int rtcalk;         /**< Real-Time Alarm Key */

   unsigned char rsv330[0x340 - 0x330];

   unsigned int piscrik;        /**< PIT Status and Control Key */
   unsigned int pitck;          /**< PIT Count Key */

   unsigned char rsv348[0x380 - 0x348];
/* Clocks and Reset Keys */

   unsigned int sccrk;          /**< System Clock Control Key */
   unsigned int plprcrk;        /**< PLL Low-Power and Reset Control Register Key */
   unsigned int rsrk;           /**< Reset Status Register Key */

}
USIU_map;

#ifdef __cplusplus
extern "C"
{
#endif

   /// Get the value of DEC register
   unsigned int GetDEC ();

   /// Get the current MSR
   unsigned int GetMSR ();

   /** 
    * Get TimeBase Register
    * @param l pointer to integer to receive the TB low
    * @param h pointer to integer to receive the TB high
    */
   void GetTB (unsigned int *l, unsigned int *h);
   //void time_init ();

   /// USIU pre-initialization routine
   void usiu_preinit ();

   /// USIU main initialization routine
   void usiu_init ();
#ifdef __cplusplus
}
#endif

/// global variable to access the USIU memory map
extern USIU_map USIU;

#endif
