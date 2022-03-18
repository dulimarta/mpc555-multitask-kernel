// $Id: MIOS_IO.cc,v 2.4 2000/09/26 17:06:54 dulimart Exp $

#include "MIOS_IO.h"
#include "mios.h"
#include "mpc555.h"

/// Default constructor does not perform anything
MIOS_IO::MIOS_IO () {
}

MIOS_IO::MIOS_IO (short id, short type) : DSignal (type) {
   ch_id = id;
   SetChType (id, type);
}

MIOS_IO::~MIOS_IO () {
}

void MIOS_IO::SetChType (short id, short type) {
   ch_id = id;
   // on MPIOSMDDR I/O pin 15 is the MSB, I/O pin 0 is the LSB
   // (Table 15-27)
   if (type == DSIGNAL_INPUT) {
      MIOS.io.ddr &= ~BITSET(15-ch_id,16);
   }
   else if (type == DSIGNAL_OUTPUT) {
      MIOS.io.ddr |= BITSET(15-ch_id,16);
   }
}

void MIOS_IO::SetHi () {
   //my_printf ("Setting Digital output on ch-%d to HIGH\n", ch_id);
   MIOS.io.dr |= BITSET(15-ch_id,16);
}

void MIOS_IO::SetLo () {
   //my_printf ("Setting Digital output on ch-%d to LOW\n", ch_id);
   MIOS.io.dr &= ~BITSET(15-ch_id,16);
}

bool MIOS_IO::GetState () const {
   /*
   my_printf ("%s: Ch-%d, GPIO=%x (%x)\n", __PRETTY_FUNCTION__,
         ch_id, MIOS.io.dr, MIOS.io.dr & BITSET(15-ch_id,16));
         */
   return MIOS.io.dr & BITSET(15-ch_id,16);
}
