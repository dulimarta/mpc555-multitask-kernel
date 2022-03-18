// $Id$
/*
 * Filename: DSignalImp.h
 * Author: Hans Dulimarta   [dulimart@computer.org]
 * Description:
 *    DSignalImp is the implementation detail of DSignal class. 
 *
 * $Log$
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef DSIGNALIMP_H
#define DSIGNALIMP_H

#define DSIGNAL_INPUT    0
#define DSIGNAL_OUTPUT   1
#define DSIGNAL_UNKNOWN -1

/**
   DSignal is an abstraction of digital signals
 */
class DSignalImp {
   public:
      /// Constructor
      DSignalImp();
     
      /**
         User-defined constructor
         @param type the type of digital channel (input/output)
      */
      DSignalImp(short type);
      
      /// virtual destructor
      virtual ~DSignalImp();

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
