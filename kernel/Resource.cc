/**
 * @file Resource.cc
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 */
/*
 * $Id$
 *
 * $Log$
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#include "Resource.h"

int Resource::ID = 0;

Resource::Resource() 
{
   id = ID++;
}

Resource::~Resource()
{
}
