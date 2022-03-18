// $Id: MIOS_PWM.cc,v 2.4 2000/09/28 19:19:19 dulimart Exp $

#include "MIOS_PWM.h"
#include "mios.h"
#include "mpc555.h"

MIOS_PWM::MIOS_PWM (short w) 
{
   wid = w;
}

#if 0
MIOS_PWM::MIOS_PWM (short id, short type) : DSignal (type) 
{
   ch_id = id;
   SetChType (id, type);
}
#endif

MIOS_PWM::~MIOS_PWM () 
{
}

void MIOS_PWM::SetChType (short id, short) 
{
   ch_id = id;
   SetPWMPeriod (ch_id, 512);
   SetPWMWidth (ch_id, 0);
   EnablePWM (ch_id);
}

void MIOS_PWM::SetMaxWidth (short w) 
{
   wid = w;
}

void MIOS_PWM::SetHi () 
{
   SetPWMWidth (ch_id, wid);
}

void MIOS_PWM::SetLo () 
{
   SetPWMWidth (ch_id, 0);
}

bool MIOS_PWM::GetState () const 
{
   return 1;
}
