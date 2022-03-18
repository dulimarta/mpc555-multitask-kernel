/**
 * @file ASignalImp.h
 *
 * @author Hans Dulimarta, Ph.D.  [dulimart@computer.org]
 *
 * @brief The class ASignal is the implementation of the ASignal class.
 */
/* 
 * $Id$
 *
 * $Log$
 *
 * Copyright (c) 2000, Hans Dulimarta, Ph.D.
 */
#ifndef ASIGNALIMP_H
#define ASIGNALIMP_H

/**
   ASignal is an abstract base class of analog signals
 */
class ASignalImp {
   public:
      /// Default constructor
      ASignalImp();

      /**
         Create an analog signal and set its channel number
         @param id channel number
      */
      ASignalImp(short id);
      
      /// destructor
      virtual ~ASignalImp();

      /// get the analog value
      virtual int GetAnalog () = 0;

   protected:

      /// channel id
      short ch_id;
};
#endif
