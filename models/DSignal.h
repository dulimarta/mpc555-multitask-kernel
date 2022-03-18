// $Id: DSignal.h,v 2.2 2000/09/26 17:54:34 dulimart Exp $
/*
 * Filename: DSignal.h
 * Author: Hans Dulimarta   [dulimart@computer.org]
 * Description:
 *    DSignal in an abstraction of a digital signal.
 *
 * $Log: DSignal.h,v $
 * Revision 2.2  2000/09/26 17:54:34  dulimart
 * Added doxygen enable comment lines
 *
 * Revision 2.1  2000/05/23 17:00:02  dulimart
 * Changed GetState to const member function
 *
 * Revision 2.0  2000/04/24 20:00:12  dulimart
 * *** empty log message ***
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef DSIGNAL_H
#define DSIGNAL_H

#define DSIGNAL_INPUT    0
#define DSIGNAL_OUTPUT   1
#define DSIGNAL_UNKNOWN -1

/**
   DSignal is an abstraction of digital signals
 */
class DSignal {
   public:
      /// Constructor
      DSignal();
     
      /**
         User-defined constructor
         @param type the type of digital channel (input/output)
      */
      DSignal(short type);
      
      /// virtual destructor
      virtual ~DSignal();

      /** 
         Virtual method for setting channel type and port number
         assignment
      */
      
      virtual void SetChType (short, short) = 0;

      /// Set digital output to logic high
      virtual void SetHi () = 0;

      /// Set digital output to logic low
      virtual void SetLo () = 0;
      
      /// Read the current status of digital channel
      virtual bool GetState () const = 0;

   protected:
      /** 
         Channel type (DSIGNAL_INPUT, DSIGNAL_OUTPUT). 
         On initialization by default constructor, ch_type is set
         to DSIGNAL_UNKNOWN
      */
      short ch_type;
};
#endif
