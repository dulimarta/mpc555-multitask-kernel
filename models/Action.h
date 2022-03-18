// $Id$

/*
 * Filename: Action.h
 *
 * Author: Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * Description:
 *    The class Action represents an action that a robot performs in
 * response to a given command.
 *
 * $Log$
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef ACTION_H
#define ACTION_H

/**
 * This is an abstract class of a command.
 */
class Action {
  public:
     /// Virtual destructor
     virtual ~Action();

     virtual void operator ()() = 0;
  protected:
     /// Default constructor
     Action();
};
#endif
