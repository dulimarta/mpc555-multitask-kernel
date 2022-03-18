// $Id: MIOS_PWM.h,v 2.2 2000/09/26 17:07:20 dulimart Exp $
/*
 * Filename: MIOS_PWM.h
 *
 * Author: Hans Dulimarta, Ph.D.
 *
 * Description:
 *    The MIOS on the MPC555 chip provides 8 channels of PWM signals.
 * MIOS PWM is created as a subclass of DSignal in order to utilize
 * the "average voltage" output of PWM signals for driving the motor
 * pump.
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#ifndef MIOS_PWM_H
#define MIOS_PWM_H

#include "DSignal.h"

/**
   The Multi I/O Subsystem (MIOS) in the MPC555 chip has several modules.
   PWM is one of the modules. Class MIOS_PWM is an abstraction of a PWM 
   module in the MIOS.
 */
class MIOS_PWM : public DSignal {
   public:
      /// Default constructor, set the default max PWM width to 256
      MIOS_PWM (short = 256);

      /// Destructor
      ~MIOS_PWM ();

      /**
         Assign channel id of this module
         @param id channel id
         @param _ second parameter is ignored
      */
      void SetChType (short id, short);

      /**
         Set the maximum PWM width
         @param w the width to set
       */
      void SetMaxWidth (short w);

      /// Set output to 100% duty cycle
      void SetHi ();

      /// Set output to 0% duty cycle
      void SetLo ();

      /// This function is not used
      bool GetState () const;
   protected:

      /// channel id
      short ch_id;

      /// maximum width
      short wid;
};
#endif
