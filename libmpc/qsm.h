/**
 * @file qsm.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 *    This files defines the memory map of the Queued Serial Multi-Channel
 * Module in MPC555 architecture.
 *
 */
/*
 * $Id: qsm.h,v 2.5 2000/09/28 19:48:23 dulimart Exp $
 *
 * $Log: qsm.h,v $
 * Revision 2.5  2000/09/28 19:48:23  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.4  2000/07/11 13:44:44  dulimart
 * Added qsm_reset(), renamed GetSByte() to RecvByte(), remove WaitTxEmpty()
 *
 * Revision 2.3  2000/07/05 20:12:08  dulimart
 * Modified Receive and Transmit Queue from 'unsigned char' to
 * 'unsigned short'.
 *
 * Revision 2.2  2000/05/23 16:54:10  dulimart
 * Added #ifdef QSM_H, renamed ReadyToSend to WaitTxEmpty
 *
 * Revision 2.1  2000/04/25 12:54:43  dulimart
 * Remove <CR>
 *
 * Revision 2.0  2000/04/24 21:36:56  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#ifndef QSM_H
#define QSM_H
/**
 * @struct QSM_map
 * Queueud Serial Module memory map
 */
typedef struct
{
   short mcr;                   /**< module configuration register */
   short qtest;                 /**< test register */
   unsigned char dsci_il;       /**< dual SCI interrupt level */
   unsigned char _rsrvd_0x05_;
   unsigned char _rsrvd_0x06_;
   unsigned char spi_il;        /**< SPI interrupt level */
   short sc1cr0;                /**< SCI1 Control Register 0 */
   short sc1cr1;                /**< SCI1 Control Register 1 */
   short sc1sr;                 /**< SCI1 Status Register */
   short sc1dr;                 /**< SCI1 Data Register */
   short _rsrvd_0x10_;
   short _rsrvd_0x12_;
   short portqs;                /**< port Q data register */
   unsigned char par;           /**< pin assignment register */
   unsigned char ddr;           /**< data direction register */
   short spicr0;                /**< spi control register 0 */
   short spicr1;                /**< spi control register 1 */
   short spicr2;                /**< spi control register 2 */
   unsigned char spicr3;        /**< spi control register 2 */
   unsigned char spsr;          /**< spi status register */
   short sc2cr0;                /**< SCI2 Control Register 0 */
   short sc2cr1;                /**< SCI2 Control Register 1 */
   short sc2sr;                 /**< SCI2 Status Register */
   short sc2dr;                 /**< SCI2 Data Register */
   short qsc1cr;                /**< QSCI1 Control Register */
   short qsc1sr;                /**< QSCI1 Status Register */
   unsigned short sctq[16];     /**< transmit queue locations */
   unsigned short scrq[16];     /**< receive queue locations */
   unsigned char rsv6c[0x140 - 0x06c];
   unsigned char recram[64];    /**< receive data RAM */
   unsigned char tranram[64];   /**< transmit data RAM */
   unsigned char comdram[32];   /**< command RAM */
}
QSM_map;

#undef QUEUED_TX
#undef QUEUED_RX
#undef RX_INTR

#ifdef __cplusplus
extern "C"
{
#endif
   /// QSM Initialization routine
   void qsm_init ();

   /// QSM reset
   void qsm_reset ();

   /// Send a byte to the serial port
   void SendByte (unsigned char sbyte);
   void SendByte2 (unsigned char sbyte);

   /// Read a byte from the serial port
   char RecvByte ();
   char RecvByte2 ();

   /// Check if there is any character available on the serial port
   int SerialReady ();
   int SerialReady2 ();
#ifdef __cplusplus
}
#endif

/** 
 * @var QSM
 * Global variable for accessing the QSM registers
 */
extern QSM_map QSM;

#endif
