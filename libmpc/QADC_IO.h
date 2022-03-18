// $Id: QADC_IO.h,v 2.2 2000/09/28 19:27:10 dulimart Exp $
/*
 * Filename: QADC_IO.h
 *
 * Author: Hans Dulimarta, Ph.D.
 *
 * Description:
 *    The QADC on the MPC555 chip provides analog and digital signals.
 * QADC_IO class is an abstraction of a channel configured as digital 
 * signal.
 *
 * $Log: QADC_IO.h,v $
 * Revision 2.2  2000/09/28 19:27:10  dulimart
 * Removed inclusion of "io.h"
 *
 * Revision 2.1  2000/09/26 16:44:26  dulimart
 * Initial checkin
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#ifndef QADC_D_SIGNAL_H
#define QADC_D_SIGNAL_H

#include "DSignal.h"
#include "qadc.h"

/**
   QADC_IO is an abstraction of the MPC555 QADC Digital I/O signal.
 */
class QADC_IO : public DSignal {
   public:
      /// Default constructor

      QADC_IO ();

      /**
         User-defined constructor
         @param ch physical channel on QADC to be used for this object
         @param ty type of diginal signal (DSIGNAL_INPUT or DSIGNAL_OUTPUT)
       */
      QADC_IO (QADC_map *qadc, short ch, short ty);

      /// Destructor

      ~QADC_IO ();

      /**
         Set channel type
         @param ch physical channel on QADC to be used for this object
         @param ty type of diginal signal (DSIGNAL_INPUT or DSIGNAL_OUTPUT)
       */
      void SetChType (short ch, short ty);

      /**
         Select one of the QADC modules
         @param q pointer to QADC memory map structure
       */
      void SetModule (QADC_map *q);

      /**
         Set digital output of a channel to logical HIGH
       */
      void SetHi ();

      /**
         Set digital output of a channel to logical LOW
       */
      void SetLo ();

      /**
         Read the status of a digital input channel
         @return the logical state of the channel
       */
      bool GetState () const;
   protected:

      /// bit id in the data port (bit-0 = MSB)
      short bit_id;
      
      /// pointer to QADC memory map structure
      QADC_map *qadc;

      /// data port (portqa or portqb) used by this signal
      unsigned char *data_port;
};
#endif
