/**
 * @file ASignal.h
 *
 * @author Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * @brief The class ASignal is an abstraction of analog signals.
 */
/* 
 * $Id: ASignal.h,v 2.2 2000/10/13 15:41:15 dulimart Exp $
 *
 * $Log: ASignal.h,v $
 * Revision 2.2  2000/10/13 15:41:15  dulimart
 * Modified comment to doxygen enabled comment.
 *
 *
 * Copyright (c) 2000, Hans Dulimarta, Ph.D.
 */
#ifndef ASIGNAL_H
#define ASIGNAL_H

/**
   ASignal is an abstract base class of analog signals
 */
class ASignal {
   public:
      /// Default constructor
      ASignal();

      /**
         Create an analog signal and set its channel number
         @param id channel number
      */
      ASignal(short id);
      
      /// destructor
      virtual ~ASignal();

      /// get the analog value
      virtual int GetAnalog () = 0;

   protected:

      /// channel id
      short ch_id;
};
#endif
