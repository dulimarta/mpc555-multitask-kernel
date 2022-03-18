// $Id$

/*
 * Filename: Command.h
 *
 * Author: Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * Description:
 *    The class Command represents a command sent to a robot controller.
 *
 * $Log$
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef COMMAND_H
#define COMMAND_H

#include "Robot.h"

/**
 * This is an abstract class of a command.
 */
class Command {
  public:
     /// Virtual destructor
     virtual ~Command();

     /** 
        Execute the command.
        @param old_state the current state of the command interpreter
        @return new state to be set in the command intepreter
     */
     virtual int Execute (int old_state) = 0;
  protected:
     /// Default constructor
     Command();
  private:
};
#endif
