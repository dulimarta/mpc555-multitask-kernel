// $Id: QADC_Sig.h,v 2.1 2000/09/26 17:08:32 dulimart Exp $

/*
 * Filename: QADC_Sig.h
 * Author: Hans Dulimarta, Ph.D. [dulimart@computer.org]
 * Description:
 *   The analog signal on the MPC555 chip is handled by the QADC module.
 * This QADC_Sig is an abstraction on analog signals in the QADC.
 * 
 * $Log: QADC_Sig.h,v $
 * Revision 2.1  2000/09/26 17:08:32  dulimart
 * Added doxygen enable comment lines
 *
 * Revision 2.0  2000/04/24 17:24:06  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#ifndef QADC_A_SIGNAL_H
#define QADC_A_SIGNAL_H

#include "qadc.h"
#include "ASignal.h"

/**
   QADC signals can be configured as either analog or digital lines.
   This class configures a channel as an analog signal 
 */
class QADC_Sig : public ASignal {
   public:
      /// default constructor
      QADC_Sig ();

      /// destructor
      ~QADC_Sig ();

      /** 
         map this object to a channel in QADC module
      */
      void SetChannel (QADC_map*, short);

      /// read the analog value
      int GetAnalog ();
   protected:

      /// pointer to the QADC module
      QADC_map *adc;

      /// index of this channel in the analog queue
      int q_index;
   private:
};
#endif
