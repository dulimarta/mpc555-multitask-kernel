// $Id: DSignal.cc,v 2.1 2001/02/19 21:03:20 dulimart Exp $

/*
 * Filename: DSignal.cc
 * Author: Hans Dulimarta, Ph.D.   [dulimart@computer.org]
 * Description: see DSignal.h
 *
 * $Log: DSignal.cc,v $
 * Revision 2.1  2001/02/19 21:03:20  dulimart
 * Removed inclusion of stream.h
 *
 * Revision 2.0  2000/04/24 19:58:25  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include "DSignal.h"

DSignal::DSignal () { 
}

DSignal::DSignal (short type) {
   ch_type = type;
}

DSignal::~DSignal () { 
}
