/**
 * @file Resource.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief This class is an abstraction of a system resource. Resource is
 * an abstract base class.
 */
/*
 * $Id$
 *
 * $Log$
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef RESOURCE_H
#define RESOURCE_H

#include "intr.h"
#include "Task.h"
#include <vector>

class Resource
{
   public:
      /// Create a new resource
      Resource();

      /// virtual destructor
      virtual ~Resource();

   protected:
      vector<Task*> requesters;
      Task* owner;

   private:
      static int ID;
      int id;
};
#endif
