// $Id: Controller.cpp,v 2.2 2000/09/28 17:44:12 dulimart Exp $
//
// Author: Hans Dulimarta [dulimart@egr.msu.edu, dulimart@computer.org]
//
// Description: see Controller.h
//
// $Log: Controller.cpp,v $
// Revision 2.2  2000/09/28 17:44:12  dulimart
// Removed inclusion of "utility.h"
//
// Revision 2.1  2000/06/30 16:26:33  dulimart
// Removed istream from ctor and removed debugging messages
//
// Revision 2.0  2000/04/25 15:50:03  dulimart
// Ported to MPC555 environment.
//
//
// Copyright (c) 1999,2000   Hans Dulimarta, Ph.D.
//

#include "Controller.h"

Controller::Controller () {
   /*
    * In the MPC555 this function can be used for initializing
    * various units used by the software. Each unit can be 
    * configured/initialized using parameter values stored in the
    * Flash ROM.
    */
   //cerr << __PRETTY_FUNCTION__ << endl;
}

Controller::~Controller () {
   //cerr << __PRETTY_FUNCTION__ << endl;
}

