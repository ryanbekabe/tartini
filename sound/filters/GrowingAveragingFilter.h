/***************************************************************************
                          GrowingAveragingFilter.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef GROWING_AVERAGING_FILTER_H
#define GROWING_AVERAGING_FILTER_H

#include "Filter.h"
#include "array1d.h"

/**
   An unwindowed (rectangular window) simple averaging filter with a fixed size
   It uses a simple incremental algorithm
*/
class GrowingAverageFilter : public Filter
{
 public:
  GrowingAverageFilter(void) { }
  GrowingAverageFilter(int size);
  void init(int size);
  void filter(const float *input, float *output, int n);
  void reset(void);
  inline int delay(void) const { return _size/2; }

 private:
  int _size;
  int _count;
  double total_sum;

  /**
     the last size input values
  */
  Array1d<float> _x;
};

#endif // GROWING_AVERAGING_FILTER_H
// EOF
