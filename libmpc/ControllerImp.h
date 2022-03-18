// $Id$

/*
 * Filename: ControllerImp.h
 *
 * Author: Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * Description:
 *    The class ControllerImp is the implementation detail of the class
 * Controller.
 *
 * $Log$
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef CONTROLLERIMP_H
#define CONTROLLERIMP_H

/**
 * This is an abstract implementation of a microcontroller
 */
class ControllerImp {
  public:
     /// Default constructor
     ControllerImp();
     /// Virtual destructor
     virtual ~ControllerImp();

     //vector<DSignal*> DSVec;
     //vector<ASignal*> ASVec;
};
#endif
