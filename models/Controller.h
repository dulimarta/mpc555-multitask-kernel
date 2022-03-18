// $Id: Controller.h,v 2.4 2001/02/19 21:03:52 dulimart Exp $

/*
 * Filename: Controller.h
 *
 * Author: Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * Description:
 *    The class Controller represents a hardware device for controlling
 * a robot. Usually it consists of a microcontroller and several hardware
 * interface units.
 *
 * $Log: Controller.h,v $
 * Revision 2.4  2001/02/19 21:03:52  dulimart
 * Changed header comment
 *
 * Revision 2.3  2000/09/28 17:43:43  dulimart
 * Added doxygen enable comment lines
 *
 * Revision 2.2  2000/06/27 16:47:51  dulimart
 * Removed istream from constructor
 *
 * Revision 2.1  2000/04/25 15:49:36  dulimart
 * Remove typedef boolean
 *
 * Revision 2.0  2000/04/24 19:57:03  dulimart
 * ported for MPC555
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef CONTROLLER_H
#define CONTROLLER_H

/**
 * This is an abstract class of a microcontroller
 */
class Controller {
  public:
     /// Default constructor
     Controller();
     /// Virtual destructor
     virtual ~Controller();
};
#endif
