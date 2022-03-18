/**
 * @file intr.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief functions to handle interrupt routines
 */

/*
 * $Id: intr.h,v 2.7 2001/10/25 16:46:06 dulimart Exp $
 *
 * $Log: intr.h,v $
 * Revision 2.7  2001/10/25 16:46:06  dulimart
 * Added comment line for DEC interrupt handler
 *
 * Revision 2.6  2000/12/08 22:22:18  dulimart
 * Added SetDECHandler
 *
 * Revision 2.5  2000/12/06 17:50:23  dulimart
 * Added Trace Interrupt Handling
 *
 * Revision 2.4  2000/11/16 02:54:05  dulimart
 * Changed DisableInterrupt() to return previous MSR.
 * Added RestoreInterrupt() for restoring previously stored interrupt status.
 *
 * Revision 2.3  2000/09/28 19:45:30  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.2  2000/06/13 02:50:58  dulimart
 * Renamed hand_ptr_t to handler_ptr_t.
 *
 * Revision 2.1  2000/06/09 13:32:16  dulimart
 * Added SetIRQHandler and SetLVLHandler
 *
 * Revision 2.0  2000/04/24 21:01:26  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 *
 */

/// pointer to interrupt handler function
typedef void (*handler_ptr_t)(void);

/// Decrementer Interrupt Handler
extern handler_ptr_t DECHandler;

#ifdef __cplusplus
extern "C" {
#endif
   /**
    * Disable All External Interrupt
    * @return current interrupt status
    */
   int DisableInterrupt();

   /**
    * Restore interrupt to previously saved state
    * @param s previous interrupt status
    */
   void RestoreInterrupt (int s);

   /// Enable External Interrupt
   void EnableInterrupt();

   /// Enable Power Management Mode
   void EnablePowerMgt();

   /// Disable Power Management Mode
   void DisablePowerMgt();

   /// Set IRQ interrupt handler
   handler_ptr_t SetIRQHandler (int, handler_ptr_t);

   /// Set Level interrupt handler
   handler_ptr_t SetLVLHandler (int, handler_ptr_t);

   /// Set Decrementer Interrupt handler
   handler_ptr_t SetDECHandler (handler_ptr_t);

   /// Enable Trace Interrupt
   void EnableTrace();

   /// Disable Trace Interrupt
   void DisableTrace();
#ifdef __cplusplus
}
#endif
