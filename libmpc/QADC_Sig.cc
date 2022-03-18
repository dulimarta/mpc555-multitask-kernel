// $Id: QADC_Sig.cc,v 2.4 2000/09/26 17:08:55 dulimart Exp $

/*
 * Filename: QADC_Sig.cc
 * Author: Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * $Log: QADC_Sig.cc,v $
 * Revision 2.4  2000/09/26 17:08:55  dulimart
 * Added doxygen enable comment lines
 *
 * Revision 2.3  2000/09/14 20:34:36  dulimart
 * Removed debugging messages
 *
 * Revision 2.2  2000/09/06 22:14:15  dulimart
 * Put debugging message inside #ifdef DEBUG
 *
 * Revision 2.1  2000/05/23 16:32:35  dulimart
 * Removed debugginf messages
 *
 * Revision 2.0  2000/04/24 17:24:21  dulimart
 * *** empty log message ***
 *
 * 
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#include "QADC_Sig.h"

QADC_Sig::QADC_Sig () {
}

QADC_Sig::~QADC_Sig () {
}

/**
   Assign an analog signal to a channel in a QADC module
   @param q pointer to QADC_map structure
   @param ch channel number in the specified QADC module
 */
void QADC_Sig::SetChannel (QADC_map *q, short ch) {
   ch_id = ch;
   adc = q;
   q_index = qadcAddChannel (q, QUEUE1, ch);
}

/**
   Read the value of the analog signal
   @return right justified value from the QADC result register
 */
int QADC_Sig::GetAnalog () { 
   qadcSetMode (*adc, QUEUE1, QMODE_SWTRIG | QMODE_SSE);
   return adc->rjurr[q_index];
}
