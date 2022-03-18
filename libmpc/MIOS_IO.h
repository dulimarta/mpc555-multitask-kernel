// $Id: MIOS_IO.h,v 2.2 2000/09/26 17:06:54 dulimart Exp $
/*
 * Filename: MIOS_IO.h
 *
 * Author: Hans Dulimarta, Ph.D.
 *
 * Description:
 *    The MIOS on the MPC555 chip provides 16 channels of digital I/O 
 * signals. The MIOS_IO class is an abstraction of one such channel.
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 *
 */
#ifndef MIOS_D_SIGNAL_H
#define MIOS_D_SIGNAL_H

#include "DSignal.h"

/**
   MIOS_IO is an abstraction of the MPC555 MIOS Digital I/O signal.
 */
class MIOS_IO : public DSignal {
   public:
      /// Default constructor

      MIOS_IO ();

      /**
         User-defined constructor
         @param ch physical channel on MIOS to be used for this object
         @param ty type of diginal signal (DSIGNAL_INPUT or DSIGNAL_OUTPUT)
       */
      MIOS_IO (short ch, short ty);

      /// Destructor

      ~MIOS_IO ();

      /**
         Set channel type
         @param ch physical channel on MIOS to be used for this object
         @param ty type of diginal signal (DSIGNAL_INPUT or DSIGNAL_OUTPUT)
       */
      void SetChType (short ch, short ty);
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
      /// channel id
      short ch_id;
};
#endif
