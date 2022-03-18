/** 
 * @author Hans Dulimarta    [dulimart@computer.org]
 *
 * @brief This is an abstract Robot class. Currently the class is empty.
 */

//
// $Id: Robot.h,v 2.2 2000/09/28 18:10:41 dulimart Exp $
//
// $Log: Robot.h,v $
// Revision 2.2  2000/09/28 18:10:41  dulimart
// Modified commend to doxygen enabled comments
//
// Revision 2.1  2000/06/30 16:27:44  dulimart
// Defined empy ctor and dtor
//
// Revision 2.0  2000/06/06 18:47:13  dulimart
// Added header comment lines.
//
//
// Copyright (c) 1999-2000   Hans Dulimarta, Ph.D.
//

#ifndef ROBOT_H
#define ROBOT_H

/**
 * Abstract class Robot
 */
class Robot {
   public:
      /// Constructur
      Robot();
      /// Virtual destructur
      virtual ~Robot();
};
#endif
