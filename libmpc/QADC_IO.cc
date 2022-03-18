// $Id: QADC_IO.cc,v 2.3 2000/10/02 20:13:41 dulimart Exp $

#include "QADC_IO.h"
#include "mpc555.h"
#ifdef DEBUG
#include "io.h"
#endif

QADC_IO::QADC_IO ()
{
}

QADC_IO::QADC_IO (QADC_map *q, short id, short type)
   : DSignal (type), qadc(q)
{
   SetChType (id, type);
}

QADC_IO::~QADC_IO ()
{
}

void QADC_IO::SetModule (QADC_map *q)
{
   qadc = q;
}

void QADC_IO::SetChType (short id, short type) {
   if (type == DSIGNAL_INPUT) {
      if (0 <= id && id <= 3) {
         data_port = &(qadc->portqb);
         bit_id = 7 - id;
      }
      else if (48 <= id && id <= 51) {
         data_port = &(qadc->portqb);
         bit_id = 51 - id;
      }
      else if (52 <= id && id <= 59) {
         data_port = &(qadc->portqa);
         bit_id = 59 - id;
         qadc->ddrqa &= ~BITSET (bit_id,16); /* clear bit */
#ifdef DEBUG
         my_printf ("%s: Input ch-%d ==> PortA bit-%d, DirReq=%x\n",
               __PRETTY_FUNCTION__, id, bit_id, qadc->ddrqa);
#endif
      }
   }
   else if (type == DSIGNAL_OUTPUT) {
      data_port = 0;
      if (52 <= id && id <= 59) {
         bit_id = 59 - id;
         qadc->ddrqa |= BITSET (bit_id,16); /* set bit */
#ifdef DEBUG
         my_printf ("%s: Output ch-%d ==> PortA bit-%d, DirReq=%x\n",
               __PRETTY_FUNCTION__, id, bit_id, qadc->ddrqa);
#endif
      }
#ifdef DEBUG
      else {
         my_printf ("%s: Invalid port # for OUTPUT\n", __PRETTY_FUNCTION__);
         exit (-1);
      }
#endif
   }
}

void QADC_IO::SetHi ()
{
   //my_printf ("Setting Digital output on ch-%d to HIGH\n", bit_id);
   qadc->portqa |= BITSET(bit_id,8);
}

void QADC_IO::SetLo ()
{
   //my_printf ("Setting Digital output on ch-%d to LOW\n", bit_id);
   qadc->portqa &= ~BITSET(bit_id,8);
}

bool QADC_IO::GetState () const
{
   //my_printf ("%s: Ch-%d data port at %p\n", 
         //__PRETTY_FUNCTION__, bit_id, data_port);
   return data_port ? *data_port & BITSET(bit_id, 8) : 0;
}
