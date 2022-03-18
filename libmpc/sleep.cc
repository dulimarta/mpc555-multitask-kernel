/**
 *
 * @file sleep.cc
 *
 * @Author Hans Dulimarta  [dulimart@computer.org]
 *
 * @brief This file implements a sleep blocking call.
 */
/*
 * $Id: sleep.cc,v 2.2 2000/12/08 22:21:30 dulimart Exp $
 *
 * $Log: sleep.cc,v $
 * Revision 2.2  2000/12/08 22:21:30  dulimart
 * Change tick count passed to SetAlarm
 *
 * Revision 2.1  2000/12/06 21:55:49  dulimart
 * initial checkin
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */

#include "Clock.h"
#include "stdlib.h"
#include "usiu.h"

unsigned int sleep (unsigned int seconds) {
	usleep (seconds * 1000000);
	return 0;
}

void usleep (unsigned long s) { /* s in microsecond */
   /* if TBS in SCCR is 0, 
    * the Timebase clock on the MPC555 is 4Mhz / 4 = 1 MHz.
    * Therefore, 1 msec is equal to 1000 pulses
    * and 1 usec is equal to 1 pulses
    */

#if 0
   Clock::SetAlarm (s);
#else
   unsigned now, target;

   GetTB (&now, 0);
   target = now + s;
   do {
      GetTB(&now, 0);
   } while (now < target);
#endif
}
