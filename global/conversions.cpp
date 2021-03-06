/***************************************************************************
                          conversions.cpp  -  
                             -------------------
    begin                : 16/01/2006
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <math.h>
#include "gdata.h"

//------------------------------------------------------------------------------
double linear2dB(const double & x, const GData & p_data)
{
  return (x > 0.0) ? bound((log10(x) * 20.0), p_data.dBFloor(), 0.0) : p_data.dBFloor();
}

//------------------------------------------------------------------------------
double dB2Linear(const double & x)
{
  return pow10(x / 20.0);
}

//------------------------------------------------------------------------------
double dB2Normalised(const double & x, const GData & p_data)
{
  return bound(1.0 - (x / p_data.dBFloor()), 0.0, 1.0);
}

//------------------------------------------------------------------------------
double normalised2dB(const double & x, const GData & p_data)
{
  return (1.0 - x) * p_data.dBFloor();
}
//------------------------------------------------------------------------------
double dB2ViewVal(const double & x)
{
  return pow10(1.0 + x / 40.0);
}

//------------------------------------------------------------------------------
double same(const double & x, const GData & p_data)
{
  p_data.do_nothing();
 return x;
}

//------------------------------------------------------------------------------
double oneMinus(const double & x, const GData & p_data)
{
  p_data.do_nothing();
 return 1.0 - x;
}

//------------------------------------------------------------------------------
double dB2Normalised(const double & x,const double & theCeiling, const double & theFloor)
{
  return bound(1.0 + ((x - theCeiling) / (theCeiling - theFloor)), 0.0, 1.0);
}
//EOF
