// $Id: MPC.h,v 2.4 2001/02/19 17:11:43 dulimart Exp $

/*
 * Filename: MPC.h
 *
 * Author: Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * Description:
 *    MPC is an abstraction of the Motorola PowerPC 555 chip.
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef MPC_CONTROLLER_H
#define MPC_CONTROLLER_H

#include "MIOS_IO.h"
#include "MIOS_PWM.h"
#include "QADC_Sig.h"
#include "QADC_IO.h"
#include "Controller.h"

/**
   MPC is an abstraction of an MPC555 chip as it is viewed
   by the robot controller application.
 */
class MPC : public Controller {
   public:
      /// Default constructor
      MPC ();
      
      /// Destructor
      ~MPC ();
      
      /// Digital signals on the MIOS
      MIOS_IO ds[16];

      /// PWM signals on the MIOS
      MIOS_PWM pwm [2];

      /// Analog signals on the QADC
      QADC_Sig as[3];

      /// Digital signals on the QADC
      QADC_IO qio[6];

   protected:
};
#endif
