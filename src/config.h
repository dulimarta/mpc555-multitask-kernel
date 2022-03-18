// $Id: config.h,v 2.2 2001/02/21 20:20:03 dulimart Exp $
//
// Author: Hans Dulimarta  dulimart@[egr.msu.edu, computer.org]
//
// Description:
//    This file contains the routines for run-time environment
// configuration.
//
// $Log: config.h,v $
// Revision 2.2  2001/02/21 20:20:03  dulimart
// Changed executable modules from function to subclass of Command.
//
// Revision 2.1  2000/10/02 20:33:24  dulimart
// Initial checkin
//
//
// Copyright (c) 1999,2000  Hans Dulimarta, Ph.D.
//

#include "stream.h"
#include "Command.h"
#include "WallClimber.h"

//void ClockSetting();

//void PIDSetting ();

class ConfigCmd: public Command {
   public:
      ConfigCmd (WallClimber *_r) : wc(_r) { }
      // 
      // @parm curr_state: current state of the command interpreter
      // @return new state to be set by the command interpreter
      int Execute (int curr_state);
   private:
      WallClimber* wc;
};

//int Config (Robot * r, istream & cmd, int);
